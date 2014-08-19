# mruby-memprof

mruby-memprf is a simple memory profiler for mruby.

## Usage

```
% mruby-memprof -m hello.rb
### Just after mrb_open_allocf():
number of malloc calls:                  6480
number of realloc calls:                    0
number of free calls:                    1436
number of free(!NULL) calls:              334
number of free(NULL) calls:              1102
total size of allocated memory:        689710 bytes
current number of allocated objects:     6146
current size of allocated memory:      611230 bytes

Hello, world!
### Just after mrb_close():
number of malloc calls:                  6509
number of realloc calls:                    4
number of free calls:                   10530
number of free(!NULL) calls:             6509
number of free(NULL) calls:              4021
total size of allocated memory:        745658 bytes
current number of allocated objects:        0
current size of allocated memory:           0 bytes
```

 1. "number of malloc calls" and "number of free(!NULL) calls" must be same
    (or some memory was leaked).
 2. "current number of allocated objects" must be 0 (or some memory was leaked).

```
% cat gc.rb
puts "Before GC:"
MemProf.show

GC.start

puts "After GC:"
MemProf.show

% mruby-memprof gc.rb
Before GC:
number of malloc calls:                  6510
number of realloc calls:                    4
number of free calls:                    1447
number of free(!NULL) calls:              341
number of free(NULL) calls:              1106
total size of allocated memory:        745793 bytes
current number of allocated objects:     6169
current size of allocated memory:      611937 bytes

After GC:
number of malloc calls:                  6515
number of realloc calls:                    4
number of free calls:                    2281
number of free(!NULL) calls:              759
number of free(NULL) calls:              1522
total size of allocated memory:        745969 bytes
current number of allocated objects:     5756
current size of allocated memory:      580153 bytes

```

 - It shows GC frees 413 objects that uses 31784 bytes.


## License

Copyright (c) 2014 Internet Initiative Japan Inc.

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
