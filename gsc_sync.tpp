//
//  gsc_sync.cpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_helper.h"
#include <stdio.h>
#include <algorithm>    // std::copy, memset
#include <cstdlib>

namespace gsc {

/**
 *  @param mark array of bits
 *  @param size array size
 *  @param bufferSizeMax size of accumulative buffer
 */
template <typename IntegerType, typename BinaryType>
Sync<IntegerType, BinaryType>::Sync(const BinaryType * const mark, size_t size, size_t bufferSizeMax) {
    mark_ = new std::vector<BinaryType>(mark, mark + size);
    accumulator_.reserve(bufferSizeMax);
    
    m_synchronized = false;
}

template <typename IntegerType, typename BinaryType>
Sync<IntegerType, BinaryType>::Sync::~Sync()
{
    delete mark_;
    mark_ = nullptr;
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
    size_t m_size = mark_->size();
    assert(size >= m_size && "Container is too small.");
    assert(!m_synchronized && "Already synchronized.");
    
    const size_t step = 1;
    BinaryType *m_ptr = &mark_->front();
    size_t integrate_len = integrate<IntegerType, BinaryType>(container, size, 0, step, m_ptr, m_size);
    bool result = integrate_len == m_size;
    
    assert(result && "Failed to integrate sync marker.");
    
    if(result) {
        m_synchronized = true;
        printf("\033[95m[gsc_core]\033[0m: \033[93m synchronized \033[0m\n");
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
    
    assert(mark_->size() > 0 && "ArgumentError");
    assert(accumulator_.capacity() % size == 0 && "ArgumentError");
    assert(!m_synchronized && "Already synchronized.");
    
    if(accumulator_.size() == accumulator_.capacity()) {
        accumulator_.erase(accumulator_.begin() + size);    // delete first size elements
    }
    
    for(size_t i = 0; i < size; ++i)
    {
        BinaryType *bits = 0;
        integerToBits(container[i], &bits);
        accumulator_.push_back(tmp_abs(bits[0]));
        delete_arr_primitive_s(&bits);
    }
    
    assert(accumulator_.capacity() >= accumulator_.size() && "ArgumentError");
    
    size_t pos;

    BinaryType *m_ptr = &mark_->front();
    BinaryType *a_ptr = &accumulator_.front();
    bool result = contains<BinaryType>(m_ptr, mark_->size(), a_ptr, accumulator_.size(), pos);
    
    // index of the last integrated bit that was recovered from the original (not accumulative) container
    if(result) {
        m_synchronized = true;
        endIdx = pos + accumulator_.size() - (accumulator_.end() - accumulator_.begin());
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
    accumulator_.erase(accumulator_.begin() + accumulator_.capacity());
    m_synchronized = false;
}
    
template <typename IntegerType, typename BinaryType>
inline bool Sync<IntegerType, BinaryType>::isSynchronized() const
{
    return m_synchronized;
}
    

template <typename IntegerType, typename BinaryType>
inline size_t Sync<IntegerType, BinaryType>::markerSize() const
{
    return mark_->size();
}
    
}
