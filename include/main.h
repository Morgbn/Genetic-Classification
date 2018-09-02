#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utilities.h"
#include "core.h"
#include "tree.h"
#include "ga.h"

int TF_IDF, LUHN_ON_ALL, LUHN_ON_EACH, USE_EUCL_DIST, minK, maxK, multPop0, MaxGen, PopSize;
int verboseGa, verboseLemm, verboseDist;

const char * help_usage_short = "Utilisation : %s <nom du dossier> [OPTION...]\n";
const char * help_usage_long = "Utilisation : %s <nom du dossier> [--tdidf] [--luhn {0|1|2}]\n            [--eucl] [--cos] [--help] [--usage]\n";
const char * help[]  = {
  "\n Contrôle du prétraitement des données :",
  "\n  --tfidf       utilise la méthode de pondération TF-IDF",
  "\n  --luhn        utilise la conjecture de Luhn",
  "\n                  --luhn = 0 pour ne pas l'utiliser"
  "\n                  --luhn = 1 pour l'utiliser sur chaque document, séparément",
  "\n                  --luhn = 2 pour l'utiliser sur l'ensemble des documents",
  "\n  --eucl        utilise la distance euclidienne",
  "\n                  (désactivé par --cos)",
  "\n  --cos         utilise la similarité cosinus",
  "\n                  (désactivé par --eucl)",
  "\n Contrôle de l'algorithme génétique :",
  "\n  --min=NOMBRE  nombre minimal de groupe à créer",
  "\n  --max=NOMBRE  nombre maximal de groupe à créer",
  "\n  --mul=NOMBRE  multiplicateur pour la 1er population",
  "\n  --gen=NOMBRE  maximum de generations",
  "\n  --pop=NOMBRE  taille de la population",
  "\n Option d'affichage:",
  "\n  --printGA     afficher le report de l'algorithme génétique"
  "\n  --printLemm   afficher le résultat de la lemmatisation"
  "\n  --printDist   afficher la distance entre les documents"
  "\n Autres options :",
  "\n  -? --help     affiche cette aide",
  "\n  --usage       donne un court message d'utilisation\n",
  NULL
};

/**
 * Traiter les options
 * @param  i    nième option
 * @param  argv tout les arguments
 * @param  argc nombre d'argument total
 * @return      nomvre d'argument utilisé
 */
int changeOption(int i, const char *argv[], int argc);
#endif
