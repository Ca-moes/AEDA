#include "veiculo.h"
#include <iostream>
#include <utility>

using namespace std;


Veiculo::Veiculo(string mc, int m, int a) { marca = move(mc); mes = m; ano = a; }
string Veiculo::getMarca() const { return marca; }
int Veiculo::getAno() const { return ano; }
int Veiculo::info(ostream &o) const {
    o << "Marca : " << marca << endl;
    o << "Mes   : " << mes << endl;
    o << "Ano   : " << ano << endl;
    return 3;
}
int Veiculo::info() const
{ return Veiculo::info(cout); }



Motorizado::Motorizado(string mc, int m, int a, string c, int cil) : Veiculo(move(mc),m,a){ combustivel = c; cilindrada = cil; }
string Motorizado::getCombustivel() const { return combustivel; }
int Motorizado::info(ostream &o) const {
    o << "Marca       : " << marca << endl;
    o << "Mes         : " << mes << endl;
    o << "Ano         : " << ano << endl;
    o << "Combustivel : " << combustivel << endl;
    o << "Cilindrada  : " << cilindrada << endl;
    return 5;
}
int Motorizado::info() const { return Veiculo::info(cout); }

Automovel::Automovel(string mc, int m, int a, string c, int cil) : Motorizado(move(mc), m, a, move(c), cil) {}
int Automovel::info() const { return Motorizado::info(cout); }
int Automovel::info(ostream &o) const {
    return Motorizado::info(o);
}

Camiao::Camiao(string mc, int m, int a, string c, int cil, int cm) : Motorizado(move(mc), m, a, move(c), cil){ carga_maxima = cm; }
int Camiao::info() const { return 6; }

Bicicleta::Bicicleta(string mc, int m, int a, string t) : Veiculo (move(mc),m,a){ tipo = t; }
int Bicicleta::info() const {return 4; }



