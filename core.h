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
void str2vec_s(char *const src, IntegerType **dest, Integer32 size);
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
void vec2str_s(IntegerType *src, char **dest, Integer32 size);
/*
 *	Converts integer to binary array
 */
template <typename IntegerType>
Integer16 D2B(IntegerType x, IntegerType **binary);
/*
 *	Converts binary array to integer
 */
template <typename IntegerType>
void B2D(IntegerType *binary, IntegerType &x);

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

#endif /* defined(__core__core__) */
