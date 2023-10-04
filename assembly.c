#include "assembly.h"

const char *reg_4[5] = {"null", "%r8d", "%r9d", "%r10d", "%r11d"}; // Exemplo: *reg_4[1] = '%r8d' => Registrador de 4 bytes para a primeira variável de registrador
const char *reg_8[5] = {"null", "%r8", "%r9", "%r10", "%r11"};     // Exemplo: *reg_8[3] = '%r10' => Registrador de 8 bytes para a terceira variável de registrador
const char *params_8[4] = {"null", "%rdi", "%rsi", "%rdx"};        // Exemplo: *params_8[3] = '%rdx' => Registrador de 8 bytes para o terceiro parâmetro
const char *params_4[4] = {"null", "%edi", "%esi", "%edx"};        // Exemplo: *params_4[1] = '%edi' => Registrador de 4 bytes para o primeiro parâmetro
const char ini[] = "pushq %rbp\n  movq %rsp, %rbp";                // Início padrão das funções
const char fim[] = "leave\n  ret\n";                               // Fim padrão das funções
const char *opRelacionais[12] = {
    "eq",  // BLP
    "jne", // Assembly
    "ne",  // BLP
    "je",  // Assembly
    "lt",  // BLP
    "jg",  // Assembly
    "le",  // BLP
    "jge", // Assembly
    "gt",  // BLP
    "jl",  // Assembly
    "ge",  // BLP
    "jle", // Assembly
};

void remove_newline(char *ptr)
{
  while (*ptr)
  {
    if (*ptr == '\n')
      *ptr = 0;
    else
      ptr++;
  }
}

// --------------------------------------------------------------------
// -----------------------------FUNÇÃO---------------------------------
// --------------------------------------------------------------------

// Identifica quantos parâmetros tem a função, depois atribui o tipo, número e flag de cada um deles
void function(int r, int f_num, char c1, int p1_num, char c2, int p2_num, char c3, int p3_num, P *parametros)
{
  switch (r)
  {
  case 1: // Função sem parâmetros
    printf("  .globl f%d\nf%d:\n  %s\n", f_num, f_num, ini);
    break;

  case 3: // Função com 1 parâmetros
    printf("  .globl f%d\nf%d:\n  %s\n", f_num, f_num, ini);

    parametros[0].tipo = c1;
    parametros[0].numero = p1_num;
    parametros[0].emUso = true;
    break;

  case 5: // Função com 2 parâmetros
    printf("  .globl f%d\nf%d:\n  %s\n", f_num, f_num, ini);

    parametros[0].tipo = c1;
    parametros[0].numero = p1_num;
    parametros[0].emUso = true;
    parametros[1].tipo = c2;
    parametros[1].numero = p2_num;
    parametros[1].emUso = true;
    break;

  case 7: // Função com 3 parâmetro
    printf("  .globl f%d\nf%d:\n  %s\n", f_num, f_num, ini);

    parametros[0].tipo = c1;
    parametros[0].numero = p1_num;
    parametros[0].emUso = true;
    parametros[1].tipo = c2;
    parametros[1].numero = p2_num;
    parametros[1].emUso = true;
    parametros[2].tipo = c3;
    parametros[2].numero = p3_num;
    parametros[2].emUso = true;
    break;

  default:
    break;
  }
}

// Função para ajustes
void reset_all(P *parametros, VI *varInt, VA *varArr, VR *varReg)
{
  for (int i = 0; i < MAX_VAR; i++)
  {
    if (i <= MAX_PARAM)
    {
      parametros[i].emUso = false;
    }
    varInt[i].emUso = false;
    varArr[i].emUso = false;
    varReg[i].emUso = false;
  }
}

// --------------------------------------------------------------------
// ----------------------ALOCAÇÃO DAS VARIAVEIS------------------------
// --------------------------------------------------------------------

