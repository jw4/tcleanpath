#pragma once

#define PATH_MAX 4096

typedef struct linked_list_node* nptr;
typedef struct linked_list_node {
  nptr next;
  const char* data;
  int len;
} linked_list_node;

nptr create_node (const char* w, const int len);

char* copy_data (char* buf, nptr node, int max_len);

int valid_path (nptr node);

void print_node (nptr node);

int compare_node (nptr lhs, nptr rhs);

int compare_data (nptr lhs, const char* w, const int len);

nptr add_node (nptr node, const char* w, const int len);

