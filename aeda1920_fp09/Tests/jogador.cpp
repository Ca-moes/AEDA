#include "jogador.h"

//a alterar
void Jogador::adicionaAposta(const Aposta & ap)
{
    apostas.insert(ap);
}

//a alterar
unsigned Jogador::apostasNoNumero(unsigned num) const
{
    unsigned count = 0;
    for (const Aposta& apost : apostas){
        if(apost.getNumeros().find(num) != apost.getNumeros().end()) {  // not found
          count++;
        }
    }
	return count;
}

//a alterar
tabHAposta Jogador::apostasPremiadas(const tabHInt & sorteio) const
{
	tabHAposta money;
	unsigned int num = 3;
    for (Aposta apost : apostas) {
        if (apost.calculaCertos(sorteio) > 3)
            money.insert(apost);
    }
	return money;
}
