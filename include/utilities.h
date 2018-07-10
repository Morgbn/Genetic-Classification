#ifndef UTILITIES_INCLUDED
#define UTILITIES_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Lire un fichier
 * @param  pathname chemin vers fichier
 * @param  size     pointer sur nb de mots
 * @return          tableau de mots
 */
char ** readFile(const char * pathname, int *size);

/**
 * Enlever diacritiques et ligatures d'un str,
 * et passe tout en minuscule
 * @param str chaine char à nettoyer
 */
void cleanStr(unsigned char * str);

/**
 * Affiche un message d'erreur et quite
 * @param error message
 */
void usage(const char * error);

#endif
