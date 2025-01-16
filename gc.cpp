#include "gc.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <vector>

namespace gc {

struct cell {
size n, c;
char *p; };

static std::vector<ptr> range;
static std::vector<void (*)(value)> cleanups;
static ptr root;
constexpr size field_size = sizeof(value) + sizeof(type);

ptr alloc() {
ptr const p = new cell{ };
p->n = 0;
p->c = 0;
p->p = 0;
range.push_back(p);
return p; }

void resize(ptr p, size n) {
cell const old = *p;
size const nmax = std::max(n, old.n);
p->n = n;
p->c = n;
p->p = reinterpret_cast<char *>(malloc(p->c * field_size));
if (old.p) {
  memcpy(p->p, old.p, nmax * sizeof(value));
  memcpy(p->p + p->c * sizeof(value), old.p + old.c * sizeof(value), nmax * sizeof(type));
  free(old.p); } }

static void grow(ptr p) {
cell const old = *p;
p->c = p->c ? 2 * p->c : 2;
p->p = reinterpret_cast<char *>(malloc(p->c * field_size));
if (old.p) {
  memcpy(p->p, old.p, p->n * sizeof(value));
  memcpy(p->p + p->c * sizeof(value), old.p + old.c * sizeof(value), p->n * sizeof(type));
  free(old.p); } }

size get_size(ptr p) {
return p->n; }

void push_field(ptr p, type t, value v) {
if (p->c == p->n) {
  grow(p); }
set_field(p, p->n, t, v);
p->n++; }

void pop_field(ptr p) {
p->n--; }

void set_field(ptr p, size i, type t, value v) {
  set_type(p, i, t);
  set_value(p, i, v); }

void set_type(ptr p, size i, type t) {
if (t > 2 && t != (type)-1) {
  std::cerr << "hi";
}
*reinterpret_cast<type *>(p->p + p->c * sizeof(value) + i * sizeof(type)) = t; }

void set_value(ptr p, size i, value v) {
*reinterpret_cast<size_t *>(p->p + i * sizeof(value)) = v; }

type get_type(ptr p, size i) {
return *reinterpret_cast<type *>(p->p + p->c * sizeof(value) + i * sizeof(type)); }

value get_value(ptr p, size_t i) {
return *reinterpret_cast<size_t *>(p->p + i * sizeof(size_t)); }

void set_root(ptr p) {
root = p; }

void set_cleanup(size type, void (*f)(value)) {
if (cleanups.size() < type + 1) {
  cleanups.resize(type + 1); }
cleanups[type] = f; }

void cycle() {
std::unordered_set<ptr> s;
s.max_load_factor(0.5);
if (root) {
  std::stack<ptr> stack;
  stack.push(root);
  while (!stack.empty()) {
    ptr p = stack.top();
    stack.pop();
    if (s.find(p) != s.end()) {
      continue; }
    s.insert(p);
    for (size i = 0; i < p->n; i++) {
      if (get_type(p, i) == (type)-1) {
        value const v = get_value(p, i);
        if (v) {
          stack.push(reinterpret_cast<ptr>(v)); } } } } }
const auto pivot = std::partition(range.begin(), range.end(), [&s](ptr p) -> bool {
  auto const it = s.find(p);
  if (it == s.end()) {
    for (size i = 0; i < p->n; i++) {
      type const t = get_type(p, i);
      if (t != (type)-1) {
        cleanups[t](get_value(p, i)); } }
      if (p->p) {
        free(p->p); }
      delete p;
    return false; }
  else {
    return true; } });
range.erase(pivot, range.end()); }

static size const min_trigger = 16;
static size trigger = min_trigger;

void help() {
if (range.size() > trigger) {
  cycle();
  if (range.size() > trigger / 2) {
    trigger *= 2; }
  else if (range.size() < trigger / 4 && trigger > min_trigger) {
    trigger /= 2; } } }

}