#include "tree.h"

treeList initTree() {
  return newNode('+', -1);
}

treeList newNode(char c, float val) {
  treeList aNode = (treeList) malloc(sizeof(struct node));
  if (aNode == NULL) usage("error malloc in newNode");
  aNode->c = c;
  aNode->val = val;
  aNode->nChilds = 0;
  aNode->childs = NULL;
  return aNode;
}

treeList appendNode(treeList tree, char c, float val) {
  treeList child = newNode(c, val);
  tree->nChilds += 1;
  tree->childs = (struct node **) realloc(tree->childs, tree->nChilds * sizeof(struct node));
  if (tree->childs == NULL) usage("error realloc in appendNode");
  tree->childs[tree->nChilds-1] = child;
  return child;
}

void displayNodes(treeList aNode, int ret) {
  if (aNode == NULL) return;
  if (aNode->val) printf("(%c %g%s", aNode->c, aNode->val, aNode->nChilds ? " " : "");
  for (int i = 0; i < aNode->nChilds; i++)
    displayNodes(aNode->childs[i], 0);
  printf(")%s", ret ? "\n" : " ");
}

void addToTree(treeList head, char *str, float val, float noVal) {
  int finded = 0, j;
  for (j = 0; j < head->nChilds && !finded; j++) { // parcours les enfants (max 26 enfants)
    if (head->childs[j]->c == *str) finded = 1;
  }
  if (finded) addToTree(head->childs[j-1], str+1, val, noVal); // char existe déjà→continuer
  else {
    head = appendNode(head, *str, noVal); // créer un noeud avec le char absent
    if (str[1] == '\0') head->val = val;  // fin du mot, attacher la valeur
    else addToTree(head, str+1, val,noVal); // sinon continuer sur nouveau noeud
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

#if TEST
int main(int argc, char const *argv[]) {
  treeList a = initTree();
  appendNode(a, 'a', 1);
  appendNode(a, 'b', 2);
  appendNode(a, 'c', 3);
  appendNode(a->childs[0], 'c', 4);
  addToTree(a, "abcd", 5, -1);
  addToTree(a, "ok", 6, -1);
  addToTree(a, "abce", 7, -1);
  displayNodes(a, 1);

  treeList b = getNode(a, "abce");
  displayNodes(b, 1);
  return 0;
}
#endif
