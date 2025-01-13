#ifndef MICROGC_GC_HPP
#define MICROGC_GC_HPP

#include <cstddef>

namespace gc {

struct cell;

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

bool has(ptr const &p, size_t field);

ptr get_stem(ptr const &p, size_t field);
void set_stem(ptr const &p, size_t field, ptr value);

size_t get_leaf(ptr const &p, size_t field);
void set_leaf(ptr const &p, int type, size_t field, size_t value);

void unset(ptr const &p, size_t field);

void set_cleanup(int type, void (*cb)(size_t));

void cycle();
void help();

}

#endif