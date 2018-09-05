#include "tree.h"

treeList initTree() {
  return newNode('+', NULL);
}

treeList newNode(char c, void *val) {
  treeList aNode = (treeList) malloc(sizeof(struct node));
  if (aNode == NULL) usage("error malloc in newNode");
  aNode->c = c;
  aNode->val = val;
  aNode->nChilds = 0;
  aNode->childs = NULL;
  aNode->parent = NULL;
  return aNode;
}

treeList appendNode(treeList tree, char c, void *val) {
  treeList child = newNode(c, val);
  child->parent = tree;

  tree->nChilds += 1;
  tree->childs = (struct node **) realloc(tree->childs, tree->nChilds * sizeof(struct node));
  if (tree->childs == NULL) usage("error realloc in appendNode");
  tree->childs[tree->nChilds-1] = child;
  return child;
}

void displayNodes(treeList aNode, int ret, type t) {
  if (aNode == NULL) return;

  if (aNode->val != NULL) {   // val non nul
    printf("(%c ", aNode->c); // → afficher
    switch (t) {
      case Char:   printf("%c", *(char *)  aNode->val); break;
      case String: printf("\"%s\"",  (char *)  aNode->val); break;
      case Float:  printf("%g", *(float *) aNode->val); break;
      case Int:    printf("%i", *(int *)   aNode->val); break;
    }
    printf(" %s", aNode->nChilds ? " " : "");
  }
  else printf("(%c%s", aNode->c, aNode->nChilds ? " " : ""); // val nul→ne pas afficher

  for (int i = 0; i < aNode->nChilds; i++) // afficher la suite (les enfants)
    displayNodes(aNode->childs[i], 0, t);
  printf(")%s", ret ? "\n" : " "); // fermer liste
}

char * getTreePath(treeList aNode, int skipFirst) {
  if (aNode == NULL) return NULL;
  char path[128];
  int len = 0;
  path[len++] = aNode->c;
  treeList pnode = aNode;
  while ((pnode = pnode->parent)) {
    path[len++] = pnode->c;
  }
  if (skipFirst) --len; // passer la racine
  path[len] = '\0';
  char tmp;
  for (int i = 0; i < len / 2; i++) { // reverse
    tmp = path[i];
    path[i] = path[len - i - 1];
    path[len - i - 1] = tmp;
  }
  return strdup(path);
}

void displayTreePath(treeList aNode, int ret) {
  if (aNode == NULL) return;
  printf("%s", getTreePath(aNode, 0));
  if (ret) printf("\n");
}

void displayVals(treeList head, type t) {
  if (head == NULL) return;
  if (head->val) {
    switch (t) {
      case Char:   printf("%c\n", *(char *)  head->val); break;
      case String: printf("\"%s\"\n",  (char *)  head->val); break;
      case Float:  printf("%g\n", *(float *) head->val); break;
      case Int:    printf("%i\n", *(int *)   head->val); break;
    }
  }
  for (int i = 0; i < head->nChilds; i++) // afficher la suite (les enfants)
    displayVals(head->childs[i], t);
}

void displayPaths(treeList head) {
  if (head == NULL) return;
  if (head->val) {
    char * path = getTreePath(head, 1);
    if (path != NULL) printf("%s\n", path);
    free(path);
  }
  for (int i = 0; i < head->nChilds; i++) // afficher la suite (les enfants)
    displayPaths(head->childs[i]);
}

void displayPathsVals(treeList head, type t) {
  if (head == NULL) return;
  if (head->val) {
    char * path = getTreePath(head, 1);
    if (path != NULL) {
      printf("%s: ", path);
      switch (t) {
        case Char:   printf("%c\n", *(char *)  head->val); break;
        case String: printf("\"%s\"\n",  (char *)  head->val); break;
        case Float:  printf("%g\n", *(float *) head->val); break;
        case Int:    printf("%i\n", *(int *)   head->val); break;
      }
    }
    free(path);
  }
  for (int i = 0; i < head->nChilds; i++) // afficher la suite (les enfants)
    displayPathsVals(head->childs[i], t);
}

void addToTree(treeList head, char *str, void *val, int freeVal) {
  int finded = 0, j;
  for (j = 0; j < head->nChilds && !finded; j++) { // parcours les enfants (max 26 enfants)
    if (head->childs[j]->c == *str) finded = 1;
  }
  if (finded) {
    if (str[1] == '\0') {                     // valeur déjà présente
      if (freeVal) {
        free(head->childs[j-1]->val);
        head->childs[j-1]->val = NULL;
      }
      head->childs[j-1]->val = val;           // l'écraser
    } else {                                  // char existe déjà→continuer
      addToTree(head->childs[j-1], str+1, val, freeVal);
    }
  } else {
    head = appendNode(head, *str, NULL);      // créer un noeud avec le char absent
    if (str[1] == '\0') head->val = val;      // fin du mot, attacher la valeur
    else addToTree(head, str+1, val, freeVal);// sinon continuer sur nouveau noeud
  }
}