void push_var(int *pilha, VI *varInt, int vN)
{
  pilha[(vN - 1) + 3] = 4; // + 3 por conta dos parâmetros

  int offset = 0;
  for (int i = 0; i < vN + 3; i++)
  {
    offset = offset + pilha[i];
  }

  for (int i = 0; i < MAX_VAR; i++)
  {
    // Encontra a primeira variável livre
    if (!varInt[i].emUso)
    {
      varInt[i].emUso = true;
      varInt[i].numero = vN;
      varInt[i].offset = offset;
      break;
    }
  }

  printf("  # vi%d: -%d\n", vN, offset);
  return;
}

void push_vet(int *pilha, VA *varArr, int vN, int sizevet)
{
  pilha[(vN - 1) + 3] = sizevet * 4; // + 3 por conta dos parâmetros

  int offset = 0;
  for (int i = 0; i < vN + 3; i++)
  {
    offset = offset + pilha[i];
  }

  for (int i = 0; i < MAX_VAR; i++)
  {
    // Encontra a primeira variavel de array livre
    if (!varArr[i].emUso)
    {
      varArr[i].emUso = true;
      varArr[i].numero = vN;
      varArr[i].offset = offset;
      varArr[i].size = sizevet;
      break;
    }
  }

  printf("  # va%d: -%d\n", vN, offset);
  return;
}

void push_reg(int *pilha, VR *varReg, int vN)
{
  for (int i = 0; i < vN + 3; i++) // + 3 por conta dos parâmetros
    pilha[(vN - 1) + 3] = 8;

  int offset = 0;
  for (int i = 0; i < vN + 3; i++)
  {
    offset = offset + pilha[i];
  }

  for (int i = 0; i < MAX_VAR; i++)
  {
    // Encontra o primero registrador livre
    if (!varReg[i].emUso)
    {
      varReg[i].emUso = true;
      varReg[i].numero = vN;
      varReg[i].offset = offset;
      strcpy(varReg[i].nome, reg_4[i + 1]);
      strcpy(varReg[i].nome_8, reg_8[i + 1]);

      printf("  # vr%d: -%d -> %s\n", vN, offset, varReg[i].nome);
      break;
    }
  }
  return;
}

// Reserva espaço para os parâmetros na pilha e calcula o offset de cara parâmetro
void push_parametros(int *pilha, P *parametros)
{
  printf("\n");
  for (int i = 0; i < MAX_PARAM; i++)
  {
    if (parametros[i].emUso)
    {
      pilha[parametros[i].numero - 1] = 8;

      int offset = 0;
      for (int j = 0; j < MAX_PARAM; j++)
      {
        offset = offset + pilha[j];
      }

      parametros[i].offset = offset;

      if (parametros[i].tipo == 'i')
      {
        strcpy(parametros[i].reg, params_4[parametros[i].numero]);
      }
      else
      {
        strcpy(parametros[i].reg, params_8[parametros[i].numero]);
      }

      printf("  # p%c%d: -%d -> %s\n", parametros[i].tipo, parametros[i].numero, parametros[i].offset, parametros[i].reg);
    }
  }
}

// --------------------------------------------------------------------
// ------------------------------PILHA---------------------------------
// --------------------------------------------------------------------

// Atribui 0 a todas as posições da pilha
void limpa_pilha(int *pilha)
{
  for (int i = 0; i < MAX_PILHA; i++)
  {
    pilha[i] = 0;
  }
}

// Responsável por alocar o espaço necessário na pilha (subq $x, %rsp)
void aloca_pilha(int *pilha)
{
  int tamanhoPilha = 0;

  // Soma todo o espaço da pilha
  for (int i = 0; i < MAX_PILHA; i++)
  {
    tamanhoPilha += pilha[i];
  }

  // Força o tamanho da pilha ser múltiplo de 16
  while (tamanhoPilha % 16 != 0)
  {
    tamanhoPilha += 4;
  }

  printf("  subq $%d, %%rsp\n\n", tamanhoPilha);
}

// --------------------------------------------------------------------
// -----------------------------SELEÇÃO--------------------------------
// --------------------------------------------------------------------

