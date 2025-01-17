# MicroGC

A simple stop-the-world mark-and-sweep garbage collector.

## Setting Up

Ensure that you have included the header file in your project.

```cpp
#include "microgc/gc.hpp"
```

Compile and link `microgc/gc.cpp` in your project.

## Allocating Memory

To allocate a new cell, use `gc::alloc`. This function returns a `gc::ptr`, which is a normal pointer to the newly created cell.

```cpp
gc::ptr my_cell = gc::alloc();
```

## Setting and Getting Values

A `gc::cell` manages a vector of fields.
A field has a value and a type.
The type is used by the garbage collector to control cleanup.

```cpp
resize(my_cell, 2);
set_field(my_cell, 0, 0, 42);
set_type(my_cell, 1, 1);
set_value(my_cell, 1, reinterpret_cast<gc::value>(malloc(16)));
```

## Setting Cleanup Functions

You can define cleanup functions which will be called when a cell is being cleaned up. Use `gc::set_cleanup` to specify a cleanup function for a particular type. The cleanup function takes a `gc::value` parameter, which is large enough to hold a pointer.

```cpp
void cleanup_nop(gc::value) { }

void cleanup_free(gc::value ptr) {
  free(reinterpret_cast<void *>(ptr)); }

gc::set_cleanup(0, cleanup_nop);
gc::set_cleanup(1, cleanup_free);
```

## Working with Pointers

Cells can point to other cells.
Use type `-1` to specify that a field contains a `gc::ptr`.

```cpp
gc::ptr another_cell = gc::alloc();
push_field(another_cell, -1, reinterpret_cast<gc::value>(another_cell));
```

## The `set_root` Function

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
