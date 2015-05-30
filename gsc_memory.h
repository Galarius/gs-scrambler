//
//  core_memory.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__memory__
#define __core__memory__

#include <cassert>
#include <cstddef>

namespace gsc {

/*
 *	Safe new[] for array of primitive elements
 */
template <typename Type>
inline void new_arr_primitive_s(Type **arrPtr, size_t size)
{
    assert(!*arrPtr && "Allocating error: 'ptr' shoud be equals to 0 initially.");
    *arrPtr = new Type[size];
    assert(*arrPtr && "Allocating error: failed to allocate.");
}
/*
 *	Safe delete[] for array of primitive elements
 */
template <typename ArrType>
inline void delete_arr_primitive_s(ArrType **arrPtr)
{
    assert(*arrPtr != 0 && "Allocating error: 'ptr' shoudn't be equals to 0.");
    delete[] *arrPtr;
    *arrPtr = nullptr;
}
    
}

#endif