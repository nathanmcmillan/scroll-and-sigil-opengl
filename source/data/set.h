#ifndef SET_H
#define SET_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include "core/mem.h"
#include "core/slice.h"

typedef struct set_item set_item;

struct set_item {
    unsigned long hash;
    void *key;
    set_item *next;
};

typedef struct set set;

struct set {
    bool (*equals_fn)(void *, void *);
    unsigned long (*hashcode_fn)(void *);
    unsigned int size;
    unsigned int bins;
    set_item **items;
};

bool set_string_equal(void *a, void *b);
unsigned long set_string_hashcode(void *key);

bool set_address_equal(void *a, void *b);
unsigned long set_address_hashcode(void *key);

set *create_set(bool (*equals_fn)(void *, void *), unsigned long (*hashcode_fn)(void *));

void set_add(set *self, void *key);
bool set_has(set *self, void *key);

void set_remove(set *self, void *key);
void set_clear(set *self);

bool set_is_empty(set *self);
bool set_not_empty(set *self);
unsigned int set_size(set *self);

void release_set(set *self);
void delete_set(set *self);

typedef struct set_iterator set_iterator;

struct set_iterator {
    set *pointer;
    unsigned int bin;
    set_item *item;
};

set_iterator create_set_iterator(set *self);
bool set_iterator_has_next(set_iterator *iter);
void *set_iterator_next(set_iterator *iter);

#endif
