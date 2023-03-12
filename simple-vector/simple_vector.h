#pragma once
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iterator>


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
    
    SimpleVector(const SimpleVector& other):arr(other.size),size(other.size), capacity(other.capacity) {
        if (!other.IsEmpty()) {
              SimpleVector scum(other.capacity);
              std::copy(other.begin(),other.end(),scum.begin());
              arr.swap(scum.arr);
        }

    }
    
    SimpleVector(SimpleVector&& other) {
        
        swap(other);
        
    }
    
    SimpleVector(size_t size): arr(size),size(size),capacity(size) {
        std::generate(begin(), end(), []{return Type();});
    }
    
    SimpleVector(size_t size_, const Type& value): arr(size_), size(size_), capacity(size_){
        std::fill(arr.Get(), arr.Get() + size_, value);
    }
    
    SimpleVector(size_t size_, Type&& value): arr(size_), size(size_), capacity(size_) {
        SimpleVector<Type> scum(size_);
        generate(scum.begin(),scum.end(),[value]{return move(value);});
        swap(scum);
    }

    SimpleVector(std::initializer_list<Type> init) : arr(init.size()),size(init.size()),capacity(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }
    
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if((rhs.arr).Get() != arr.Get()){
            SimpleVector scum(rhs);
            swap(scum);
        }
        
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        if( rhs.arr.Get() != arr.Get()){
            SimpleVector scum(rhs);
            size = std::exchange(rhs.size,size);
            capacity = std::exchange(rhs.capacity,capacity);
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
        assert(index < size);
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size);
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size){
            throw std::out_of_range("index > size");
        }
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        
        if(index >= size) {
            throw std::out_of_range("index > size");
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
                scum[0] = std::move(value);
                arr.swap(scum);
                size = 1;
                return;
            }
            else {
                ArrayPtr<Type> scum(capacity);
                scum[0] = std::move(value);
                arr.swap(scum);
                size = 1;
                return;
            }
        }
        if (size < capacity) {
            arr[size] = std::move(value);
            ++size;
        }
        else {
            ArrayPtr<Type> scum(capacity * 2);
            std::move(arr.Get(), arr.Get() + size, scum.Get());
            scum[size] = std::move(value);
            arr.swap(scum);
            ++size;
            capacity = capacity * 2;
        }
    }
    
    
    void PopBack() noexcept {
        assert(size > 0);
        --size;
    }
    
    void swap(SimpleVector& other) noexcept {
        arr.swap(other.arr);
        size_t size_other = size;
        size_t capacity_other = capacity;
        size = other.size;
        other.size = size_other;
        capacity = other.capacity;
        other.capacity = capacity_other;
    }
    
    void swap(SimpleVector&& other) noexcept {
        arr.swap(move(other.arr));
        size_t siz = size;
        size_t cap = capacity;
        size = std::move(other.size);
        other.size = siz;
        capacity = std::move(other.capacity);
        other.capacity = cap;
    }
    
    Iterator Erase(ConstIterator pos) {
        assert(!IsEmpty());
        assert(pos >= begin() && pos <= end());
        auto distance = std::distance(cbegin(), pos);
        std::move(begin() + distance + 1, end(), begin() + distance);
        --size;
        return begin() + distance;
    }
    
     Iterator Insert(ConstIterator pos, const Type& value) {
     assert(pos >= begin() && pos <= end());
        auto distance = std::distance(cbegin(), pos);
        if (capacity > size) {
            copy(begin() + distance , end(),begin() + distance + 1 );
            arr[distance] = value;
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
        copy(begin(), begin() + distance, scum.begin());
        copy(begin() + distance, end(), scum.begin() + distance + 1);
        scum[distance] = value;
        arr.swap(scum.arr);
        ++size;
        capacity = c_s;
        }
        return begin() + distance;
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
    assert(pos >= begin() && pos <= end());
        auto cop = std::distance(cbegin(), pos);
        if (capacity > size) {
            std::move(begin() + cop , end(),begin() + cop + 1 );
            arr[cop] = std::move(value);
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
            std::move(begin(), begin() + cop, scum.begin());
            std::move(begin() + cop, end(), scum.begin() + cop +1);
            scum[cop] = std::move(value);
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
            std::generate(arr.Get() + size, arr.Get() + new_size, []{return Type();});
            size = new_size;
        }
        else {
            SimpleVector<Type> arr_new(new_size * 2);
            std::move(begin(),end(),arr_new.begin());
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
                std::copy(begin(),end(),scum.Get());
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
    if(lhs.GetSize() == rhs.GetSize()){
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }
    return false;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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

