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
Integer32 calculate_semi_period(Integer16* data, Integer32 n);

#endif /* defined(__core__core__) */
