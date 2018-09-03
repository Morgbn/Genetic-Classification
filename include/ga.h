#ifndef GA_INCLUDED
#define GA_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "utilities.h"
#include "core.h"

extern int verboseGa, multPop0, MaxGen, PopSize;

typedef int Chromo[256]; // maximum 256 cluster

typedef struct { // données d'un individu
  Chromo Gtype;
  int * Ptype;   // pr le style
  int Len;       // nombre de clusters
	double Fitness;
} individual, * indiv;

typedef indiv * Population;

/**
 * Genetic Algo
 * basé sur l'AG présenté dans le cours :
 * "modèles discrets du continu techniques d'abstraction" - J.FEAT 2011
 * @param  docs liste de document
 * @param  nDoc nombre de document
 * @param  nClu nombre de clusters finaux
 * @param  minK nombre minimal de cluster
 * @param  maxK nombre maximal de cluster
 * @return      liste de groupe de document
 */
doc *** GA(doc *docs, int nDoc, int * nClu, const int minK, const int maxK);

/**
 * Fonction objectif
 * @param  ptype phenotype d'un individu
 * @param  K     nombre d'info codé
 * @param  docs  liste de document
 * @param  nDoc  nombre de document
 * @return       fitness
 */
double objectiveFunc(int * ptype, int K, doc *docs, int nDoc);

/**
 * Créer un chromosome
 * @param chromo  chromosome
 * @param K       nombre d'info codé
 * @param nDoc    nombre de document
 */
void makeChromo(Chromo chromo, int K, int nDoc);

/**
 * Afficher un chromosome
 * @param chromo chromosome
 * @param K     nombre d'info codé
 */
void putchrom(Chromo chromo, int K);

/**
 * Afficher des infomations utiles à la maintenance de l'algo
 * @param gen numéro de la génération
 */
void report(int gen);

/**
 * Allouer une population
 * @param  popSize taille de la population
 * @param  k       nombre d'info codée
 * @return         population
 */
Population allocPop(const int popSize, const int k);

/**
 * Supprimer une population
 * @param popSize taille de la population
 * @param pop     population
 */
void freePop(Population pop, const int popSize);

/**
 * Copier une population
 * @param pop  population à copier
 * @param copy copie
 */
void copyPop(Population pop, Population copy);

/**
 * Décoder un génotype
 * @param  Gtype génotype
 * @param  K     nombre d'info codé
 * @return       génotype décodé (score)
 */
int * decode(Chromo Gtype, int K);

/**
 * Convertir un morceau de chromosome (bit string)
 * @param  spec bitstring
 * @return      valeur
 */
int decodeSpec(char * spec);

/**
 * Initialise les populations
 * la 1er aléatoirement
 * et fait de la place pour Pop2
 * @param minK nombre minimum de clusters
 * @param maxK nombre maximum de clusters
 * @param docs  liste de document
 * @param nDoc  nombre de document
 */
void genPops(const int minK, const int maxK, doc *docs, int nDoc);

/**
 * Collecte les statistiques globales
 * @param pop population
 */
void statistics(Population pop);

/**
 * linear transformation of Fitness curve
 * @param pop population
 */
void scale(Population pop);

/**
 * generer une population aléatoirement
 * @param docs liste de document
 * @param nDoc nombre de document
 */
void generate(doc *docs, int nDoc);

/**
 * Regarde si un allele est présent dans un chromosome
 * @param  el  allele
 * @param  arr chromosome
 * @param  len taille du chromosome
 * @return     1 si présent, 0 sinon
 */
int alleleInChromo(int el, Chromo chromo, int len);

/**
 * Copier un chromosome
 * @param P1 chromosome à copier
 * @param C1 copie
 */
void copyChromo(Chromo P1, Chromo C1);

/**
 * Effectuer crossover entre 2 chromos
 * @param  P1 1er parent
 * @param  P2 2eme parent
 * @param  C1 1er enfant
 * @param  C2 2eme enfant
 * @param  K1 taille 1er enfant
 * @param  K2 taille 2eme enfant
 * @return    point du crossover
 */
void crossover(Chromo P1, Chromo P2, Chromo C1, Chromo C2, int K1, int K2);

/**
 * Mettre à jour un individu
 * @param ind   individu
 * @param newK  nouvelle taille
 * @param docs  liste de document
 * @param nDoc  nombre de document
 */
void updateIndiv(indiv ind, int newK, doc *docs, int nDoc);

/**
 * Mélanger un vecteur de int
 * @param a   vecteur
 * @param len taille du vecteur
 */
void shuffleVect(int * a, int len);

/**
 * Sélectioner des individus ds une population
 * Méthode du reste stochastique
 * @param  pop population
 * @return     individus
 */
int * pick(Population pop);

/**
 * Effectuer une mutation
 * @param  val allele à muter (ou pas)
 * @return     allele muté (ou pas)
 */
int mutate(int val);

/**
 * pile ou face
 * @param  proba probabilité
 * @return       vrai ou faux
 */
int flip(double proba);

/**
 * Générer un nb aléatoire entre deux bornes
 * @param  low  borne inférieur
 * @param  high borne supérieur
 * @return      entier aléatoire
 */
int randRange(int low, int high);

/** @return réel aléatoire */
double floatRand();

#endif
