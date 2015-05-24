//
//  gsc_sync.h
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__gsc_sync__
#define __core__gsc_sync__

#include "gsc_types.h"

namespace gsc {
    
/**
 *  Core class to perform hiding/recovering data in/from container.
 */
class Sync
{
public:
    /**
     *  @param mark array of bits
     *  @param size array size
     *  @param bufferMaxSize size of accumulative buffer
     */
    Sync(const Binary * const mark, Integer32 size, Integer32 bufferMaxSize);
    ~Sync();

public:
    /**
     *  Insert marker inside the container.
     *
     *  @param container container to hide marker in
     *  @param size      size of the contaner
     *
     *  @return true if marker fully integrated inside the container and 
     *          false if more data required to insert what was left from marker
     */
    bool put(Integer16 **container, Integer32 size);
    /**
     *  Try to detect marker inside container.
     *
     *  @param container container to detect marker in
     *  @param size      size of the container
     *
     *  @return operation result
     */
    bool scan(const Integer16 *const container, Integer32 size);
    /**
     *  Reset scanner
     */
    void reset();
    
private:
    Sync()                       = delete;
    Sync(Sync const&)            = delete;
    void operator=(Sync const&)  = delete;

    
private:
    Binary *m_mark;           // synchronization marker
    Integer32 m_markSize;   // synchronization marker size
    Binary *m_pointer;        // current position in marker
    
    Binary *m_accBuffer;              // synchronization accumulative buffer
    Integer32 m_accBufferMaxSize;   // synchronization accumulative buffer maximum size
    Integer32 m_accBufferSize;      // synchronization accumulative buffer current size
    
    bool m_synchronized;
};
    
}

#endif /* defined(__core__gsc_sync__) */