VI encontra_Vi(int n, VI *varInt)
{
  for (int i = 0; i < MAX_VAR; i++)
  {
    if (varInt[i].numero == n)
    {
      return varInt[i];
    }
  }
}

VA encontra_Va(int n, VA *varArr)
{
  for (int i = 0; i < MAX_VAR; i++)
  {
    if (varArr[i].numero == n)
    {
      return varArr[i];
    }
  }
}

VR encontra_Vr(int n, VR *varReg)
{
  for (int i = 0; i < MAX_VAR; i++)
  {
    if (varReg[i].numero == n)
    {
      return varReg[i];
    }
  }
}

P encontra_P(int n, P *parametros)
{
  for (int i = 0; i < MAX_PARAM; i++)
  {
    if (parametros[i].numero == n)
    {
      return parametros[i];
    }
  }
}

// --------------------------------------------------------------------
// ---------------------------ATRIBUIÇÕES------------------------------
// --------------------------------------------------------------------

void atribuicao_simples(int r, char c1, int n1, char c2, char c3, int n2, P *parametros, VI *varInt, VR *varReg)
{
  // Atribuição a uma variavel inteira
  if (c1 == 'i')
  {
    // Recebe outra variável
    if (c2 == 'v')
    {
      // Variável inteira
      if (c3 == 'i')
      {
        VI ret = encontra_Vi(n1, varInt);
        VI ret_2 = encontra_Vi(n2, varInt);
        printf("  # vi%d = vi%d\n", n1, n2);
        printf("  movl -%d(%%rbp), %%ecx\n", ret_2.offset);
        printf("  movl %%ecx, -%d(%%rbp)\n\n", ret.offset);
      }
      // Variável de registrador
      if (c3 == 'r')
      {
        VI ret = encontra_Vi(n1, varInt);
        VR ret_reg = encontra_Vr(n2, varReg);
        printf("  # vi%d = vr%d\n", n1, n2);
        printf("  movl %s, -%d(%%rbp)\n\n", ret_reg.nome, ret.offset);
      }
    }
    // Recebe um parâmetro
    if (c2 == 'p')
    {
      VI ret = encontra_Vi(n1, varInt);
      P ret_p = encontra_P(n2, parametros);
      printf("  # vi%d = pi%d\n", n1, n2);
      printf("  movl %s, -%d(%%rbp)\n\n", ret_p.reg, ret.offset);
    }
    // Recebe uma constante
    if (c2 == 'c')
    {
      VI ret = encontra_Vi(n1, varInt);
      printf("  # vi%d = ci%d\n", n1, n2);
      printf("  movl $%d, -%d(%%rbp)\n\n", n2, ret.offset);
    }
  }

  // Atribuição a uma variável de registrador
  if (c1 == 'r')
  {
    // Recebe uma variável
    if (c2 == 'v')
    {
      // Variável inteira
      if (c3 == 'i')
      {
        VR ret_reg = encontra_Vr(n1, varReg);
        VI ret_vi = encontra_Vi(n2, varInt);
        printf("  # vr%d = vi%d\n", n1, n2);
        printf("  movl -%d(%%rbp), %s\n\n", ret_vi.offset, ret_reg.nome);
      }
      // Variável de registrador
      if (c3 == 'r')
      {
        VR ret_1 = encontra_Vr(n1, varReg);
        VR ret_2 = encontra_Vr(n2, varReg);
        printf("  # vr%d = vr%d\n", n1, n2);
        printf("  movl %s, %s\n\n", ret_2.nome, ret_1.nome);
      }
    }
    // Recebe um parâmetro
    if (c2 == 'p')
    {
      VR ret_reg = encontra_Vr(n1, varReg);
      P ret_par = encontra_P(n2, parametros);
      printf("  # vr%d = pi%d\n", n1, n2);
      printf("  movl %s, %s\n\n", ret_par.reg, ret_reg.nome);
    }
    // Recebe uma constante
    if (c2 == 'c')
    {
      VR ret = encontra_Vr(n1, varReg);
      printf("  # vr%d = ci%d\n", n1, n2);
      printf("  movl $%d, %s\n\n", n2, ret.nome);
    }
  }
}

