#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];

  if (argc < 4) {
    sprintf(error, "usage: %s <nom du dossier> <nom du dossier contenant les catégories> <nombre de catégorie>", argv[0]);
    usage(error);
  }

  int nCat = 0, nDoc = 0;
  doc * cats = getData(argv[2], &nCat, 0);
  doc * docs = getData(argv[1], &nDoc, 1);
  for (int i = 0; i < nDoc; i++) {                      // calculer distance pr chq doc
    docs[i].dist = (double *) malloc(nCat * sizeof(double *));
    if (docs[i].dist == NULL) usage("error malloc in ...");

    for (int j = 0; j < nCat; j++) {                    // avec chq catégorie
      docs[i].dist[cats[j].id] = distBtwDoc(docs[i].terms, cats[j].terms);
      printf("D(%s, %s) = %g\n", docs[i].name, cats[j].name, docs[i].dist[cats[j].id]);
    }
  }
  return 0;
}
