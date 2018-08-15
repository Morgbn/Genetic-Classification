#include "core.h"

const float threshold = .15;
int TF_IDF = 01; // incompatible avec LUHN_ON_ALL et USELUHN
int LUHN_ON_ALL = 0;
int LUHN_ON_EACH = 0; // si LUHN_ON_ALL = 1 USELUHN ignoré.

doc * getData(const char * path, int * len) {
  struct dirent *file;
  DIR *dr = opendir(path);
  if (dr == NULL) usage("Problème avec le dossier.");

  treeList stopList = initTree();
  treeList wordList = initTree();
  initLemma("resources/stop.txt", stopList, "resources/verb.txt", wordList);

  int nDoc = 0, nTerm = 0, sumO = 0;          // nb de doc, nb de terme, somme des occurrences
  doc *docList = NULL;                        // liste regroupant tt les documents
  int *maxO = NULL;                           // fréquence brute max de chq doc
  treeList termInNDoc = initTree();           // termInNDoc[terme] = int * nb de doc ou present
  treeList occOfTerms = initTree();           // occOfTerms[terme] = int * nb_occurrence
  treeList listTerms = initTree();            // listTerms[chemin] = char ** terms
  int *nTermIn = NULL;                        // nb de termes pr chq document
  char **filenames = NULL;                    // nom de chq document

  while ((file = readdir(dr)) != NULL) {      // lire les fichiers d'un dossier
    if ( file->d_type != DT_REG ) continue;   // fichier = dossier → passer

    maxO = (int *) realloc(maxO, (nDoc+1)*sizeof(int));
    if (maxO == NULL) usage("error realloc in getData");

    nTermIn = (int *) realloc(nTermIn, (nDoc+1)*sizeof(int));
    if (nTermIn == NULL) usage("error realloc in getData");

    if (LUHN_ON_ALL) {
      filenames = (char **) realloc(filenames, (nDoc+1)*sizeof(char *));
      if (filenames == NULL) usage("error realloc in getData");
    }

    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);
    if (LUHN_ON_ALL) filenames[nDoc] = strdup(pathname);

    char **terms = readFile(pathname, &nTermIn[nDoc], 0);
    if (LUHN_ON_ALL) addToTree(listTerms, pathname, terms);  // ajouter la liste de terme

    lemmatisation(terms, nTermIn[nDoc], stopList, wordList); // lemmatiser tt les mots

    treeList aDoc = initTree();
    int cpyNTerm = nTermIn[nDoc];
    int nO = 0;
    if (!LUHN_ON_ALL) sumO = 0;               // reset somme
    for (int i = 0; i < cpyNTerm; i++) {      // pour chq terme
      if (terms[i] == NULL) {                 // terme supprimé
        nTermIn[nDoc]--;
        free(terms[i]);
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
      float min = fAv-fAv*threshold;          // seuil +- 10% de la moyenne
      float max = fAv+fAv*threshold;
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
    float min = fAv-fAv*threshold;            // seuil +- 15% de la moyenne
    float max = fAv+fAv*threshold;
    applyLuhn(occOfTerms, min, max);
    cleanTree(occOfTerms);

    for (int i = 0; i < nDoc; i++) {
      treeList node = getNode(listTerms, filenames[i]);
      if (node == NULL) usage("error getNode in getData");

      char ** terms = node->val;
      treeList aDoc = initTree();             // creer un doc pr cette liste de terme
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
  // print
  for (int i = 0; i < nDoc; i++) {
    printf("%s :\n", docList[i].name);
    displayPathsVals(docList[i].terms, Float);
    puts(" ");puts(" ");
  }

  freeNode(termInNDoc, 1, 1);
  freeNode(listTerms, 1, 1);
  freeNode(occOfTerms, 1, 1);

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
  addToTree(tree, term, pFloat(1));           // // pas de chemin vers le terme ? → l'ajouter
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
