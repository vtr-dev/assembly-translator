#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 256
#define MAX_VAR 4
#define MAX_PARAM 3
#define MAX_PILHA 15
#define true 1
#define false 0

struct parametro
{
    char emUso;
    char tipo;
    int numero;
    int offset;
    char reg[5];
};

typedef struct parametro P;

struct var
{
    char emUso;
    int numero;
    int offset;
};

typedef struct var VI;

struct vet
{
    char emUso;
    int numero;
    int size;
    int offset;
};

typedef struct vet VA;

struct reg
{
    char emUso;
    int numero;
    char nome[6];
    char nome_8[6];
    int offset;
    char tamanho;
};

typedef struct reg VR;

extern const char *opRelacionais[12];
extern const char *reg_4[5];
extern const char *reg_8[5];
extern const char *params_8[4];
extern const char *params_4[4];
extern const char ini[]; // Início padrão das funções
extern const char fim[]; // Fim padrão das funções

void remove_newline(char *ptr); // Ok

void function(int r, int f_num, char c1, int p1_num, char c2, int p2_num, char c3, int p3_num, P *parametros); // OK

void reset_all(P *parametros, VI *varInt, VA *varArr, VR *varReg); // OK

void push_var(int *pilha, VI *varInt, int vN);              // OK
void push_vet(int *pilha, VA *varArr, int vN, int sizevet); // OK
void push_reg(int *pilha, VR *varReg, int vN);              // OK
void push_parametros(int *pilha, P *parametros);            // OK

void limpa_pilha(int *pilha); // Ok
void aloca_pilha(int *pilha); // Ok

VI encontra_Vi(int n, VI *varInt);  // Ok
VA encontra_Va(int n, VA *varArr);  // Ok
VR encontra_Vr(int n, VR *varReg);  // Ok
P encontra_P(int n, P *parametros); // Ok

void atribuicao_simples(int r, char c1, int n1, char c2, char c3, int n2, P *parametros, VI *varInt, VR *varReg);                                        // OK
void atribuicao_aritimetica(int r, char c1, int n1, char c2, char c3, int n2, char op, char c4, char c5, int n3, P *parametros, VI *varInt, VR *varReg); // Ok

void salva_parametros(P *parametros);    // Ok
void recupera_parametros(P *parametros); // Ok

void salva_reg(VR *varReg);    // Ok
void recupera_reg(VR *varReg); // Ok

void call(int r, char c1, int n1, int f_num, char c2, char c3, int n2, char c4, char c5, int n3, char c6, char c7, int n4, P *parametros, VI *varInt, VR *varReg, VA *varArr); // Ok

void get_array(char c1, int n1, int n2, char c2, char c3, int n3, P *parametros, VI *varInt, VR *varReg, VA *varArr); // Ok
void set_array(char c1, int n1, int n2, char c2, char c3, int n3, P *parametros, VI *varInt, VR *varReg, VA *varArr); // Ok

void get_op(char a, char b);
void if_structure(char c1, char c2, int n1, char c3, char c4, char c5, char c6, int n2, P *parametros, VI *varInt, VR *varReg, VA *varArr, int i_if); // Ok

void verifica_return(int r, char c1, char c2, int return_num, P *parametros, VI *varInt, VA *varArr, VR *varReg); // Ok

#endif