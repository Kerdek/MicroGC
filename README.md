# MicroGC

A simple stop-the-world mark-and-sweep garbage collector.

## Setting Up

Ensure that you have included the header file in your project.

```cpp
#include "gc.hpp"
```

Compile and link `gc.cpp` in your project.

## Allocating Memory

To allocate a new cell, use `alloc`. This function returns a `ptr`, which is a smart pointer to the newly created cell.

```cpp
gc::ptr myCell = gc::alloc();
```

## Setting and Getting Values

You can set and get values in the cell using `set_leaf` and `get_leaf`.

**Setting a Value**: Use `set_leaf` to store a value in the cell.

```cpp
size_t fieldId = 1;
int type = 1;
size_t value = 42;

set_leaf(myCell, type, fieldId, value);
```

**Getting a Value**: Use `get_leaf` to retrieve the value stored in the cell. Accessing a field that does not exist will throw an exception.

```cpp
size_t retrievedValue = get_leaf(myCell, type, fieldId);
```

## Setting Cleanup Functions

You can define cleanup functions which will be called when a field is modified or unset or when the cell is being cleaned up. Use `set_cleanup` to specify a cleanup function for a particular type. The cleanup function takes a `size_t` parameter.

```cpp
void myCleanupFunction(size_t ptr) {
  free(reinterpret_cast<void *>(ptr)); }

// Set the cleanup function for type 2
gc::set_cleanup(2, myCleanupFunction);
```

## Working with Pointers

You can create pointers to other cells. Use `set_stem` and `get_stem` to manage these pointers.

**Setting a Pointer**: First, allocate another cell and set it as a pointer in the first cell. Any pointer or non-pointer which was previously in the field will be overwritten, and any resources which were managed will be freed.

```cpp
gc::ptr anotherCell = gc::alloc();
set_stem(myCell, fieldId, anotherCell);
```

**Getting a Pointer**: Retrieve the pointer from the first cell.

```cpp
gc::ptr retrievedPointer = get_stem(myCell, fieldId);
```

## The `help` Function

The `help` function is for reclaiming memory. Trigger it frequently, whenever the garbage collector can perform cleanup operations.

```cpp
for(;;) { // evaluation loop
gc::help();
```

## The `cycle` Function

Call `cycle` directly at the end of your program to finalize any remaining cleanup.

Happy Coding!
