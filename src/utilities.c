#include "utilities.h"

char ** readFile(const char * pathname, int *size, int keepNumber) {
  FILE *fp = fopen (pathname, "r");
  if (fp == NULL) usage("error reading a file");

  unsigned char buf[1024];
  int len = 0;
  char **terms = (char **) malloc(1 * sizeof(char *));
  if (terms == NULL) usage("error malloc in readFile");

  while (fscanf(fp, "%s", buf) != EOF) { // lire du fr, avec des accents
    cleanStr(buf, keepNumber);
    char *ch;
    ch = strtok((char *) buf, " "); // redécouper les espaces
    while (ch != NULL) {
      terms = (char**) realloc(terms, ++len * sizeof(char *));
      if (terms == NULL) usage("error realloc in readFile");
      terms[len-1] = (char *) malloc(strlen(ch) + 1);
      if (terms[len-1] == NULL) usage("error malloc in readFile");
      strcpy(terms[len-1], ch);
      ch = strtok(NULL, " ,");
    }
  }
  *size = len;
  fclose(fp);
  return terms;
}

void cleanStr(unsigned char * str, int keepNumber) {
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
    else if (str[i] >= '0' && str[i] <= '9') { // nombres
      if (keepNumber) str[k] = str[i];
      else str[k] = ' ';
    }
    else str[k] = ' '; // autre char spéciaux remplacé par ' '
    k++;
  }
  str[k] = '\0';
}

void usage(const char * error) {
  fprintf(stderr, "%s\n", error); // affiche le message
  exit(1);                        // quitter
}
