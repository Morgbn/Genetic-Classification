#include "ga.h"

double GlobalFitness, Average, MaxFit, MinFit;
Population Pop1, Pop2; // for swapping

/** @global MaxGen maximum de generations */
short int MaxGen = 20;
/** @global ProbCross probabilité crossover */
double ProbCross = .95;
/** @global ProbMut probabilité mutation */
// double ProbMut = .02;
double ProbMut = .02;
/** @global PopSize taille population */
int PopSize = 30;
// !:::::: temporaire ::::!
int Bits, K;
int ChromSize; // max de bit * k
doc *Docs;
int NDoc;

void putchrom(Chromo Gtype) {
  printf("[");
  for (int p = 0; p < K; p++) {
    char * bitstr = &(Gtype.A[p * Bits]);
    printf("%i%s", decodeSpec(bitstr), (p+1==K) ? "" : ", ");
  }
  printf("]");
}

void report(int gen) {
  printf("------- %i -------\n", gen);
  for (int i = 0; i < PopSize; i++) {
    indiv ind = &Pop1.A[i];	// old string
    putchrom(ind->Gtype);
    printf(" score = %g\n", ind->Fitness);
  }
}

doc *** GA(doc *docs, int nDoc, int * nClu) {
  // const int minClu = 2;
  // const int maxClu = nDoc / 2;
  Bits = 3;
  K = 2;
  ChromSize = Bits * K;
  Docs = docs;
  NDoc = nDoc;

  genPops(docs);
	for (int gen = 0 ; gen < MaxGen ; gen++) {
    scale(&Pop1);
    generate();
    Pop1 = Pop2;
    report(gen);
  }

  // *nClu = 2;
  // doc ***clusters = (doc ***) malloc(*nClu * sizeof(doc **));
  // if (clusters == NULL) usage("error malloc in GA");
  // clusters[0] = (doc **) malloc(3 * sizeof(doc));
  // if (clusters[0] == NULL) usage("error malloc in GA");
  // clusters[1] = (doc **) malloc(2 * sizeof(doc));
  // if (clusters[1] == NULL) usage("error malloc in GA");
  // clusters[0][0] = &docs[1];
  // clusters[0][1] = &docs[0];
  // clusters[0][2] = NULL;
  // clusters[1][0] = &docs[2];
  // clusters[1][1] = NULL;
  *nClu = 0;
  doc *** clusters = NULL;
  return clusters;
}

// application interface specific code ~~~~~~~~~~~~~~~~~

double objectiveFunc(int * ptype) {
  double score = 1.e-10;
  // vérifier que chromo est valide :
  for (int i = 0; i < K; i++) {
    if (ptype[i] >= NDoc) return 0;       // si n° invalide → pas de reproduction
    for (int j = i+1; j < K; j++) {
      if (ptype[i] == ptype[j]) return 0; // si doublon → pas de reproduction
    }
  }

  for (int i = 0; i < NDoc; i++) {        // pr chq document
    double dj;
    for (int ki = 0; ki < K; ki++) {      // trouver sa distance minimal
      double d = Docs[i].dist[ptype[ki]]; // avec le centre ki
      if (!ki || d < dj) dj = d;
    }
    score += dj / NDoc;                  // dj / n
  }
  return 1 / score; // score inversé (+ petit = mieux)
}

int * decode(Chromo Gtype)	{
  int * ptype = (int *) malloc(K * sizeof(int));
  if (ptype == NULL) usage("error malloc in decode");

  for (int p = 0; p < K; p++) {
    char * bitstr = &Gtype.A[p * Bits];
    ptype[p] = decodeSpec(bitstr);
  }
	return ptype;
}

int decodeSpec(char * spec) {
  int value = 0;
	for (int B = Bits - 1, power = 1 ; B >= 0 ; B--, power *= 2)
		value += spec[B] * power;
	return value;
}

void makeChromo(allele * chromo, int minSize, int maxSize) {
  for (int i = 0; i < K*Bits;) {
    int n = randRange(0, K);         // nb aléatoire entre 0 et K
    for (int c = Bits-1; c >= 0; c--) {
      int k = n >> c;
      chromo[i++] = (k & 1) ? 1 : 0; // converti en binaire
    }
  }
}