treeList getNode(treeList head, char *name) {
  if (*name == '\0') return head; // fin du nom
  for (int j = 0; j < head->nChilds; j++) {
    if (head->childs[j]->c == *name) // trouvé
      return getNode(head->childs[j], name+1);
  }
  return NULL; // pas trouvé
}

int nLeaf(treeList head) {
  int n = 0;
  if (head->val != NULL) n++;
  for (int j = 0; j < head->nChilds; j++)
    n += nLeaf(head->childs[j]);
  return n;
}

void freeNode(treeList aNode, int freeVal) {
  for (int i = 0; i < aNode->nChilds; i++)
    freeNode(aNode->childs[i], freeVal);
  free(aNode->childs);
  aNode->childs = NULL;

  if (freeVal) {
    free(aNode->val);
    aNode->val = NULL;
  }

  free(aNode);
  aNode = NULL;
}

void delNode(treeList aNode, int freeVal) {
  if (aNode == NULL) return;
  // decaler la node à la fin des enfants du parent
  treeList parent = aNode->parent;
  if (parent != NULL) {
    int j = 0;
    for (int i = 0; i < parent->nChilds-1; i++) {
      if (parent->childs[i]->c == aNode->c) j = 1;
      parent->childs[i] = parent->childs[i+j]; // → décaler les enfants
    }
    parent->childs[parent->nChilds-1] = NULL;  // au cas ou
    parent->nChilds--;
  }
  // nettoyer
  freeNode(aNode, freeVal);
}

void cleanTree(treeList head) {
  for (int j = 0; j < head->nChilds; j++)
    cleanTree(head->childs[j]);

  if (head->nChilds == 0 && head->val == NULL) { // node inutile
    treeList parent = head->parent;
    if (parent == NULL) return;                 // ne pas supprimer racine de l'abre !
    int j = 0;
    for (int i = 0; i < parent->nChilds-1; i++) {
      if (parent->childs[i]->c == head->c) j = 1;
      parent->childs[i] = parent->childs[i+j]; // → décaler les enfants
    }
    parent->childs[--parent->nChilds] = NULL;  // au cas ou
    free(head);
  }
}

#if TEST
int main(int argc, char const *argv[]) {
  treeList a = initTree();
  float arr[] = {0, 1.1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  appendNode(a, 'a', &arr[1]);
  appendNode(a, 'b', &arr[2]);
  appendNode(a, 'c', &arr[3]);
  appendNode(a->childs[0], 'c', &arr[4]);
  addToTree(a, (char *) "abcd", &arr[5], 0);
  addToTree(a, (char *) "ok", &arr[6], 0);
  addToTree(a, (char *) "abce", &arr[7], 0);
  addToTree(a, (char *) "azb", &arr[8], 0);
  addToTree(a, (char *) "ace", &arr[9], 0);
  addToTree(a, (char *) "ace", &arr[10], 0);
  displayNodes(a, 1, Float);

  float * ptr = malloc(1*sizeof(float));
  if (ptr == NULL) usage("error malloc ptr");
  *ptr = 123;
  addToTree(a, (char *) "ptr", ptr, 1);
  displayNodes(a, 1, Float);
  delNode(getNode(a, "ptr"), 1);
  displayNodes(a, 1, Float);
  printf("\n");


  treeList b = getNode(a, (char *) "abce");
  displayNodes(b, 1, Float);

  treeList c = initTree();
  addToTree(c, (char *) "ok", (void *) "dacc", 1);
  addToTree(c, (char *) "op", (void *) "sudo", 1);
  addToTree(c, (char *) "dacc", (void *) "del2", 1);
  addToTree(c, (char *) "dac", (void *) "del1", 1);
  addToTree(c, (char *) "daz", (void *) "apres", 1);
  displayNodes(c, 1, String);
  treeList aNode = getNode(c, "dac");
  displayNodes(aNode, 1, String);
  delNode(aNode, 0);
  displayNodes(c, 1, String);

  delNode(getNode(c, "dacc"), 0);
  displayNodes(c, 1, String);

  addToTree(c, (char *) "opppp", (void *) "del3", 1);
  displayNodes(c, 1, String);
  delNode(getNode(c, "opppp"), 0);
  displayNodes(c, 1, String);

  aNode = getNode(c, "daz");
  char * path = getTreePath(aNode, 1);
  if (path != NULL) printf("%s\n", path);
  free(path);

  displayVals(c, String);
  displayPaths(c);
  displayPathsVals(a, Float);

  freeNode(a, 0);
  freeNode(c, 0);

  return 0;
}
#endif
