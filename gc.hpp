#ifndef MICROGC_GC_HPP
#define MICROGC_GC_HPP

namespace gc {

struct cell;

using size = decltype(sizeof(0));
using value = unsigned long;
using type = unsigned char;
using ptr = cell *;

ptr alloc();
void resize(ptr p, size n);
size get_size(ptr p);
void push_field(ptr p, type t, value v);
void pop_field(ptr p);
void set_field(ptr p, size i, type t, value v);
void set_type(ptr p, size i, type t);
void set_value(ptr p, size i, value v);
type get_type(ptr p, size i);
value get_value(ptr p, size i);
void set_root(ptr p);
void set_cleanup(size type, void (*cb)(value));
void cycle();
void help();

}

#endif