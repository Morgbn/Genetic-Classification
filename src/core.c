#include "core.h"

const float threshold = .15;

doc * getData(const char * path, int * len) {
  struct dirent *file;
  DIR *dr = opendir(path);
  if (dr == NULL) usage("Problème avec le dossier.");

  treeList stopList = initTree();
  treeList wordList = initTree();
  initLemma("resources/stop.txt", stopList, "resources/verb.txt", wordList);

  int nDoc = 0, nTerm = 0, sumO = 0;          // nb de doc, nb de terme, somme des occurrences
  treeList occOfTerms = initTree();           // occOfTerms[terme] = int * nb_occurrence
  treeList listTerms = initTree();            // listTerms[chemin] = char ** terms
  doc *docList = NULL;                        // liste regroupant tt les documents
  int *nTermIn = NULL;                        // nb de termes pr chq document
  char **filenames = NULL;                    // nom de chq document

  while ((file = readdir(dr)) != NULL) {      // lire les fichiers d'un dossier
    if ( file->d_type != DT_REG ) continue;   // fichier = dossier → passer

    nTermIn = (int *) realloc(nTermIn, (nDoc+1)*sizeof(int));
    if (nTermIn == NULL) usage("error realloc in getData");

    filenames = (char **) realloc(filenames, (nDoc+1)*sizeof(char *));
    if (filenames == NULL) usage("error realloc in getData");

    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);
    filenames[nDoc] = strdup(pathname);

    char **terms = readFile(pathname, &nTermIn[nDoc], 0);
    addToTree(listTerms, pathname, terms);    // ajouter la liste de terme

    lemmatisation(terms, nTermIn[nDoc], stopList, wordList); // lemmatiser tt les mots

    int cpyNTerm = nTermIn[nDoc];
    for (int i = 0; i < cpyNTerm; i++) {      // pour chq terme
      if (terms[i] == NULL) {                 // terme supprimé
        nTermIn[nDoc]--;
        free(terms[i]);
        continue;
      }
      addTermInTree(occOfTerms, terms[i]);    // ajouter le mot dans l'arbre
      sumO++;
    }
    nTerm += nTermIn[nDoc];
    nDoc++;
  }

  int nbO = nLeaf(occOfTerms);                // nombre d'occurrence total
  divideAllTreeBy(occOfTerms, nTerm);         // ÷ chq occurrence par le nb de terme

  // appliquer de la conjecture de Luhn sur tt les termes
  float fAv = (float) sumO / nbO / nTerm;     // fréquence moyenne
  float min = fAv-fAv*threshold;              // seuil +- 15% de la moyenne
  float max = fAv+fAv*threshold;
  applyLuhn(occOfTerms, min, max);
  cleanTree(occOfTerms);

  for (int i = 0; i < nDoc; i++) {
    treeList node = getNode(listTerms, filenames[i]);
    if (node == NULL) usage("error getNode in getData");

    char ** terms = node->val;
    treeList aDoc = initTree();               // creer un doc pr cette liste de terme
    for (int j = 0; j < nTermIn[i]; j++) {
      if (terms[j] == NULL) continue;         // terme supprimé
      if ((node = getNode(occOfTerms, terms[j]))) {
        if (node->val)                        // s'il y a une occurrence de ce terme
          addTermInTree(aDoc, terms[j]);      // l'ajouter dans le doc
      }
    }
    divideAllTreeBy(aDoc, nTermIn[i]);        // ÷ chq occurrence par le nb de terme
    addDoc(&docList, i, filenames[i], aDoc);  // ajouter aux données le doc
  }
  freeNode(listTerms, 1, 1);
  freeNode(occOfTerms, 1, 1);

  closedir(dr);
  *len = nDoc;
  return docList;
}

void addTermInTree(treeList tree, char * term) {
  if (term == NULL) return;
  treeList node;
  if ((node = getNode(tree, term))) {         // chemin existe vers ce terme ?
    if (!node->val)                           // chemin existe ms pas de valeur attachée
      node->val = pFloat(1);                  // y attacher 1
    else (*(float *) node->val)++;            // ajouter une occurrence
  }
  else                                        // pas de chemin vers le terme ?
    addToTree(tree, term, pFloat(1));         // l'ajouter
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
