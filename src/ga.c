#include "ga.h"

/**
 * VARIABLE GLOBAL :
 * @global ProbCross     probabilité crossover
 * @global ProbMut       probabilité mutation
 * @global GlobalFitness somme des valeurs d'aptitude
 * @global Average       moyenne des valeurs d'aptitude
 * @global MaxFit        maximum des valeurs d'aptitude
 * @global MinFit        minimum des valeurs d'aptitude
 * @global Pop1          population parent
 * @global Pop2          population enfant
 */
double ProbCross = .95, ProbMut   = .02;
double GlobalFitness, Average, MaxFit, MinFit;
Population Pop1, Pop2;

doc *** GA(doc *docs, int nDoc, int * nClu, const int minK, const int maxK) {
  assert(!(PopSize%2)); // si mvs réglage
  if (nDoc < 2)         // regroupe pas - de 2 doc
    usage("Le dossier ne contient qu'un fichier!");

  genPops(minK, maxK, docs, nDoc); // initialisation des populations
  if (verboseGa) report(-1);
	for (int gen = 0 ; gen < MaxGen ; gen++) {
    statistics(Pop1);
    scale(Pop1);
    generate(docs, nDoc);
    if (verboseGa) report(gen);
    copyPop(Pop2, Pop1);
  }

  // récupérer le meilleur clustering trouvé
  indiv ind;
  for (int j = 0; j < PopSize; j++) {
    ind = Pop1[j];
    if (ind->Fitness == MaxFit) break;
  }
  *nClu = ind->Len;

  // faire de la place pour la liste de liste de document
  doc ***clusters = (doc ***) malloc(*nClu * sizeof(doc **));
  if (clusters == NULL) usage("error malloc in GA");
  for (int i = 0; i < *nClu; i++) {
    clusters[i] = (doc **) malloc((nDoc+2) * sizeof(doc *));
    if (clusters[i] == NULL) usage("error malloc in GA");
  }

  int nIn[*nClu]; // nombre de doc ds chq clusters
  memset(nIn, 0, *nClu * sizeof(int));

  int * ptype = ind->Gtype;
  for (int ki = 0; ki < *nClu; ki++)          // mettre en tête les centres
    clusters[ki][nIn[ki]++] = &docs[ptype[ki]];

  for (int i = 0; i < nDoc; i++) {            // pr chq document
    int bestK = 0;
    double dmin = docs[i].dist[ptype[0]];
    for (int ki = 1; ki < *nClu; ki++) {      // trouver sa distance minimal
      double d = docs[i].dist[ptype[ki]];     // avec l'un des centres
      if (d < dmin) { dmin = d; bestK = ki; }
    }
    if (ptype[bestK] == i) continue;          // déjà ajouter (au début)
    clusters[bestK][nIn[bestK]++] = &docs[i];
  }

  for (int i = 0; i < *nClu; i++)             // pr facilement detecter la fin
    clusters[i][nIn[i]] = NULL;

  // FREE variables non retournés
  freePop(Pop1, PopSize);
  freePop(Pop2, PopSize);

  return clusters;
}

double objectiveFunc(int * ptype, int K, doc *docs, int nDoc) {
  double score = 1.e-10;
  // vérifier que chromo est valide :
  for (int i = 0; i < K; i++) {
    if (ptype[i] >= nDoc || ptype[i] < 0)  // si n° invalide
     return -1;                            // → pas de reproduction
    for (int j = i+1; j < K; j++) {
      if (ptype[i] == ptype[j]) return -1;// si doublon → pas de reproduction
    }
  }
  for (int i = 0; i < nDoc; i++) {        // pr chq document
    double dj = 0;
    for (int ki = 0; ki < K; ki++) {      // trouver sa distance minimal
      double d = docs[i].dist[ptype[ki]]; // avec le centre ki
      if (!ki || d < dj) dj = d;
    }
    score += dj;                          // dj / n
  }
  score /= nDoc;
  return 1 / score;                       // score inversé (+ petit = mieux)
}

void makeChromo(Chromo chromo, int K, int nDoc) {
  for (int i = 0; i < K; i++)
    chromo[i] = randRange(0, nDoc+1);       // nb aléatoire entre 0 et le nb de doc
}

void putchrom(Chromo chromo, int K) {
  printf("[");
  for (int p = 0; p < K; p++)
    printf("%i%s", chromo[p], (p+1==K) ? "" : ", ");
  printf("]");
}

void report(int gen) {
  printf("++Gen n° %i ; MinFit = %g ; Av = %g ; MaxFit = %g ; GF = %g\n", gen, MinFit, Average, MaxFit, GlobalFitness);
  for (int i = 0; i < PopSize; i++) {
    printf("(%03d) ", i);
    putchrom(Pop1[i]->Gtype, Pop1[i]->Len);
    printf(" score = %g\n", Pop1[i]->Fitness);
  }
  puts(" ");
}

