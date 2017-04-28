#ifndef SORTEDVECTORWRAPPER_H
#define SORTEDVECTORWRAPPER_H
#include <vector>
#include <algorithm>
#include <iostream>
#include <functional>

template <class T>
struct SortedVectorWrapper
{

    std::vector<T> vector_; //thats my vector hector

    SortedVectorWrapper(){}
    SortedVectorWrapper(std::initializer_list<T> elements) : vector_{elements} {}

    size_t size() { return vector_.size(); }

    T& operator[](size_t ind) { return vector_[ind];}

    void push_back(T elem) {vector_.push_back(elem);}

    void sort() {
        std::sort(vector_.begin(), vector_.end());
    }

    bool contains(T elem) {
        return getIndexOf(elem) != vector_.size();
    }

    void push_back_sorted(T elem, std::function<bool(const T&, const T&)> comp) {
        int index = 0;
        while(comp(vector_[index], elem) && index < vector_.size()) { ++index; }
        if(index == vector_.size()) vector_.push_back(elem);
        else vector_.insert(vector_.begin() + index, elem);
    }

    template<typename _Equality>
    bool contains(T elem, _Equality eq) {
        return getIndexOf(elem, eq) != vector_.size();
    }

    template <typename _Compare>
    void sort(_Compare comparer) {
        std::sort(vector_.begin(), vector_.end(), comparer);
    }

    size_t getIndexOf(const T elem) const {
        return std::lower_bound(vector_.begin(), vector_.end(), elem) - vector_.begin();
    }

    template <class Other>
    size_t getIndexOf(const Other& elem) const  {
        return std::lower_bound(vector_.begin(), vector_.end(), elem) - vector_.begin();
    }

    size_t getIndexOf(T elem, std::function<bool(const T&, const T&)> equal) const  {
        return std::lower_bound(vector_.begin(), vector_.end(), elem, equal) - vector_.begin();
    }

    void print() {
        for(auto elem : vector_) std::cout << elem << " ";
        std::cout << '\n';
    }

    template<typename _Printable>
    void print(_Printable printer) {
        for(auto elem : vector_) std::cout << printer(elem) << " ";
        std::cout << '\n';
    }

    auto begin() { return vector_.begin(); }

    auto end() { return vector_.end(); }

};

#endif // SORTEDVECTORWRAPPER_H
