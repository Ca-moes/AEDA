#include "frota.h"
#include <string>

using namespace std;


int Frota::numVeiculos() const
{ return veiculos.size(); }

void Frota::adicionaVeiculo(Veiculo *v1)
{ veiculos.push_back(v1); }

int Frota::menorAno() const
{
	if (veiculos.empty()) return 0;
	auto it=veiculos.begin();
	int menorA=(*it)->getAno();
	it++;
	while(it!=veiculos.end()) {
		int ano1=(*it)->getAno();
		if (ano1<menorA) menorA=ano1;
		it++;
	}
	return menorA;
}

float Frota::totalImposto() const
{
	if (veiculos.empty()) return 0;
	float total=0;
	auto it=veiculos.begin();
	while(it!=veiculos.end()) {
		total +=(*it)->calcImposto();
		it++;
	}
	return total;
}

vector<Veiculo *> Frota::operator() (int a) const {
	vector<Veiculo *> veiculosAno;
	if (veiculos.empty()) return veiculosAno;
	auto it=veiculos.begin();
	while(it!=veiculos.end()) {
		if ((*it)->getAno() == a)
			veiculosAno.push_back(*it);
		it++;
	}
	return veiculosAno;
}