void atribuicao_aritimetica(int r, char c1, int n1, char c2, char c3, int n2, char op, char c4, char c5, int n3, P *parametros, VI *varInt, VR *varReg)
{
  printf("  # v%c%d = %c%c%d %c %c%c%d\n", c1, n1, c2, c3, n2, op, c4, c5, n3);

  if (op == '+' || op == '-' || op == '*')
  {
    // movl "primeiro operando" %eax
    printf("  movl ");
    if (c2 == 'v')
    {
      if (c3 == 'i')
      {
        VI v2 = encontra_Vi(n2, varInt);
        printf("-%d(%%rbp), ", v2.offset);
      }
      else if (c3 == 'r')
      {
        VR v2 = encontra_Vr(n2, varReg);
        printf("%s, ", v2.nome);
      }
    }
    else if (c2 == 'p')
    {
      P v2 = encontra_P(n2, parametros);
      printf("%s, ", v2.reg);
    }
    else if (c2 == 'c')
    {
      printf("$%d, ", n2);
    }

    printf("%%eax\n");

    switch (op)
    {
    case '+':
      printf("  addl ");
      break;

    case '-':
      printf("  subl ");
      break;

    case '*':
      printf("  imull ");
      break;

    default:
      break;
    }

    // operador "segundo operando" %eax

    if (c4 == 'v')
    {
      if (c5 == 'i')
      {
        VI v3 = encontra_Vi(n3, varInt);
        printf("-%d(%%rbp), ", v3.offset);
      }
      else if (c5 == 'r')
      {
        VR v3 = encontra_Vr(n3, varReg);
        printf("%s, ", v3.nome);
      }
    }
    else if (c4 == 'p')
    {
      P v3 = encontra_P(n3, parametros);
      printf("%s, ", v3.reg);
    }
    else if (c4 == 'c')
    {
      printf("$%d, ", n3);
    }

    printf("%%eax\n");

    // Resultado em %eax
    if (c1 == 'i')
    {
      VI v1 = encontra_Vi(n1, varInt);
      printf("  movl %%eax, -%d(%%rbp)\n", v1.offset);
    }
    else if (c1 == 'r')
    {
      VR v1 = encontra_Vr(n1, varReg);
      printf("  movl %%eax, %s\n", v1.nome);
    }
    printf("\n");
  }
  else // Divisão
  {
    if (parametros[2].emUso) // Verifica se o registrador dx está em uso, caso positivo, salva o dx
    {
      P ret = encontra_P(3, parametros);
      printf("  # Salvando %%rdx\n  movq %%rdx, -%d(%%rbp)\n\n", ret.offset);
    }

    // movl "dividendo", %eax
    printf("  movl ");

    if (c2 == 'v')
    {
      if (c3 == 'i')
      {
        VI v2 = encontra_Vi(n2, varInt);
        printf("-%d(%%rbp), ", v2.offset);
      }
      else if (c3 == 'r')
      {
        VR v2 = encontra_Vr(n2, varReg);
        printf("%s, ", v2.nome);
      }
    }
    else if (c2 == 'p')
    {
      P v2 = encontra_P(n2, parametros);
      printf("%s, ", v2.reg);
    }
    else if (c2 == 'c')
    {
      printf("$%d, ", n2);
    }

    printf("%%eax\n");

    // movl "divisor", %ecx
    printf("  movl ");

    if (c4 == 'v')
    {
      if (c5 == 'i')
      {
        VI v3 = encontra_Vi(n3, varInt);
        printf("-%d(%%rbp), ", v3.offset);
      }
      else if (c5 == 'r')
      {
        VR v3 = encontra_Vr(n3, varReg);
        printf("%s, ", v3.nome);
      }
    }
    else if (c4 == 'p')
    {
      P v3 = encontra_P(n3, parametros);
      printf("%s, ", v3.reg);
    }
    else if (c4 == 'c')
    {
      printf("$%d, ", n3);
    }

    printf("%%ecx\n");

    // Comandos da divisão
    printf("  cltd\n");
    printf("  idivl %%ecx\n\n");

    // Resultado em %eax
    if (c1 == 'i')
    {
      VI v1 = encontra_Vi(n1, varInt);
      printf("  movl %%eax, -%d(%%rbp)\n", v1.offset);
    }
    else if (c1 == 'r')
    {
      VR v1 = encontra_Vr(n1, varReg);
      printf("  movl %%eax, %s\n", v1.nome);
    }
    printf("\n");

    // Recupera o dx se estiver em uso
    if (parametros[2].emUso)
    {
      P ret = encontra_P(3, parametros);
      printf("  # Recuperando %%rdx\n  movq  -%d(%%rbp), %%rdx\n\n", ret.offset);
    }
  }
}