Population allocPop(const int popSize, const int k) {
  Population pop = NULL;
  pop = (Population) malloc(popSize * sizeof(indiv));
  if (pop == NULL) usage("malloc error in allocPop");

  for (int i = 0; i < popSize; i++) {
    pop[i] = NULL;
    pop[i] = (indiv) calloc(1, sizeof(individual));
    if (pop[i] == NULL) usage("malloc error in allocPop");

    pop[i]->Gtype = (int *) calloc(k, sizeof(int));
    if (pop[i]->Gtype == NULL) usage("malloc error in genPops");
  }
  return pop;
}

void freePop(Population pop, const int popSize) {
  for (int i = 0; i < popSize; i++) {
    free(pop[i]->Gtype); pop[i]->Gtype = NULL;
    free(pop[i]); pop[i] = NULL;
  }
  free(pop);
  pop = NULL;
}

void copyPop(Population pop, Population copy) {
  for (int i = 0; i < PopSize; i++) {
    copyChromo(pop[i]->Gtype, copy[i]->Gtype, pop[i]->Len);
    copy[i]->Ptype = NULL;
    copy[i]->Len = pop[i]->Len;
    copy[i]->Fitness = pop[i]->Fitness;
  }
}

int genPops_cmpfunc(const void * a, const void * b) {
  indiv ind1 = (indiv) a;
  indiv ind2 = (indiv) b;
  return (ind1->Fitness < ind2->Fitness) ? 1 : (ind1->Fitness > ind2->Fitness) ? -1 : 0;
}

void genPops(const int minK, const int maxK, doc *docs, int nDoc) {
  // pop n°0 multPop0 fois plus grosse
  Population bigPop = allocPop(PopSize * multPop0, maxK);
  for (int i = 0; i < PopSize * multPop0; i++) {
    int K = randRange(minK, maxK+1);          // taille aléatoire
    makeChromo(bigPop[i]->Gtype, K, nDoc);
    updateIndiv(bigPop[i], K, docs, nDoc);
  }

  // trier bigPop pr avoir meilleur en haut
  qsort(bigPop, PopSize * multPop0, sizeof(indiv), genPops_cmpfunc);

  Pop2 = allocPop(PopSize, maxK);
  Pop1 = allocPop(PopSize, maxK);
  for (int i = 0; i < PopSize; i++) {         // copier les valides
    copyChromo(bigPop[i]->Gtype, Pop1[i]->Gtype, bigPop[i]->Len);
    Pop1[i]->Len = bigPop[i]->Len;          // (important!)
    Pop1[i]->Fitness = bigPop[i]->Fitness;  // (aussi)
  }
  // bigPop n'est plus utile → free
  freePop(bigPop, PopSize * multPop0);
}

void statistics(Population pop)	{
  MaxFit = 0;
  MinFit = 0;
  GlobalFitness = 0;
	for (int x = 0; x < PopSize; x++) {
    if (pop[x]->Fitness < 0) continue;       // indiv invalide
		GlobalFitness += pop[x]->Fitness;
		if (pop[x]->Fitness > MaxFit) MaxFit = pop[x]->Fitness;
		if (!MinFit ||
        pop[x]->Fitness < MinFit) MinFit = pop[x]->Fitness;
  }
	Average = GlobalFitness / (double) PopSize;
}

void scale(Population pop) {
  const double FM = 2;	                  // espérance du nb de copies souhaité pour le meilleur individu
	double delta, slope, offset;	          // slope and offset for linear equation
  //	calculer le coefficient pr la transformation linéaire
	if (MinFit > ((FM * Average - MaxFit) / (FM - 1))) { // non-negatif ? (bcp de bons individus)
	  delta = MaxFit - Average;	            // transformation normal
		slope = (FM - 1) * Average / delta;
		offset = Average * (MaxFit - FM * Average) / delta;
  }
	else {                                  // negatif (peu de bons individus)
    delta = Average - MinFit;	            // transformation le plus possible
		slope = Average / delta;
		offset = -MinFit * Average / delta;
  }
	GlobalFitness = 0;
	for (int x = 0; x < PopSize; x++) {
    if (pop[x]->Fitness < 0) continue; // ne pas s'occuper des indiv non valides
		pop[x]->Fitness = (slope * (pop[x]->Fitness)) + offset;	// y = ax + b, translated adaptation value
		GlobalFitness += pop[x]->Fitness;
  }
}

