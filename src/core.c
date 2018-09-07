#include "core.h"

const float thresholdAll  = .3;
const float thresholdEach = .2;

doc * getData(const char * path, int * len) {
  struct dirent *file;
  DIR *dr = opendir(path);
  char error[255];
  if (dr == NULL) {
    sprintf(error, "Un problème a été rencontré avec le dossier « %s ».", path);
    usage(error);
  }

  treeList stopList = initTree();
  treeList wordList = initTree();
  initLemma("resources/stop.txt", stopList, "resources/verb.txt", wordList);

  int nDoc = 0, nTerm = 0, sumO = 0;          // nb de doc, nb de terme, somme des occurrences
  doc *docList = NULL;                        // liste regroupant tt les documents

  treeList termInNDoc = initTree();           // termInNDoc[terme] = int * nb de doc ou present
  treeList occOfTerms = initTree();           // occOfTerms[terme] = int * nb_occurrence
  char ** *listTerms = NULL;
  int *maxO = NULL;                           // fréquence brute max de chq doc
  int *nTermIn = NULL;                        // nb de termes pr chq document
  int *nTermRead = NULL;
  char **filenames = NULL;                    // nom de chq document

  while ((file = readdir(dr)) != NULL) {      // lire les fichiers d'un dossier
    if ( file->d_type != DT_REG ) continue;   // fichier = dossier → passer

    maxO = (int *) realloc(maxO, (nDoc+1)*sizeof(int));
    if (maxO == NULL) usage("error realloc in getData");
    maxO[nDoc] = 0;

    nTermIn = (int *) realloc(nTermIn, (nDoc+1)*sizeof(int));
    if (nTermIn == NULL) usage("error realloc in getData");

    nTermRead = (int *) realloc(nTermRead, (nDoc+1)*sizeof(int));
    if (nTermRead == NULL) usage("error realloc in getData");

    filenames = (char **) realloc(filenames, (nDoc+1)*sizeof(char *));
    if (filenames == NULL) usage("error realloc in getData");

    listTerms = (char ***) realloc(listTerms, (nDoc+1)*sizeof(char **));
    if (listTerms == NULL) usage("error realloc in getData");

    char pathname[256];
    if (path[strlen(path)-1] == '/')          // besoin de rajouter '/' ?
      sprintf(pathname, "%s%s", path, file->d_name);
    else sprintf(pathname, "%s/%s", path, file->d_name);
    filenames[nDoc] = strdup(pathname);

    char **terms = readFile(pathname, &nTermIn[nDoc], 0);

    lemmatisation(terms, nTermIn[nDoc], stopList, wordList); // lemmatiser tt les mots
    listTerms[nDoc] = terms;

    treeList aDoc = initTree();
    nTermRead[nDoc] = nTermIn[nDoc];
    int nO = 0;
    if (!LUHN_ON_ALL) sumO = 0;               // reset somme
    for (int i = 0; i < nTermRead[nDoc]; i++) { // pour chq terme
      if (terms[i] == NULL) {                 // terme supprimé
        nTermIn[nDoc]--;
        continue;
      }
      if (LUHN_ON_ALL) {
        nO = addTermInTree(occOfTerms, terms[i], 1);
      } else {
        nO = addTermInTree(aDoc, terms[i], 1);// l'ajouter dans le doc
      }
      if (TF_IDF)                             // indiquer que le terme est present ds ce doc
        addTermInTree(termInNDoc, terms[i], (nO == 1));
      if (nO > maxO[nDoc]) maxO[nDoc] = nO;
      sumO++;
    }
    if (LUHN_ON_EACH) {
      int nbO = nLeaf(aDoc);                  // nombre d'occurrence
      divideAllTreeBy(aDoc, nTermIn[nDoc]);   // ÷ chq occurrence par le nb de terme
      float fAv = (float) sumO / nbO / nTermIn[nDoc]; // fréquence moyenne
      float min = fAv-fAv*thresholdEach;      // seuil +- 20% de la moyenne
      float max = fAv+fAv*thresholdEach;
      applyLuhn(aDoc, min, max);              // application de la conjecture de Luhn
    }

    if (!LUHN_ON_ALL) addDoc(&docList, nDoc, pathname, aDoc); // l'ajouter aux données
    nTerm += nTermIn[nDoc];
    nDoc++;
  }

  if (LUHN_ON_ALL) {
    int nbO = nLeaf(occOfTerms);              // nombre d'occurrence total
    divideAllTreeBy(occOfTerms, nTerm);       // ÷ chq occurrence par le nb de terme

    // appliquer de la conjecture de Luhn sur tt les termes
    float fAv = (float) sumO / nbO / nTerm;   // fréquence moyenne
    float min = fAv-fAv*thresholdAll;         // seuil +- 30% de la moyenne
    float max = fAv+fAv*thresholdAll;
    applyLuhn(occOfTerms, min, max);
    cleanTree(occOfTerms);

    for (int i = 0; i < nDoc; i++) {
      char ** terms = listTerms[i];
      treeList node, aDoc = initTree();       // creer un doc pr cette liste de terme
      for (int j = 0; j < nTermIn[i]; j++) {
        if (terms[j] == NULL) continue;       // terme supprimé
        if ((node = getNode(occOfTerms, terms[j])))
          if (node->val)                      // s'il y a une occurrence de ce terme
            addTermInTree(aDoc, terms[j], 1); // l'ajouter dans le doc
      }
      addDoc(&docList, i, filenames[i], aDoc);// ajouter aux données le doc
    }
  }

  if (TF_IDF) {
    for (int i = 0; i < nDoc; i++)
      tfidf(docList[i].terms, maxO[i], nDoc, termInNDoc);
  }
  if (verboseLemm)
    for (int i = 0; i < nDoc; i++) {
      printf("%s :\n", docList[i].name);
      displayPathsVals(docList[i].terms, Float);
      puts(" ");puts(" ");
    }

  freeNode(termInNDoc, 1);

  for (int i = 0; i < nDoc; i++) {
    for (int j = 0; j < nTermRead[i]; j++)
      free(listTerms[i][j]);
    free(listTerms[i]);
  }
  free(listTerms);

  freeNode(occOfTerms, 1);
  freeNode(stopList, 0);
  freeNode(wordList, 1);
  free(maxO);
  free(nTermIn);
  free(nTermRead);
  for (int i = 0; i < nDoc; i++) free(filenames[i]);
  free(filenames);

  closedir(dr);
  *len = nDoc;
  return docList;
}

