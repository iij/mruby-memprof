#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mruby.h"
#include "mruby/array.h"
#include "mruby/compile.h"
#include "mruby/dump.h"
#include "mruby/variable.h"

struct memheader {
  size_t len;      /* size of obj (not including len) */
  union {          /* union for alignment */
    void *ptr;
    long long l;
    double d;
  } obj;
};

struct memprof_userdata {
  unsigned int malloc_cnt;
  unsigned int realloc_cnt;
  unsigned int free_cnt;
  unsigned int freezero_cnt;
  unsigned long long total_size;

  unsigned int current_objcnt;
  unsigned long long current_size;
};

extern void memprof_init(mrb_allocf *, void **);
extern void memprof_show(void *);


static void *
memprof_allocf(struct mrb_state *mrb, void *ptr, size_t size, void *ud0)
{
  struct memprof_userdata *ud = ud0;
  struct memheader *mptr;
  size_t oldsize = 0;

  if (ptr != NULL) {
    mptr = (struct memheader *)((char *)ptr - offsetof(struct memheader, obj));
  } else {
    mptr = NULL;
  }

  if (size == 0) {
    /* free(ptr) */
    ud->free_cnt++;
    if (mptr != NULL) {
      ud->current_objcnt--;
      ud->current_size -= mptr->len;
      mptr->len = SIZE_MAX;
      free(mptr);
    } else {
      ud->freezero_cnt++;
    }
    return NULL;
  }
  else {
    /* malloc(size) or realloc(ptr, size) */
    if (ptr == NULL) {
      ud->malloc_cnt++;
    } else {
      ud->realloc_cnt++;
      oldsize = mptr->len;
    }
    mptr = realloc(mptr, size + sizeof(*mptr) - sizeof(mptr->obj));
    if (mptr == NULL) {
      return NULL;
    }
    mptr->len = size;
    if (ptr == NULL) {
      ud->current_objcnt++;
    } else {
      ud->current_size -= oldsize;
    }
    ud->current_size += size;
    ud->total_size += size;
    return (void *)&mptr->obj;
  }
}

void
memprof_init(mrb_allocf *funp, void **udp)
{
  struct memprof_userdata *ud;
  ud = calloc(1, sizeof(*ud));
  if (ud == NULL) {
    abort(); /* fatal! */
  }

  *funp = memprof_allocf;
  *udp  = ud;
}

static mrb_value
mrb_memprof_show(mrb_state *mrb, mrb_value self)
{
  if (mrb->allocf != memprof_allocf) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "memory profiling allocator is not installed");
  }

#if MRUBY_RELEASE_NO < 10000
  memprof_show(mrb->ud);
#else
  memprof_show(mrb->allocf_ud);
#endif
  return mrb_nil_value();
}

void
memprof_show(void *ptr)
{
  const struct memprof_userdata *ud = ptr;
  printf("number of malloc calls:              %8u\n", ud->malloc_cnt);
  printf("number of realloc calls:             %8u\n", ud->realloc_cnt);
  printf("number of free calls:                %8u\n", ud->free_cnt);
  printf("number of free(!NULL) calls:         %8u\n",
         ud->free_cnt - ud->freezero_cnt);
  printf("number of free(NULL) calls:          %8u\n", ud->freezero_cnt);
  printf("total size of allocated memory:  %12llu bytes\n", ud->total_size);
  printf("current number of allocated objects: %8u\n", ud->current_objcnt);
  printf("current size of allocated memory:%12llu bytes\n", ud->current_size);
  printf("\n");
  fflush(stdout);
}

void
mrb_mruby_memprof_gem_init(mrb_state *mrb)
{
  struct RClass *m;

  m = mrb_define_module(mrb, "MemProf");
  mrb_define_class_method(mrb, m, "show", mrb_memprof_show, MRB_ARGS_NONE());
}

void
mrb_mruby_memprof_gem_final(mrb_state *mrb)
{
}
