//
//  gsc_core.cpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_core.h"
#include "gsc_helper.h"

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
 *  @param container container to integrate data to
 *  @param c_size    size of the container
 *  @param info      bits sequence (1,0,1,0,...) to hide
 *  @param i_size    size of the info
 */
void Core::hide(Integer16 **container, Integer32 c_size, const Binary * const info, Integer32 i_size)
{
    if (!m_synchronizer->isSynchronized()) {
        // insert sync mark firstly
        Integer16 *c_ptr;
        if(m_synchronizer->put(container, c_size, c_ptr) && c_ptr != *container + c_size)
        {
            // if marker was fully integrated and there are some space in the container, accumulate it
            std::copy(c_ptr, *container+c_size, m_frameBuffer);
            m_frameSize = (Integer32)(*container+c_size - c_ptr);
        }
    } else {
        // synchronized, continue inserting info
        
    }
}
/**
 *  Recover 'info' from the container
 *
 *  @param container container to recover 'info' from
 *  @param c_size    size of the container
 *  @param info      buffer to store info in
 */
void Core::recover(const Integer16 * const container, Integer32 c_size, Binary **info)
{
    
}
    
}
