//
//  gsc_core.h
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__gsc_core__
#define __core__gsc_core__

#include "gsc_sync.h"
#include "gsc_acc_buffer.h"

namespace gsc {
    
/**
 *  Core class to perform hiding/recovering data in/from container.
 */
template <typename IntegerType, typename BinaryType>
class Core
{
public:
    /**
     *  @param mark array of bits
     *  @param size mark array size
     *  @param frameSize the size of frame buffer
     *  @param scanBufferMaxSize the size for acummulative buffer used for sync marker detection, normally 3 * frameSize.
     */
    Core(const BinaryType * const mark, size_t size, size_t frameSize, size_t scanBufferMaxSize);
    ~Core();
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
    size_t hide(const IntegerType * const seed, size_t s_size, IntegerType **container, size_t c_size, const BinaryType * const info, size_t i_size);
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
    size_t recover(const IntegerType * const seed, size_t s_size, const IntegerType * const container, size_t c_size, BinaryType **info);
    
private:
    Core()                       = delete;
    Core(Core const&)            = delete;
    void operator=(Core const&)  = delete;
    
private:
    AccBuffer<IntegerType> m_frame;    // container buffer
    AccBuffer<IntegerType> m_seed;     // seed container buffer (to calculate unique step)
    Sync<IntegerType, BinaryType> *m_synchronizer;
};
    
}

/* Include implementation for template class */
#include "gsc_core.tpp"

#endif /* defined(__core__gsc_core__) */
