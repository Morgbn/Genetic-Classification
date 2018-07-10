#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];

  if (argc < 4) {
    sprintf(error, "usage: %s <nom du dossier> <nombre de catégorie> <nom du fichier contenant les catégories", argv[0]);
    usage(error);
  }

  int nDoc = 0;
  doc * data = getData(argv[1], &nDoc);
  // int nWord = 0;
  for (int i = 0; i < nDoc; i++) {
    printf("%s %i\n", data[i].name, nNodeSupTo(data[i].terms, 0));
    // nWord += nNodeSupTo(data[i].terms, 0);
  }
  // printf("\nNombre de mot au total dans les %i documents : %i\n", nDoc, nWord);

  return 0;
}
