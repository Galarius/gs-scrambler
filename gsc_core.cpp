//
//  gsc_core.cpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_core.h"
#include "gsc_helper.h"
#include <stdio.h>

#include <algorithm>    // std::copy, memset

namespace gsc {
    
/**
 *  Creates the instance of core class.
 *
 *  @param mark array of bits
 *  @param size mark array size
 *  @param frameSize the size of frame buffer
 *  @param scanBufferMaxSize the size for acummulative buffer used for sync marker detection, normally 3 * frameSize.
 *
 *  @return ICore instance initialized with Core
 */
ICore *createCoreInstance(const Binary * const mark, Integer32 size, Integer32 frameSize, Integer32 scanBufferMaxSize)
{
    return new Core(mark, size, frameSize, scanBufferMaxSize);
}
/*----------------------------------------------------------------------------*/
/**
 *  @param mark array of bits
 *  @param size mark array size
 *  @param frameSize the size of frame buffer
 *  @param scanBufferMaxSize the size for acummulative buffer used for sync marker detection, normally 3 * frameSize.
 */
Core::Core(const Binary * const mark, Integer32 size, Integer32 frameSize, Integer32 scanBufferMaxSize) :
        ICore(), m_synchronizer(0) {
    m_frame.buffer = 0;
    m_frame.bufferMaxSize = frameSize;
    m_frame.bufferSize = 0;
    new_arr_primitive_s(&m_frame.buffer, m_frame.bufferMaxSize);
    memset(m_frame.buffer, 0, m_frame.bufferMaxSize);
    m_seed.buffer = 0;
    m_seed.bufferMaxSize = frameSize;
    m_seed.bufferSize = 0;
    new_arr_primitive_s(&m_seed.buffer, m_seed.bufferMaxSize);
    memset(m_seed.buffer, 0, m_seed.bufferMaxSize);
    m_synchronizer = new Sync(mark, size, scanBufferMaxSize);
}

Core::~Core() {
    delete_arr_primitive_s(&m_frame.buffer);
    delete m_synchronizer;
}
/*----------------------------------------------------------------------------*/
/**
 *  Hide 'info' inside 'container'.
 *
 *  @param seed      data source to calculate step for info integration
 *  @param s_size    size of the seed array
 *  @param container container to integrate data to
 *  @param c_size    size of the container
 *  @param info      bits sequence (1,0,1,0,...) to hide
 *  @param i_size    size of the info
 *  @return          the amount of data that was integrated
 */
Integer32 Core::hide(const Integer16 * const seed, Integer32 s_size, Integer16 **container, Integer32 c_size, const Binary * const info, Integer32 i_size)
{
    Integer32 integrated = 0;
    if (m_synchronizer->isSynchronized()) {
        // synchronized, continue inserting info
        Integer32 semi_p = calculate_semi_period(seed, s_size);
        if(semi_p != -1) {
            integrated = integrate(container, c_size, semi_p, 1, info, i_size);
        }
    } else {
        // insert sync mark
        m_synchronizer->put(container, c_size);
    }
    return integrated;
}
/**
 *  Recover 'info' from the container
 *
 *  @param seed      data source to calculate step for info recovering
 *  @param s_size    size of the seed array
 *  @param container container to recover 'info' from
 *  @param c_size    size of the container
 *  @param info      buffer to store info in
 *  @return          message length
 */
Integer32 Core::recover(const Integer16 * const seed, Integer32 s_size, const Integer16 * const container, Integer32 c_size, Binary **info)
{
    if(s_size != c_size)
    {
        printf("[gsc_core error]: s_size != c_size. Not supported yet.");
        return 0;
    }
    
    Integer32 recovered = 0;
    if (m_synchronizer->isSynchronized()) {
        // synchronized
        Integer32 c_idx = 0;                 // start index in container to start accumulation from
        if(m_frame.bufferSize < 0) {
            // skip m_frameSize samples
            c_idx = -m_frame.bufferSize;
            m_frame.bufferSize = 0;
            m_seed.bufferSize = 0;
        } else if (m_frame.bufferSize >= m_frame.bufferMaxSize) {
            m_frame.bufferSize = 0;
            m_seed.bufferSize = 0;
        }
        for(Integer32 i = m_frame.bufferSize; i < m_frame.bufferMaxSize && c_idx < c_size; ++i, ++c_idx) {
            m_frame.buffer[i] = container[c_idx];
            ++m_frame.bufferSize;
            m_seed.buffer[i]  = seed[c_idx];
            ++m_seed.bufferSize;
        }
        
        if(m_frame.bufferSize == m_frame.bufferMaxSize)
        {
            Integer32 semi_p = calculate_semi_period(m_seed.buffer, m_seed.bufferSize);
            if(semi_p != -1) {
                recovered = deintegrate(m_frame.buffer, m_frame.bufferSize, semi_p, 1, info);
            }
            m_frame.bufferSize = 0;
            m_seed.bufferSize = 0;
        }
    } else {
        Integer32 end_idx;
        if(m_synchronizer->scan(container, c_size, end_idx)) {
            Integer32 marker_size = m_synchronizer->markerSize();   // size of sync marker
            // expected free space after sync marker and before secret info
            Integer32 expect_size_left = c_size >= marker_size ? c_size - marker_size : c_size - marker_size % c_size;
            Integer32 real_size_left = c_size - end_idx;                // real free space after sync marker and before secret info
            Integer32 substruct_size = real_size_left - expect_size_left;
            if(substruct_size >= 0)
            {
                Integer32 c_idx = end_idx + real_size_left;                 // start index in container to start accumulation from
                // accumulate the part of frame buffer
                for(int i = 0; i < substruct_size && c_idx < c_size; ++i, ++c_idx)
                {
                    m_frame.buffer[i] = container[c_idx];
                    ++m_frame.bufferSize;
                    m_seed.buffer[i] = seed[c_idx];
                    ++m_seed.bufferSize;
                    --substruct_size;
                }
            } else {
                Integer32 c_idx = end_idx;
                for(int i = 0; i < substruct_size && c_idx < c_size; ++i, ++c_idx) {
                    --m_frame.bufferSize;
                    --m_seed.bufferSize;
                }
            }
        }
    }
    return recovered;
}
    
}
