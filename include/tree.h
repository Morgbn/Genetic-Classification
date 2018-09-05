#ifndef TREE_INCLUDED
#define TREE_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

typedef enum { Char, String, Float, Int } type;

typedef struct node {
  char c;               // lettre
  struct node **childs; // enfant (max 26: a-z)
  struct node *parent;
  int nChilds;          // nombre d'enfant
  void * val;           // valeur attaché à ce noeud
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
 * Trouver le chemin vers un noeud de l'arbre
 * @param  aNode     noeud
 * @param  skipFirst 1=sauter la racine, 0 sinon
 * @return           chemin
 */
char * getTreePath(treeList aNode, int skipFirst);

/**
 * Afficher le chemin vers un noeud de l'arbre (f recursive)
 * @param aNode noeud
 * @param ret   1=retour à la ligne, 0 sinon
 */
void displayTreePath(treeList aNode, int ret);

/**
 * Afficher les valeurs d'un arbre (f recursive)
 * @param head   tête de l'arbre
 * @param format format
 */
void displayVals(treeList head, type t);

/**
 * Afficher tous les chemins des noeuds auxquels est attachée une valeur (f recursive)
 * @param head tête de l'arbre
 */
void displayPaths(treeList head);

/**
 * Afficher tous les chemins et valeurs (f recursive)
 * @param head   tête de l'arbre
 * @param format format
 */
void displayPathsVals(treeList head, type t);

/**
 * Ajouter des élements à l'arbre (f recursive)
 * @param head    tête de l'arbre
 * @param str     chaine de caractère
 * @param val     valeur à attacher à la fin
 * @param freeVal si valeur à attacher est alloué
 */
void addToTree(treeList head, char *str, void *val, int freeVal);

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
 * Supprimer un arbre
 * @param aNode      arbre
 * @param freeVal    1=supprimer les valeurs attachées, 0 sinon
 */
void freeNode(treeList aNode, int freeVal);

/**
 * Supprimer un noeud
 * @param aNode   noeud
 * @param freeVal 1=supprimer les valeurs attachées, 0 sinon
 */
void delNode(treeList aNode, int freeVal);

/**
 * Nettoyer un arbre de ses noeuds inutiles
 * @param head tête de l'arbre
 */
void cleanTree(treeList head);

#endif
