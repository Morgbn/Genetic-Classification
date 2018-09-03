#include "main.h"

int main(int argc, char const *argv[]) {
  char error[255];
  if (argc < 2) {
    sprintf(error, help_usage_long, argv[0]);
    usage(error);
  }
  srand(time(NULL));

  /**
   * VARIABLES GLOBALES (valeurs par défaut) :
   * @global TF_IDF         utiliser la méthode de pondération TF-IDF
   * @global LUHN_ON_ALL    utiliser la conjecture de Luhn sur le corpus
   * @global LUHN_ON_EACH   utiliser la conjecture de Luhn sur chq doc
   * @global USE_EUCL_DIST  utiliser la distance euclidienne
   * @global verboseLemm    afficher le résultat de la lemmatisation
   * @global verboseGa      afficher le report de l'algorithme génétique
   * @global verboseDist    afficher la distance entre les documents
   * @global multPop0       multiplicateur pour la 1er population
   * @global MaxGen         nombre de génération
   * @global PopSize        taille de la population
   * @global minK           nombre minimal de groupe à créer
   * @global maxK           nombre maximal de groupe à créer
   */
  TF_IDF = 0;
  LUHN_ON_ALL = LUHN_ON_EACH = USE_EUCL_DIST = 0;
  verboseLemm = verboseGa = verboseDist = 0;
  multPop0 = 4;
  MaxGen = 1000, PopSize = 100;
  minK = maxK = -1;                           // réglé plus tard

  for (int i = 1; i < argc;)                  // traiter toutes les options
    i += changeOption(i, argv, argc);

  if (minK > maxK) usage("L'option --min prend comme argument un nombre inférieur à l'argument de --max.");

  double minDist, maxDist;

  int nDoc = 0;
  doc * docs = getData(argv[1], &nDoc);
  for (int i = 0; i < nDoc; i++) {            // calculer distance entre chq doc
    docs[i].dist = (double *) malloc((nDoc) * sizeof(double *));
    if (docs[i].dist == NULL) usage("error malloc in ...");

    docs[i].dist[i] = 0;                      // distance a<->a = 0
    for (int j = 0; j != i; j++) {
      if (USE_EUCL_DIST) {
        docs[i].dist[j]                       // distance a<->b
          = docs[j].dist[i]                   // égal distance b<->a
          = distBtwDoc(docs[i].terms, docs[j].terms);
      }
      else {
        docs[i].dist[j]
          = docs[j].dist[i]
          = 1 - cosineSimilarity(docs[i].terms, docs[j].terms); // cos = 1 si similaires
      }
      if (!i || docs[i].dist[j] > maxDist) maxDist = docs[i].dist[j];
      if (!i || docs[i].dist[j] < minDist) minDist = docs[i].dist[j];
    }
  }
  for (int i = 0; i < nDoc; i++)  // normaliser les distances
    for (int j = 0; j != i; j++) {
      docs[i].dist[j]
        = docs[j].dist[i]
        = (docs[i].dist[j] - minDist) / (maxDist - minDist);
      if (verboseDist) printf("d(%s,%s)= %.3g\n", docs[i].name, docs[j].name, docs[i].dist[j]);
    }

  if (maxK == -1) maxK = (nDoc > 5) ? nDoc / 3 : 2; // par défaut
  if (minK == -1) minK = 2;

  int nCluster;
  doc *** clusteredDoc = GA(docs, nDoc, &nCluster, minK, maxK);

  for (int i = 0; i < nCluster; i++) {
    printf("Cluster n%i:\n", i);
    for (int j = 0; clusteredDoc[i][j]; j++)
      printf("\t%s\n", clusteredDoc[i][j]->name);
  }

  for (int i = 0; i < nCluster; i++) free(clusteredDoc[i]);
  free(clusteredDoc);

  return 0;
}

