#ifndef CORE_INCLUDED
#define CORE_INCLUDED

#include <dirent.h>
#include <string.h>

#include "utilities.h"
#include "lemma.h"
#include "tree.h"

typedef struct _doc{
  char * name;
  int id;
  treeList terms;
  double * dist;
} doc;

/**
 * Lire les fichiers texte d'un dossier, et les traiter
 * @param  path   chemin vers le dossier
 * @param  len    nombre de documents traités
 * @param  toLuhn 1=appliquer conjecture de Luhn, 0 sinon
 * @return        tout les documents traités
 */
doc * getData(const char * path, int * len, int toLuhn);

/**
 * Diviser tout l'arbre par un entier
 * @param tree arbre
 * @param n    entier
 */
void divideAllTreeBy(treeList tree, int n);

/**
 * Ajouter un document à une liste de documents
 * @param data  liste de documents
 * @param len   longeur de la liste
 * @param name  nom du document
 * @param terms termes du document
 */
void addDoc(doc **data, int len, char * name, treeList terms);

/**
 * Allouer un pointeur sur un float
 * @param  f float
 * @return   pointeur sur f
 */
float * pFloat(float f);

/**
 * Calculer la distance entre deux treeList
 * @param  a arbre a
 * @param  b arbre b
 * @return   distance
 */
double distBtwDoc(treeList a, treeList b);

#endif
