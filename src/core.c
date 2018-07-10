#include "core.h"

doc * getData(const char * path, int * len) {
  struct dirent *file;
  DIR *dr = opendir(path);
  if (dr == NULL) usage("Problème avec le dossier.");

  treeList stopList = initTree();
  initLemma("src/stop.txt", stopList);

  int nDoc = 0;
  doc *data = NULL;
  while ((file = readdir(dr)) != NULL) {
    if ( file->d_type != DT_REG ) continue;  // dossier → passer
    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);

    int nTerm;
    char **terms = readFile(pathname, &nTerm);
    lemmatisation(terms, nTerm, stopList);   // lemmatiser tt les mots

    treeList node, aDoc = initTree();
    for (int i = 0; i < nTerm; i++) {
      if (terms[i] == NULL) continue;        // terme supprimé
      if ((node = getNode(aDoc, terms[i])))  // déjà dedans
        node->val++;                         // → ajouter une occurrence
      else addToTree(aDoc, terms[i], 1, 0);  // sinon l'ajouter
    }
    divideAllTreeBy(aDoc, nTerm);            // ÷ le nb d'occurrence par le nb de terme
    addDoc(&data, &nDoc, pathname, aDoc);    // l'ajouter aux données
  }

  closedir(dr);
  *len = nDoc;
  return data;
}

void divideAllTreeBy(treeList tree, int n) {
  if (tree->val > 0) tree->val /= n;
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
