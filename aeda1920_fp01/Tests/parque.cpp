#include "parque.h"
#include <vector>

using namespace std;


ParqueEstacionamento::ParqueEstacionamento(unsigned int lot, unsigned int nMaxCli) : lotacao(lot), numMaximoClientes(nMaxCli) {
    vagas = lot;
}

unsigned int ParqueEstacionamento::getNumLugares() const {
    return lotacao;
}

unsigned int ParqueEstacionamento::getNumMaximoClientes() const {
    return numMaximoClientes;
}

int ParqueEstacionamento::posicaoCliente(const string &nome) const {
    for (int i = 0; i < clientes.size(); ++i) {
        if (clientes.at(i).nome == nome)
            return i;
    }
    return -1;
}

bool ParqueEstacionamento::adicionaCliente(const string &nome) {
    if (posicaoCliente(nome) == -1)
        return false;
    return true;
}
