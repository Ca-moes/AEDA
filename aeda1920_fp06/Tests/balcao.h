#ifndef BALCAO_H_
#define BALCAO_H_

#include <iostream>
#include <queue>

using namespace std;

class Cliente {
      int numPresentes{};
public:
       Cliente();
       int getNumPresentes() const;
};


class Balcao {
      queue<Cliente> clientes;
      const int tempo_embrulho; /**< tempo que demora a embrulhar um presente */
      int prox_chegada;         /**< tempo em que ocorre a proxima chegada cliente fila*/
      int prox_saida;           /**< tempo em que ocorre a procima saida cliente fila */
      int tempo_atual;          /**< tempo atual da simulacao*/
      int clientes_atendidos;
public:
       Balcao(int te=2);
       int getTempoAtual() const;
       int getProxChegada() const;
       int getClientesAtendidos() const;
       int getTempoEmbrulho() const;
       int getProxSaida() const;
       Cliente & getProxCliente();

       void chegada();
       void saida();
       void proximoEvento();

       friend ostream & operator << (ostream & out, const Balcao & b1);
};


#endif
