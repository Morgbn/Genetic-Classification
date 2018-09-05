#include "lemma.h"

void initLemma(const char *stopPath, treeList stopList, const char *wordPath, treeList wordList) {
  // lire la liste des mots vides
  int len;
  char ** stopListStr = readFile(stopPath, &len, 0);
  for (int i = 0; i < len; i++) {
    addToTree(stopList, stopListStr[i], NULL, 0);
    free(stopListStr[i]);
  }
  free(stopListStr);
  // lire la liste de mot
  char ** wordListStr = readFile(wordPath, &len, 1);
  for (int i = 0; i < len; i++) {
    if (wordListStr[i][0] >= '0' && wordListStr[i][0] <= '9') { // nombre
      int n = atoi(wordListStr[i]);
      char *infi = wordListStr[++i]; // infinitif
      for (int j = 0; j < n+1; j++)
        addToTree(wordList, wordListStr[i+j], strdup(infi), 1); // conjugaison<->infinitif
      i += n;
    }
    else usage("error format file verb.txt");
  }
  for (int i = 0; i < len; i++) free(wordListStr[i]);
  free(wordListStr);
}

void lemmatisation(char **words, int len, treeList stopList, treeList wordList) {
  for (int i = 0; i < len; i++) {
    if (getNode(stopList, words[i])) { // mot vide
      free(words[i]);
      words[i] = NULL;
      continue;
    }
    treeList node = NULL;
    if ((node = getNode(wordList, words[i])) && node->val) { // verbe conjugué
      free(words[i]);
      words[i] = strdup((char *) node->val); // remplacé par infinitif
      continue;
    }
    rmFrPlural(words[i]);        // sinon enlever pluriel
    int len = rmFrFem(words[i]); // et enlever féminin
    if (len < 2) {
      free(words[i]);
      words[i] = NULL;
    }
  }
}

int rmFrPlural(char * word) {
  int len = strlen(word)-1;

  if (len > 4) {
    if (word[len]=='x') { // ...aux
      if (word[len-1]=='u' && word[len-2]=='a') {
        word[len-1]='l'; // devient ...l
      }
      word[len]='\0'; len--;
    }
  }
  if (len > 0 && word[len]=='s') {
    word[len]='\0'; len--;
  }
  return len +1;
}

int rmFrFem(char * word) {
  int len = strlen(word)-1;
  if (len > 0 && word[len]=='e') // é
    { word[len]='\0'; len--; }
  if (len > 0 && word[len]=='e')
    { word[len]='\0'; len--; }
  if (len > 0 && word[len] == word[len-1])
    { word[len]='\0'; len--; }
  return len +1;
}

void applyLuhn(treeList head, float min, float max) {
  for (int j = 0; j < head->nChilds; j++)
    applyLuhn(head->childs[j], min, max);
  if (head->val != NULL) {
    if (*(float *) head->val < min
     || *(float *) head->val > max)  {      // freq en dehors du seuil
      free(head->val);
      head->val = NULL;
    }
  }
}
