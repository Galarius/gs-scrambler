//
//  core.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//
//  Implementation file for template functions.

#include "core_memory.h"
#include <math.h>

#define SIGN(x) ((x) == (0) ? (0) : ((x) > (0) ? (1) : (-1)))

template <typename IntegerType>
static inline IntegerType gabs(IntegerType j)
{
    return (j < 0 ? -j : j);
}

/*
 *	Converts char array into vector of type IntegerType.
 *  <src> - source string
 *  <dest> - destination vector
 *  <size> - src length
 *
 *  requirenments:
 *  size > 0
 *  src != null
 *  dest = null
 */
template <typename IntegerType>
void str2vec_s(char *const src, IntegerType **dest, Integer32 size)
{
    if(size > 0 && src && !(*dest))
    {
        new_arr_primitive_s<IntegerType>(dest, size);
        for(Integer32  i = 0; i < size; ++i)
            (*dest)[i] = static_cast<IntegerType>(src[i]);
    } else {
        printf("ArgumentsError");
    }
}
//------------------------------------------------------------------------------------
/*
 *	Converts IntegerType array into char array.
 *  <src> - source IntegerType vector
 *  <dest> - destination char array
 *  <size> - vector length
 *
 *  requirenments:
 *  size > 0
 *  src != null
 *  dest = null
 */
template <typename IntegerType>
void vec2str_s(IntegerType *src, char **dest, Integer32 size)
{
    if(size > 0 && src && !(*dest))
    {
        new_arr_primitive_s<char>(dest, size + 1);
        for(Integer32  i = 0; i < size; ++i)
            (*dest)[i] = static_cast<char>(src[i]);
        (*dest)[size] = '\0';
    } else {
        printf("ArgumentsError");
    }
}
//------------------------------------------------------------------------------------
/*
 *	Converts decimal to binary array
 */
template <typename IntegerType>
Integer16 D2B(IntegerType x, IntegerType **binary)
{
    Integer16 sizeInBits = 0;
    // Use little endian order (lsb is the first bit from left)
    if(!(*binary))
    {
        sizeInBits = sizeof(IntegerType) * 8;
        new_arr_primitive_s<IntegerType>(binary, sizeInBits);
        Integer16 sign = SIGN(x);
        for(Integer16 i = 0; i < sizeInBits; ++i)
        {
            (*binary)[i] = gabs(x) % 2;
            x = floor(gabs(x) / 2);
        }
        for(Integer16 i = 0; i < sizeInBits; ++i)
            (*binary)[i] *= sign;
    }
    return sizeInBits;
}
//------------------------------------------------------------------------------------
/*
 *	Converts binary array to decimal
 */
template <typename IntegerType>
void B2D(IntegerType *binary, IntegerType &x)
{
    if(binary)
    {
        IntegerType sizeInBits = sizeof(IntegerType) * 8;
        x = 0;
        for(int i = 0; i < sizeInBits; ++i)
            x += binary[i] * pow(2, i);
    }
}
//------------------------------------------------------------------------------------

