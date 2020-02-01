#include "jogador.h"


void Jogador::adicionaAposta(const Aposta & ap)
{
	apostas.insert(ap);
}

unsigned Jogador::apostasNoNumero(unsigned num) const
{
	unsigned count = 0;
	
	for(const auto & aposta : apostas)
		if (aposta.contem(num))
			count++;
			
	return count;
}


tabHAposta Jogador::apostasPremiadas(const tabHInt & sorteio) const
{
	tabHAposta money;
	money.clear();

	for(const auto & aposta : apostas)
	{
	    if (aposta.calculaCertos(sorteio) > 3)
	        money.insert(aposta);
	}
			
	return money;
}
