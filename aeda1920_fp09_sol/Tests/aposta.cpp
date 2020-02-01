#include "aposta.h"
#include <iostream>
#include <sstream>

using namespace std;

bool Aposta::contem(unsigned num) const
{
	tabHInt::const_iterator it = numeros.find(num);
    return !(it == numeros.end());
}


void Aposta::geraAposta(const vector<unsigned> & valores, unsigned n)
{
	numeros.clear();
	unsigned int nValores=0, i=0;
	while (nValores<n) {
		pair<tabHInt::iterator, bool> res = numeros.insert(valores[i]);
		if (res.second)
			nValores++;
		i++;
	}
}


unsigned Aposta::calculaCertos(const tabHInt & sorteio) const
{
	unsigned certos = 0;
	for (unsigned int it : sorteio) {
		if ( contem(it) )
			certos++;
	}
	return certos;
}


unsigned Aposta::somaNumeros() const
{
	unsigned soma = 0;
	for (unsigned int numero : numeros)
		soma +=numero;
	return soma;
}

unsigned Aposta::tamanho() const
{
	return numeros.size();
}



