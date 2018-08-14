#include "ga.h"

/**
 * VARIABLE GLOBAL :
 * @global MaxGen        maximum de generations
 * @global PopSize       taille population
 * @global Bits          nombre de bits pr coder l'information
 *                       - trouver automatiquement dans ga() -
 * @global ProbCross     probabilité crossover
 * @global ProbMut       probabilité mutation
 * @global GlobalFitness somme des valeurs d'aptitude
 * @global Average       moyenne des valeurs d'aptitude
 * @global MaxFit        maximum des valeurs d'aptitude
 * @global MinFit        minimum des valeurs d'aptitude
 * @global Pop1          population parent
 * @global Pop2          population enfant
 */
int MaxGen = 20, PopSize = 30, Bits;
double ProbCross = .95, ProbMut   = .02;
double GlobalFitness, Average, MaxFit, MinFit;
Population Pop1, Pop2;

doc *** GA(doc *docs, int nDoc, int * nClu) {
  assert(!(PopSize%2)); // si mvs réglage
  if (nDoc < 2)         // regroupe pas - de 2 doc
    usage("need more than one document!");

  // calculer le nombre de bits nécessaire
  Bits = (int) log2(nDoc-1)+1;

  // K varie entre 2 et 1/2 nombre de document
  const int minK = 2;
  const int maxK = (nDoc > 5) ? nDoc / 2 : 2;

  genPops(minK, maxK, docs, nDoc); // initialisation des populations
	for (int gen = 0 ; gen < MaxGen ; gen++) {
    statistics(&Pop1);
    scale(&Pop1);
    generate(docs, nDoc);
    report(gen);
    Pop1 = Pop2;
  }

  // récupérer le meilleur clustering trouvé
  indiv ind;
  for (int j = 0; j < PopSize; j++) {
    ind = &Pop1.A[j];
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
  int nIn[*nClu];
  memset(nIn, 0, *nClu * sizeof(int));

  int * ptype = decode(ind->Gtype, ind->Len); // calculer le ptype
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
  free(ptype);
  for (int i = 0; i < PopSize; i++) free((&Pop2.A[i])->Gtype.A);
  free(Pop2.A);

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

int * decode(Chromo Gtype, int K)	{
  int * ptype = (int *) malloc(K * sizeof(int));
  if (ptype == NULL) usage("error malloc in decode");

  for (int ki = 0; ki < K; ki++) {
    char * bitstr = &Gtype.A[ki * Bits];  // centres ki
    ptype[ki] = decodeSpec(bitstr);
  }
	return ptype;
}

int decodeSpec(char * spec) {
  int value = 0;
	for (int B = Bits - 1, power = 1 ; B >= 0 ; B--, power *= 2)
		value += spec[B] * power;
	return value;
}

void makeChromo(allele * chromo, int K, int nDoc) {
  for (int i = 0; i < K*Bits;) {
    int n = randRange(0, nDoc+1);         // nb aléatoire entre 0 et le nb de doc
    for (int c = Bits-1; c >= 0; c--) {   // converti en binaire
      int k = n >> c;
      chromo[i++] = (k & 1) ? 1 : 0;
    }
  }
}

void putchrom(Chromo Gtype, int K) {
  printf("[");
  for (int p = 0; p < K; p++) {
    char * bitstr = &(Gtype.A[p * Bits]);
    printf("%i%s", decodeSpec(bitstr), (p+1==K) ? "" : ", ");
  }
  printf("]");
}

void report(int gen) {
  printf("++Gen n° %i ; MinFit = %g ; Av = %g ; MaxFit = %g ; GF = %g\n", gen, MinFit, Average, MaxFit, GlobalFitness);
  for (int i = 0; i < PopSize; i++) {
    indiv ind = &Pop1.A[i];	// old string
    printf("(%03d) ", i);
    putchrom(ind->Gtype, ind->Len);
    printf(" score = %g\n", ind->Fitness);
  }
  puts(" ");
}

void allocPop(Population * pop, const int k) {
  pop->A = (individual *) malloc(PopSize * sizeof(individual));
  if (pop->A == NULL) usage("malloc error in allocPop");

  for (int i = 0; i < PopSize; i++) {
    indiv ind = &pop->A[i];
    ind->Gtype.A = (allele *) malloc((k * Bits) * sizeof(allele));
    if (ind->Gtype.A == NULL) usage("malloc error in genPops");
  }
}

void genPops(const int minK, const int maxK, doc *docs, int nDoc) {
  allocPop(&Pop2, maxK);
  Pop1 = Pop2;

  for (int i = 0; i < PopSize; i++) {
    int K = randRange(minK, maxK+1);      // taille aléatoire
    indiv ind1 = &Pop1.A[i]; // look out ~~~
		makeChromo(ind1->Gtype.A, K, nDoc);
		updateIndiv(ind1, 0, 0, 0, K, docs, nDoc);
  }
}

void statistics(Population * pop)	{
  MaxFit = 0;
  MinFit = 0;
  GlobalFitness = 0;
	for (int x = 0; x < PopSize; x++) {
	  indiv ind = &pop->A[x];
    if (ind->Fitness < 0) continue;       // indiv invalide
		GlobalFitness += ind->Fitness;
		if (ind->Fitness > MaxFit) MaxFit = ind->Fitness;
		if (!MinFit ||
        ind->Fitness < MinFit) MinFit = ind->Fitness;
  }
	Average = GlobalFitness / (double) PopSize;
}

void scale(Population * pop) {
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
    indiv ind = &pop->A[x];
    if (ind->Fitness < 0) continue; // ne pas s'occuper des indiv non valides
		ind->Fitness = (slope * (ind->Fitness)) + offset;	// y = ax + b, translated adaptation value
		GlobalFitness += ind->Fitness;
  }
}

void generate(doc *docs, int nDoc) {
  int X ;	                                // crossover point
  int * picked1 = pick(&Pop1);            // select mates
	for (int z = 0; z < PopSize; z += 2) {
    int m1 = picked1[z];	                // selected mate 1
		int m2 = picked1[z+1];                // selected mate 2
		X = crossover(
      &Pop1.A[m1].Gtype, &Pop1.A[m2].Gtype,
      &Pop2.A[z].Gtype, &Pop2.A[z+1].Gtype,
      Pop1.A[m1].Len, Pop1.A[m2].Len
    );
		updateIndiv(&Pop2.A[z], m1, m2, X, Pop1.A[m1].Len, docs, nDoc);
		updateIndiv(&Pop2.A[z + 1], m1, m2, X, Pop1.A[m2].Len, docs, nDoc);
  }
  free(picked1);
}

int crossover(Chromo * P1, Chromo * P2, Chromo * C1, Chromo * C2, int K1, int K2) {
  int X;	                                // point du crossover à retourner

  int Kmin = (K1 < K2) ? K1 : K2;
  int Kmax = (K1 >= K2) ? K1 : K2;

	if (flip(ProbCross))
    X = randRange(1, Kmin) * Bits;        // coupé correctement
	else X = Kmin*Bits;

	for (int k = 0; k < X; k++)	{           // 1er partie
    C1->A[k] = mutate(P1->A[k]);	        // enfant 1 depuis le parent 1
		C2->A[k] = mutate(P2->A[k]);          // enfant 2 depuis le parent 2
  }
	for (int k = X; k < Kmax * Bits; k++) {
    if (k < K2*Bits)                      // ne pas dépasser la taille du parent 2
      C1->A[k] = mutate(P2->A[k]);	      // enfant 1 depuis le parent 2
		if (k < K1*Bits)                      // "..." 1
      C2->A[k] = mutate(P1->A[k]);        // enfant 2 depuis le parent 1
  }
	return X;
}

void updateIndiv(indiv ind, int m1, int m2, int X, int newK, doc *docs, int nDoc)	{
	ind->Ptype = decode(ind->Gtype, newK);
	ind->Fitness = objectiveFunc(ind->Ptype, newK, docs, nDoc);
  free(ind->Ptype);
	ind->Parent_1 = m1;
	ind->Parent_2 = m2;
	ind->CrossPoint = X;
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

int * pick(Population * pop) {
  int * picked = NULL, npick = 0;
  picked = (int *) malloc(PopSize * sizeof(int));
  if (picked == NULL) usage("error malloc in pick");

  double expected[PopSize];               // nombre de copie attendu pr chq individu
  int N[PopSize];                         // partie entière de expected
  double sumR = 0;                        // somme des restes
  for (int i = 0; i < PopSize; i++) {
    if (pop->A[i].Fitness < 0) expected[i] = 0;
    else expected[i] = pop->A[i].Fitness / (GlobalFitness / PopSize);
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

allele mutate(allele bval) {
  if (flip(ProbMut)) bval = !bval;
	return (allele) bval;
}

boolean flip(double proba) {
  if (!(proba < 1)) return 1;
  double x = floatRand();
  return (boolean) (x <= proba);
}

int randRange(int low, int high) {
  return rand() % (high - low) + low ;
}

double floatRand() {
  return (double) (rand() % 1000) / 1000 ;
}
