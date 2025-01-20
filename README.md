# MicroGC

A simple, precise, stop-the-world, tracing garbage collector.

## Table of Contents

- [Setting Up](#setting-up)
- [Allocating Memory](#allocating-memory)
- [Setting and Getting Values](#setting-and-getting-values)
- [Setting Cleanup Functions](#setting-cleanup-functions)
- [Working with Pointers](#working-with-pointers)
- [The `gc::set_root` Function](#the-gcset_root-function)
- [The `gc::help` Function](#the-gchelp-function)
- [The `gc::cycle` Function](#the-gccycle-function)
- [API Reference](#api-reference)
- [Usage Examples from lcpp](#usage-examples-from-lcpp)

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
Things reachable from the root pointer are preserved during calls to `gc::help` and `gc::cycle`.

```cpp
gc::set_root(my_cell);
gc::help(); // `another_cell` could be freed.
```

## The `gc::help` Function

The `gc::help` function is for reclaiming memory. In a typical interpreter, garbage collection cycles are triggered based on memory usage conditions, such as when the allocated memory exceeds a certain threshold or when the system detects that memory is becoming scarce. `gc::help` examines memory usage and initiates garbage collection if necessary. Any cells not reachable from the root will be eligible for reclamation, so all cells which are in use must be mounted somewhere at the point of the call to `gc::help`.

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

## API Reference

### Types

- `ptr`: A pointer to a memory cell.
- `size`: Represents the size of memory.
- `value`: An unsigned long value.
- `type`: An unsigned char representing the type of data.

### Functions

- `gc::ptr gc::alloc()`: Allocates a new memory cell with no capacity and returns a pointer to it.
- `void resize(gc::ptr p, gc::size n)`: Change the size and capacity of `p` to `n`.
- `gc::size get_size(gc::ptr p)`: Returns the size of `p`.
- `void push_field(gc::ptr p, gc::type t, gc::value v)`: Pushes a field of type `t` and value `v` onto the end of `p`. Increases the capacity of the cell as necessary.
- `template<typename T> void push_field(gc::ptr p, gc::type t, T v)`: Pushes a field of type `t` and value `v` onto the end of `p`. `v` is converted from `T` to a `gc::value` by a c-style cast.
- `void push_field(gc::ptr, gc::ptr v)`: Pushes a field of type `-1` and value `v` onto the end of `p`.
- `void pop_field(gc::ptr p)`: Pops the last field from the memory cell.
- `void set_field(gc::ptr p, gc::size i, gc::type t, gc::value v)`: Sets the field at index `i` in `p` to type `t` and value `v`.
- `template<typename T> void set_field(gc::ptr p, gc::size i, gc::type t, T v)`: Sets the field at index `i` in `p` to type `t` and value `v`. `v` is converted from `T` to a `gc::value` by a c-style cast.
- `void set_field(gc::ptr p, gc::size i, gc::ptr v)`: Sets the field at index `i` in `p` to type `-1` and value `v`.
- `void set_type(gc::ptr p, gc::size i, gc::type t)`: Sets the type of the field at index `i` in `p` to `t`.
- `void set_value(gc::ptr p, gc::size i, gc::value v)`: Sets the value of the field at index `i` in `p` to `v`.
- `template<typename T> void set_value(gc::ptr p, gc::size i, T v)`: Sets the value of the field at index `i` in `p` to `v`. `v` is converted from `T` to a `gc::value` by a c-style cast.
- `gc::type get_type(gc::ptr p, gc::size i)`: Retrieves the type of the field at index `i` in `p`.
- `gc::value get_value(gc::ptr p, gc::size i)`: Retrieves the value of the field at index `i` in `p`.
- `template<typename T> T get_value(gc::ptr p, gc::size i)`: Retrieves the value of the field at index `i` in the memory cell. `v` is converted to `T` from `gc::value` by a c-style cast.
- `void set_root(gc::ptr p)`: Sets the pointer `p` as the root for garbage collection.
- `void gc::set_cleanup(gc::size type, void (*cb)(gc::value))`: Registers a cleanup callback for a specific type.
- `void gc::cycle()`: Performs a garbage collection cycle.
- `void gc::help()`: Potentially erforms a garbage collection cycle depending on memory usage conditions.

## Usage Examples from lcpp

```cpp
struct string { gc::ptr p; };

string new_string(stringbuf buf, size_t begin, size_t end);

stringbuf buf(string s);
size_t begin(string s);
size_t end(string s);

std::string_view text(string s);
```

```cpp

enum {
k_buf,
k_begin,
k_end,
k_max };

string new_string(stringbuf buf, size_t begin, size_t end) {
gc::ptr p = gc::alloc();
resize(p, k_max);
set_field(p, k_buf, buf.p);
set_field<size_t>(p, k_begin, 0, begin);
set_field<size_t>(p, k_end, 0, end);
return { .p = p }; }

stringbuf buf(string s) {
return { .p = get_ptr(s.p, k_buf) }; }

size_t begin(string s) {
return get_value<size_t>(s.p, k_begin); }

size_t end(string s) {
return get_value<size_t>(s.p, k_end); }

std::string_view text(string s) {
char const *const p = data(buf(s));
return { p + begin(s), p + end(s) }; }
```

Happy Coding!