void generate(doc *docs, int nDoc) {
  int * picked1 = pick(Pop1);             // select mates
	for (int z = 0; z < PopSize; z += 2) {
    int m1 = picked1[z];	                // selected mate 1
		int m2 = picked1[z+1];                // selected mate 2
		crossover(
      Pop1[m1]->Gtype, Pop1[m2]->Gtype,
      Pop2[z]->Gtype, Pop2[z+1]->Gtype,
      Pop1[m1]->Len, Pop1[m2]->Len
    );
		updateIndiv(Pop2[z]  , Pop1[m1]->Len, docs, nDoc);
		updateIndiv(Pop2[z+1], Pop1[m2]->Len, docs, nDoc);
  }
  free(picked1);
}

int alleleInChromo(int el, Chromo chromo, int len) {
  for (int i = 0; i < len; i++)
    if (el == chromo[i]) return 1; // el ds chromo
  return 0;
}

void copyChromo(Chromo P1, Chromo C1, int k) {
  C1 = (Chromo) realloc(C1, k * sizeof(int));
  if (C1 == NULL) usage("error realloc copyChromo");
  for (int i = 0; i < k; i++) C1[i] = mutate(P1[i]);
}

void crossover(Chromo P1, Chromo P2, Chromo C1, Chromo C2, int K1, int K2) {
  copyChromo(P1, C1, K1);
  copyChromo(P2, C2, K2);
  if (!flip(ProbCross)) return; // pas de crossover

  int Kmax = (K1 >= K2) ? K1 : K2;
  int U1[Kmax], U2[Kmax]; // index des centres ne se retrouvant pas dans l'autre
  int u1 = 0, u2 = 0;

  for (int i = 0; i < K1; i++)
    if (!alleleInChromo(P1[i], P2, K2)) U1[u1++] = i; // centres non présents dans P2
  for (int i = 0; i < K2; i++)
    if (!alleleInChromo(P2[i], P1, K1)) U2[u2++] = i; // centres non présents dans P2

  int Umin = (u1 < u2) ? u1 : u2;
  for (int i = 0; i < Umin; i++) {
    if (flip(ProbCross) > 0.5) continue; // 1 chance sur 2 de crossover ce centre
    int tmp = C1[U1[i]];
    C1[U1[i]] = C2[U2[i]];               // inter changer les centres
    C2[U2[i]] = tmp;
  }
}

void updateIndiv(indiv ind, int newK, doc *docs, int nDoc)	{
	ind->Ptype = ind->Gtype;
	ind->Fitness = objectiveFunc(ind->Ptype, newK, docs, nDoc);
  ind->Len = newK;
}

void shuffleVect(int * a, int len) {
  int tmp, n;

  for (int i = 0; i < len; i++) {
    n = randRange(0, len);                // prend un index aléaléatoirement
    tmp  = a[i];
    a[i] = a[n];                          // échange les valeurs a[i] et a[n]
    a[n] = tmp;
  }
}

int * pick(Population pop) {
  int * picked = NULL, npick = 0;
  picked = (int *) malloc(PopSize * sizeof(int));
  if (picked == NULL) usage("error malloc in pick");

  double expected[PopSize];               // nombre de copie attendu pr chq individu
  int N[PopSize];                         // partie entière de expected
  double sumR = 0;                        // somme des restes
  for (int i = 0; i < PopSize; i++) {
    if (pop[i]->Fitness < 0) expected[i] = 0;
    else expected[i] = pop[i]->Fitness / (GlobalFitness / PopSize);
    N[i] = (int) expected[i];
    expected[i] -= N[i];                  // laisser le reste dans expected
    sumR += expected[i];
  }

  // affecte automatiquement à chq individu Ni copies
  for (int i = 0; (i < PopSize) && (npick < PopSize); i++)
    for (int j = 0; j < N[i]; j++)
      picked[npick++] = i;

  // recalcule probabilité avec ce qui reste
  for (int i = 0; i < PopSize; i++)
    if (expected[i]) expected[i] /= sumR;


  // roulette avec les restes pr compléter la population
  for (int i = 0; npick < PopSize; i++) {
    double accumulator = 0;
    double limit = floatRand();	          // réel aléatoire entre 0 et 1
    int mate;
    for (mate = 0; mate < PopSize; mate++) {
      accumulator += expected[mate];
    	if (accumulator > limit) break;     // si depasse la limite → renvoyer
    }
    picked[npick++] = (mate < PopSize) ? mate : PopSize - 1;
  }

  shuffleVect(picked, PopSize);
  return picked;
}

int mutate(int val) {
  if (flip(ProbMut)) {
    float mul = floatRand();  // diminuer val
    if (flip(.5)) mul += 1;   // ou l'augmenter
    val *= mul;
  }
	return val;
}

int flip(double proba) {
  if (!(proba < 1)) return 1;
  double x = floatRand();
  return (x <= proba);
}

int randRange(int low, int high) {
  return rand() % (high - low) + low ;
}

double floatRand() {
  return (double) (rand() % 1000) / 1000 ;
}
