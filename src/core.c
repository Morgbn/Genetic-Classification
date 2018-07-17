#include "core.h"

const float threshold = .01;

doc * getData(const char * path, int * len, int toLemm) {
  struct dirent *file;
  DIR *dr = opendir(path);
  if (dr == NULL) usage("Problème avec le dossier.");

  treeList stopList = initTree();
  treeList wordList = initTree();
  initLemma("resources/stop.txt", stopList, "resources/verb.txt", wordList);

  int nDoc = 0;
  doc *data = NULL;
  while ((file = readdir(dr)) != NULL) {
    if ( file->d_type != DT_REG ) continue;  // dossier → passer
    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);

    int nTerm;
    char **terms = readFile(pathname, &nTerm, 0);

    if (toLemm)
      lemmatisation(terms, nTerm, stopList, wordList); // lemmatiser tt les mots

    treeList node, aDoc = initTree();
    for (int i = 0; i < nTerm; i++) {
      if (terms[i] == NULL) continue;         // terme supprimé
      if ((node = getNode(aDoc, terms[i]))) { // déjà dedans
        if (!node->val) node->val = floatToPfloat(1);
        else (*(float *) node->val)++;        // → ajouter une occurrence
      }
      else // sinon l'ajouter
        addToTree(aDoc, terms[i], floatToPfloat(1));
    }
    nTerm = nLeaf(aDoc);
    divideAllTreeBy(aDoc, nTerm);            // ÷ le nb d'occurrence par le nb de terme

    if (toLemm) {
      float sum = getSumFreq(aDoc);            // somme des fréquences des mots
      float fAv = sum / nTerm;                 // fréquence moyenne
      float min = fAv-fAv*threshold;           // seuil +- 25% de la moyenne
      float max = fAv+fAv*threshold;
      applyLuhn(aDoc, min, max);               // application de la conjecture de Luhn
    }

    addDoc(&data, &nDoc, pathname, aDoc);    // l'ajouter aux données
  }

  closedir(dr);
  *len = nDoc;
  return data;
}

void divideAllTreeBy(treeList tree, int n) {
  if (tree->val) *(float *) tree->val /= n;
  for (int i = 0; i < tree->nChilds; i++)
    divideAllTreeBy(tree->childs[i], n);
}

void addDoc(doc **data, int * len, char * name, treeList terms) {
  *len += 1;
  *data = (doc *) realloc(*data, *len * sizeof(doc));
  if (data == NULL) usage("error realloc in addDoc");
  // cutName = name;
  (*data)[*len-1].name = strdup(name);
  (*data)[*len-1].id = *len-1;
  (*data)[*len-1].terms = terms;
}

float * floatToPfloat(float f) {
  float * val = (float *) malloc (sizeof(float));
  if (val == NULL) usage("error malloc in getData");
  *val = f;
  return val;
}
