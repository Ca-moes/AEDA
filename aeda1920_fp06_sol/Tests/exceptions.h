#ifndef SRC_EXCEPTIONS_H_
#define SRC_EXCEPTIONS_H_

#include <string>
using namespace std;

class FilaVazia {
	string msg;
public:
	FilaVazia() {msg = "Fila Vazia";}
	string getMsg() const {return msg;}
	//friend ostream& operator<<(ostream& os, const FilaVazia& fv);
};

/*ostream& operator<< (ostream& os, const FilaVazia& fv){
    os << fv.getMsg();
    return os;*/

#endif /* SRC_EXCEPTIONS_H_ */
