#include <stdlib.h>
#include <stdio.h>
#include "processo.h"
#include "err.h"
#include "es.h"
#include "contr.h"

#ifndef ESCALONADOR_CIRC_H
#define ESCALONADOR_CIRC_H
typedef struct no_t no_t;

typedef struct esc_circ_t esc_circ_t;

esc_circ_t* esc_cria(void);

void esc_destroi(esc_circ_t* self);

no_t* cria_no(processo_t* processo);

void insereF_fila(esc_circ_t *self, processo_t* processo);

void insereI_lista(no_t** head, processo_t* processo);

processo_t* retorna_proximo_pronto(esc_circ_t* self);

err_t finaliza_processo_em_exec(esc_circ_t* self, rel_t *rel);

void bloqueia_processo_em_exec(esc_circ_t* self, mem_t *mem, 
                               cpu_estado_t *cpu_estado, int disp, 
                               acesso_t chamada, rel_t *rel);

void varre_processos(esc_circ_t* self, contr_t *contr, rel_t *rel);

bool tem_processo_executando(esc_circ_t* self);

bool tem_processo_vivo(esc_circ_t* self);

void esc_executa(esc_circ_t* self, processo_t* processo);

void imprime_tabela(no_t* self);

processo_t* esc_processo_executando(esc_circ_t* self);

void esc_tik(esc_circ_t* self);


#endif