// --------------------------------------------------------------------
// ---------------------SALVAMENTO E RECUPERAÇÃO-----------------------
// --------------------------------------------------------------------

void salva_parametros(P *parametros)
{
  printf("  # Salvando Parâmetros\n");
  for (int i = 0; i < MAX_PARAM; i++)
  {
    if (parametros[i].emUso) // Salva apenas os registradores em uso
    {
      printf("  movq %s, -%d(rbp)\n", params_8[i + 1], parametros[i].offset); // Salva o registrador inteiro, questão de segurança
    }
  }
  printf("\n");
}

void recupera_parametros(P *parametros)
{
  printf("  # Recuperando Parâmetros\n");
  for (int i = 0; i < MAX_PARAM; i++)
  {
    if (parametros[i].emUso)
    {
      printf("  movq -%d(rbp), %s\n", parametros[i].offset, params_8[i + 1]);
    }
  }
  printf("\n");
}

void salva_reg(VR *varReg)
{
  printf("  # Salvando Registradores\n");
  for (int i = 0; i < MAX_VAR; i++)
  {
    if (varReg[i].emUso) // Salva apenas os registradores em uso
    {
      printf("  movq %s, -%d(rbp)\n", varReg[i].nome_8, varReg[i].offset); // Salva o registrador inteiro, questão de segurança
    }
  }
  printf("\n");
}

void recupera_reg(VR *varReg)
{
  printf("  # Recuperando Registradores\n");
  for (int i = 0; i < MAX_VAR; i++)
  {
    if (varReg[i].emUso)
    {
      printf("  movq -%d(rbp), %s\n", varReg[i].offset, varReg[i].nome_8);
    }
  }
  printf("\n");
}

// --------------------------------------------------------------------
// ------------------------CHAMADA DE FUNÇÃO---------------------------
// --------------------------------------------------------------------