int addTermInTree(treeList tree, char * term, int inc) {
  if (term == NULL) return 0;
  treeList node;
  if ((node = getNode(tree, term))) {         // chemin existe vers ce terme ?
    if (!node->val)                           // chemin existe ms pas de valeur attachée
      node->val = pFloat(1);                  // y attacher 1
    else if (inc) (*(float *) node->val)++;   // ajouter une occurrence
    return (int) (*(float *) node->val);
  }
  addToTree(tree, term, pFloat(1), 1);        // pas de chemin vers le terme ? → l'ajouter
  return 1;
}

void divideAllTreeBy(treeList tree, int n) {
  if (tree->val) *(float *) tree->val /= n;
  for (int i = 0; i < tree->nChilds; i++)
    divideAllTreeBy(tree->childs[i], n);
}

void tfidf(treeList tree, int max, int nDoc, treeList termInNDoc) {
  if (tree->val) {
    float tf  = .5 + .5 * (*(float *) tree->val / max); // normalisation « 0.5 » par le max

    char * term = getTreePath(tree, 1);       // terme attaché à ce noeud
    treeList r = getNode(termInNDoc, term);   // chercher dans l'abre terme<->n de doc
    free(term);
    int inNdoc = (r && r->val) ? *(float *) r->val : 1;
    float idf = log10(nDoc / inNdoc);         // nb total de doc / nb de doc où le terme apparaît

    *(float *) tree->val = tf * idf;
  }
  for (int i = 0; i < tree->nChilds; i++)
    tfidf(tree->childs[i], max, nDoc, termInNDoc);
}

