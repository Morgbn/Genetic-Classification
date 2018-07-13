#ifndef LEMMA_INCLUDED
#define LEMMA_INCLUDED

#include <string.h>

#include "utilities.h"
#include "tree.h"

/**
 * Lammatiser liste de mots
 * @param words    liste de mots
 * @param len      taille de la liste
 * @param stopList stop liste
 * @param wordList dico verbes
 */
void lemmatisation(char **words, int len, treeList stopList, treeList wordList);

/**
 * Initialiser les données pour la lemmatisation
 * @param stopPath chemin stop liste
 * @param stopList arbre pour la stop liste
 * @param wordPath chemin dico verbes
 * @param wordList arbre pour les verbes
 */
void initLemma(const char *stopPath, treeList stopList, const char *wordPath, treeList wordList);

/**
 * Enléve le pluriel d'un mot en français
 * @param  word mot
 * @return      taille du mot lemmatisé
 */
int rmFrPlural(char * word);

/**
 * Enléve le féminin d'un mot en français
 * @param  word mot
 * @return      taille du mot lemmatisé
 */
int rmFrFem(char * word);

/**
 * Calculer la somme des fréquences
 * @param  head tête de l'arbre
 * @return      somme
 */
float getSumFreq(treeList head);

/**
 * Appliquer la conjecture de Luhn (f recursive)
 * @param head tête de l'arbre
 * @param min  seuil inférieur
 * @param max  seuil supérieur
 */
void applyLuhn(treeList head, float min, float max);

#endif
