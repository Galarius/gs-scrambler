//
//  core_types.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//
//  Typedefs for some types.
//
//  It's not enough to use int when working with huge stego containers, so the main integer type should be long long or long.
//  Types redefined for comfort and better control.

#ifndef __core__types__
#define __core__types__

#include <cstdint>

namespace gsc {

typedef int64_t Integer64;
typedef int32_t Integer32;
typedef int16_t Integer16;
typedef bool    Binary;
    
typedef struct BinaryBufferAcc {
    Binary *buffer;                 // accumulative buffer
    Integer32 bufferMaxSize;        // accumulative buffer maximum size
    Integer32 bufferSize;           // accumulative buffer current size
} BinaryBufferAcc;
    
}

#endif