#pragma once

#define PATH_MAX 4096

typedef struct linked_list_node *nptr;
typedef struct linked_list_node {
  nptr next;
  const char *data;
  int len;
} linked_list_node;

nptr create_node(const char *w, const int len);

// Create node with normalized path (removes trailing slashes)
nptr create_node_normalized(const char *w, const int len);

char *copy_data(char *buf, nptr node, int max_len);

int valid_path(nptr node);

// Alternative validation with error reporting (not currently used)
// Returns: 1 for valid directory, 0 for invalid, -1 for stat error
int valid_path_verbose(nptr node);

void print_node(nptr node);

int compare_node(nptr lhs, nptr rhs);

int compare_data(nptr lhs, const char *w, const int len);

nptr add_node(nptr node, const char *w, const int len);
