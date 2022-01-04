#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

nptr
create_node (const char* w, const int len) {
  nptr newNode = (nptr) malloc (sizeof (linked_list_node) );
  newNode->next = 0;
  newNode->data = w;
  newNode->len = len;
  return newNode;
}

char*
copy_data (char* buf, nptr node, int max_len) {
  if (node == 0) {
    return 0;
  }

  if (node->len == 0) {
    return 0;
  }

  int len = max_len - 1;
  int ix = 0;

  if (node->len < len) {
    len = node->len;
  }

  for (; ix < len; ix++) {
    buf[ix] = node->data[ix];
  }

  for (; ix < max_len; ix++) {
    buf[ix] = 0;
  }

  return buf;
}

int
valid_path (nptr node) {
  char buf[PATH_MAX];
  char* path = copy_data (buf, node, PATH_MAX);

  if (path == 0) {
    return 0;
  }

  if (path[0] != '/') {
    return 0;
  }

  struct stat sbuf;

  if (0 == stat (buf, &sbuf) ) {
    switch (sbuf.st_mode & S_IFMT) {
    case S_IFDIR:
      return 1;
    }
  }

  return 0;
}

void
print_node (nptr node) {
  if (node == 0) {
    fprintf (stderr, "<nil>\n");
    return;
  }

  for (int ix = 0; ix < node->len; ix++) {
    fprintf (stdout, "%c", node->data[ix]);
  }
}

int
compare_node (nptr lhs, nptr rhs) {
  if (lhs == rhs) {
    return 0;
  }

  if (lhs == 0) {
    return -1;
  }

  if (rhs == 0) {
    return 1;
  }

  int len = 0;

  if (lhs->len < rhs->len) {
    len = lhs->len;
  } else {
    len = rhs->len;
  }

  for (int ix = 0; ix < len; ix++) {
    if (lhs->data[ix] == rhs->data[ix]) {
      continue;
    }

    if (lhs->data[ix] < rhs->data[ix]) {
      return -1;
    } else {
      return 1;
    }
  }

  if (lhs->len == rhs->len) {
    return 0;
  }

  if (lhs->len < rhs->len) {
    return -1;
  }

  return 1;
}

int
compare_data (nptr lhs, const char* w, const int len) {
  nptr t = create_node (w, len);
  int c = compare_node (lhs, t);
  free (t);
  return c;
}

nptr
add_node (nptr node, const char* w, const int len) {
  nptr nn = create_node (w, len);

  while (node != 0) {
    if (0 == compare_node (node, nn) ) {
      free (nn);
      return node;
    }

    if (node->next == 0) {
      node->next = nn;
      break;
    }

    node = node->next;
  }

  return nn;
}

