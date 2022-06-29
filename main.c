#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gera_codigo.h"

typedef int (*funcp)(int x);
void gera_codigo(FILE *f, unsigned char code[], funcp *entry);

int main(int argc, char *argv[]) {
  FILE *fp;
  funcp funcLBS;
  int res;
  unsigned char code[300];
  int p0;
  /* Abre o arquivo para leitura */
  fp = fopen("funcao.txt", "r");
  /* Gera o codigo */
  gera_codigo(fp, code, &funcLBS);
  if (funcLBS == NULL) {
    printf("Erro na geracao\n");
  }
  for (int c = 0; c < 300; c++) {
    //printf("%x\n",code[c]);
  }
  /* Chama a função gerada */
  p0 = 21;
  res = (*funcLBS)(p0);
  printf("f(%d) = %d\n", p0, res);

  fclose(fp);
}
