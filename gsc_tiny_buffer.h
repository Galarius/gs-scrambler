//
//  gsc_tiny_buffer.h
//  core
//
//  Created by Galarius on 28.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__gsc_tiny_buffer__
#define __core__gsc_tiny_buffer__

#include <cstddef>
#include <stdexcept>
#include <cassert>

/**
 *  Custom container that implements placement new to reconstruct objects on preallocated memory.
 *
 *  Requirements for T:
 *      1) copy constructor
 *      2) non-throw destructor
 *
 */
template <typename T>
class TinyBuffer
{
public:
    explicit TinyBuffer(size_t, const T& value = T());
    
    TinyBuffer(const TinyBuffer &);
    
    TinyBuffer();
    
    ~TinyBuffer() throw();
    
    TinyBuffer& operator=(const TinyBuffer &);
    
public:
    size_t size() const;

    T& operator[](size_t);				// setter
    const T& operator[](size_t) const;	// getter
    
private:
    T* data;
    size_t length;
};

/* include implementation */
#include "gsc_tiny_buffer.tpp"

#endif /* defined(__core__gsc_tiny_buffer__) */
