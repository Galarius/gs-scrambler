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
Sync::Sync(const Binary * const mark, Integer32 size, Integer32 bufferMaxSize)
    : m_mark(0), m_markSize(size), m_markSizeCurrent(size), m_accBuffer(0), m_accBufferMaxSize(bufferMaxSize), m_accBufferSize(0) {
        
    new_arr_primitive_s(&m_mark, m_markSize);
    new_arr_primitive_s(&m_accBuffer, m_accBufferMaxSize);
    memset(m_accBuffer, 0, m_accBufferMaxSize);
    std::copy(mark, mark + m_markSize, m_mark);
    m_pointer = m_mark;
}

Sync::~Sync()
{
    delete_arr_primitive_s(&m_mark);
    delete_arr_primitive_s(&m_accBuffer);
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
bool Sync::put(Integer16 **container, Integer32 size)
{
    if(m_synchronized)
    {
        printf("resetting...\n");
        reset();
    }
    
    const int step = 1;
    Integer32 l = integrate(container, size, 0, step, m_pointer, m_markSizeCurrent);
    m_pointer += l;
    bool result = l >= m_markSizeCurrent;
    if(result) {
        m_synchronized = true;
    } else {
        m_markSizeCurrent -= l;    // new marker size
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
bool Sync::scan(const Integer16 *const container, Integer32 size, Integer32 &endIdx)
{
    if(m_synchronized)
    {
        printf("already synchronized\n");
        return true;
    }
    
    Integer32 idx;
    
    if(m_accBufferSize >= m_accBufferMaxSize) {
        m_accBufferSize = idx = 0;
    } else {
        idx = m_accBufferSize;
    }
    
    for(Integer32 i = 0; i < size; ++i, ++idx)
    {
        Binary *bits = 0;
        integerToBits(container[i], &bits);
        m_accBuffer[idx] = tmp_abs(bits[0]);
        delete_arr_primitive_s(&bits);
        ++m_accBufferSize;
    }
    
    Integer32 pos;
    bool result = contains(m_mark, m_markSize, m_accBuffer, m_accBufferSize, pos);
    
    // index of the last integrated bit that was recovered from the original (not accumulative) container
    Integer32 last_idx = 0;
    if(result) {
        m_synchronized = true;
        last_idx = pos + m_markSize - idx;
    }
    
    return result;
}

/**
 *  Reset scanner
 */
void Sync::reset()
{
    memset(m_accBuffer, 0, m_accBufferMaxSize);
    m_pointer = m_mark;
    m_markSizeCurrent = m_markSize;
    m_synchronized = false;
}
    
bool Sync::isSynchronized() const
{
    return m_synchronized;
}
    
Integer32 Sync::markerSize() const
{
    return m_markSize;
}
    
}
