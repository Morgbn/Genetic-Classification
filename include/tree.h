#ifndef TREE_INCLUDED
#define TREE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

typedef enum { Char, String, Float, Int } type;

typedef struct node {
  char c; // lettre
  struct node **childs; // enfant (max 26: a-z)
  struct node *parent;
  int nChilds; // nombre d'enfant
  void * val; // valeur attaché à ce noeud
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
treeList newNode(char c, void *val);

/**
 * Ajouter un élement à l'arbre
 * @param  tree arbre
 * @param  c    caractère
 * @param  val  valeur
 * @return      l'élément ds la liste
 */
treeList appendNode(treeList tree, char c, void *val);

/**
 * Afficher l'arbre (f recursive)
 * @param aNode arbre
 * @param ret   1=retour à la ligne, 0 sinon
 * @param  t    type attaché aux noeuds de l'arbre
 */
void displayNodes(treeList aNode, int ret, type t);

/**
 * Afficher le chemin vers un noeud de l'arbre (f recursive)
 * @param aNode noeud
 * @param ret   1=retour à la ligne, 0 sinon
 */
void displayTreePath(treeList aNode, int ret);

/**
 * Ajouter des élements à l'arbre (f recursive)
 * @param head  tête de l'arbre
 * @param str   chaine de caractère
 * @param val   valeur à attacher à la fin
 */
void addToTree(treeList head, char *str, void *val);

/**
 * Trouver un noeud par son nom (f recursive)
 * @param  head tête de l'arbre
 * @param  name nom
 * @return      noeud
 */
treeList getNode(treeList head, char *name);

/**
 * Trouver le nombre de valeur non nul attaché à l'arbre
 * (f recursive)
 * @param  head tête de l'arbre
 * @return      nombre de valeur attaché à l'abre
 */
int nLeaf(treeList head);

/**
 * Supprimer les parents inutiles (qui ne porte pas de valeur) d'un noeud
 * (f recursive)
 * @param child noeud
 */
void freeNodeParents(treeList child);

/**
* Supprimer un/des noeuds d'un arbre (f recursive)
* @param node        noeud
* @param delVal      1 = supprimer valeur attaché, 0 sinon
* @param recursively 1 = supprimer enfants, 0 sinon
*/
void deleteNode(treeList aNode, int delVal, int recursively);

/**
 * Supprimer un noeud et tout les parents qui y mène, parents maitenant inutiles
 * @param node noeud
 */
void freePathToNode(treeList aNode);

#endif
