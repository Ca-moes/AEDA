#include <queue>
#include <cstdlib>
#include "balcao.h"
#include "exceptions.h"

using namespace std;

//a alterar
Cliente::Cliente() {
    numPresentes = rand() % 5 + 1;
}

//a alterar
int Cliente::getNumPresentes() const {
    return numPresentes;
}

//a alterar
Balcao::Balcao(int te): tempo_embrulho(te) {
    clientes_atendidos=0;
    tempo_atual = 0;
    prox_chegada = rand() % 20 + 1;
    prox_saida = 0;
}

//a alterar
void Balcao:: proximoEvento()
{

}

//a alterar
void Balcao::chegada()
{
    Cliente novo;
    clientes.push(novo);
    prox_chegada = rand()%20+1;
    prox_saida = tempo_atual + novo.getNumPresentes() * tempo_embrulho;
}

//a alterar
void Balcao::saida()
{
}


int Balcao::getTempoAtual() const { return tempo_atual; }

int Balcao::getProxChegada() const { return prox_chegada; }

//a alterar
ostream & operator << (ostream & out, const Balcao & b1)
{
     return out;
}

//a alterar
int Balcao::getTempoEmbrulho() const {
	return tempo_embrulho;
}

//a alterar
int Balcao::getProxSaida() const {
	return prox_saida;
}

//a alterar
int Balcao::getClientesAtendidos() const {
	return clientes_atendidos;
}

//a alterar
Cliente & Balcao::getProxCliente() {
    if(clientes.empty())
        throw FilaVazia();
    return clientes.front();
}

      
