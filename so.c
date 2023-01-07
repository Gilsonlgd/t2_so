#include "so.h"
#include "tela.h"
#include "escalonador.h"
#include "processo.h"
#include <stdlib.h>

struct so_t {
  contr_t *contr;       // o controlador do hardware
  bool paniquei;        // apareceu alguma situação intratável
  cpu_estado_t *cpue;   // cópia do estado da CPU
  no_t* escalonador;   // tabela de processos
};

// funções auxiliares
static void init_mem(so_t *self);
static void panico(so_t *self);

so_t *so_cria(contr_t *contr)
{
  so_t *self = malloc(sizeof(*self));
  if (self == NULL) return NULL;
  self->contr = contr;
  self->paniquei = false;
  self->cpue = cpue_cria();
  self->escalonador = NULL;
  init_mem(self);
  //Cria o primeiro processo
  processo_t* processo = processo_cria(0, pronto);
  processo_executa(processo);
  insere(&self->escalonador, processo);
  // coloca a CPU em modo usuário
  exec_copia_estado(contr_exec(self->contr), self->cpue);
  cpue_muda_modo(self->cpue, usuario);
  exec_altera_estado(contr_exec(self->contr), self->cpue);
  return self;
}

void so_destroi(so_t *self)
{
  cpue_destroi(self->cpue);
  free(self);
}

// trata chamadas de sistema

// chamada de sistema para leitura de E/S
// recebe em A a identificação do dispositivo
// retorna em X o valor lido
//            A o código de erro
static void so_trata_sisop_le(so_t *self)
{
  bool pronto;
  int inc = 2;
  int disp = cpue_A(self->cpue);
  int val;
  err_t err = ERR_OK;

  pronto = es_pronto(contr_es(self->contr), disp, leitura);
  if(pronto) {
    err = es_le(contr_es(self->contr), disp, &val);
    cpue_muda_A(self->cpue, err);
    cpue_muda_X(self->cpue, val);
  } else {
    bloqueia_processo_em_exec(&self->escalonador, contr_mem(self->contr), 
                              self->cpue, disp, leitura);
    processo_t* processo = retorna_proximo_pronto(self->escalonador);
    if (processo == NULL) {
      cpue_muda_modo(self->cpue, zumbi);
    } else {
      cpue_muda_modo(self->cpue, usuario);
      processo_executa(processo);
      cpue_copia(processo_cpu(processo), self->cpue);
      contr_copia_mem(self->contr, processo_mem(processo));
      inc = 0;
    }
  }
  // incrementa o PC
  cpue_muda_PC(self->cpue, cpue_PC(self->cpue) + inc);
  // interrupção da cpu foi atendida
  cpue_muda_erro(self->cpue, err, 0);
  // altera o estado da CPU
  exec_altera_estado(contr_exec(self->contr), self->cpue);
}

// chamada de sistema para escrita de E/S
// recebe em A a identificação do dispositivo
//           X o valor a ser escrito
// retorna em A o código de erro
static void so_trata_sisop_escr(so_t *self)
{
  bool pronto;
  int inc = 2;
  int disp = cpue_A(self->cpue);
  int val = cpue_X(self->cpue);
  err_t err = ERR_OK;

  pronto = es_pronto(contr_es(self->contr), disp, escrita);
  if(pronto) {
    err = es_escreve(contr_es(self->contr), disp, val);
    cpue_muda_A(self->cpue, err);
  } else {
    bloqueia_processo_em_exec(&self->escalonador, contr_mem(self->contr), 
                              self->cpue, disp, escrita);
    processo_t* processo = retorna_proximo_pronto(self->escalonador);
    if (processo == NULL) {
      cpue_muda_modo(self->cpue, zumbi);
    } else {
      cpue_muda_modo(self->cpue, usuario);
      processo_executa(processo);
      cpue_copia(processo_cpu(processo), self->cpue);
      contr_copia_mem(self->contr, processo_mem(processo));
      inc = 0;
    }
  }

  // interrupção da cpu foi atendida
  cpue_muda_erro(self->cpue, err, 0);
  // incrementa o PC
  cpue_muda_PC(self->cpue, cpue_PC(self->cpue)+ inc);
  // altera o estado da CPU
  exec_altera_estado(contr_exec(self->contr), self->cpue);
}

