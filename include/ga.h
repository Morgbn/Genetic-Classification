#ifndef GA_INCLUDED
#define GA_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "utilities.h"
#include "core.h"

typedef char boolean ;
typedef boolean allele ;

typedef struct {
  allele *A;
} Chromo;

typedef struct { // données d'un individu
  Chromo Gtype;
  int * Ptype;
	double Fitness;
	int Parent_1, Parent_2, CrossPoint;
} individual, * indiv;

typedef struct {
  individual *A;
} Population;

/**
 * Genetic Algo
 * basé sur l'AG présenté dans le cours :
 * "modèles discrets du continu techniques d'abstraction" - J.FEAT 2011
 * @param  docs liste de document
 * @param  nDoc nombre de document
 * @param  nClu nombre de cluster
 * @return      liste de groupe de document
 */
doc *** GA(doc * docs, int nDoc, int * n);

// application interface specific code ~~~~~~~~~~~~~~~~~

/**
 * Fonction objectif
 * @param  ptype phenotype d'un individu
 * @return       fitness
 */
double objectiveFunc(int * ptype);

/**
 * Décoder un génotype
 * @param  Gtype génotype
 * @return       génotype décodé (score)
 */
int * decode(Chromo Gtype);

/**
 * Convertir un morceau de chromosome (bit string)
 * @param  spec bitstring
 * @return      valeur
 */
int decodeSpec(char * spec);

/**
 * Créer un chromosome
 * @param chromo  emplacement du chromosome
 * @param minSize taille minimum
 * @param maxSize taille maximum
 */
void makeChromo(allele * chromo, int minSize, int maxSize);

// GA engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * Collecte statistiques globales
 * @param pop population
 */
void statistics(Population * pop);

/**
 * Initialise les populations
 * la 1er aléatoirement
 * et fait de la place pour Pop2
 */
void genPops();

/**
 * linear transformation of Fitness curve
 * @param pop population
 */
void scale(Population * pop);

/** generer une population aléatoirement */
void generate();

/**
 * Effectuer crossover entre 2 chromos
 * @param  P1 1er parent
 * @param  P2 2eme parent
 * @param  C1 1er enfant
 * @param  C2 2eme enfant
 * @return    point du crossover
 */
int crossover(Chromo * P1, Chromo * P2, Chromo * C1, Chromo * C2);

/**
 * Mettre à jour un individu
 * @param ind individu
 * @param m1  1er parent
 * @param m2  2eme parent
 * @param X   point du crossover
 */
void updateIndiv(indiv ind, int m1, int m2, int X);

/**
 * Sélectioner un individu ds une population
 * @param  pop population
 * @return     individu
 */
int pick(Population * pop);

/**
 * Effectuer une mutation (0→1; 1→0)
 * @param  bval allele à muter (ou pas)
 * @return      allele muté (ou pas)
 */
allele mutate(allele bval);

/**
 * pile ou face
 * @param  proba probabilité
 * @return       vrai ou faux
 */
boolean flip(double proba);

/**
 * Générer un nb aléatoire entre deux bornes
 * @param  low  borne inférieur
 * @param  high borne supérieur
 * @return      entier aléatoire
 */
int randRange(int low, int high);

/** @return réel aléatoire */
double floatRand(); // INLINE ???

#endif
