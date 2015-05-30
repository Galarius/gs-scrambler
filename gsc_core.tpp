//
//  gsc_core.tpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_helper.h"
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
Core<IntegerType, BinaryType>::Core(const BinaryType * const mark, size_t size, size_t frameSize, size_t scanbufferSizeMax) : skipSamples_(0), synchronizer_(nullptr) {
            
    assert(scanbufferSizeMax > frameSize && "ArgumentError");
            
    frames_.reserve(scanbufferSizeMax);
    seed_.reserve(scanbufferSizeMax);
            
    synchronizer_ = new Sync<IntegerType, BinaryType>(mark, size, scanbufferSizeMax);
}

template <typename IntegerType, typename BinaryType>
Core<IntegerType, BinaryType>::~Core() {
    
    delete synchronizer_;
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
    if (synchronizer_->isSynchronized()) {
        // synchronized, continue inserting info
        size_t semi_p = calculate_semi_period(seed, s_size);
        if(semi_p != SIZE_T_MAX) {
            integrated = integrate<IntegerType, BinaryType>(container, c_size, semi_p, 1, info, i_size);
        }
    } else {
        // insert sync mark
        synchronizer_->put(container, c_size);
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
    assert(s_size == c_size && "[gsc_core error]: s_size != c_size.");
    assert(frames_.capacity() % c_size == 0 && "ArgumentError");
    assert(seed_.capacity() % s_size == 0 && "ArgumentError");
    
    size_t recovered = 0;
    if (synchronizer_->isSynchronized()) {
        
        // synchronized
        
        if(frames_.size() == frames_.capacity()) {
            // delete first 'size' elements
            frames_.erase(frames_.begin(), frames_.begin() + c_size);
            seed_.erase(seed_.begin(), seed_.begin() + s_size);
        }
        
        size_t c_idx = 0;
        if(skipSamples_ > 0)
        {
            c_idx = skipSamples_;
            skipSamples_ = 0;
        }
        
        // add more datas
        for(size_t i = frames_.size();
            i < frames_.size() + c_size && c_idx < c_size;
            ++i, ++c_idx)
        {
            frames_.push_back(container[c_idx]);
            seed_.push_back(seed[c_idx]);
        }
        
        if(frames_.size() >= c_size)
        {
            assert(!seed_.empty() && "Buffer must not be empty.");
            assert(!frames_.empty() && "Buffer must not be empty.");

            IntegerType *s_ptr = &seed_.front();
            IntegerType *f_ptr = &frames_.front();
        
            size_t semi_p = calculate_semi_period(s_ptr, s_size);           // take first s_size elements
            if(semi_p != SIZE_T_MAX) {
                recovered = deintegrate(f_ptr, c_size, semi_p, 1, info);    // take first c_size elements
                
                // delete first 'size' elements
                frames_.erase(frames_.begin(), frames_.begin() + c_size);
                seed_.erase(seed_.begin(), seed_.begin() + s_size);
            }
        }
        
    } else {
        size_t end_idx;
        // try to find sync marker inside container
        if(synchronizer_->scan(container, c_size, end_idx)) {
            size_t marker_size = synchronizer_->markerSize();   // size of sync marker
            // expected free space after sync marker and before secret info
            size_t expect_size_left = c_size >= marker_size ? c_size - marker_size : c_size - marker_size % c_size;
            size_t real_size_left = c_size - end_idx;           // real free space after sync marker and before secret info
            if(real_size_left > expect_size_left)
            {
                size_t c_idx = end_idx + real_size_left;        // start index in container to start accumulation from
                // accumulate the part of frame buffer and seed  buffer
                for(size_t i = 0; c_idx < c_size; ++i, ++c_idx) {
                    frames_.push_back(container[c_idx]);
                    seed_.push_back(seed[c_idx]);
                }
            } else {
                skipSamples_ =  expect_size_left - real_size_left;
            }
        }
    }
    return recovered;
}
}
