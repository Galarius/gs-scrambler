//
//  gsc_acc_buffer.h
//  core
//
//  Created by Galarius on 29.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef core_gsc_acc_buffer_h
#define core_gsc_acc_buffer_h

#include "gsc_memory.h"
#include <cstddef>
// keep assert enabled in release
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <algorithm>    // std::copy

namespace gsc {

template <typename T>
struct AccBuffer {
    T* buffer;                   // accumulative buffer
    size_t bufferSizeMax;        // accumulative buffer maximum size
    size_t bufferSizeCurrent;    // accumulative buffer current size (filled)
};

template <typename T>
inline void allocate_buffer(AccBuffer<T>& b, size_t size)
{
    b.buffer = nullptr;
    new_arr_primitive_s(&b.buffer, size);
    b.bufferSizeMax = size;
    b.bufferSizeCurrent = 0;
}

template <typename T>
inline void deallocate_buffer(AccBuffer<T>& b)
{
    delete_arr_primitive_s(&b.buffer);
    b.bufferSizeCurrent = b.bufferSizeMax = 0;
}

template <typename T>
inline void add_data_to_buffer(const T* const data, size_t size, AccBuffer<T>& b)
{
    assert(size <= b.bufferSizeMax-b.bufferSizeCurrent && "Not enough space.");
    
    std::copy(data, data+size, b.buffer + b.bufferSizeCurrent);
    b.bufferSizeCurrent += size;
}

}

#endif
