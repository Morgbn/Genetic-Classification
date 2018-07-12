#include "lemma.h"

void initLemma(const char *stopPath, treeList stopList, const char *wordPath, treeList wordList) {
  // lire la liste des mots vides
  int len;
  char ** stopListStr = readFile(stopPath, &len, 0);
  for (int i = 0; i < len; i++) {
    addToTree(stopList, stopListStr[i], NULL);
  }
  // lire la liste de mot
  char ** wordListStr = readFile(wordPath, &len, 1);
  for (int i = 0; i < len; i++) {
    if (wordListStr[i][0] >= '0' && wordListStr[i][0] <= '9') { // nombre
      int n = atoi(wordListStr[i++]);
      char *infi = wordListStr[i++]; // infinitif
      for (int j = 0; j < n; j++)
        addToTree(wordList, wordListStr[i+j], infi); // conjugaison<->infinitif
      i += n-1;
    }
    else usage("error format file verb.txt");
  }
}

void lemmatisation(char **words, int len, treeList stopList, treeList wordList) {
  for (int i = 0; i < len; i++) {
    if (getNode(stopList, words[i])) { // mot vide
      words[i] = NULL;
      continue;
    }
    treeList node;
    if ((node = getNode(wordList, words[i]))) { // verbe
      words[i] = (char *) node->val;
      continue;
    }
    rmFrPlural(words[i]); // sinon enlever pluriel
    rmFrFem(words[i]);    // et enlever féminin
  }
}

void rmFrPlural(char * word) {
  int len = strlen(word)-1;

  if (len > 4) {
    if (word[len]=='x') { // ...aux
      if (word[len-1]=='u' && word[len-2]=='a') {
        word[len-1]='l'; // devient ...l
      }
      word[len]='\0';
    }
  }
  if (word[len]=='s') {
    word[len]='\0'; len--;
  }
}

void rmFrFem(char * word) {
  int len = strlen(word)-1;
  // if (word[len]=='r')
  //   { word[len]='\0'; len--; }
  if (word[len]=='e') // é
    { word[len]='\0'; len--; }
  if (word[len]=='e')
    { word[len]='\0'; len--; }
  if (word[len] == word[len-1])
    word[len]='\0';
}
