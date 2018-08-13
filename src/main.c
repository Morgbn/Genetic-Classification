#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];
  if (argc < 2) {
    sprintf(error, "usage: %s <nom du dossier>", argv[0]);
    usage(error);
  }

  srand(time(NULL));

  int nDoc = 0;
  doc * docs = getData(argv[1], &nDoc);
  for (int i = 0; i < nDoc; i++) {        // calculer distance entre chq doc
    docs[i].dist = (double *) malloc((nDoc) * sizeof(double *));
    if (docs[i].dist == NULL) usage("error malloc in ...");

    docs[i].dist[i] = 0;                  // distance a<->a = 0
    for (int j = 0; j != i; j++) {
      docs[i].dist[j]                     // distance a<->b
        = docs[j].dist[i]                 // égal distance b<->a
        = distBtwDoc(docs[i].terms, docs[j].terms);
      printf("d(%s, %s) = %g\n", docs[i].name, docs[j].name, docs[i].dist[j]);
    }
  }

  int nCluster;
  doc *** clusteredDoc = GA(docs, nDoc, &nCluster);
  
  for (int i = 0; i < nCluster; i++) {
    printf("Cluster n°%i:\n", i);
    for (int j = 0; clusteredDoc[i][j]; j++)
      printf("\t%s\n", clusteredDoc[i][j]->name);
  }

  for (int i = 0; i < nCluster; i++) free(clusteredDoc[i]);
  free(clusteredDoc);

  return 0;
}