void call(int r, char c1, int n1, int f_num, char c2, char c3, int n2, char c4, char c5, int n3, char c6, char c7, int n4, P *parametros, VI *varInt, VR *varReg, VA *varArr)
{

  // 1 ou 2 ou 3 parâmetros
  if (r == 6 || r == 9 || r == 12)
  {
    switch (r)
    {
    case 6: // 1 parâmetro
      printf("  # v%c%d = call f%d %c%c%d\n", c1, n1, f_num, c2, c3, n2);
      break;

    case 9: // 2 parâmetro
      printf("  # v%c%d = call f%d %c%c%d %c%c%d\n", c1, n1, f_num, c2, c3, n2, c4, c5, n3);
      break;

    case 12: // 3 parâmetros
      printf("  # v%c%d = call f%d %c%c%d %c%c%d %c%c%d\n", c1, n1, f_num, c2, c3, n2, c4, c5, n3, c6, c7, n3);
      break;

    default:
      break;
    }

    // Salvamento de Registradores
    salva_parametros(parametros);
    salva_reg(varReg);

    // Primeiro parâmetro é uma constante
    if (c2 == 'c')
    {
      printf("  movl $%d, %%edi\n", n2);
    }

    // Primeiro parâmetro é parâmetro
    if (c2 == 'p')
    {
      P ret = encontra_P(n2, parametros);
      if (c3 == 'i') // 4 bytes
      {
        printf(" movl -%d(%%rbp), %%edi\n", ret.offset); // Bucando o Parâmetro direto da memória para evitar chamadas cruzadas
      }
      else // 8 bytes
      {
        printf(" movq -%d(%%rbp), %%rdi\n", ret.offset); // Bucando o Parâmetro direto da memória para evitar chamadas cruzadas
      }
    }

    // Primeiro parâmetro é variável local
    if (c2 == 'v')
    {
      // Variável inteira
      if (c3 == 'i')
      {
        VI ret = encontra_Vi(n2, varInt);
        printf("movl -%d(%%rbp), %%edi\n", ret.offset); // Utiliza o endereço da memória
      }
      // Variável registrador
      if (c3 == 'r')
      {
        VR ret = encontra_Vr(n2, varReg);
        printf("  movl %s, %%edi\n", ret.nome); // Utiliza o nome do registrador
      }
      // Variável array
      if (c3 == 'a')
      {
        VA ret = encontra_Va(n2, varArr);
        printf("  leaq -%d(%%rbp), %%rdi\n", ret.offset); // Endereço de memória de acordo com o offset
      }
    }

    // 2 ou 3 parâmetros
    if (r == 9 || r == 12)
    {
      // Segundo parâmetro é uma constante
      if (c4 == 'c')
      {
        printf("  movl $%d, %%esi\n", n3);
      }

      // Segundo parâmetro é um parâmetro
      if (c4 == 'p')
      {
        P ret = encontra_P(n3, parametros);
        if (c5 == 'i') // 4bytes
        {
          printf("  movl -%d(%%rbp), %%esi\n", ret.offset);
        }
        else // 8 bytes
        {
          printf("  movq -%d(%%rbp), %%rsi\n", ret.offset);
        }
      }

      // Segundo parâmetro é uma variável local
      if (c4 == 'v')
      {
        if (c5 == 'i')
        {
          VI ret = encontra_Vi(n3, varInt);
          printf("  movl -%d(%%rbp), %%esi\n", ret.offset);
        }
        if (c5 == 'r')
        {
          VR ret = encontra_Vr(n3, varReg);
          printf("  movl %s, %%esi\n", ret.nome);
        }
        if (c5 == 'a')
        {
          VA ret = encontra_Va(n3, varArr);
          printf("  leaq -%d(%%rbp), %%rsi\n", ret.offset);
        }
      }

      // 3 parâmetros
      if (r == 12)
      {
        if (c6 == 'c')
        {
          printf("  movl $%d, %%edx\n", n4);
        }

        if (c6 == 'p')
        {
          P ret = encontra_P(n4, parametros);
          if (c7 == 'i')
          {
            printf(" movl -%d(%%rbp), %%edx\n", ret.offset);
          }
          else
          {
            printf(" movq -%d(%%rbp), %%rdx\n", ret.offset);
          }
        }

        if (c6 == 'v')
        {
          if (c7 == 'i')
          {
            VI ret = encontra_Vi(n4, varInt);
            printf("  movl -%d(%%rbp), %%edx\n", ret.offset);
          }
          if (c7 == 'r')
          {
            VR ret = encontra_Vr(n4, varReg);
            printf("  movl %s, %%edx\n", ret.nome);
          }
          if (c7 == 'a')
          {
            VA ret = encontra_Va(n4, varArr);
            printf("  leaq -%d(%%rbp), %%rdx\n", ret.offset);
          }
        }
      }
    }

    // Chamada de função
    printf("  call f%d\n", f_num);

    // Verifica se a atribuição é para uma variável inteira ou registrador
    if (c1 == 'i')
    {
      VI ret = encontra_Vi(n1, varInt);
      printf("  movl %%eax, -%d(%%rbp)\n\n", ret.offset);
    }
    else
    {
      VR ret = encontra_Vr(n1, varReg);
      printf("  movl %%eax, %s\n\n", ret.nome);
    }

    // Recuperando registradores
    recupera_parametros(parametros);
    recupera_reg(varReg);
  }
}

