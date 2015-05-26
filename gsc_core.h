//
//  gsc_core.h
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__gsc_core__
#define __core__gsc_core__

#include "gsc_icore.h"
#include "gsc_sync.h"

namespace gsc {
    
/**
 *  Core class to perform hiding/recovering data in/from container.
 */
class Core : public ICore
{
public:
    /**
     *  @param mark array of bits
     *  @param size mark array size
     *  @param frameSize the size of frame buffer
     *  @param scanBufferMaxSize the size for acummulative buffer used for sync marker detection, normally 3 * frameSize.
     */
    Core(const Binary * const mark, Integer32 size, Integer32 frameSize, Integer32 scanBufferMaxSize);
    virtual ~Core();
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
    virtual Integer32 hide(const Integer16 * const seed, Integer32 s_size, Integer16 **container, Integer32 c_size, const Binary * const info, Integer32 i_size);
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
    virtual Integer32 recover(const Integer16 * const seed, Integer32 s_size, const Integer16 * const container, Integer32 c_size, Binary **info);
    
private:
    Core()                       = delete;
    Core(Core const&)            = delete;
    void operator=(Core const&)  = delete;
    
protected:
    Binary *m_frameBuffer;            // accumulative buffer
    Integer32 m_frameSizeMax;         // max size of frame buffer
    Integer32 m_frameSize;            // current size of frame buffer
    Sync *m_synchronizer;
};
    
}

#endif /* defined(__core__gsc_core__) */
