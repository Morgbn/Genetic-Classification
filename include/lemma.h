#ifndef LEMMA_INCLUDED
#define LEMMA_INCLUDED

#include <string.h>

#include "utilities.h"
#include "tree.h"

void lemmatisation(char **words, int len, treeList stopList);

void initLemma(const char *stopPath, treeList stopList);

/**
 * Enléve le pluriel et le féminin des mots Fr
 * http://members.unine.ch/jacques.savoy/clef/frenchStemmer.txt
 * @param  word mot
 */
void rmFrPlural(char * word);


#endif