// chamada de sistema para término do processo
static void so_trata_sisop_fim(so_t *self)
{
  err_t err = finaliza_processo_em_exec(&self->escalonador);
  if(err != ERR_OK) {
    t_printf("Erro na finalização do processo.");
    self->paniquei = true;
  } else {
    processo_t* processo = retorna_proximo_pronto(self->escalonador);
    if (processo == NULL) {
      cpue_muda_modo(self->cpue, zumbi);
    } else {
      cpue_muda_modo(self->cpue, usuario);
      processo_executa(processo);
      cpue_copia(processo_cpu(processo), self->cpue);
      contr_copia_mem(self->contr, processo_mem(processo));     
    }
    // interrupção da cpu foi atendida
    cpue_muda_erro(self->cpue, ERR_OK, 0);
    // incrementa o PC
    cpue_muda_PC(self->cpue, cpue_PC(self->cpue));
    // altera o estado da CPU (deveria alterar o estado do processo)
    exec_altera_estado(contr_exec(self->contr), self->cpue);
  }
}

// chamada de sistema para criação de processo
static void so_trata_sisop_cria(so_t *self)
{
  processo_t* processo = processo_cria(cpue_A(self->cpue), pronto);
  err_t err = processo_init_mem(processo);
  if (err != ERR_OK) {
    panico(self);
  } else {
    insere(&self->escalonador, processo);
  }
  // interrupção da cpu foi atendida
  cpue_muda_erro(self->cpue, err, 0);
  // incrementa o PC
  cpue_muda_PC(self->cpue, cpue_PC(self->cpue)+2);
  // altera o estado da CPU 
  exec_altera_estado(contr_exec(self->contr), self->cpue);
}

// trata uma interrupção de chamada de sistema
static void so_trata_sisop(so_t *self)
{
  // o tipo de chamada está no "complemento" do cpue
  exec_copia_estado(contr_exec(self->contr), self->cpue);
  so_chamada_t chamada = cpue_complemento(self->cpue);
  switch (chamada) {
    case SO_LE:
      so_trata_sisop_le(self);
      break;
    case SO_ESCR:
      so_trata_sisop_escr(self);
      break;
    case SO_FIM:
      so_trata_sisop_fim(self);
      break;
    case SO_CRIA:
      so_trata_sisop_cria(self);
      break;
    default:
      t_printf("so: chamada de sistema não reconhecida %d\n", chamada);
      panico(self);
  }
}

// trata uma interrupção de tempo do relógio
static void so_trata_tic(so_t *self)
{
  processo_t* processo;

  if (!tem_processo_executando(self->escalonador)) {
    processo = retorna_proximo_pronto(self->escalonador);
    if (processo == NULL) {
      cpue_muda_modo(self->cpue, zumbi);
    } else {
      cpue_muda_modo(self->cpue, usuario);
      processo_executa(processo);
      cpue_copia(processo_cpu(processo), self->cpue);
      contr_copia_mem(self->contr, processo_mem(processo));
    }
    // interrupção da cpu foi atendida
    cpue_muda_erro(self->cpue, ERR_OK, 0);
    // incrementa o PC
    cpue_muda_PC(self->cpue, cpue_PC(self->cpue));
    // altera o estado da CPU
    exec_altera_estado(contr_exec(self->contr), self->cpue);
  }
}

// houve uma interrupção do tipo err — trate-a
void so_int(so_t *self, err_t err)
{
  varre_processos(&self->escalonador, self->contr);
 
  switch (err) {
    case ERR_SISOP:
      so_trata_sisop(self);
      break;
    case ERR_TIC:
      so_trata_tic(self);
      break;
    default:
      t_printf("SO: interrupção não tratada [%s]", err_nome(err));
      self->paniquei = true;
  }
}

// retorna false se o sistema deve ser desligado
bool so_ok(so_t *self)
{
  if (!tem_processo_vivo(self->escalonador)) {
    self->paniquei = true;
    t_printf("Sistema Finalizado");
  }
  return !self->paniquei;
}

// carrega o programa inicial na memória
static void init_mem(so_t *self)
{
  // programa para executar na nossa CPU
  int progr[] = {
    #include "init.maq"
  };
  int tam_progr = sizeof(progr)/sizeof(progr[0]);

  // inicializa a memória com o programa 
  mem_t *mem = contr_mem(self->contr);
  for (int i = 0; i < tam_progr; i++) {
    if (mem_escreve(mem, i, progr[i]) != ERR_OK) {
      t_printf("so.init_mem: erro de memória, endereco %d\n", i);
      panico(self);
    }
  }
}
  
static void panico(so_t *self) 
{
  t_printf("Problema irrecuperavel no SO");
  self->paniquei = true;
}

