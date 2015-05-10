//
//  core_memory.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__memory__
#define __core__memory__

#include "core_types.h"
#include <stdio.h>

/*
 *	Safe new[] for array of primitive elements
 */
template <typename Type>
inline void new_arr_primitive_s(Type **arrPtr, Integer64 size)
{
    if(!*arrPtr) {
        *arrPtr = new Type[size];
        if(!*arrPtr) { printf("allocating error: failed to allocate %lli bytes", size * sizeof(Type)); }
    } else { printf("allocating error: 'ptr' shoud be equals to 0 initially"); }
}
/*
 *	Safe delete[] for array of primitive elements
 */
template <typename ArrType>
inline void delete_arr_primitive_s(ArrType **arrPtr)
{
    if(*arrPtr) {
        delete[] *arrPtr;
        *arrPtr = 0;
    }
}
#endif