void addDoc(doc **docList, int len, char * name, treeList terms) {
  *docList = (doc *) realloc(*docList, (len+1) * sizeof(doc));
  if (docList == NULL) usage("error realloc in addDoc");

  (*docList)[len].name = strdup(name);
  (*docList)[len].id = len;
  (*docList)[len].terms = terms;
  (*docList)[len].dist = NULL;
}

float * pFloat(float f) {
  float * val = (float *) malloc (sizeof(float));
  if (val == NULL) usage("error malloc in getData");
  *val = f;
  return val;
}

double distBtwDoc(treeList a, treeList b) {
  if (!a && !b) return 0;

  float aVal = (a && a->val) ? *(float *) a->val : 0; // valeur de a
  float bVal = (b && b->val) ? *(float *) b->val : 0; // valeur de b
  double dist = (aVal - bVal);                        // (a-b)
  dist *= dist;                                       // au carré

  for (int k = 0; a && (k < a->nChilds); k++) {       // pr tt les enfants de a
    treeList childB = NULL;
    for (int j = 0; b && (j < b->nChilds); j++)       // trouver son correspondant
      if (b->childs[j]->c == a->childs[k]->c)         // dans b
        childB = b->childs[j];
    dist += distBtwDoc(a->childs[k], childB);         // descendre avec lui
  }

  for (int j = 0; b && (j < b->nChilds); j++) {       // pr tt les enfants de b
    int isIn = 0;
    for (int k = 0; a && (k < a->nChilds); k++)       // si enfant ds a
      if (b->childs[j]->c == a->childs[k]->c) {       // ne pas descendre avec lui
        isIn = 1; break;                              // (déjà fait avant)
      }
    if (!isIn) dist += distBtwDoc(NULL, b->childs[j]);// continuer seul
  }

  return dist;
}

void cosineSimilarity_aux(treeList a, treeList b, double *dot, double *denomA, double *denomB) {
  if (!a && !b) return;

  float aVal = (a && a->val) ? *(float *) a->val : 1; // valeur de a
  float bVal = (b && b->val) ? *(float *) b->val : 1; // valeur de b
  *dot += aVal * bVal;                                // (a*b)
  *denomA += aVal * aVal;                             // (a*a)
  *denomB += bVal * bVal;                             // (b*b)

  for (int k = 0; a && (k < a->nChilds); k++) {       // pr tt les enfants de a
    treeList childB = NULL;
    for (int j = 0; b && (j < b->nChilds); j++)       // trouver son correspondant
      if (b->childs[j]->c == a->childs[k]->c)         // dans b
        childB = b->childs[j];
    cosineSimilarity_aux(a->childs[k], childB, dot, denomA, denomB);
  }

  for (int j = 0; b && (j < b->nChilds); j++) {       // pr tt les enfants de b
    int isIn = 0;
    for (int k = 0; a && (k < a->nChilds); k++)       // si enfant ds a
      if (b->childs[j]->c == a->childs[k]->c) {       // ne pas descendre avec lui
        isIn = 1; break;                              // (déjà fait avant)
      }
    if (!isIn)                                        // continuer seul
      cosineSimilarity_aux(NULL, b->childs[j], dot, denomA, denomB);
  }
}

double cosineSimilarity(treeList a, treeList b) {
  double dotAB  = 0.0;
  double denomA = 0.0;
  double denomB = 0.0;
  cosineSimilarity_aux(a, b, &dotAB, &denomA, &denomB);

  return dotAB / sqrt(denomA * denomB);
}
