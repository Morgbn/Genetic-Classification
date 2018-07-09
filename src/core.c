#include "core.h"

void cleanStr(unsigned char * str) {
  int k = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == 195 && str[i+1] != '\0') { // sur 2 octets
      switch (str[i+1]) {
        // Diacritiques et ligatures ordonnés par frèquence d'apparission
        case 169: case 137: str[k] = 'e'; break; // é É
        case 168: case 136: str[k] = 'e'; break; // è È
        case 160: case 129: case 128: str[k] = 'a'; break; // à Á À
        case 170: case 138: str[k] = 'e'; break; // ê Ê
        case 174: case 142: str[k] = 'i'; break; // î Î
        case 167: case 135: str[k] = 'c'; break; // ç Ç
        case 162: case 130: str[k] = 'a'; break; // â Â
        case 180: case 148: str[k] = 'o'; break; // ô Ô
        case 187: case 155: str[k] = 'u'; break; // û Û
        case 185: case 153: str[k] = 'u'; break; // ù Ù
        case 175: case 143: str[k] = 'i'; break; // ï Ï
        case 171: case 139: str[k] = 'e'; break; // ë Ë
        case 164: case 132: str[k] = 'a'; break; // ä Ä
        case 182: case 150: str[k] = 'o'; break; // ö Ö
        case 188: case 156: str[k] = 'u'; break; // ü Ü
        case 191: str[k] = 'y'; break; // ÿ
        case 166: str[k++] = 'a'; str[k] = 'e'; break; // æ
      }
      i++;
    }
    else if (str[i] == 197 && str[i+1] != '\0') { // sur 2 octets
      switch (str[i+1]) {
        case 147: str[k++] = 'o'; str[k] = 'e'; break; // œ
        case 184: str[k] = 'y'; break; // Ÿ
      }
      i++;
    }
    else if (str[i] >= 65 && str[i] <= 90) {  // majuscule [A-Z]
      str[k] = str[i] + 32;                   // vers minuscule
    }
    else if (str[i] >= 97 && str[i] <= 122) { // déjà en minuscule [a-z]
      str[k] = str[i];                        // le garder
    }
    else str[k] = ' '; // autre char spéciaux & nombres remplacé par ' '
    k++;
  }
  str[k] = '\0';
}

doc * getData(const char * path, int * len) {
  struct dirent *file;
  DIR *dr = opendir(path);
  if (dr == NULL) usage("Problème avec le dossier.");

  int nDoc = 0;
  doc *data = NULL;
  while ((file = readdir(dr)) != NULL) {
    if ( file->d_type != DT_REG ) continue; // dossier → passer
    char pathname[256];
    sprintf(pathname, "%s%s", path, file->d_name);

    int nTerm;
    char **terms = readFile(pathname, &nTerm);

    treeList node, aDoc = initTree();
    for (int i = 0; i < nTerm; i++) {
      if ((node = getNode(aDoc, terms[i])))  // déjà dedans
        node->val++;                         // → ajouter une occurrence
      else addToTree(aDoc, terms[i], 1, -1); // sinon l'ajouter
    }
    divideAllTreeBy(aDoc, nTerm);            // ÷ le nb d'occurrence par le nb de terme
    addDoc(&data, &nDoc, pathname, aDoc);     // l'ajouter aux données
  }

  closedir(dr);
  *len = nDoc;
  return data;
}

char ** readFile(char * pathname, int *size) {
  FILE *fp = fopen (pathname, "r");
  if (fp == NULL) usage("error reading a file");

  unsigned char buf[1024];
  int len = 0;
  char **terms = malloc(1 * sizeof(char *));
  if (terms == NULL) usage("error malloc in readFile");

  while (fscanf(fp, "%s", buf) != EOF) { // lire du fr, avec des accents
    cleanStr(buf);
    char *ch;
    ch = strtok((char *) buf, " "); // redécouper les espaces
    while (ch != NULL) {
      terms = (char**) realloc(terms, ++len * sizeof(char *));
      if (terms == NULL) usage("error realloc in readFile");
      terms[len-1] = malloc(strlen(ch) + 1);
      if (terms[len-1] == NULL) usage("error malloc in readFile");
      strcpy(terms[len-1], ch);
      ch = strtok(NULL, " ,");
    }
  }
  *size = len;
  fclose(fp);
  return terms;
}

void divideAllTreeBy(treeList tree, int n) {
  if (tree->val > 0) tree->val /= n;
  for (int i = 0; i < tree->nChilds; i++)
    divideAllTreeBy(tree->childs[i], n);
}

void addDoc(doc **data, int * len, char * name, treeList terms) {
  *len += 1;
  *data = (doc *) realloc(*data, *len * sizeof(doc));
  if (data == NULL) usage("error realloc in addDoc");
  // cutName = name;
  (*data)[*len-1].name = strdup(name);
  (*data)[*len-1].terms = terms;
}
