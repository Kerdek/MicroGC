#ifndef GCPP_GC_HPP
#define GCPP_GC_HPP

#include <cstddef>
#include <unordered_map>

namespace gc {

struct field {
size_t value;
int type; };

struct cell {
std::unordered_map<size_t, field> p; };

struct ptr {
ptr();
ptr(cell *i);
ptr(ptr const &);
ptr(ptr &&);
ptr &operator=(ptr const &);
ptr &operator=(ptr &&);
~ptr();
cell *i; };

extern ptr const null;

ptr alloc();

bool is_null(ptr const &p);

bool has(ptr const &p, size_t f);

ptr get_stem(ptr const &p, size_t f);
void set_stem(ptr const &p, size_t f, ptr v);

size_t get_leaf(ptr const &p, size_t f);
void set_leaf(ptr const &p, int type, size_t f, size_t v);

void unset(ptr const &p, size_t f);

void set_cleanup(int type, void (*)(size_t));

void cycle();
void help();

}

#endif