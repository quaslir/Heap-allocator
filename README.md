# Heap-allocator

## Overview

#### Heap-allocator is a small project that is quite similar to the 'malloc' function from <stdlib.h>. The 'mmap' function was used to obtain a block of memory from RAM. Additionally, if a block is free and its size exceeds 100,000 bytes, the block is returned to RAM. The major benefit of this memory allocator is that the user is protected from double-freeing memory, whereas with 'malloc' from <stdlib.h>, the user’s program would crash with a segmentation fault.

## Getting started

### Clone the repository 

```bash
git clone https://github.com/quaslir/Heap-allocator/
cd Heap-allocator
```
#### To create a main.c file you can use the provided command:
```bash
touch main.c
```
#### Inside main.c you have to include Heap-allocator function: 
```bash
#include "heap_allocation.h"
```
#### To compile your project you can use provided command:
```bash
gcc main.c heap_allocator.c -o my_program && ./my_program
```


#### Enjoy allocating memory without errors!
