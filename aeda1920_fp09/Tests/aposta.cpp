#include "aposta.h"
#include <iostream>
#include <sstream>

using namespace std;

// a alterar
bool Aposta::contem(unsigned num) const
{
    return !(numeros.find(num) == numeros.end());
}

//a alterar
void Aposta::geraAposta(const vector<unsigned> & valores, unsigned n)
{
    int m = 0;
    for (unsigned int valor : valores) {
        if(numeros.find(valor) == numeros.end()) {  // not found
            numeros.insert(valor);
            m++;
        }
        if (n==m) break;
    }
}

//a alterar
unsigned Aposta::calculaCertos(const tabHInt & sorteio) const
{
    unsigned count = 0;
    for (unsigned int it : sorteio) {
        for (unsigned int numero : numeros) {
            if (it == numero) {
                count++;
                break;
            }
        }
    }
    return count;
}





