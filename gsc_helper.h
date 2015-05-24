//
//  core.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__core__
#define __core__core__

#include "gsc_types.h"
#include "gsc_memory.h"
#include <math.h>

/**
 *  Template functions
 */
namespace gsc {
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
void strToIntegerArray(char *const src, IntegerType **dest, Integer32 size);
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
void integerArrayToStr(IntegerType *src, char **dest, Integer32 size);
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
Integer16 integerToBits(IntegerType x, Binary **binary);
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
void bitsToInteger(Binary *binary, IntegerType &x);

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
bool contains(const IntegerType* small, Integer32 size_small, IntegerType *big, Integer32 size_big, Integer32 &out_pos);
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
inline Integer32 dynamicStep(IntegerType *src, Integer32 size, Integer32 k);
    
/* Include implementation for template functions */
#include "gsc_helper.tpp"    
}

/**
 *  Non-template functions
 */
namespace gsc {
/**
 *  Calculate semi-period for discrete function using Alter-Johnson formula:
 *     a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
 *  n - total number of samples,
 *
 *  @param data values of discrete function
 *  @param n    number of samples
 *
 *  @return semi_period
 *      semi_period = argmin(a(tau)),
 *      semi_period_min <= semi_period <= semi_period_max
 */
Integer32 calculate_semi_period(const Integer16* const data, Integer32 n);

/**
 *  Calculate semi-period for discrete function using Alter-Johnson formula:
 *     a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
 *  n - total number of samples,
 *
 *  @param data values of discrete function
 *  @param n    number of samples
 *  @param out_data processed values
 *
 *  @return semi_period
 *      semi_period = argmin(a(tau)),
 *      semi_period_min <= semi_period <= semi_period_max
 */
Integer32 calculate_semi_period(const Integer16* const data, Integer32 n, float **out_data);

/**
 *  Integrate data from 'info' to 'container' of size 'size' begining with 'begin' with step 'step'.
 *
 *  @param container container to modify
 *  @param size      container size
 *  @param begin     integration start pos
 *  @param step      integration step
 *  @param info      message (encoded) to integrate
 *  @return          the amount of data that was integrated
 */
Integer32 integrate(Integer16 **container, Integer32 size, Integer32 begin, Integer32 step, Binary * const info);

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
Integer32 deintegrate(const Integer16 * const container, Integer32 size, Integer32 begin, Integer32 step, Binary **info);
}

#endif /* defined(__core__core__) */
