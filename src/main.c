#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];

  if (argc < 4) {
    sprintf(error, "usage: %s <nom du dossier> <nom du dossier contenant les catégories> <nombre de catégorie>", argv[0]);
    usage(error);
  }

  int nCat = 0;
  doc * cats = getData(argv[2], &nCat, 0);
  for (int i = 0; i < nCat; i++) {
    printf("%s %i\n", cats[i].name, nLeaf(cats[i].terms));
    displayNodes(cats[i].terms, 1, Float);
  }
  int nDoc = 0;
  doc * data = getData(argv[1], &nDoc, 1);
  int nWord = 0;
  for (int i = 0; i < nDoc; i++) {
    printf("%s %i\n", data[i].name, nLeaf(data[i].terms));
    nWord += nLeaf(data[i].terms);
  }
  printf("Nombre de mot au total dans les %i documents : %i\n", nDoc, nWord);
  return 0;
}
