#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  char *name;
  size_t stack_loc;
} Variable;

typedef struct {
  Variable *vars;
  size_t size;
  size_t capacity;
} VariableTable;

void var_table_init(VariableTable *vt);
void var_table_free(VariableTable *vt);
void var_table_append(VariableTable *vt, const char *name, size_t stack_size);
Variable *var_table_find(VariableTable *vt, const char *name);
bool var_table_contains(VariableTable *vt, const char *name);