// GA engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void genPops() {
  Pop1.A = (individual *) malloc(PopSize * sizeof(individual));
  Pop2.A = (individual *) malloc(PopSize * sizeof(individual));
  if (Pop1.A == NULL || Pop2.A == NULL)
    usage("malloc error in genPops");

  for (int i = 0; i < PopSize; i++) {
    indiv ind1 = &Pop1.A[i];	// look out ~~~
    indiv ind2 = &Pop2.A[i];	// look out ~~~
    ind1->Gtype.A = (allele *) malloc(ChromSize * sizeof(allele));
    ind2->Gtype.A = (allele *) malloc(ChromSize * sizeof(allele));
    if (ind1->Gtype.A == NULL || ind2->Gtype.A == NULL)
      usage("malloc error in genPops");

		makeChromo(ind1->Gtype.A, ChromSize, ChromSize);
		updateIndiv(ind1, 0, 0, 0);
  }
}

void statistics(Population * pop)	{
  MaxFit = 0;
  MinFit = 1;
	for (int x = 0; x < PopSize; x++) {
	  indiv ind = &pop->A[x];
		GlobalFitness += ind->Fitness;
		if (ind->Fitness > MaxFit) MaxFit = ind->Fitness;
		if (ind->Fitness < MinFit) MinFit = ind->Fitness;
  }
	Average = GlobalFitness / (double) PopSize;
}

void scale(Population * pop) {
  const double FM = 2;	                              // Fitness Multiple : Goldberg p. 88
	double delta, slope, offset;	                      // slope and offset for linear equation
  //	compute coefficient for linear scale
	if (MinFit > ((FM * Average - MaxFit) / (FM - 1))) {// non-negative ?
	  delta = MaxFit - Average;	// normal scale
		slope = (FM - 1) * Average / delta;
		offset = Average * (MaxFit - FM * Average) / delta;
  }
	else {
    delta = Average - MinFit;	                        // scale as much as possible
		slope = Average / delta;
		offset = -MinFit * Average / delta;
  }
	GlobalFitness = 0;
	for (int x = 0; x < PopSize; x++) {
    indiv ind = &pop->A[x];
		ind->Fitness = (slope * (ind->Fitness)) + offset;	// y = ax + b, translated adaptation value
		GlobalFitness += ind->Fitness;
  }
}

void generate() {
  int X ;	                  // crossover point
	for (int z = 0; z < PopSize; z += 2) {
	  int m1 = pick(&Pop1) ;	// select mate 1
		int m2 = pick(&Pop1) ;	// select mate 2
		X = crossover(&Pop1.A[m1].Gtype, &Pop1.A[m2].Gtype, &Pop2.A[z].Gtype, &Pop2.A[z+1].Gtype);
		updateIndiv(&Pop2.A[z], m1, m2, X);
		updateIndiv(&Pop2.A[z + 1], m1, m2, X);
  }
}

int crossover(Chromo * P1, Chromo * P2, Chromo * C1, Chromo * C2) {
  int X;	                        // crossover point to be returned

	if (flip(ProbCross)) X = randRange(1, K) * Bits; // coupé correctement
	else X = ChromSize;

	for (int k = 0; k < X; k++)	{
    C1->A[k] = mutate(P1->A[k]);	// child 1 from parent 1
		C2->A[k] = mutate(P2->A[k]);  // child 2 from parent 2
  }
	for (int k = X; k < ChromSize; k++) {
    C1->A[k] = mutate(P2->A[k]);	// child 1 from parent 2
		C2->A[k] = mutate(P1->A[k]);  // child 2 from parent 1
  }
	return X;
}

void updateIndiv(indiv ind, int m1, int m2, int X)	{
	ind->Ptype = decode(ind->Gtype);	              // phenotype
	ind->Fitness = objectiveFunc(ind->Ptype);	      // raw adaptation value
  free(ind->Ptype);
	ind->Parent_1 = m1;
	ind->Parent_2 = m2;
	ind->CrossPoint = X;
}

int pick(Population * pop) {
  double accumulator = 0;
	double limit = GlobalFitness * floatRand();	// random real number between 0 and 1
	int mate;
	for (mate = 0; mate < PopSize; mate++) {
    accumulator += pop->A[mate].Fitness;
		if (accumulator > limit) break;
  }
	return mate < PopSize ? mate : PopSize - 1;
}

allele mutate(allele bval) {
  if (flip(ProbMut)) bval = !bval;
	return (allele) bval;
}

boolean flip(double proba) {
  if (!(proba < 1)) return 1;	    // works in the range 0 É 1
  double x = floatRand();	        // enough precision ?
  return (boolean) (x <= proba);
}

int randRange(int low, int high) {
  return rand() % (high - low) + low ;
}

double floatRand() {
  return (double) (rand() % 1000) / 1000 ;
}
