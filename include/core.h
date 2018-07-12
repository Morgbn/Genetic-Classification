#ifndef CORE_INCLUDED
#define CORE_INCLUDED

#include <dirent.h>
#include <string.h>

#include "utilities.h"
#include "lemma.h"
#include "tree.h"

typedef struct {
  char * name;
  int id;
  treeList terms;
} doc;

/**
 * Lire les fichiers texte d'un dossier, et les traiter
 * @param path chemin vers le dossier
 * @param len  nombre de documents traités
 * @return     tout les documents traités
 */
doc * getData(const char * path, int * len);

void divideAllTreeBy(treeList tree, int n);

void addDoc(doc ** data, int * len, char * name, treeList terms);

/**
 * Allouer un pointeur sur un float
 * @param  f float
 * @return   pointeur sur f
 */
float * floatToPfloat(float f);

#endif