int changeOption(int i, const char *argv[], int argc) {
  if (!strcmp(argv[i], "--tfidf")) {          // utiliser la méthode TF-IDF
    TF_IDF = 1;
  }
  else if (!strcmp(argv[i], "--luhn")) {      // utiliser la conjecture de Luhn ?
    if (i+1 >= argc) usage("L'option --luhn prend un argument {0|1|2}");
    switch (atoi(argv[i+1])) {                // si LUHN_ON_ALL = 1 USELUHN ignoré et vice-versa.
      case 0:  LUHN_ON_ALL = 0; LUHN_ON_EACH = 0; break;
      case 1:  LUHN_ON_ALL = 0; LUHN_ON_EACH = 1; break;
      case 2:  LUHN_ON_ALL = 1; LUHN_ON_EACH = 0; break;
      default: usage("L'option --luhn prend comme argument {0|1|2}.");
    }
    return 2;
  }
  else if (!strcmp(argv[i], "--eucl")) {      // utiliser distance euclidienne
    USE_EUCL_DIST = 1;
  }
  else if (!strcmp(argv[i], "--cos")) {       // utiliser la similarité cosinus
    USE_EUCL_DIST = 0;
  }
  else if (!strcmp(argv[i], "--min")) {
    if (i+1 >= argc) usage("L'option --min un nombre comme argument");
    minK = atoi(argv[i+1]);
    if (minK < 2) usage("L'option --min prend comme argument un nombre compris entre 2 (compris) et l'infini.");
    return 2;
  }
  else if (!strcmp(argv[i], "--max")) {
    if (i+1 >= argc) usage("L'option --max un nombre comme argument");
    maxK = atoi(argv[i+1]);
    if (maxK < 2) usage("L'option --max prend comme argument un nombre compris entre 2 (compris) et l'infini.");
    return 2;
  }
  else if (!strcmp(argv[i], "--mul")) {
    if (i+1 >= argc) usage("L'option --mul un nombre comme argument");
    multPop0 = atoi(argv[i+1]);
    if (multPop0 < 1) usage("L'option --mul prend comme argument un nombre compris entre 1 (compris) et l'infini.");
    return 2;
  }
  else if (!strcmp(argv[i], "--gen")) {
    if (i+1 >= argc) usage("L'option --gen un nombre comme argument");
    MaxGen = atoi(argv[i+1]);
    if (MaxGen < 1) usage("L'option --gen prend comme argument un nombre compris entre 1 (compris) et l'infini.");
    return 2;
  }
  else if (!strcmp(argv[i], "--pop")) {
    if (i+1 >= argc) usage("L'option --pop un nombre comme argument");
    PopSize = atoi(argv[i+1]);
    if (PopSize < 2 || PopSize%2)
      usage("L'option --pop prend comme argument un nombre pair compris entre 2 (compris) et l'infini.");
    return 2;
  }
  else if (!strcmp(argv[i], "--printGA")) {
    verboseGa = 1;
    return 1;
  }
  else if (!strcmp(argv[i], "--printLemm")) {
    verboseLemm = 1;
    return 1;
  }
  else if (!strcmp(argv[i], "--printDist")) {
    verboseDist = 1;
    return 1;
  }
  else if (!strcmp(argv[i], "--help")         // besoin d'aide ?
        || !strcmp(argv[i], "-h")
        || !strcmp(argv[i], "-?")) {
    printf(help_usage_short, argv[0]);
    for (int i = 0; help[i]; i++) printf("%s", help[i]);
    exit(0);
  }
  else if (!strcmp(argv[i], "--usage")) {
    printf(help_usage_long, argv[0]);
    exit(0);
  }
  else if (i > 1) { // si dossier passé, c'est une option... non reconnue
    fprintf(stderr, "%s : option non reconnue « %s »\n", argv[0], argv[i]);
    fprintf(stderr, "Saisissez « %s --help » pour plus d'informations.\n", argv[0]);
    exit(2);
  }
  return 1;
}
