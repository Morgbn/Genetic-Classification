#include "core.h"

const float threshold = .01;

doc * getData(const char * path, int * len, int toLuhn) {
  struct dirent *file;
  DIR *dr = opendir(path);
  if (dr == NULL) usage("Problème avec le dossier.");

  treeList stopList = initTree();
  treeList wordList = initTree();
  initLemma("resources/stop.txt", stopList, "resources/verb.txt", wordList);

  int nDoc = 0;
  doc *docList = NULL;
  while ((file = readdir(dr)) != NULL) {
    if ( file->d_type != DT_REG ) continue;  // dossier → passer
    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);

    int nTerm, sumF = 0; // nb de terme, somme des fréquences des mots
    char **terms = readFile(pathname, &nTerm, 0);

    lemmatisation(terms, nTerm, stopList, wordList); // lemmatiser tt les mots

    treeList node, aDoc = initTree();
    printf("%s: %i termes\n", pathname, nTerm);

    for (int i = 0; i < nTerm; i++) {           // pour chq terme
      if (terms[i] == NULL) {                   // terme supprimé
        free(terms[i]);
        continue;
      }
      if ((node = getNode(aDoc, terms[i]))) { // chemin existe vers ce terme ?
        if (!node->val)                       // chemin existe ms pas de valeur attachée
          node->val = pFloat(0);              // y attacher 0
        (*(float *) node->val)++;             // ajouter une occurrence
      }
      else                                    // pas de chemin vers le terme ?
        addToTree(aDoc, terms[i], pFloat(1)); // l'ajouter
      sumF++;
    }
    free(terms);
    nTerm = nLeaf(aDoc);
    divideAllTreeBy(aDoc, nTerm);             // ÷ le nb d'occurrence par le nb de terme

    if (toLuhn) {
      float fAv = sumF / nTerm;               // fréquence moyenne
      float min = fAv-fAv*threshold;          // seuil +- 25% de la moyenne
      float max = fAv+fAv*threshold;
      applyLuhn(aDoc, min, max);              // application de la conjecture de Luhn
    }

    addDoc(&docList, nDoc++, pathname, aDoc); // l'ajouter aux données
  }

  closedir(dr);
  *len = nDoc;
  return docList;
}

void divideAllTreeBy(treeList tree, int n) {
  if (tree->val) *(float *) tree->val /= n;
  for (int i = 0; i < tree->nChilds; i++)
    divideAllTreeBy(tree->childs[i], n);
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

double distBtwDoc_auxA(treeList a, treeList b) {
  double dist = 0;
  if (a->val) {
    treeList el = getNode(b, getTreePath(a, 1)); // chercher le même élément dans b
    if (!el || !el->val) dist = (double) (*(float *) a->val); // freq du mot ds b = 0
    else dist = (double) ((*(float *) a->val) - (*(float *) el->val));
    dist *= dist; // (a-b)²
  }
  for (int i = 0; i < a->nChilds; i++)
    dist += distBtwDoc_auxA(a->childs[i], b);
  return dist;
}

double distBtwDoc_auxB(treeList a, treeList b) {
  double dist = 0;
  if (b->val) {
    treeList el = getNode(a, getTreePath(b, 1));  // chercher le même élément dans a
    if (el) dist = 0; // déjà traité dans distBtwDoc_auxA
    else dist = (double) (*(float *) b->val); // (-b)² = b²
    dist *= dist;
  }
  for (int i = 0; i < b->nChilds; i++)
    dist += distBtwDoc_auxB(b->childs[i], a);
  return dist;
}

double distBtwDoc(treeList a, treeList b) {
  return distBtwDoc_auxA(a, b) + distBtwDoc_auxB(a, b);
}
