#ifndef CORE_INCLUDED
#define CORE_INCLUDED

#include <dirent.h>
#include <string.h>

#include "utilities.h"
#include "tree.h"

typedef struct {
  char * name;
  treeList terms;
} doc;

/**
 * Lire les fichiers texte d'un dossier, et les traiter
 * @param path chemin vers le dossier
 * @param len  nombre de documents traités
 * @return     tout les documents traités
 */
doc * getData(const char * path, int * len);

/**
 * Enlever diacritiques et ligatures d'un str,
 * et passe tout en minuscule
 * @param str chaine char à nettoyer
 */
void cleanStr(unsigned char * str);

char ** readFile(char * pathname, int *size);

void divideAllTreeBy(treeList tree, int n);

void addDoc(doc ** data, int * len, char * name, treeList terms);

#endif
