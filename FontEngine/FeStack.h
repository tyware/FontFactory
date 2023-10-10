#pragma once
#include "ApiHeader.h"
//#include <vector>
#include <exception>

#define STACK_INCREMENT_AMOUNT           256

namespace FF_Font_Factory
{
    template<typename T> 
    class __declspec(dllexport) CFeStack
    {
    public:
        CFeStack<T>();
        virtual ~CFeStack<T>();

        CFeStack<T>& operator=(const CFeStack<T> &s);

        void push(T x);
        void pop();
        T top();
        T get(int index);
        bool isEmpty();
        size_t size();
        void reset();
        void moveToTop(int index);

    //private:
        size_t maxElems;
        size_t numElems;
        T *pElems;;
    };

    template<typename T> 
    CFeStack<T>::CFeStack()
        : maxElems(0), numElems(0)
    {
        pElems = NULL;//new T[STACK_INCREMENT_AMOUNT];
    }


    template<typename T> 
    CFeStack<T>::~CFeStack()
    {
        reset();
    }

    template<typename T>
    void CFeStack<T>::reset()
    {
        if(pElems) delete[] pElems;
        pElems = NULL;
        numElems = maxElems = 0;
    }

    template<typename T>
    CFeStack<T>& CFeStack<T>::operator=(const CFeStack<T> &s)
    {
        maxElems = s.maxElems;
        numElems = s.numElems;
        if(maxElems > 0)
        {
            if(pElems) delete[] pElems;
            pElems = new T[maxElems];
            if(numElems > 0)
                memcpy(pElems, s.pElems, numElems * sizeof(T));
        }
    }

    template<typename T>
    bool CFeStack<T>::isEmpty(){
	    if(numElems == 0){
		    return true;
	    }else{
		    return false;
	    }
    }

    template<typename T>
    size_t CFeStack<T>::size()
    {
        return numElems;
    }

    template<typename T>
    void CFeStack<T>::push(T x){
        if(numElems >= maxElems)
        {
            T* pNewElems = new T[maxElems + STACK_INCREMENT_AMOUNT];
            memcpy(pNewElems, pElems, sizeof(T) * maxElems);
            delete pElems;
            pElems = pNewElems;
            maxElems += STACK_INCREMENT_AMOUNT;
        }
        pElems[numElems++] = x;
    }

    template<typename T>
    void CFeStack<T>::pop(){
        if(numElems > 0)
            numElems--;
    }

    template<typename T>
    T CFeStack<T>::top(){
        if(numElems <= 0 )
            throw std::range_error("No element in stack");
        return pElems[numElems - 1];
    }

    template<typename T>
    T CFeStack<T>::get(int index){

        if(index < 0 || index >= numElems)
            throw std::range_error("No element in stack");
        return pElems[numElems - index - 1];
    }

    template<typename T>
    void CFeStack<T>::moveToTop(int index){

        if(index < 0 || index >= numElems)
            throw std::range_error("No element in stack");

        int start = (int)numElems - index;
        T ele = pElems[start];
        for(int i = start; i < numElems; i++)
            pElems[i] = pElems[i+1];

        pElems[numElems - 1] = ele;
    }
};


