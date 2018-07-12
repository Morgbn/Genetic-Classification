#ifndef UTILITIES_INCLUDED
#define UTILITIES_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Lire un fichier
 * @param  pathname  chemin vers fichier
 * @param  size      pointer sur nb de mots
 * @param keepNumber 1 si garder les nbr, 0 sinon
 * @return           tableau de mots
 */
char ** readFile(const char * pathname, int *size, int keepNumber);

/**
 * Enlever diacritiques et ligatures d'un str,
 * et passe tout en minuscule
 * @param str        chaine char à nettoyer
 * @param keepNumber 1 si garder les nbr, 0 sinon
 */
void cleanStr(unsigned char * str, int keepNumber);

/**
 * Affiche un message d'erreur et quite
 * @param error message
 */
void usage(const char * error);

#endif
