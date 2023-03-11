#pragma once
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iterator>

using namespace std;


class ReserveProxyObj {
private:
    size_t a;
public:
    ReserveProxyObj(size_t c) : a(c) {};
    
    int get_a() {
        return a;
    }
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}



template <typename Type>
class SimpleVector {
private:
    ArrayPtr<Type> arr;
    size_t size = 0;
    size_t capacity = 0;

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;
    
    SimpleVector(ReserveProxyObj res) {
        Reserve(res.get_a());
    }
    
    SimpleVector(const SimpleVector& other) {
        if (!other.IsEmpty()) {
              SimpleVector scum(other.capacity);
              scum.size = other.size;
              scum.capacity = other.capacity;
              copy(other.begin(),other.end(),scum.begin());
              swap(scum);
        }

    }
    
    SimpleVector(SimpleVector&& other) {
        
        swap(other);
        
    }
    
    SimpleVector(size_t size): arr(size),size(size),capacity(size) {
        generate(begin(), end(), []{return Type();});
    }
    
    SimpleVector(size_t size_, const Type& value): arr(size_){
        fill(arr.Get(), arr.Get() + size_, value);
        size = size_;
        capacity = size_;
    }
    
    SimpleVector(size_t size_, Type&& value): arr(size_) {
        SimpleVector<Type> scum(size_);
        
        for(int i = 0; i < size_; ++i) {
            scum[i] = move(value);
        }
        
        size = size_;
        capacity = size_;
        swap(scum);
    }

    SimpleVector(std::initializer_list<Type> init) : arr(init.size()),size(init.size()),capacity(init.size()) {
        copy(init.begin(), init.end(), begin());
    }
    
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if(!rhs.IsEmpty() && (rhs.arr).Get() != arr.Get()){
            SimpleVector scum(rhs);
            swap(scum); 
        }
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        if( !rhs.IsEmpty() && (rhs.arr).Get() != arr.Get()){
            SimpleVector scum(rhs);
            size = move(rhs.size);
            capacity = move(rhs.capacity);
            rhs.size = 0;
            rhs.capacity = 0;
            swap(scum);
            
        }
        return *this;
    }


    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size){
            throw out_of_range("index > size");
        }
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        
        if(index >= size) {
            throw out_of_range("index > size");
        }
        return arr[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size = 0;
    }
    
    void PushBack(const Type& value){
        if (IsEmpty()) {
            if(capacity == 0){
                capacity = 1;
                ArrayPtr<Type> scum(1);
                scum[0] = value;
                arr.swap(scum);
                size = 1;
                return;
            }
            else{
                ArrayPtr<Type> scum(capacity);
                scum[0] = value;
                arr.swap(scum);
                size = 1;
                return;
              }
        }
        
        if (size < capacity) {
            arr[size] = value;
            ++size;
        }
        
        else {
            ArrayPtr<Type> scum(capacity * 2);
            copy(arr.Get(), arr.Get() + size, scum.Get());
            scum[size] = value;
            arr.swap(scum);
            ++size;
            capacity = capacity * 2;
        }
    }
    
    void PushBack(Type&& value){
        if (IsEmpty()) {
            if(capacity == 0){
                capacity = 1;
                ArrayPtr<Type> scum(1);
                scum[0] = move(value);
                arr.swap(scum);
                size = 1;
                return;
            }
            else {
                ArrayPtr<Type> scum(capacity);
                scum[0] = move(value);
                arr.swap(scum);
                size = 1;
                return;
            }
        }
        if (size < capacity) {
            arr[size] = move(value);
            ++size;
        }
        else {
            ArrayPtr<Type> scum(capacity * 2);
            move(arr.Get(), arr.Get() + size, scum.Get());
            scum[size] = move(value);
            arr.swap(scum);
            ++size;
            capacity = capacity * 2;
        }
    }
    
    
    void PopBack() noexcept {
        if(!IsEmpty()){
            --size;
        }
    }
    
    void swap(SimpleVector& other) noexcept {
        arr.swap(other.arr);
        size_t siz = size;
        size_t cap = capacity;
        size = other.size;
        other.size = siz;
        capacity = other.capacity;
        other.capacity = cap;
    }
    
    void swap(SimpleVector&& other) noexcept {
        arr.swap(move(other.arr));
        size_t siz = size;
        size_t cap = capacity;
        size = move(other.size);
        other.size = siz;
        capacity = move(other.capacity);
        other.capacity = cap;
    }
    
    Iterator Erase(ConstIterator pos) {
        if (IsEmpty()) {
            return nullptr;
        }
        auto cop = distance(cbegin(), pos);
        move(begin() + cop + 1, end(), begin() + cop);
        --size;
        return begin() + cop;
    }
    
     Iterator Insert(ConstIterator pos, const Type& value) {
        auto cop = std::distance(cbegin(), pos);
        if (capacity > size) {
            copy(begin() + cop , end(),begin() + cop + 1 );
            arr[cop] = value;
            ++size;
        }
        else {
        size_t c_s;
            if(size + 1 > capacity * 2){
         c_s = size + 1;
            }
            else {
            c_s = capacity * 2;
            }
        
        SimpleVector<Type> scum(c_s);
        copy(begin(), begin() + cop, scum.begin());
        copy(begin() + cop, end(), scum.begin() + cop +1);
        scum[cop] = value;
        arr.swap(scum.arr);
        ++size;
        capacity = c_s;
        }
        return begin() + cop;
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        auto cop = std::distance(cbegin(), pos);
        if (capacity > size) {
            move(begin() + cop , end(),begin() + cop + 1 );
            arr[cop] = move(value);
            ++size;
        }
        else {
            size_t c_s;
            if(size + 1 > capacity * 2) {
                c_s = size + 1;
            }
            else {
                c_s = capacity * 2;
            }
       
            SimpleVector<Type> scum(c_s);
            move(begin(), begin() + cop, scum.begin());
            move(begin() + cop, end(), scum.begin() + cop +1);
            scum[cop] = move(value);
            arr.swap(scum.arr);
            ++size;
            capacity = c_s;
        }
        return begin() + cop;
   }
    
    void Resize(size_t new_size) {
        if(new_size <= size){
            size = new_size;
        }
        else if(new_size <= capacity) {
            generate(arr.Get() + size, arr.Get() + new_size, []{return Type();});
            size = new_size;
        }
        else {
            SimpleVector<Type> arr_new(new_size * 2);
            move(begin(),end(),arr_new.begin());
            arr.swap(arr_new.arr);
            capacity = new_size * 2;
            size = new_size;
        }
    }
        
    void Reserve(size_t new_capacity) {
        if (new_capacity == capacity) {
            return;
        }
        if (new_capacity > capacity) {
            ArrayPtr<Type> scum(new_capacity);
            if (capacity == 0){
                arr.swap(scum);
                capacity = new_capacity;
            }
            else {
                copy(begin(),end(),scum.Get());
                capacity = new_capacity;
            }
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
      return   Iterator{arr.Get()};
        
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
   return  Iterator{arr.Get() + size};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return   ConstIterator{arr.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return   ConstIterator{arr.Get()+size};
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return   ConstIterator{arr.Get()};
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return   ConstIterator{arr.Get()+size};
    }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    
    return equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs < rhs);
}