// --------------------------------------------------------------------
// --------------------------ACESSO ARRAY------------------------------
// --------------------------------------------------------------------

// "get %ca%d index ci%d to %c%c%d"
void get_array(char c1, int n1, int n2, char c2, char c3, int n3, P *parametros, VI *varInt, VR *varReg, VA *varArr)
{
  if (c1 == 'v') // Acessando um array local
  {
    VA ret = encontra_Va(n1, varArr);
    printf("  leaq -%d(%%rbp), %%rcx\n", ret.offset);
    printf("  movabs $%d, %%rax\n", n2);
    printf("  imulq $4, %%rax\n");
    printf("  addq %%rcx, %%rax\n");
  }
  if (c1 == 'p') // Acessando um array recebido como parâmetro
  {
    P ret = encontra_P(n1, parametros);
    printf("  # pa%d -> %s\n", n1, ret.reg);
    printf("  movabs $%d, %%rax\n", n2);
    printf("  imulq $4, %%rax\n");
    printf("  addq %s, %%rax\n", ret.reg);
  }

  if (c2 == 'v') // Atribuido a uma variável local
  {
    if (c3 == 'i') // Variável inteira
    {
      VI ret = encontra_Vi(n3, varInt);
      printf("  movl (%%rax), %%ecx\n");
      printf("  movl %%ecx, -%d(%%rbp)\n", ret.offset);
    }
    if (c3 == 'r') // Variável registrador
    {
      VR ret = encontra_Vr(n3, varReg);
      printf("  movl (%%rax), %s\n", ret.nome);
    }
  }

  if (c2 == 'p') // Atribuindo a um parâmetro
  {
    P ret = encontra_P(n3, parametros);
    printf("  movl (%%rax), %s\n", ret.reg);
  }
  printf("\n");
}

void set_array(char c1, int n1, int n2, char c2, char c3, int n3, P *parametros, VI *varInt, VR *varReg, VA *varArr)
{
  // Atribuindo a um array local
  if (c1 == 'v')
  {
    VA ret = encontra_Va(n1, varArr);
    printf("  leaq -%d(%%rbp), %%rcx\n", ret.offset);
    printf("  movabs $%d, %%rax\n", n2);
    printf("  imulq $4, %%rax\n");
    printf("  addq %%rcx, %%rax\n");
  }
  // Atribuindo a um array recebido como parâmetro
  if (c1 == 'p')
  {
    P ret = encontra_P(n1, parametros);
    printf("  # pa%d -> %s\n", n1, ret.reg);
    printf("  movabs $%d, %%rax\n", n2);
    printf("  imulq $4, %%rax\n");
    printf("  addq %s, %%rax\n", ret.reg);
  }

  // Valor de uma variável local
  if (c2 == 'v')
  {
    if (c3 == 'i') // Inteira
    {
      VI ret = encontra_Vi(n3, varInt);
      printf("  movl -%d(%%rbp), %%ecx\n", ret.offset);
      printf("  movl %%ecx, (%%rax)\n");
    }
    if (c3 == 'r') // Registrador
    {
      VR ret = encontra_Vr(n3, varReg);
      printf("  movl %s, (%%rax)\n", ret.nome);
    }
  }
  // Valor de um parâmetro
  if (c2 == 'p')
  {
    P ret = encontra_P(n3, parametros);
    printf("  movl %s, (%%rax)\n", ret.reg);
  }
  // Valor de uma constante
  if (c2 == 'c')
  {
    printf("  movl $%d, (%%rax)\n", n3);
  }
  printf("\n");
}

