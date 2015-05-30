//
//  gsc_core.tpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_helper.h"
#include "gsc_acc_buffer.h"
#include <stdio.h>
#include <algorithm>    // std::copy, memset

namespace gsc {
/*----------------------------------------------------------------------------*/
/**
 *  @param mark array of bits
 *  @param size mark array size
 *  @param frameSize the size of frame buffer
 *  @param scanbufferSizeMax the size for acummulative buffer used for sync marker detection, normally 3 * frameSize.
 */
template <typename IntegerType, typename BinaryType>
Core<IntegerType, BinaryType>::Core(const BinaryType * const mark, size_t size, size_t frameSize, size_t scanbufferSizeMax) :
        m_synchronizer(nullptr) {
            
    allocate_buffer(m_frame, frameSize);
    allocate_buffer(m_seed, frameSize);
            
    m_synchronizer = new Sync<IntegerType, BinaryType>(mark, size, scanbufferSizeMax);
}

template <typename IntegerType, typename BinaryType>
Core<IntegerType, BinaryType>::~Core() {
    
    deallocate_buffer(m_frame);
    deallocate_buffer(m_seed);
    
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

template <typename IntegerType, typename BinaryType>
size_t Core<IntegerType, BinaryType>::hide(const IntegerType * const seed, size_t s_size, IntegerType **container, size_t c_size, const BinaryType * const info, size_t i_size)
{
    size_t integrated = 0;
    if (m_synchronizer->isSynchronized()) {
        // synchronized, continue inserting info
        size_t semi_p = calculate_semi_period(seed, s_size);
        if(semi_p != -1) {
            integrated = integrate<IntegerType, BinaryType>(container, c_size, semi_p, 1, info, i_size);
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
template <typename IntegerType, typename BinaryType>
    size_t Core<IntegerType, BinaryType>::recover(const IntegerType * const seed, size_t s_size, const IntegerType * const container, size_t c_size, BinaryType **info)
{
    assert(s_size == c_size && "[gsc_core error]: s_size != c_size. Not supported yet.");
    
    size_t recovered = 0;
    if (m_synchronizer->isSynchronized()) {
        // synchronized
        size_t c_idx = 0;                 // start index in container to start accumulation from
        if(m_frame.bufferSizeCurrent < 0) {
            // skip m_frameSize samples
            c_idx = -m_frame.bufferSizeCurrent;
            m_frame.bufferSizeCurrent = 0;
            m_seed.bufferSizeCurrent = 0;
        } else if (m_frame.bufferSizeCurrent >= m_frame.bufferSizeMax) {
            m_frame.bufferSizeCurrent = 0;
            m_seed.bufferSizeCurrent = 0;
        }
        for(size_t i = m_frame.bufferSizeCurrent; i < m_frame.bufferSizeMax && c_idx < c_size; ++i, ++c_idx) {
            m_frame.buffer[i] = container[c_idx];
            ++m_frame.bufferSizeCurrent;
            m_seed.buffer[i]  = seed[c_idx];
            ++m_seed.bufferSizeCurrent;
        }
        
        if(m_frame.bufferSizeCurrent == m_frame.bufferSizeMax)
        {
            size_t semi_p = calculate_semi_period(m_seed.buffer, m_seed.bufferSizeCurrent);
            if(semi_p != -1) {
                recovered = deintegrate(m_frame.buffer, m_frame.bufferSizeCurrent, semi_p, 1, info);
            }
            m_frame.bufferSizeCurrent = 0;
            m_seed.bufferSizeCurrent = 0;
        }
    } else {
        size_t end_idx;
        if(m_synchronizer->scan(container, c_size, end_idx)) {
            size_t marker_size = m_synchronizer->markerSize();   // size of sync marker
            // expected free space after sync marker and before secret info
            size_t expect_size_left = c_size >= marker_size ? c_size - marker_size : c_size - marker_size % c_size;
            size_t real_size_left = c_size - end_idx;                // real free space after sync marker and before secret info
            int64_t substruct_size = real_size_left - expect_size_left;
            if(substruct_size >= 0)
            {
                size_t c_idx = end_idx + real_size_left;                 // start index in container to start accumulation from
                // accumulate the part of frame buffer
                for(int i = 0; i < substruct_size && c_idx < c_size; ++i, ++c_idx)
                {
                    m_frame.buffer[i] = container[c_idx];
                    ++m_frame.bufferSizeCurrent;
                    m_seed.buffer[i] = seed[c_idx];
                    ++m_seed.bufferSizeCurrent;
                    --substruct_size;
                }
            } else {
                size_t c_idx = end_idx;
                for(int i = 0; i < substruct_size && c_idx < c_size; ++i, ++c_idx) {
                    --m_frame.bufferSizeCurrent;
                    --m_seed.bufferSizeCurrent;
                }
            }
        }
    }
    return recovered;
}
}
