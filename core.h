//
//  core.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__core__
#define __core__core__

#include "core_types.h"

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
Integer16 integerToBits(IntegerType x, IntegerType **binary);
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
void bitsToInteger(IntegerType *binary, IntegerType &x);

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
extern bool contains(const IntegerType* small, Integer32 size_small, IntegerType *big, Integer32 size_big, Integer32 &out_pos);

/* Include implementation for template functions */
#include "core.tpp"

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
 *  Integrate data from 'stream' to 'container' of size 'size' begining with 'begin' with step 'step'.
 *
 *  @param container container to modify
 *  @param size      container size
 *  @param begin     integration start pos
 *  @param step      integration step
 *  @param stream    message (encoded) to integrate
 *  @return          the amount of data that was integrated
 */
Integer32 integrate(Integer16 **container, Integer32 size, Integer32 begin, Integer32 step, Integer16 *stream);

/**
 *  Recover data to 'stream' from 'container' of size 'size' begining with 'begin' with step 'step'.
 *
 *  @param container container to extract data from
 *  @param size      container size
 *  @param begin     deintegration start pos
 *  @param step      deintegration step
 *  @param stream    message (encoded) to deintegrate to
 *  @return          message length
 */
Integer32 deintegrate(const Integer16 * const container, Integer32 size, Integer32 begin, Integer32 step, Integer16 **stream);

//----------------------------------------------------------------
// Detector
//----------------------------------------------------------------

/**
 *  Detect synchronization marker in container of bits
 */
class Detector
{
public:
    /**
     *  @param mark int16 array of bits
     *  @param size array size
     *  @param buffer_size size of accumulation container
     */
    Detector(const Integer16 * const mark, Integer32 size, Integer32 bufferSize);
    /**
     *  detector method
     *
     *  @param container container to scan
     *  @param size      size of the container
     *
     *  @return scan result
     */
    bool detect(const Integer16 * const container, Integer32 size);
    ~Detector();
private:
    Integer16 *m_mark;
    Integer32 m_mark_size;
    Integer16 *m_accumulation_container;
    Integer32 m_accumulation_container_size;
    Integer32 m_accumulation_container_fill_size;
};

#endif /* defined(__core__core__) */
