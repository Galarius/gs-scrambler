//
//  gsc_tiny_buffer.cpp
//  core
//
//  Created by Galarius on 28.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

template<typename T>
TinyBuffer<T>::TinyBuffer(size_t size, const T& value) : length(size)
{
    data = static_cast<T*>(::operator new(this->length*sizeof(T)));
    T *ptr = data;
    while(size--) {
        new(ptr++)T(value);
    }
}


template<typename T>
TinyBuffer<T>::TinyBuffer(const TinyBuffer & other) : length(other.length)
{
    data = static_cast<T*>(::operator new(this->length*sizeof(T)));
    T *ptr = data, *other_ptr=other.data;
    size_t l = this->length;
    while (l--) {
        new(ptr++)T(*other_ptr++);
    }
}

template<typename T>
TinyBuffer<T>::TinyBuffer(void) :
    length(0), data(nullptr)
{ }

template<typename T>
TinyBuffer<T>::~TinyBuffer(void) throw()
{
    if(data) {
        while(length--) {
            (*data++).~T();
        }
        ::operator delete(data);
        data = nullptr;
    }
}

template<typename T>
TinyBuffer<T>& TinyBuffer<T>::operator=(const TinyBuffer<T> &other)
{
    if(this != &other) {
        if(data) {
            while(length--) {
                (*data++).~T();
            }
            ::operator delete(data);
            data = nullptr;
        }

        this->length = other.length;
        data = static_cast<T*>(::operator new(this->length*sizeof(T)));
        T *ptr = data, *other_ptr=other.data;
        size_t l = this->length;
        while (l--) {
            new(ptr++)T(other_ptr++);
        }
    }
    return *this;
}

template<typename T>
size_t TinyBuffer<T>::size() const
{
    return this->length;
}

template<typename T>
T& TinyBuffer<T>::operator[](size_t index)
{
    assert(index >= length && "Index must be less then TinyBuffer's length.");
    return *(data+index);
}

template<typename T>
const T& TinyBuffer<T>::operator[](size_t index) const
{
    assert(index >= length && "Index must be less then TinyBuffer's length.");
    return *(data+index);
}

