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
    if ( file->d_type != DT_REG ) continue;   // dossier → passer
    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);

    int nTerm, sumO = 0; // nb de terme, somme des occurrences
    char **terms = readFile(pathname, &nTerm, 0);

    lemmatisation(terms, nTerm, stopList, wordList); // lemmatiser tt les mots

    treeList node, aDoc = initTree();
    int nTermCpy = nTerm;
    for (int i = 0; i < nTermCpy; i++) {      // pour chq terme
      if (terms[i] == NULL) {                 // terme supprimé
        nTerm--;
        free(terms[i]);
        continue;
      }
      if ((node = getNode(aDoc, terms[i]))) { // chemin existe vers ce terme ?
        if (!node->val)                       // chemin existe ms pas de valeur attachée
          node->val = pFloat(1);              // y attacher 1
        else (*(float *) node->val)++;        // ajouter une occurrence
      }
      else                                    // pas de chemin vers le terme ?
        addToTree(aDoc, terms[i], pFloat(1)); // l'ajouter
      sumO++;
    }
    free(terms);                              // plus besoin de la liste de mots
    int nbO = nLeaf(aDoc);                    // nombre d'occurrence
    divideAllTreeBy(aDoc, nTerm);             // ÷ chq occurrence par le nb de terme

    if (toLuhn) {
      float fAv = (float) sumO / nbO / nTerm; // fréquence moyenne
      float min = fAv-fAv*threshold;          // seuil +- 10% de la moyenne
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
