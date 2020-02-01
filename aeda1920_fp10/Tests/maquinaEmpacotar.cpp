#include "maquinaEmpacotar.h"
#include <sstream>


MaquinaEmpacotar::MaquinaEmpacotar(int capCaixas): capacidadeCaixas(capCaixas)
{}

unsigned MaquinaEmpacotar::numCaixasUsadas() {
    /*int j = caixas.size();
    cout << "caixas size: " << caixas.size() << endl;
    for (int i = 0; i < j; i++) {
        cout << "ID - " << caixas.top().getID() << endl;
        caixas.pop();
    }*/
	return caixas.size();
}

unsigned MaquinaEmpacotar::addCaixa(Caixa& cx) {
	caixas.push(cx);
	return caixas.size();
}

HEAP_OBJS MaquinaEmpacotar::getObjetos() const {
	return this->objetos;
}

HEAP_CAIXAS MaquinaEmpacotar::getCaixas() const {
	return this->caixas;
}


// a alterar
unsigned MaquinaEmpacotar::carregaPaletaObjetos(vector<Objeto> &objs) {
    HEAP_OBJS temp;
    vector<Objeto>::iterator it;
    for (it = objs.begin(); it != objs.end(); it++) {
        temp.push(*it);
    }
    objs.clear();
    int j = temp.size(), counter = 0;
    for (int i = 0; i < j; ++i) {
        if (temp.top().getPeso() <= capacidadeCaixas) {
            counter++;
            objetos.push(temp.top());
        } else
            objs.push_back(temp.top());
        temp.pop();
    }
    return counter;
}

// a alterar
Caixa MaquinaEmpacotar::procuraCaixa(Objeto& obj) {
    int size = caixas.size();
    HEAP_CAIXAS caixas2;
    Caixa cx;
    bool found = false;
    for (int i = 0; i < size; ++i) {
        if (caixas.top().getCargaLivre() == obj.getPeso() && !found){
            cx = caixas.top();
            found = true;
        }
        else
            caixas2.push(caixas.top());
        caixas.pop();
    }

    size = caixas2.size();
    for (int i = 0; i < size; ++i) {
        caixas.push(caixas2.top());
        caixas2.pop();
    }

    if(size == caixas2.size())
        caixas.push(cx);

	return cx;
}


// a alterar
unsigned MaquinaEmpacotar::empacotaObjetos() {
	return 0;
}

// a alterar
string MaquinaEmpacotar::imprimeObjetosPorEmpacotar() const {
	return "";
}

// a alterar
Caixa MaquinaEmpacotar::caixaMaisObjetos() const {
	Caixa cx;
	return cx;
}



