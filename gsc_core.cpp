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
        ICore(), m_frameBuffer(0), m_frameSizeMax(frameSize), m_frameSize(0), m_synchronizer(0) {
    new_arr_primitive_s(&m_frameBuffer, m_frameSizeMax);
    memset(m_frameBuffer, 0, m_frameSizeMax);
    m_synchronizer = new Sync(mark, size, scanBufferMaxSize);
}

Core::~Core() {
    delete_arr_primitive_s(&m_frameBuffer);
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
    Integer32 recovered = 0;
    if (m_synchronizer->isSynchronized()) {
        // synchronized
        Integer32 c_idx = 0;                 // start index in container to start accumulation from
        if(m_frameSize < 0) {
            // skip m_frameSize samples
            c_idx = -m_frameSize;
            m_frameSize = 0;
        } else if (m_frameSize >= m_frameSizeMax) {
            m_frameSize = 0;
        }
        // printf("[recover] m_frameSize: %i; c_idx: %i;\n", m_frameSize, c_idx);
        for(Integer32 i = m_frameSize; i < m_frameSizeMax && c_idx < c_size; ++i, ++c_idx) {
            m_frameBuffer[i] = container[c_idx];
            ++m_frameSize;
        }
        
        if(m_frameSize == m_frameSizeMax)
        {
            Integer32 semi_p = calculate_semi_period(seed, s_size);
            if(semi_p != -1) {
                recovered = deintegrate(container, c_size, semi_p, 1, info);
            }
            m_frameSize = 0;
        }
    } else {
        printf("syncing...\n");
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
                    m_frameBuffer[i] = container[c_idx];
                    ++m_frameSize;
                    --substruct_size;
                }
            } else {
                Integer32 c_idx = end_idx;
                for(int i = 0; i < substruct_size && c_idx < c_size; ++i, ++c_idx) {
                    --m_frameSize;
                }
            }
        }
    }
    return recovered;
}
    
}
