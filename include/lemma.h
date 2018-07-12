#ifndef LEMMA_INCLUDED
#define LEMMA_INCLUDED

#include <string.h>

#include "utilities.h"
#include "tree.h"

void lemmatisation(char **words, int len, treeList stopList, treeList wordList);

void initLemma(const char *stopPath, treeList stopList, const char *wordPath, treeList wordList);

/**
 * Enléve le pluriel d'un mot en français
 * @param  word mot
 */
void rmFrPlural(char * word);

/**
 * Enléve le féminin d'un mot en français
 * @param  word mot
 */
void rmFrFem(char * word);

#endif
