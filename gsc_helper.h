//
//  core.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__core__
#define __core__core__

#include "gsc_memory.h"
#include <math.h>
#include <cstdlib>
// keep assert enabled in release
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cstring>
#include <cwchar>

/**
 *  Template functions
 */
namespace gsc {
    
//-------------------------------------------------------------------------
// char array to integer array
//-------------------------------------------------------------------------
/**
 *  Converts char array into array of type IntegerType.
 *  The calculation of size of the destination array and memory allocation is performed inside.
 *  @param src  source string
 *  @param dest reference to the destination array
 *  @param size src length
 *
 *  requirenments:
 *      dest = nullptr
 *
 *  @return size of dest array
 */
template <typename IntegerType>
size_t strToIntegerArray(char *const src, size_t size, IntegerType **dest);
/**
 *  Converts wide char array into array of type IntegerType.
 *  The calculation of size of the destination array and memory allocation is performed inside.
 *  @param src  source string
 *  @param dest reference to the destination array
 *  @param size src length
 *
 *  requirenments:
 *      dest = nullptr
 *
 *  @return size of dest array
 */
template <typename IntegerType>
size_t strToIntegerArray(wchar_t *const src, size_t size, IntegerType **dest);
//-------------------------------------------------------------------------
// integer array to char array
//-------------------------------------------------------------------------
/**
 *  Converts IntegerType array into char array.
 *  The calculation of size of the destination array and memory allocation is performed inside.
 *
 *  @param src  source IntegerType array
 *  @param dest reference to the destination array
 *  @param size src array length
 *
 *  requirenments:
 *      dest = nullptr
 */
template <typename IntegerType>
void integerArrayToStr(IntegerType *src, size_t size, char **dest);
/**
 *  Converts wide char array into array of type IntegerType.
 *  The calculation of size of the destination array and memory allocation is performed inside.
 *  @param src  source string
 *  @param dest reference to the destination array
 *  @param size src length
 *
 *  requirenments:
 *      dest = nullptr
 *
 *  @return size of dest array
 */
template <typename IntegerType>
void integerArrayToStr(IntegerType *src, size_t size, wchar_t **dest);
//-------------------------------------------------------------------------
// integer to binary array
//-------------------------------------------------------------------------
/**
 *  Converts integer to binary array.
 *
 *  @param x      IntegerType value to convert
 *  @param binary out parameter - binary array
 *
 *  requirenments:
 *     binary = nullptr
 *
 *  @return size of binary array
 */
template <typename IntegerType, typename BinaryType>
size_t integerToBits(IntegerType x, BinaryType **binary);
//-------------------------------------------------------------------------
// binary array to integer
//-------------------------------------------------------------------------
/**
 *  Converts binary array to integer.
 *
 *  @param binary binary array of type IntegerType to be converted to IntegerType value
 *  @param x      out parameter - result
 */
template <typename BinaryType, typename IntegerType>
void bitsToInteger(BinaryType *binary, IntegerType &x);
//-------------------------------------------------------------------------
/**
 *  Check if big array contains small one
 *
 *  @param small      small array
 *  @param size_small size of a small array
 *  @param big        big array
 *  @param size_big   size of a big array
 *  @param out_pos    position of small array inside big one or SIZE_T_MAX
 *
 *  @return operation result -> bool
 */
template <typename IntegerType>
bool contains(const IntegerType* small, size_t size_small, IntegerType *big, size_t size_big, size_t &out_pos);
/**
 *  Calculates dynamic step from array of bits.
 *  res = k * sum(src)
 *
 *  @param src   array of bits
 *  @param size  size of array
 *  @param k     step modificator
 *
 *  @return step
 */
template <typename IntegerType>
inline size_t dynamicStep(IntegerType *src, size_t size, size_t k);
//------------------------------------------------------------------------------------
/**
 *  Calculate semi-period for discrete function using Alter-Johnson formula:
 *     a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
 *  n - total number of samples,
 *
 *  @param data values of discrete function
 *  @param n    number of samples
 *
 *  @return semi_period or SIZE_T_MAX
 *      semi_period = argmin(a(tau)),
 *      semi_period_min <= semi_period <= semi_period_max
 */
template<typename IntegerType>
size_t calculate_semi_period(const IntegerType* const data, size_t n);
/**
 *  Calculate semi-period for discrete function using Alter-Johnson formula:
 *     a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
 *  n - total number of samples,
 *
 *  @param data values of discrete function
 *  @param n    number of samples
 *  @param out_data processed values
 *
 *  @return semi_period or SIZE_T_MAX
 *      semi_period = argmin(a(tau)),
 *      semi_period_min <= semi_period <= semi_period_max
 */
template<typename IntegerType>
size_t calculate_semi_period(const IntegerType* const data, size_t n, float **out_data);
//------------------------------------------------------------------------------------
/**
 *  Integrate data from 'info' to 'container' of size 'size' begining with 'begin' with step 'step'.
 *
 *  @param container container to modify
 *  @param c_size    container size
 *  @param begin     integration start pos
 *  @param step      integration step
 *  @param info      message (encoded) to integrate
 *  @param i_size    message (encoded) size
 *  @return          the amount of data that was integrated
 */
template<typename IntegerType, typename BinaryType>
size_t integrate(IntegerType **container, size_t c_size, size_t begin, size_t step, const BinaryType * const info, size_t i_size);
/**
 *  Recover data to 'info' from 'container' of size 'size' begining with 'begin' with step 'step'.
 *
 *  @param container container to extract data from
 *  @param size      container size
 *  @param begin     deintegration start pos
 *  @param step      deintegration step
 *  @param info      message (encoded) to deintegrate to
 *  @return          message length
 */
template<typename IntegerType, typename BinaryType>
size_t deintegrate(const IntegerType * const container, size_t size, size_t begin, size_t step, BinaryType **info);
//------------------------------------------------------------------------------------
    
/* Include implementation for template functions */
#include "gsc_helper.tpp"    
}

#endif /* defined(__core__core__) */
