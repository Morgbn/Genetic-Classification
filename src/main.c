#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];

  if (argc < 2) {
    sprintf(error, "usage: %s <nom du dossier>", argv[0]);
    usage(error);
  }

  int nDoc = 0;
  doc * docs = getData(argv[1], &nDoc, 1);
  for (int i = 0; i < nDoc; i++) {                    // calculer distance entre chq doc
    docs[i].dist = (double *) malloc((nDoc-1) * sizeof(double *));
    if (docs[i].dist == NULL) usage("error malloc in ...");

    for (int j = 0; j != i; j++) {
      docs[i].dist[docs[j].id]                        // distance a<->b
        = docs[j].dist[docs[i].id]                    // égal distance b<->a
        = distBtwDoc(docs[i].terms, docs[j].terms);
      printf("D(%s, %s) = %g\n", docs[i].name, docs[j].name, docs[i].dist[docs[j].id]);
    }
  }
  return 0;
}
