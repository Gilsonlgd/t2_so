#  T2 - Trabalho 2
- Implementação de escalonadores para execução de processos;
- São implementados dois escalonadores: Escalonador Circular e Escalonador Processo mais Rápido.
- Nessa etapa também são implementadas métricas de avaliação do sistema.
- É também implementado sistema de quantum, onde um processo tem um tempo limite para permanecer executando, após isso ele sofre preempção.

## Observações importantes:
- Ao rodar o sistema, caso haja necessidade de trocar o escalonador, descomente o `#include` do outro escalonador e comente o do atual. Após isso o compilador dirá quais funções não batem e te leva até onde elas devem ser comentadas ou não;
- Essa parte do trabalho está com um bug ao utilizar os processos criados pelo professor para o T3 (eles seguem em loop infinito). Eu corrigi o problema durante a implementação do T3 porém não corrigi no T2 e agora não lembro o que era kkkk;
- Dessa forma as métricas apresentadas são correspondentes a 3 processos: p1(imprime de 1 até o num digitado no t0), p2(igual p1 so que no terminal 1) e p3 (lê numeros aleatórios);

## Escalonador Circular:
- São implementadas 2 listas:
   - Uma corresponde aos processos prontos, esperando pra executar. Recebe processos que estavam bloqueados ou que sofreram preempção;
   - A outra corresponde aos processos bloqueados, esperando desbloqueio.
- Há ainda um ponteiro para o processo em execução. 
- Dessa forma a estrutura atua de forma circular, com o processo iniciando sua vida na lista de prontos, executando, bloqueando, desbloqueando, executando...

### Métricas:
```
QUANTUM PEQUENO: 5
|Informacoes do SO:|
TempoT_exec: 1237, Tempo_CPU: 1237, Num_Interrup: 241
----------------------------------------------
|Metricas dos processos: quantum = 5|
num:1, tempo_retorno: 744, tempo_bloq: 481,
 tempo_exec: 135, tempo_esp: 341, tempoM_retorno: 56,8333320618,
 num_bloq: 2, num_preemp: 4
 
num:2, tempo_retorno: 1233, tempo_bloq: 1012,
 tempo_exec: 133, tempo_esp: 200, tempoM_retorno: 40,0000000000,
 num_bloq: 2, num_preemp: 3 

num:3, tempo_retorno: 860, tempo_bloq: 580,
 tempo_exec: 104, tempo_esp: 171, tempoM_retorno: 19,0000000000,
 num_bloq: 9, num_preemp: 0 

QUANTUM MEDIO: 10
|Informacoes do SO:|
TempoT_exec: 805, Tempo_CPU: 805, Num_Interrup: 157
----------------------------------------------
|Metricas dos processos: quantum = 10|
num:1, tempo_retorno: 454, tempo_bloq: 186,
 tempo_exec: 145, tempo_esp: 164, tempoM_retorno: 41,0000000000,
 num_bloq: 3, num_preemp: 1

num:2, tempo_retorno: 801, tempo_bloq: 648,
 tempo_exec: 52, tempo_esp: 60, tempoM_retorno: 60,0000000000,
 num_bloq: 1, num_preemp: 0 

num:3, tempo_retorno: 590, tempo_bloq: 276,
 tempo_exec: 106, tempo_esp: 203, tempoM_retorno: 20,2999992371,
 num_bloq: 10, num_preemp: 0 

QUANTUM GRANDE: 20
|Informacoes do SO:|
TempoT_exec: 760, Tempo_CPU: 760, Num_Interrup: 147
----------------------------------------------
|Metricas dos processos: quantum = 20|
num:1, tempo_retorno: 553, tempo_bloq: 306,
 tempo_exec: 153, tempo_esp: 86, tempoM_retorno: 21,5000000000,
 num_bloq: 4, num_preemp: 0 

num:2, tempo_retorno: 756, tempo_bloq: 597,
 tempo_exec: 53, tempo_esp: 65, tempoM_retorno: 65,0000000000,
 num_bloq: 1, num_preemp: 0 

num:3, tempo_retorno: 575, tempo_bloq: 262,
 tempo_exec: 107, tempo_esp: 201, tempoM_retorno: 22,3333339691,
 num_bloq: 9, num_preemp: 0 
```

## Escalonador Processo mais Curto:
- Nesse escalonador é escolhido o processo que se acha que vai executar por menos tempo, entre os processos prontos. O tempo esperado de execução de um processo é calculado cada vez que ele perde o processador, seja por bloqueio ou por preempção, como a média entre o tempo esperado anterior e o tempo de CPU recebido até a perda do processador. 
- Funciona de forma parecida com o circular, porém tem uma particularidades:
    - Agora os processos são inseridos de forma ordenada na lista de prontos;
    - São ordenados de acordo com seu tempo médio de execução.

### Métricas:
```
QUANTUM PEQUENO: 5
|Informacoes do SO:|
TempoT_exec: 797, Tempo_CPU: 797, Num_Interrup: 151
----------------------------------------------
|Metricas dos processos: quantum = 5|
num:1, tempo_retorno: 559, tempo_bloq: 221,
 tempo_exec: 185, tempo_esp: 354, tempoM_retorno: 39,3333320618,
 num_bloq: 4, num_preemp: 5

num:2, tempo_retorno: 793, tempo_bloq: 606,
 tempo_exec: 87, tempo_esp: 222, tempoM_retorno: 74,0000000000,
 num_bloq: 1, num_preemp: 2 

num:3, tempo_retorno: 566, tempo_bloq: 261,
 tempo_exec: 104, tempo_esp: 195, tempoM_retorno: 21,6666660309,
 num_bloq: 9, num_preemp: 0  

QUANTUM MEDIO: 10
|Informacoes do SO:|
TempoT_exec: 815, Tempo_CPU: 815, Num_Interrup: 159
----------------------------------------------
|Metricas dos processos: quantum = 10|
num:1, tempo_retorno: 469, tempo_bloq: 254,
 tempo_exec: 145, tempo_esp: 111, tempoM_retorno: 27,7500000000,
 num_bloq: 3, num_preemp: 1

num:2, tempo_retorno: 811, tempo_bloq: 642,
 tempo_exec: 53, tempo_esp: 75, tempoM_retorno: 75,0000000000,
 num_bloq: 1, num_preemp: 0 

num:3, tempo_retorno: 600, tempo_bloq: 339,
 tempo_exec: 105, tempo_esp: 151, tempoM_retorno: 15,1000003815,
 num_bloq: 10, num_preemp: 0  

QUANTUM GRANDE: 20
|Informacoes do SO:|
TempoT_exec: 945, Tempo_CPU: 945, Num_Interrup: 184
----------------------------------------------
|Metricas dos processos: quantum = 20|
num:1, tempo_retorno: 678, tempo_bloq: 431,
 tempo_exec: 153, tempo_esp: 86, tempoM_retorno: 21,5000000000,
 num_bloq: 4, num_preemp: 0 
 
num:2, tempo_retorno: 941, tempo_bloq: 776,
 tempo_exec: 53, tempo_esp: 71, tempoM_retorno: 71,0000000000,
 num_bloq: 1, num_preemp: 0 

num:3, tempo_retorno: 700, tempo_bloq: 440,
 tempo_exec: 107, tempo_esp: 148, tempoM_retorno: 16,4444446564,
 num_bloq: 9, num_preemp: 0 
```
