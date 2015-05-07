#ifndef DIRTREE_H
#define DIRTREE_H

#include <stdbool.h>
#include <stdint.h>

// methods
void dirtree_init(void);
void dirtree_show(uint8_t);
void dirtree_move_up(void);
void dirtree_move_down(void);
void dirtree_expand(void);
void dirtree_contract(void);
bool dirtree_showing(void);

// all important selection event (takes back-slash delimitated file name)
void dirtree_set_select_func(void (*select_func)(const char*));

#endif
