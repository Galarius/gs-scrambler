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

/**
 *  template abs
 */
template <typename IntegerType>
static inline IntegerType tmp_abs(IntegerType x)
{
    return (x < 0 ? -x : x);
}

/**
 *  Converts char array into array of type IntegerType.
 *
 *  @param src  source string
 *  @param dest destination array
 *  @param size src length
 *
 *  requirenments:
 *      size > 0
 *      src != null
 *      dest = null
 */
template <typename IntegerType>
void strToIntegerArray(char *const src, IntegerType **dest, Integer32 size)
{
    if(size <= 0 || !src || (*dest)) {
        printf("ArgumentsError");
        return;
    }
    
    new_arr_primitive_s(dest, size);
    for(Integer32  i = 0; i < size; ++i)
        (*dest)[i] = static_cast<IntegerType>(src[i]);
}
//------------------------------------------------------------------------------------
/**
 *  Converts IntegerType array into char array.
 *
 *  @param src  source IntegerType array
 *  @param dest destination char array
 *  @param size src array length
 *
 *  requirenments:
 *      size > 0
 *      src != null
 *      dest = null
 */
template <typename IntegerType>
void integerArrayToStr(IntegerType *src, char **dest, Integer32 size)
{
    if(size <= 0 || !src || (*dest)) {
        printf("ArgumentsError");
        return;
    }
    
    new_arr_primitive_s(dest, size + 1);
    for(Integer32  i = 0; i < size; ++i)
        (*dest)[i] = static_cast<char>(src[i]);
    (*dest)[size] = '\0';
}
//------------------------------------------------------------------------------------
/**
 *  Converts integer to binary array.
 *
 *  @param x      IntegerType value to convert
 *  @param binary out parameter - binary array
 * 
 *  requirenments:
 *     binary == 0
 *
 *  @return size of binary array
 */
template <typename IntegerType>
Integer16 integerToBits(IntegerType x, IntegerType **binary)
{
    if((*binary))
    {
        printf("ArgumentsError");
        return 0;
    }
    
    Integer16 sizeInBits = sizeof(IntegerType) * 8;
    new_arr_primitive_s<IntegerType>(binary, sizeInBits);
    Integer16 sign = SIGN(x);
    for(Integer16 i = 0; i < sizeInBits; ++i)
    {
        (*binary)[i] = tmp_abs(x) % 2;
        x = floor(tmp_abs(x) / 2);
    }
    for(Integer16 i = 0; i < sizeInBits; ++i)
        (*binary)[i] *= sign;
    
    return sizeInBits;
}
//------------------------------------------------------------------------------------
/**
 *  Converts binary array to integer.
 *
 *  requirenments:
 *     binary != 0
 *
 *  @param binary binary array of type IntegerType to be converted to IntegerType value
 *  @param x      out parameter - result
 */
template <typename IntegerType>
void bitsToInteger(IntegerType *binary, IntegerType &x)
{
    if(!binary)
    {
        printf("ArgumentsError");
        return;
    }
    
    x = 0;
    IntegerType sizeInBits = sizeof(IntegerType) * 8;
    for(int i = 0; i < sizeInBits; ++i)
        x += binary[i] * pow(2, i);
}
//------------------------------------------------------------------------------------
/**
 *  Check if big array contains small one
 *
 *  @param small      small array
 *  @param size_small size of a small array
 *  @param big        big array
 *  @param size_big   size of a big array
 *  @param out_pos    position of small array inside big one or -1
 *
 *  @return operation result -> bool
 */
template <typename IntegerType>
bool contains(const IntegerType* small, Integer32 size_small, IntegerType *big, Integer32 size_big, Integer32 &out_pos)
{
    if (size_small == 0) {
        printf("Zero size array.\n");
        return true;
    }

    if (size_small > size_big) {
        printf("Wrong arguments.\n");
        printf("%i\n", size_small);
        printf("%i\n", size_big);
        return false;
    }
    
    bool matches = false;
    out_pos = -1;
    for (Integer32 i = 0; i < size_big-size_small+1; ++i) {
        matches = true;
        for (Integer32 j = 0; j < size_small; ++j) {
            if (big[i+j] != small[j]) {
                matches = false;
                break;
            }
        }
        if(matches) {
            out_pos = i;
            break;
        }
    }
    return matches;
}
//------------------------------------------------------------------------------------
