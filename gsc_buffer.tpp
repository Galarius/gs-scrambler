//
//  gsc_buffer.tpp
//  core
//
//  Created by Galarius on 30.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include <cassert>

namespace gsc {
    
/*-------------------------------------------------------*/

template <typename T>
Buffer<T>::Buffer(const T* const data, size_t size) : buf_(data), sizeMax_(size)
{ }
    
template <typename T>
Buffer<T>::Buffer(size_t size) : sizeMax_(size)
{
    buf_.reserve(sizeMax_);
}

template <typename T>
Buffer<T>::Buffer(const Buffer & other) :
    buf_(other.buf_)
{ }

template <typename T>
Buffer<T>::Buffer(Buffer &&other)  :
    buf_(std::move(other.buf_))
{ }

template <typename T>
Buffer<T>::Buffer() :
    buf_(), sizeMax_(0)
{ }

template <typename T>
Buffer<T>::~Buffer() throw()
{ }

template <typename T>
Buffer& Buffer<T>::operator=(const Buffer &other)
{
    if(this != &other)
    {
        buf_ = other.buf_;
    }
    return *this;
}

template <typename T>
Buffer& Buffer<T>::operator=(Buffer &&other)
{
    if (this!=&other)
    {
        buf_ = std::move(other.buf_);
    }
    return *this;
}
 
/*-------------------------------------------------------*/
template <typename T>
inline T* Buffer<T>::get_ptr(size_t offset) const
{
    auto it = buf_.begin();
    return (&*it) + offset;
}
    
}
