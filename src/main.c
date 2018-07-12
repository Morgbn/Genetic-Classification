#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];

  if (argc < 4) {
    sprintf(error, "usage: %s <nom du dossier> <nombre de catégorie> <nom du fichier contenant les catégories", argv[0]);
    usage(error);
  }

  int nDoc = 0;
  doc * data = getData(argv[1], &nDoc);
  int nWord = 0;
  for (int i = 0; i < nDoc; i++) {
    printf("%s %i\n", data[i].name, nLeaf(data[i].terms));
    nWord += nLeaf(data[i].terms);
  }
  printf("Nombre de mot au total dans les %i documents : %i\n", nDoc, nWord);

  return 0;
}
