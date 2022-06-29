/*
  Luca Ribeiro - 2112662 - 3WA
  Thiago Becker - 2110449 - 3WA
  Nota: 10.0
*/

#include <stdio.h>
#include <stdlib.h>
#include "gera_codigo.h"

typedef int (*funcp)(int x);

void gera_codigo(FILE *f, unsigned char code[], funcp *entry)
{
  int line = 1;
  int c;
  int posCode = 0;
  int j = 0; /* contador das posições do vetor de funções */
  unsigned long int funcPos[20]; /* vetor auxiliar para guardar posição das funções */
  unsigned long int auxPos = 0; /* variável auxiliar para verificar o endereço de cada uma das funções lidas */
  unsigned long int aux1, aux2;

  while ((c = fgetc(f)) != EOF)
  {
    switch (c)
    {
    case 'f':
    { /* function */
      char c0;
      fscanf(f, "unction%c", &c0);

      /* preparaçãodo registro de ativação */
      code[posCode++] = 0x55;
      code[posCode++] = 0x48;
      code[posCode++] = 0x89;
      code[posCode++] = 0xe5;

      /* abrir espaço de 32 bytes na pilha */
      code[posCode++] = 0x48;
      code[posCode++] = 0x83;
      code[posCode++] = 0xec;
      code[posCode++] = 0x20;

      funcPos[j++] = auxPos;

      auxPos += 8;

      break;
    }
    case 'e':
    { /* end */
      char c0;
      fscanf(f, "nd%c", &c0);
      
      break;
    }
    case 'r':
    { /* retorno incondicional */
      int idx0;
      char var0;
      fscanf(f, "et %c%d", &var0, &idx0);
      
      if (var0 == '$')
      {
        code[posCode++] = 0xb8; 
        
        /* cálculo da constante em little endian */
        for (int i = 0; i < 4; i++, posCode++)
        {
          code[posCode] = idx0 & 0xff;
          idx0 = idx0 >> 8;
        }
        
        auxPos += 5;
      }
      else if (var0 == 'v')
      {
        code[posCode++] = 0x8b;
        code[posCode++] = 0x45;
        code[posCode++] = 0xec + 4 * idx0;

        auxPos += 3;
      }
      else if (var0 == 'p')
      {
        code[posCode++] = 0x89;
        code[posCode++] = 0xf8;

        auxPos += 2;
      }

      /* comandos leave e ret */
      code[posCode++] = 0xc9;
      code[posCode++] = 0xc3;

      auxPos += 2;
      break;
    }
    case 'z':
    { /* retorno condicional */
      int idx0, idx1;
      char var0, var1;
      fscanf(f, "ret %c%d %c%d", &var0, &idx0, &var1, &idx1);

      if (var0 == 'p')
      {
        /* compara p0 com 0 */
        code[posCode++] = 0x83;
        code[posCode++] = 0xff;
        code[posCode++] = 0x00;

        auxPos += 3;
      }
      else if (var0 == 'v')
      {
        /* compara variável com 0 */
        code[posCode++] = 0x83;
        code[posCode++] = 0x7d;
        code[posCode++] = 0xec + 4 * idx0;
        code[posCode++] = 0x00;

        auxPos += 4;
      }
      else if (var0 == '$')
      {
        /* move constante para %ecx */
        code[posCode++] = 0xb9;
        for (int i = 0; i < 4; i++, posCode++)
        {
          code[posCode] = idx0 & 0xff;
          idx0 = idx0 >> 8;
        }

        /* compara %ecx com 0 */
        code[posCode++] = 0x83;
        code[posCode++] = 0xf9;
        code[posCode++] = 0x00;

        auxPos += 8;
      }

      if (var1 == 'p')
      {
        /* jne para próxima instrução */
        code[posCode++] = 0x75;
        code[posCode++] = 0x04;

        /* retorna p0 */
        code[posCode++] = 0x89;
        code[posCode++] = 0xf8;

        auxPos += 4;
      }
      else if (var1 == 'v')
      {
        /* jne para próxima instrução */
        code[posCode++] = 0x75;
        code[posCode++] = 0x05;

        /* retorna variável */
        code[posCode++] = 0x8b;
        code[posCode++] = 0x45;
        code[posCode++] = 0xec + 4 * idx1;

        auxPos += 5;
      }
      else if (var1 == '$')
      {
        /* jne para próxima instrução */
        code[posCode++] = 0x75;
        code[posCode++] = 0x07;

        /* retorna constante */
        code[posCode++] = 0xb8;
        for (int i = 0; i < 4; i++, posCode++)
        {
          code[posCode] = idx1 & 0xff;
          idx1 = idx1 >> 8;
        }

        auxPos += 7;
      }

      /* leave e ret */
      code[posCode++] = 0xc9;
      code[posCode++] = 0xc3;

      auxPos += 2;
      break;
    }
    case 'v':
    { /* atribuicao */
      int idx0;
      char c0;
      fscanf(f, "%d = %c", &idx0, &c0);
      
      if (c0 == 'c')
      { /* call */
        int g, idx1;
        char var1;
        fscanf(f, "all %d %c%d\n", &g, &var1, &idx1);

        /* guarda valor de p0 na pilha */
        code[posCode++] = 0x89;
        code[posCode++] = 0x7d;
        code[posCode++] = 0xe8;

        auxPos += 3;

        if (var1 == 'v') 
        {
          code[posCode++] = 0x8b;
          code[posCode++] = 0x7d;
          code[posCode++] = 0xec + 4 * idx1;

          auxPos += 3;
        }
        else if (var1 == '$') 
        {
          code[posCode++] = 0xbf;

          for (int i = 0; i < 4; i++, posCode++)
          {
            code[posCode] = idx1 & 0xff;
            idx1 = idx1 >> 8;
          }

          auxPos += 5;
        }

        /* call de função */
        code[posCode++] = 0xe8;

        aux1 = funcPos[g]; 
        aux2 = auxPos + 5;

        aux1 -= aux2;

        for (int i = 0; i < 4; i++, posCode++)
        {
          code[posCode] = aux1 & 0xff;
          aux1 = aux1 >> 8;
        }

        /* devolve valor de p0 */
        code[posCode++] = 0x8b;
        code[posCode++] = 0x7d;
        code[posCode++] = 0xe8;

        /* atribui retorno da função à variável */
        code[posCode++] = 0x89;
        code[posCode++] = 0x45;
        code[posCode++] = 0xec + 4 * idx0;

        auxPos += 11;
      }
      else
      { /* operação aritmética */
        int idx1, idx2;
        char var1 = c0, var2, op;
        fscanf(f, "%d %c %c%d", &idx1, &op, &var2, &idx2);

        /* mover primeiro operando para %ecx */
        if (var1 == 'p')
        {
          code[posCode++] = 0x89;
          code[posCode++] = 0xf9;

          auxPos += 2;
        }
        else if (var1 == 'v')
        {
          code[posCode++] = 0x8b;
          code[posCode++] = 0x4d;
          code[posCode++] = 0xec + 4 * idx1;

          auxPos += 3;
        }
        else if (var1 == '$')
        {
          code[posCode++] = 0xb9;
          for (int i = 0; i < 4; i++, posCode++)
          {
            code[posCode] = idx1 & 0xff;
            idx1 = idx1 >> 8;
          }

          auxPos += 5;
        }

        if (op == '+')
        {
          /* adicionar segundo operando */
          if (var2 == 'p')
          {
            code[posCode++] = 0x01;
            code[posCode++] = 0xf9;

            auxPos += 2;
          }
          else if (var2 == 'v')
          {
            code[posCode++] = 0x03;
            code[posCode++] = 0x4d;
            code[posCode++] = 0xec + 4 * idx2;

            auxPos += 3;
          }
          else if (var2 == '$')
          {
            code[posCode++] = 0x81;
            code[posCode++] = 0xc1;

            for (int i = 0; i < 4; i++, posCode++)
            {
              code[posCode] = idx2 & 0xff;
              idx2 = idx2 >> 8;
            }
            
            auxPos += 6;
          }
        }

        else if (op == '-')
        {
          /* subtrair segundo operando */
          if (var2 == 'p')
          {
            code[posCode++] = 0x29;
            code[posCode++] = 0xf9;

            auxPos += 2;
          }
          else if (var2 == 'v')
          {
            code[posCode++] = 0x2b;
            code[posCode++] = 0x4d;
            code[posCode++] = 0xec + 4 * idx2;

            auxPos += 3;
          }
          else if (var2 == '$')
          {
            code[posCode++] = 0x81;
            code[posCode++] = 0xe9;

            for (int i = 0; i < 4; i++, posCode++)
            {
              code[posCode] = idx2 & 0xff;
              idx2 = idx2 >> 8;
            }

            auxPos += 6;
          }
        }

        else if (op == '*')
        {
          /* multiplicar segundo operando */
          if (var2 == 'p')
          {
            code[posCode++] = 0x0f;
            code[posCode++] = 0xaf;
            code[posCode++] = 0xcf;

            auxPos += 3;
          }
          else if (var2 == 'v')
          {
            code[posCode++] = 0x0f;
            code[posCode++] = 0xaf;
            code[posCode++] = 0x4d;
            code[posCode++] = 0xec + 4 * idx2;

            auxPos += 4;
          }
          else if (var2 == '$')
          {
            code[posCode++] = 0x69;
            code[posCode++] = 0xc9;

            for (int i = 0; i < 4; i++, posCode++)
            {
              code[posCode] = idx2 & 0xff;
              idx2 = idx2 >> 8;
            }

            auxPos += 6;
          }
        }

        /* move %ecx para variável */
        code[posCode++] = 0x89;
        code[posCode++] = 0x4d;
        code[posCode++] = 0xec + 4 * idx0;

        auxPos += 3;
      }

      break;
    }
    }
    line++;
    fscanf(f, " ");
  }

  /* endereço da última função do arquivo */
  *entry = (funcp)(code + funcPos[j - 1]);
}
