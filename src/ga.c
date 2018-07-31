#include "ga.h"

double GlobalFitness, Average, MaxFit, MinFit;
Population Pop1, Pop2; // for swapping

/** @global MaxGen maximum de generations */
short int MaxGen = 50;
/** @global ProbCross probabilité crossover */
double ProbCross = .95;
/** @global ProbMut probabilité mutation */
double ProbMut = .02;
/** @global PopSize taille population */
int PopSize = 30;
// !:::::: temporaire ::::!
int ChromSize = 18;

doc *** GA(doc *docs, int nDoc, int * nClu) {
  // const int minClu = 2;
  // const int maxClu = nDoc / 2;

  genPops() ;
	for (int gen = 0 ; gen < MaxGen ; gen ++) {
    scale(&Pop1);
    generate();
    Pop1 = Pop2;
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

double objectiveFunc(double x) {	// improve contrast
  const int N = 10;
	const double coeff = pow(2, ChromSize / 3);
	return pow(x / coeff, N);
}

double decode(Chromo Gtype)	{
  int score = 1;
  // int spec, score = 0, true_score = 0;
	// int selected[Classifiers] = {0};	// mark if already selected
	// int scored[StringVals] = {0};	    // mark pattern
	// for (int performer = 0; performer < Classifiers; performer++)	// for each performer
	// {	bstring BitString = &Gtype.A[performer * Bits] ;	// point to bitstring
	// 	spec = decode_spec(BitString, Bits) ;	// get his specialty
	// 	if (scored[spec])	// already used ?
	// 		score -= GainRate * scored[spec] ;	// not that good a score
	// 	else// free field
	// 	{	if (! (spec > MaxStringVal))	// a valid field ?
	// 		{	selected[performer] = spec ;	// remember that
	// 			true_score += yield[performer][spec] ;	// accumulate
	// 			score += GainRate * yield[performer][spec] ; } }	// accumulate
	// 	scored[spec]++ ; }	// increment that field anyway...
	// if (true_score > worth) worth = true_score ;
	// if (dump) dump_score(selected, true_score, Gtype) ;
	return score ;
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

		for (int c = 0; c < ChromSize; c++)
		  ind1->Gtype.A[c] = flip(0.5);	// random bit [0, 1]
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

	if (flip(ProbCross)) X = randRange(1, ChromSize);
	else X = ChromSize ;

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
