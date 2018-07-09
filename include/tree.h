#ifndef TREE_INCLUDED
#define TREE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

typedef struct node {
  char c; // lettre
  struct node **childs; // enfant (max 26: a-z)
  int nChilds; // nombre d'enfant
  float val;   // valeur attaché à ce noeud
} * treeList;

/**
 * Créer le 1er élement de l'arbre
 * @return
 */
treeList initTree();

/**
 * Créer un nouveau élement
 * @param  c   caractère
 * @param  val valeur à y attacher
 * @return     élement
 */
treeList newNode(char c, float val);

/**
 * Ajouter un élement à l'arbre
 * @param  tree arbre
 * @param  c    caractère
 * @param  val  valeur
 * @return      l'élément ds la liste
 */
treeList appendNode(treeList tree, char c, float val);

/**
 * Afficher l'arbre (f recursive)
 * @param aNode arbre
 * @param ret   1=retour à la ligne, 0 sinon
 */
void displayNodes(treeList aNode, int ret);

/**
 * Ajouter des élements à l'arbre (f recursive)
 * @param head  tête de l'arbre
 * @param str   chaine de caractère
 * @param val   valeur à attacher à la fin
 * @param noVal valeur à attacher par defaut
 */
void addToTree(treeList head, char *str, float val, float noVal);

/**
 * Trouver un noeud par son nom (f recursive)
 * @param  head tête de l'arbre
 * @param  name nom
 * @return      noeud
 */
treeList getNode(treeList head, char *name);

#endif
