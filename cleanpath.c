#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Process a path string and add valid paths to the list
int process_path_string(char *paths, char separator, nptr *head) {
  nptr node = 0;
  char *cur = paths;
  char *wp = paths;
  int lx = 0;

  while (*cur != 0) {
    if (separator == *cur) {
      if (lx > 0) {
        node = add_node(*head, wp, lx);

        if (node == 0) {
          fprintf(stderr, "Memory allocation failed\n");
          return 253;
        }

        if (*head == 0) {
          *head = node;
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
    node = add_node(*head, wp, lx);

    if (node == 0) {
      fprintf(stderr, "Memory allocation failed\n");
      return 253;
    }

    if (*head == 0) {
      *head = node;
    }
  }

  return 0;
}

void print_usage(const char *progname) {
  printf("Usage: %s [OPTIONS] [PATH_LIST...]\n\n", progname);
  printf("Clean PATH by removing invalid directories and duplicates while "
         "preserving order.\n\n");
  printf("OPTIONS:\n");
  printf("  -s, -separator, --separator CHAR\n");
  printf("                       Use CHAR as path separator (default: ':')\n");
  printf("  -h, -help, --help    Show this help message\n\n");
  printf("ARGUMENTS:\n");
  printf("  PATH_LIST            Colon-separated paths to clean\n");
  printf("                       If not provided, reads from $PATH "
         "environment\n\n");
  printf("EXAMPLES:\n");
  printf("  %s                          # Clean current $PATH\n", progname);
  printf("  %s /bin:/usr/bin            # Clean specific paths\n", progname);
  printf("  %s -s ';' 'C:\\bin;D:\\tools'  # Windows-style paths\n", progname);
  printf("\nOnly absolute directory paths that exist are kept in output.\n");
}

int main(int argc, char **argv) {
  nptr head = 0;
  char separator = ':';
  int has_path_args = 0;

  // First pass: process flags and check for help
  for (int ix = 1; ix < argc; ix++) {
    // Check for help flags
    if ((strlen(argv[ix]) == 2 && strncmp(argv[ix], "-h", 2) == 0) ||
        (strlen(argv[ix]) == 5 && strncmp(argv[ix], "-help", 5) == 0) ||
        (strlen(argv[ix]) == 6 && strncmp(argv[ix], "--help", 6) == 0)) {
      print_usage(argv[0]);
      return 0;
    }

    // Handle flags
    if ('-' == argv[ix][0]) {
      // Check for separator flags (-s, -separator, or --separator)
      if ((strlen(argv[ix]) == 2 && strncmp(argv[ix], "-s", 2) == 0) ||
          (strlen(argv[ix]) == 10 && strncmp(argv[ix], "-separator", 10) == 0) ||
          (strlen(argv[ix]) == 11 && strncmp(argv[ix], "--separator", 11) == 0)) {
        if (ix + 1 >= argc) {
          fprintf(stderr, "-separator requires an argument\n");
          return 252;
        }

        ix++; // Move to separator value

        if (0 != argv[ix][1]) {
          fprintf(stderr, "separator can only be one char, used '%s'\n",
                  argv[ix]);
          return 254;
        }

        separator = argv[ix][0];
        continue;
      }

      fprintf(stderr, "unknown flag %s\n", argv[ix]);
      fprintf(stderr, "Use -h or --help for usage information\n");
      return 255;
    }

    // Non-flag argument found
    has_path_args = 1;
  }

  // Second pass: process path arguments
  for (int ix = 1; ix < argc; ix++) {
    // Skip flags and their values
    if ('-' == argv[ix][0]) {
      if ((strlen(argv[ix]) == 2 && strncmp(argv[ix], "-s", 2) == 0) ||
          (strlen(argv[ix]) == 10 && strncmp(argv[ix], "-separator", 10) == 0) ||
          (strlen(argv[ix]) == 11 && strncmp(argv[ix], "--separator", 11) == 0)) {
        ix++; // Skip the separator value too
      }

      continue;
    }

    // Process path argument
    int result = process_path_string(argv[ix], separator, &head);

    if (result != 0) {
      return result;
    }
  }

  // If no path arguments provided, use $PATH environment variable
  if (!has_path_args) {
    char *env_path = getenv("PATH");

    if (env_path == 0) {
      fprintf(stderr, "No PATH arguments provided and $PATH environment "
              "variable not set\n");
      return 251;
    }

    int result = process_path_string(env_path, separator, &head);

    if (result != 0) {
      return result;
    }
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
