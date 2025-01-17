# MicroGC

A simple stop-the-world mark-and-sweep garbage collector.


## Table of Contents

- [Setting Up](#setting-up)
- [Allocating Memory](#allocating-memory)
- [Setting and Getting Values](#setting-and-getting-values)
- [Setting Cleanup Functions](#setting-cleanup-functions)
- [Working with Pointers](#working-with-pointers)
- [The `gc::set_root` Function](#the-gcset_root-function)
- [The `gc::help` Function](#the-gchelp-function)
- [The `gc::cycle` Function](#the-gccycle-function)

## Setting Up

To get started, ensure that you include the header file in your project:

```cpp
#include "microgc/gc.hpp"
```

Next, compile and link `microgc/gc.cpp` in your project.

## Allocating Memory

To allocate a new cell, use the `gc::alloc` function. This function returns a `gc::ptr`, which is a normal pointer to the newly created cell.

```cpp
gc::ptr my_cell = gc::alloc();
```

## Setting and Getting Values

A `gc::cell` manages a vector of fields.
Each field has a value and a type, which the garbage collector uses to manage cleanup.

```cpp
resize(my_cell, 2);
set_field(my_cell, 0, 0, 42);
set_type(my_cell, 1, 1);
set_value(my_cell, 1, malloc(16));
```

Getters and setters operate on values which can be c-style cast to `gc::value`.

```cpp
int x = get_value<int>(my_cell, 0);
char const *p = get_value<char const *>(my_cell, 1);
```

## Setting Cleanup Functions

You can define cleanup functions which will be called when a cell is being cleaned up. Use `gc::set_cleanup` to specify a cleanup function for a particular type. The cleanup function takes a `gc::value` parameter, which can hold a pointer.

```cpp
void cleanup_nop(gc::value) { }

void cleanup_free(gc::value ptr) {
  free(reinterpret_cast<void *>(ptr)); }

gc::set_cleanup(0, cleanup_nop);
gc::set_cleanup(1, cleanup_free);
```

## Working with Pointers

Cells can point to other cells.
Use type `-1` to specify that a field contains a `gc::ptr`, or just use the shorter version in the example.

```cpp
gc::ptr another_cell = gc::alloc();
push_field(another_cell, my_cell);
```

`get_ptr` is short for `get_value<gc::ptr>`.

```cpp
gc::ptr my_cell2 = get_ptr(another_cell, 0);
```

## The `gc::set_root` Function

The `gc::set_root` function controls the root pointer.
Things reachable from the root pointer are preserved.

```cpp
gc::set_root(my_cell);
gc::help(); // `another_cell` could be freed.
```

## The `gc::help` Function

The `gc::help` function is for reclaiming memory.
Trigger it frequently, whenever the garbage collector can perform cleanup operations.

```cpp
for(;;) { // evaluation loop
gc::help();
```

## The `gc::cycle` Function

Call `gc::cycle` directly at the end of your program to finalize any remaining cleanup.
Set the root to `nullptr` before the final call to `gc::cycle`.

```cpp
gc::set_root(nullptr);
gc::cycle(); // all cells are freed.
} // end of main
```

Happy Coding!
