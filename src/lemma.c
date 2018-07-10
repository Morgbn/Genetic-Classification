#include "lemma.h"

void initLemma(const char *stopPath, treeList stopList) {
  int len;
  char ** stopListStr = readFile(stopPath, &len);
  for (int i = 0; i < len; i++) {
    if (getNode(stopList, stopListStr[i])) continue;
    else addToTree(stopList, stopListStr[i], 1, -1);
  }
}

void lemmatisation(char **words, int len, treeList stopList) {
  for (int i = 0; i < len; i++) {
    if (getNode(stopList, words[i])) { // mot vide
      words[i] = NULL;
      continue;
    }
    rmFrPlural(words[i]);
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
    else {
      if (word[len]=='s')
        { word[len]='\0'; len--; }
      // if (word[len]=='r')
      //   { word[len]='\0'; len--; }
      if (word[len]=='e')
        { word[len]='\0'; len--; }
      if (word[len]=='e') // é
        { word[len]='\0'; len--; }
      if (word[len] == word[len-1])
        word[len]='\0';
    }
  }
}
