#include "frota.h"
#include <string>

using namespace std;


void Frota::adicionaVeiculo(Veiculo *v1) {
    veiculos.push_back(v1);
}

int Frota::numVeiculos() const {
    return veiculos.size();
}

int Frota::menorAno() const {
    if (numVeiculos() == 0)
        return 0;
    int menor = 2100;
    for (int i = 0; i < numVeiculos(); ++i) {
        if (veiculos.at(i)->getAno() < menor)
            menor = veiculos.at(i)->getAno();
    }
    return menor;
}

ostream &operator<<(ostream &o, const Frota &f) {
    for (int i = 0; i < f.veiculos.size(); ++i) {
        f.veiculos.at(i)->info();
    }
    return o;
}
