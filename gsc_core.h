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

namespace gsc {
    
/**
 *  Core class to perform hiding/recovering data in/from container.
 */
class Core : public ICore
{
public:
    explicit Core();
    virtual ~Core();
    /**
     *  Hide 'info' inside 'container'.
     *
     *  @param container container to integrate data to
     *  @param c_size    size of the container
     *  @param info      bits sequence (1,0,1,0,...) to hide
     *  @param i_size    size of the info
     */
    virtual void hide(Integer16 **container, Integer32 c_size, const Binary * const info, Integer32 i_size);
    /**
     *  Recover 'info' from the container
     *
     *  @param container container to recover 'info' from
     *  @param c_size    size of the container
     *  @param info      buffer to store info in
     */
    virtual void recover(const Integer16 * const container, Integer32 c_size, Binary **info);
};
    
}

#endif /* defined(__core__gsc_core__) */
