//
//  core_types.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__types__
#define __core__types__

#include <cstdint>
#include <cstddef>

namespace gsc {

template <typename T>
struct BufferAcc {
    T *buffer;                   // accumulative buffer
    size_t bufferMaxSize;        // accumulative buffer maximum size
    size_t bufferSize;           // accumulative buffer current size
};
    
}

#endif