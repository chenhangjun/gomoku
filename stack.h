#ifndef STACK_H
#define STACK_H

#include<cassert>

template<class T, int size = 250>
class Stack {
    public:
        Stack();
        void push(const T &item);
        T pop();
        void clear();
        const T &peek() const;    //取栈顶
        const T &peek2() const;  //取栈顶第二
        int Size() const;  //去栈元素个数
        bool isEmpty() const;
        bool isFull() const;
    private:
        T list[250];
        int top;
};

template<class T, int size>
Stack<T, size>::Stack(){
    top = -1;
}

template<class T, int size>
void Stack<T, size>::push(const T &item)
{
    assert(!isFull());
    list[++top] = item;
}

template<class T, int size>
T Stack<T, size>::pop()
{
    assert(!isEmpty());
    return list[top--];
}

template<class T, int size>
const T &Stack<T, size>::peek() const
{
    assert(!isEmpty());
    return list[top];
}

template<class T, int size>
const T &Stack<T, size>::peek2() const
{
    assert(top >= 1);
    return list[top - 1];
}

template<class T, int size>
int Stack<T, size>::Size() const
{
    return top + 1;
}

template<class T, int size>
bool Stack<T, size>::isEmpty() const
{
    return top == -1;
}

template<class T, int size>
bool Stack<T, size>::isFull() const
{
    return top == size - 1;
}

template<class T, int size>
void Stack<T, size>::clear() {
    top = -1;
}


#endif // STACK_H
