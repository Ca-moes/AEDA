#include <iostream>
#include <string>
#include <vector>

using namespace std;

class InfoCartao {
public:
    /**
       * a public variable.
       * nome do cliente.
       */
	string nome;
    /**
       * a public variable.
       * True se estiver dentro do parque.
       */
	bool presente;
};
/**
 *  Classe de Parque de Estacionamento.
 *  Descrição Elaborada do Parque de Estacionamento
 */
class ParqueEstacionamento {
private:
	unsigned int vagas;
	const unsigned int lotacao; /**< numero de lugares do parque */
	vector<InfoCartao> clientes;
    /**
       * a private variable.
       * Numero maximo de clientes.
       */
	const unsigned int numMaximoClientes;
public:
    /**
       * A constructor.
       * A more elaborate description of the constructor.
       */
	ParqueEstacionamento(unsigned int lot, unsigned int nMaxCli);
	/**
	 * Adiciona clientes e retorna o seu sucesso
	 * @param nome do cliente a adicionar
	 * @return True se conseguiu adiconar, falso otherwise
	 */
	bool adicionaCliente(const string & nome);
	bool retiraCliente(const string & nome);
	bool entrar(const string & nome);
	bool sair(const string & nome);
	/**
	 * Determina o index do cliente.
	 * @param nome do cliente a verificar
	 * @return -1 se não estiver dentro ou o index
	 */
	int posicaoCliente(const string & nome) const;
	unsigned int getNumLugares() const;
	unsigned int getNumMaximoClientes() const;
	unsigned int getNumLugaresOcupados() const;
	unsigned int getNumClientesAtuais() const;
	const ParqueEstacionamento & operator += (const ParqueEstacionamento & p2);
};
