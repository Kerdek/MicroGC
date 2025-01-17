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
template<typename T>
void push_field(ptr p, type t, T v) {
push_field(p, t, (gc::value)v); }
inline void push_field(ptr p, gc::ptr v) {
push_field(p, -1, v); }
void pop_field(ptr p);
void set_field(ptr p, size i, type t, value v);
template<typename T>
requires requires (T x) {
  (gc::value)x; }
void set_field(ptr p, size i, type t, T v) {
set_field(p, i, t, (gc::value)v); }
inline void set_field(ptr p, size i, gc::ptr v) {
set_field(p, i, -1, v); }
void set_type(ptr p, size i, type t);
void set_value(ptr p, size i, value v);
template<typename T>
void set_value(ptr p, size i, T v) {
set_value(p, i, (gc::value)v); }
type get_type(ptr p, size i);
value get_value(ptr p, size i);
template<typename T>
requires requires (gc::value x) {
  (T)x; }
T get_value(ptr p, size i) {
return (T)get_value(p, i); }
inline gc::ptr get_ptr(ptr p, size i) {
return get_ptr(p, i); }
void set_root(ptr p);
void set_cleanup(size type, void (*cb)(value));
void cycle();
void help();

}

#endif