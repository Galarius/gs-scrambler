//
//  gsc_sync.cpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_sync.h"
#include "gsc_helper.h"

#include <stdio.h>
#include <algorithm>    // std::copy, memset
#include <cstdlib>

namespace gsc {

/**
 *  @param mark array of bits
 *  @param size array size
 *  @param bufferMaxSize size of accumulative buffer
 */
template <typename IntegerType, typename BinaryType>
Sync<IntegerType, BinaryType>::Sync(const BinaryType * const mark, size_t size, size_t bufferMaxSize) {
    m_mark.buffer = 0;
    m_mark.bufferMaxSize = size;
    m_mark.bufferSize = size;
    new_arr_primitive_s(&m_mark.buffer, m_mark.bufferMaxSize);
    m_accBuffer.buffer = 0;
    m_accBuffer.bufferMaxSize = bufferMaxSize;
    m_accBuffer.bufferSize = 0;
    new_arr_primitive_s(&m_accBuffer.buffer, m_accBuffer.bufferMaxSize);
    memset(m_accBuffer.buffer, 0, m_accBuffer.bufferMaxSize);
    std::copy(mark, mark + size, m_mark.buffer);
    m_pointer = m_mark.buffer;
    m_synchronized = false;
}

template <typename IntegerType, typename BinaryType>
Sync<IntegerType, BinaryType>::Sync::~Sync()
{
    delete_arr_primitive_s(&m_mark.buffer);
    delete_arr_primitive_s(&m_accBuffer.buffer);
}

    
/**
 *  Insert marker inside the container.
 *
 *  @param container container to hide marker in
 *  @param size      size of the contaner
 *
 *  @return true if marker fully integrated inside the container and
 *          false if more data required to insert what was left from marker
 */
template <typename IntegerType, typename BinaryType>
bool Sync<IntegerType, BinaryType>::put(IntegerType **container, size_t size)
{
    if(m_synchronized)
    {
        printf("resetting...\n");
        reset();
    }
    
    const int step = 1;
    size_t l = integrate(container, size, 0, step, m_pointer, m_mark.bufferSize);
    m_pointer += l;
    bool result = l >= m_mark.bufferSize;
    if(result) {
        m_synchronized = true;
        printf("\033[95m[gsc_core]\033[0m: \033[93m synchronized \033[0m\n");
    } else {
        m_mark.bufferSize -= l;    // new marker size
    }
    return result;
}

/**
 *  Try to detect marker inside container.
 *
 *  @param container container to detect marker in
 *  @param size      size of the container
 *  @param endIdx    index of the last integrated bit that was recovered from the original (not accumulative) container
 *
 *  @return operation result
 */
template <typename IntegerType, typename BinaryType>
bool Sync<IntegerType, BinaryType>::scan(const IntegerType *const container, size_t size, size_t &endIdx)
{
    if(m_synchronized)
    {
        printf("already synchronized\n");
        return true;
    }
    
    size_t idx;
    if(m_accBuffer.bufferSize >= m_accBuffer.bufferMaxSize) {
        m_accBuffer.bufferSize = idx = 0;
    } else {
        idx = m_accBuffer.bufferSize;
    }
    size_t j = idx;
    for(size_t i = 0; i < size; ++i, ++j)
    {
        BinaryType *bits = 0;
        integerToBits(container[i], &bits);
        m_accBuffer.buffer[j] = tmp_abs(bits[0]);
        delete_arr_primitive_s(&bits);
        ++m_accBuffer.bufferSize;
    }
    
    size_t pos;
    bool result = contains(m_mark.buffer, m_mark.bufferMaxSize, m_accBuffer.buffer, m_accBuffer.bufferSize, pos);
    
    // index of the last integrated bit that was recovered from the original (not accumulative) container
    if(result) {
        m_synchronized = true;
        endIdx = pos + m_mark.bufferMaxSize - idx;
        printf("\033[95m[gsc_core]\033[0m: \033[93m synchronized \033[0m\n");
    }
    
    return result;
}

/**
 *  Reset scanner
 */

template <typename IntegerType, typename BinaryType>
void Sync<IntegerType, BinaryType>::reset()
{
    memset(m_accBuffer.buffer, 0, m_accBuffer.bufferMaxSize);
    m_pointer = m_mark.buffer;
    m_mark.bufferSize = m_mark.bufferMaxSize;
    m_synchronized = false;
}
    
template <typename IntegerType, typename BinaryType>
bool Sync<IntegerType, BinaryType>::isSynchronized() const
{
    return m_synchronized;
}
    

template <typename IntegerType, typename BinaryType>
size_t Sync<IntegerType, BinaryType>::markerSize() const
{
    return m_mark.bufferMaxSize;
}
    
}
