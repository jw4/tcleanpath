#include "list.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
  nptr head = 0;
  char *sepflag = "-separator";
  int sepflaglen = 10;
  char separator = ':';
  int flag = 0;

  for (int ix = 1; ix < argc; ix++) {
    if ('-' == argv[ix][0]) {
      if (strlen(argv[ix]) != sepflaglen ||
          strncmp(argv[ix], sepflag, sepflaglen) != 0) {
        fprintf(stderr, "unknown flag %s\n", argv[ix]);
        return 255;
      }

      flag = 1;
      continue;
    }

    if (0 != flag) {
      if (0 != argv[ix][1]) {
        fprintf(stderr, "separator can only be one char, used '%s'\n",
                argv[ix]);
        return 254;
      }

      separator = argv[ix][0];
      flag = 0;
      continue;
    }

    nptr node = 0;
    char *cur = argv[ix];
    char *wp = argv[ix];
    int lx = 0;

    while (*cur != 0) {
      if (separator == *cur) {
        if (lx > 0) {
          node = add_node(head, wp, lx);
          if (node == 0) {
            fprintf(stderr, "Memory allocation failed\n");
            return 253;
          }

          if (head == 0) {
            head = node;
          }
        }

        wp = cur + 1;
        lx = 0;
      } else {
        lx++;
      }

      cur++;
    }

    if (lx > 0) {
      node = add_node(head, wp, lx);
      if (node == 0) {
        fprintf(stderr, "Memory allocation failed\n");
        return 253;
      }

      if (head == 0) {
        head = node;
      }
    }
  }

  // flag is set when a flag is encountered, and then cleared to zero when a
  // value for that flag is found.
  if (flag != 0) {
    fprintf(stderr, "-separator requires an argument\n");
    return 252;
  }

  int ct = 0;

  while (head != 0) {
    if (1 == valid_path(head)) {
      if (ct > 0) {
        fprintf(stdout, "%c", separator);
      }

      print_node(head);
      ct++;
    }

    head = head->next;
  }

  return 0;
}
