#include "utilities.h"

void usage(const char * error) {
  fprintf(stderr, "%s\n", error); // affiche le message
  exit(1);                        // quitter
}
