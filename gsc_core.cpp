//
//  gsc_core.cpp
//  core
//
//  Created by Galarius on 24.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_core.h"

namespace gsc {

/**
 *  Creates the instance
 *  @return ICore instance
 */
extern ICore *createCoreInstance()
{
    return new Core();
}
/*----------------------------------------------------------------------------*/
Core::Core() : ICore() {
    
}

Core::~Core() {
    
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
