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
    printf("(%i) ", i);
    putchrom(ind->Gtype);
    printf(" score = %g\n", ind->Fitness);
  }
}

doc *** GA(doc *docs, int nDoc, int * nClu) {
  assert(!(PopSize%2));
  // const int minClu = 2;
  // const int maxClu = nDoc / 2;
  Bits = (int) log2(nDoc-1)+1;  // calculer le nombre de bits nécessaire
  K = 2;
  ChromSize = Bits * K;
  Docs = docs;
  NDoc = nDoc;

  genPops(docs);
	for (int gen = 0 ; gen < MaxGen ; gen++) {
    statistics(&Pop1);
    // scale(&Pop1);
    generate();
    report(gen);
    Pop1 = Pop2;
  }

  indiv ind;
  for (int j = 0; j < PopSize; j++) {
    ind = &Pop1.A[j];
    if (ind->Fitness == MaxFit) break; // le meilleur clustering trouvé
  }

  *nClu = K;
  // faire de la place pour la liste de liste de document
  doc ***clusters = (doc ***) malloc(K * sizeof(doc **));
  if (clusters == NULL) usage("error malloc in GA");
  for (int i = 0; i < K; i++) {
    clusters[i] = (doc **) malloc((nDoc+2) * sizeof(doc *)); // surement trop
    if (clusters[i] == NULL) usage("error malloc in GA");
  }
  int nIn[K];
  memset(nIn, 0, K * sizeof(int));

  int * ptype = decode(ind->Gtype);       // calculer le ptype
  for (int ki = 0; ki < K; ki++)          // mettre en tête les centres
    clusters[ki][nIn[ki]++] = &docs[ptype[ki]];

  for (int i = 0; i < nDoc; i++) {        // pr chq document
    int bestK = 0;
    double dmin = docs[i].dist[ptype[0]];
    for (int ki = 1; ki < K; ki++) {      // trouver sa distance minimal
      double d = docs[i].dist[ptype[ki]]; // avec l'un des centres
      if (d < dmin) {
        dmin = d;
        bestK = ki;
      }
    }
    if (ptype[bestK] == i) continue;      // déjà ajouter (au début)
    clusters[bestK][nIn[bestK]++] = &docs[i];
  }

  for (int i = 0; i < K; i++) clusters[i][nIn[i]] = NULL; // pr facilement detecter la fin
  free(ptype);

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
    double dj = 0;
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
    int n = randRange(0, NDoc+1);    // nb aléatoire entre 0 et le nb de doc
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
  int X ;	                     // crossover point
  int * picked1 = pick(&Pop1); // select mates
	for (int z = 0; z < PopSize; z += 2) {
    int m1 = picked1[z];	     // selected mate 1
		int m2 = picked1[z+1];     // selected mate 2
		X = crossover(&Pop1.A[m1].Gtype, &Pop1.A[m2].Gtype, &Pop2.A[z].Gtype, &Pop2.A[z+1].Gtype);
		updateIndiv(&Pop2.A[z], m1, m2, X);
		updateIndiv(&Pop2.A[z + 1], m1, m2, X);
  }
  free(picked1);
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

void shuffleVect(int * a, int len) {
  int tmp, n;

  for (int i = 0; i < len; i++) {
    n = randRange(0, len); // prend un index aléaléatoirement
    tmp  = a[i];
    a[i] = a[n];           // échange les valeurs a[i] et a[n]
    a[n] = tmp;
  }
}

int * pick(Population * pop) {
  int * picked = NULL, npick = 0;
  picked = (int *) malloc(PopSize * sizeof(int));
  if (picked == NULL) usage("error malloc in pick");

  double expected[PopSize]; // nombre de copie attendu pr chq individu
  int N[PopSize];           // partie entière de expected
  double sumR = 0;          // somme des restes
  for (int i = 0; i < PopSize; i++) {
    expected[i] = pop->A[i].Fitness / round(GlobalFitness / PopSize);
    N[i] = (int) expected[i];
    expected[i] -= N[i];    // laisser le reste dans expected
    sumR += expected[i];
  }

  // affecte automatiquement à chq individu Ni copies
  for (int i = 0; i < PopSize; i++)
    for (int j = 0; j < N[i]; j++) picked[npick++] = i;

  // recalcul probabilité avec ce qui reste
  for (int i = 0; i < PopSize; i++)
    if (expected[i]) expected[i] /= sumR;


  // roulette avec les restes pr compléter la population
  int c = npick;
  for (int i = 0; i < PopSize - c; i++) {
    double accumulator = 0;
    double limit = floatRand();	// random real number between 0 and 1
    int mate;
    for (mate = 0; mate < PopSize; mate++) {
      accumulator += expected[mate];
    	if (accumulator > limit) break;
    }
    picked[npick++] = (mate < PopSize) ? mate : PopSize - 1;
  }

  shuffleVect(picked, PopSize);
  return picked;
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