// --------------------------------------------------------------------
// ---------------------------CONDICIONAL------------------------------
// --------------------------------------------------------------------
void get_op(char a, char b)
{
  char str[2];
  str[0] = a;
  str[1] = b;
  // str = operador BLP

  for (int i = 0; i < 12; i += 2)
  {
    if (strcmp(str, opRelacionais[i]) == 0) // Encontra o operador BLP
    {
      printf("  %s ", opRelacionais[i + 1]); // Printa o inverso em Assembly
    }
  }
}

void if_structure(char c1, char c2, int n1, char c3, char c4, char c5, char c6, int n2, P *parametros, VI *varInt, VR *varReg, VA *varArr, int i_if)
{
  // movl ""segundo argumento", %eax
  if (c5 == 'v')
  {
    if (c6 == 'i')
    {
      VI ret = encontra_Vi(n2, varInt);
      printf("  movl -%d(%%rbp), %%eax\n", ret.offset);
    }
    if (c6 == 'r')
    {
      VR ret = encontra_Vr(n2, varReg);
      printf("  movl %s, %%eax\n", ret.nome);
    }
  }
  if (c5 == 'p')
  {
    P ret = encontra_P(n2, parametros);
    printf("  movl %s, %%eax\n", ret.reg);
  }
  if (c5 == 'c')
  {
    printf("  movl $%d, %%eax\n", n2);
  }

  // movl "primeiro argumento", %ecx
  if (c1 == 'v')
  {
    if (c2 == 'i')
    {
      VI ret = encontra_Vi(n1, varInt);
      printf("  movl -%d(%%rbp), %%ecx\n", ret.offset);
    }
    if (c2 == 'r')
    {
      VR ret = encontra_Vr(n1, varReg);
      printf("  movl %s, %%ecx\n", ret.nome);
    }
  }
  if (c1 == 'p')
  {
    P ret = encontra_P(n1, parametros);
    printf("  movl %s, %%ecx\n", ret.reg);
  }
  if (c1 == 'c')
  {
    printf("  movl $%d, %%ecx\n", n1);
  }

  printf("\n");
  printf("  cmpl %%eax, %%ecx\n"); // Comparação segundo argumento pelo primeiro
  get_op(c3, c4);                  // Utiliza o perador inverso
  printf("end_if%d\n\n", i_if);    // Utiliza o i_if para controle
}
// --------------------------------------------------------------------
// -----------------------------RETORNO--------------------------------
// --------------------------------------------------------------------

void verifica_return(int r, char type1Return, char type2Return, int return_num, P *parametros, VI *varInt, VA *varArr, VR *varReg)
{
  if (r == 3) // Return identificado
  {
    if (type1Return == 'c') // return de constante
    {
      printf("  # return ci%d\n", return_num);
      printf("  movl $%d, %%eax\n", return_num);
    }
    if (type1Return == 'v') // return de variável
    {
      if (type2Return == 'i') // inteiro
      {
        VI ret = encontra_Vi(return_num, varInt);
        printf("  # return vi%d\n", return_num);
        printf("  movl -%d(%%rbp), %%eax\n", ret.offset);
      }
      if (type2Return == 'r') // registrador
      {
        VR ret = encontra_Vr(return_num, varReg);
        printf("  # return vr%d\n", return_num);
        printf("  movq -%d(%%rbp), %%rax\n", ret.offset);
      }
    }
    if (type1Return == 'p') // return de parâmetro
    {
      if (type2Return == 'i') // parâmetro inteiro
      {
        P ret = encontra_P(return_num, parametros);
        printf("  # return pi%d\n", return_num);
        printf("  movl -%d(%%rbp), %%eax\n", ret.offset);
      }
      if (type2Return == 'a') // parâmetro array
      {
        P ret = encontra_P(return_num, parametros);
        printf("  # return pa%d\n", return_num);
        printf("  movl -%d(%%rbp), %%eax\n", ret.offset);
      }
    }
    printf("\n");
  }
}