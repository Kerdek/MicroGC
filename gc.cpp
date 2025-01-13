#include "gc.hpp"

#include <algorithm>
#include <cstring>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace gc {

struct field {
size_t value;
int type; };

struct cell {
std::unordered_map<size_t, field> p; };

std::vector<cell *> full_range;
std::unordered_multiset<cell *> live_range = [](){
  std::unordered_multiset<cell *> t;
  t.max_load_factor(0.5);
  return t; }();

ptr::ptr() : i(0) {}

ptr::ptr(cell *i) : i(i) {
if (i) {
  live_range.insert(i); } }

ptr::ptr(ptr const &x) : i(x.i) {
if (i) {
  live_range.insert(i); } }

ptr::ptr(ptr &&x) : i(x.i) {
x.i = 0; }

ptr &ptr::operator=(ptr const &x) {
if (i) {
  live_range.erase(live_range.find(i)); }
i = x.i;
if (i) {
  live_range.insert(i); }
return *this; }

ptr &ptr::operator=(ptr &&x) {
if (i) {
  live_range.erase(live_range.find(i)); }
i = x.i;
x.i = 0;
return *this; }

ptr::~ptr() {
if (i) {
  live_range.erase(live_range.find(i)); } }

ptr const null(0);

std::vector<void (*)(size_t)> cleanups;

void set_cleanup(int type, void (*f)(size_t)) {
if (cleanups.size() < type) {
  cleanups.resize(type); }
cleanups[type - 1] = f; }

ptr alloc() {
cell *const p = new cell{ .p = std::unordered_map<size_t, field>(0) };
p->p.max_load_factor(0.5);
full_range.push_back(p);
return ptr(p); }

bool is_null(ptr const &p) {
return !p.i; }

bool has(ptr const &p, size_t f) {
return p.i->p.find(f) != p.i->p.end(); }

void clean(field fe) {
if (fe.type > 0) {
  cleanups[fe.type - 1](fe.value); } }

ptr get_stem(ptr const &p, size_t f) {
field &fe = p.i->p.at(f);
return ptr(reinterpret_cast<cell *>(fe.value)); }

void set_stem(ptr const &p, size_t f, ptr v) {
field &fe = p.i->p[f];
clean(fe);
fe.type = 0;
fe.value = reinterpret_cast<size_t>(v.i); }

size_t get_leaf(ptr const &p, size_t f) {
field &fe = p.i->p.at(f);
return fe.value; }

void set_leaf(ptr const &p, int t, size_t f, size_t v) {
field &fe = p.i->p[f];
clean(fe);
fe.type = t;
fe.value = v; }

void unset(ptr const &p, size_t f) {
std::unordered_map<size_t, field> &x = p.i->p;
auto const it = x.find(f);
if (it != x.end()) {
  x.erase(x.find(f)); } }

void mark(std::unordered_set<cell *> &s, cell *c) {
std::stack<cell *> stack;
stack.push(c);
while (!stack.empty()) {
  cell *current = stack.top();
  stack.pop();
  if (s.find(current) != s.end()) {
    continue; }
  s.insert(current);
  for (const auto &[k, fe] : current->p) {
    if (fe.type == 0 && fe.value) {
      stack.push(reinterpret_cast<cell *>(fe.value)); } } } }

void sweep(cell *c) {
for (const auto [f, fe] : c->p) {
  clean(fe); }
delete c; }

void cycle() {
std::unordered_set<cell *> s;
s.max_load_factor(0.5);
std::for_each(live_range.begin(), live_range.end(), [&s](cell *x) -> void {
  mark(s, x); });
const auto pivot = std::partition(full_range.begin(), full_range.end(), [&s](cell *x) -> bool {
return s.find(x) != s.end(); });
std::for_each(pivot, full_range.end(), sweep);
full_range.erase(pivot, full_range.end()); }

size_t const min_trigger = 16;
size_t trigger = min_trigger;

void help() {
if (full_range.size() > trigger) {
  cycle();
  if (full_range.size() > trigger / 2) {
    trigger *= 2; }
  else if (full_range.size() < trigger / 4 && trigger > min_trigger) {
    trigger /= 2; } } }

}