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

void displayTreePath(treeList aNode, int ret) {
  if (aNode == NULL) return;
  displayTreePath(aNode->parent, 0);
  printf("%c", aNode->c);
  if (ret) printf("\n");
}

void addToTree(treeList head, char *str, void *val) {
  int finded = 0, j;
  for (j = 0; j < head->nChilds && !finded; j++) { // parcours les enfants (max 26 enfants)
    if (head->childs[j]->c == *str) finded = 1;
  }
  if (finded) {
    if (str[1] == '\0') head->childs[j-1]->val = val; // écraser valeur
    else addToTree(head->childs[j-1], str+1, val); // char existe déjà→continuer
  }
  else {
    head = appendNode(head, *str, NULL); // créer un noeud avec le char absent
    if (str[1] == '\0') head->val = val; // fin du mot, attacher la valeur
    else addToTree(head, str+1, val);    // sinon continuer sur nouveau noeud
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

void freePathToNode(treeList aNode) {
  freeNodeParents(aNode);
  deleteNode(aNode, 1, 0);
}

void freeNodeParents(treeList child) {
  if (child->parent) {                  // si parent alloué
    if (child->parent->nChilds < 2) {   // si n'a pas d'autre enfant
      if (child->parent->val == NULL) { // si n'a pas de valeur attaché
        freeNodeParents(child->parent); // libérer aussi grand-parents
        deleteNode(child->parent, 1, 0);// libérer parents
      }
    } else {                            // si a d'autre enfant
      treeList p = child->parent;
      int j = 0;
      for (int i = 0; i < p->nChilds-1; i++) {
        if (p->childs[i]->c == child->c) j++; // chercher la position de l'enfant à supprimer
        p->childs[i] = p->childs[i+j];  // décaler les enfants
      }
    }
    child->parent->nChilds--;
  }
}

void deleteNode(treeList aNode, int delVal, int recursively) {
  if (delVal && aNode->val) free(aNode->val);
  if (recursively && aNode->nChilds > 0) { // supprimer les enfants
    for (int i = 0; i < aNode->nChilds; i++)
      deleteNode(aNode->childs[i], delVal, recursively);
    free(aNode->childs);
  }
  free(aNode);
}

#if TEST
int main(int argc, char const *argv[]) {
  treeList a = initTree();
  float arr[] = {0, 1.1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  appendNode(a, 'a', &arr[1]);
  appendNode(a, 'b', &arr[2]);
  appendNode(a, 'c', &arr[3]);
  appendNode(a->childs[0], 'c', &arr[4]);
  addToTree(a, (char *) "abcd", &arr[5]);
  addToTree(a, (char *) "ok", &arr[6]);
  addToTree(a, (char *) "abce", &arr[7]);
  addToTree(a, (char *) "azb", &arr[8]);
  addToTree(a, (char *) "ace", &arr[9]);
  addToTree(a, (char *) "ace", &arr[10]);
  displayNodes(a, 1, Float);

  treeList b = getNode(a, (char *) "abce");
  displayNodes(b, 1, Float);

  treeList c = initTree();
  addToTree(c, (char *) "ok", (void *) "dacc");
  addToTree(c, (char *) "op", (void *) "sudo");
  addToTree(c, (char *) "dacc", (void *) "ok");
  displayNodes(c, 1, String);

  return 0;
}
#endif
