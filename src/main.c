#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];

  if (argc < 4) {
    sprintf(error, "usage: %s <nom du dossier> <nombre de catégorie> <nom du fichier contenant les catégories", argv[0]);
    usage(error);
  }

  int nDoc = 0;
  doc * data = getData(argv[1], &nDoc);
  for (int i = 0; i < nDoc; i++) {
    printf("%s\n", data[i].name);
    displayNodes(data[i].terms, 1);
  }

  return 0;
}
