# include <iostream>
# include <stack>

using namespace std;

template <class T>
class StackExt {
private:
    stack<T> valores;
    stack<T> minimos;
public:
	StackExt() {};
	bool empty() const; 
	T &top();
	void pop();
	void push(const T & val);
	T &findMin();
};

// a alterar
template <class T> 
bool StackExt<T>::empty() const
{
	return valores.empty();
}

//a alterar
template <class T> 
T& StackExt<T>::top()
{
    return valores.top();
    T *novo = new T();
    return *novo;
}

//a alterar
template <class T> 
void StackExt<T>::pop()
{
    if(!valores.empty()) {
        if (valores.top() == minimos.top()) {
            valores.pop();
            minimos.pop();
        } else
            valores.pop();
    }
}

//a alterar
template <class T> 
void StackExt<T>::push(const T & val)
{
    valores.push(val);
    if(minimos.empty() || val <= minimos.top()) {
        minimos.push(val);
    }
}

//a alterar
template <class T> 
T& StackExt<T>::findMin()
{
    return minimos.top();
//    T *novo = new T();
//    return *novo;
}

