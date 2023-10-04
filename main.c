#include "assembly.h"

int pilha[MAX_PILHA];
int i_if = 1;

P parametros[MAX_PARAM];
VI varInt[MAX_VAR];
VA varArr[MAX_VAR];
VR varReg[MAX_VAR];

int main()
{
  char c1, c2, c3, c4, c5, c6, c7, op;
  int r, f_num, p1_num, p2_num, p3_num, n1, n2, n3, n4;
  int return_num, vN, sizevet;

  char line[LINESZ];

  int count = 0;

  printf(".text\n\n");

  // Realiza ajustes necessários antes de ler a primeira função
  reset_all(parametros, varInt, varArr, varReg);

  // Lê uma linha por vez
  while (fgets(line, LINESZ, stdin) != NULL)
  {
    count++;
    remove_newline(line);

    // Verifica se é uma function
    r = sscanf(line, "function f%d p%c%d p%c%d p%c%d", &f_num, &c1, &p1_num, &c2, &p2_num, &c3, &p3_num);

    // Identifica quantos parâmetros tem a função, depois atribui o tipo, número e flag de cada um deles
    function(r, f_num, c1, p1_num, c2, p2_num, c3, p3_num, parametros);

    // Verifica se line começa com 'def' (3 letras)
    if (strncmp(line, "def", 3) == 0)
    {
      // Limpa a pilha atribuindo 0 para todo índice
      limpa_pilha(pilha);

      // Reserva espaço para os parâmetros na pilha e calcula o offset de cara parâmetro
      push_parametros(pilha, parametros);

      continue;
    }

    // var
    if (strncmp(line, "var", 3) == 0)
    {
      sscanf(line, "var vi%d", &vN);
      push_var(pilha, varInt, vN);
      continue;
    }

    // vet
    if (strncmp(line, "vet", 3) == 0)
    {
      sscanf(line, "vet va%d size ci%d", &vN, &sizevet);
      push_vet(pilha, varArr, vN, sizevet);
    }

    // reg
    if (strncmp(line, "reg", 3) == 0)
    {
      sscanf(line, "reg vr%d ", &vN);
      push_reg(pilha, varReg, vN);
    }

    // Verifica se line começa com 'enddef' (6 letras)
    if (strncmp(line, "enddef", 6) == 0)
    {
      // subq $x(%rsp)
      aloca_pilha(pilha);
      continue;
    }

    // Atribuição
    r = sscanf(line, "v%c%d = %c%c%d %c %c%c%d", &c1, &n1, &c2, &c3, &n2, &op, &c4, &c5, &n3);

    // Atribuição Simples
    if (r == 5)
    {
      atribuicao_simples(r, c1, n1, c2, c3, n2, parametros, varInt, varReg);
    }

    // Atribuição com expressão aritimética
    if (r == 9)
    {
      atribuicao_aritimetica(r, c1, n1, c2, c3, n2, op, c4, c5, n3, parametros, varInt, varReg);
    }

    // Identifica chamada de função
    r = sscanf(line, "v%c%d = call f%d %c%c%d %c%c%d %c%c%d", &c1, &n1, &f_num, &c2, &c3, &n2, &c4, &c5, &n3, &c6, &c7, &n4);

    // Realiza a chamada de função
    call(r, c1, n1, f_num, c2, c3, n2, c4, c5, n3, c6, c7, n4, parametros, varInt, varReg, varArr);

    // Get Array
    if (strncmp(line, "get", 3) == 0)
    {
      sscanf(line, "get %ca%d index ci%d to %c%c%d", &c1, &n1, &n2, &c2, &c3, &n3);
      printf("  # get %ca%d index ci%d to %c%c%d\n", c1, n1, n2, c2, c3, n3);
      get_array(c1, n1, n2, c2, c3, n3, parametros, varInt, varReg, varArr);
      continue;
    }

    // Set Array
    if (strncmp(line, "set", 3) == 0)
    {
      sscanf(line, "set %ca%d index ci%d with %c%c%d", &c1, &n1, &n2, &c2, &c3, &n3);
      printf("  # set %ca%d index ci%d with %c%c%d\n", c1, n1, n2, c2, c3, n3);
      set_array(c1, n1, n2, c2, c3, n3, parametros, varInt, varReg, varArr);
      continue;
    }

    // If
    if (strncmp(line, "if", 2) == 0)
    {
      sscanf(line, "if %c%c%d %c%c %c%c%d", &c1, &c2, &n1, &c3, &c4, &c5, &c6, &n2);
      printf("  # if_ %c%c%d %c%c %c%c%d\n", c1, c2, n1, c3, c4, c5, c6, n2);
      if_structure(c1, c2, n1, c3, c4, c5, c6, n2, parametros, varInt, varReg, varArr, i_if);
      continue;
    }

    // Endif
    if (strncmp(line, "endif", 5) == 0)
    {
      printf("  end_if%d:\n\n", i_if); // Label para controle
      i_if++;
      continue;
    }

    // Verifica se é a linha é "return"
    r = sscanf(line, "return %c%c%d", &c1, &c2, &return_num);
    verifica_return(r, c1, c2, return_num, parametros, varInt, varArr, varReg);

    // Verifica se line começa com 'end' (3 letras)
    if (strncmp(line, "end", 3) == 0)
    {
      // Reseta a flag de uso de todas as variáveis para uma possível próxima função
      reset_all(parametros, varInt, varArr, varReg);
      // Controle para o label do if
      i_if = 1;
      // leave / ret
      printf("  %s\n", fim);
      continue;
    }
  }

  return 0;
}