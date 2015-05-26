//
//  core.cpp
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_helper.h"

#include <cstdlib>

namespace gsc {

/**
 *  Calculate semi-period for discrete function using Alter-Johnson formula:
 *     a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
 *  n - total number of samples,
 *
 *  @param data values of discrete function
 *  @param n    number of samples
 *
 *  @return semi_period or -1
 *      semi_period = argmin(a(tau)),
 *      semi_period_min <= semi_period <= semi_period_max
 */
Integer32 calculate_semi_period(const Integer16* const data, Integer32 n)
{
    float_t *data_processed = 0;
    Integer32 semi_period = calculate_semi_period(data, n, &data_processed);
    delete_arr_primitive_s(&data_processed);
    return semi_period;
}

/**
 *  Calculate semi-period for discrete function using Alter-Johnson formula:
 *     a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
 *  n - total number of samples,
 *
 *  @param data values of discrete function
 *  @param n    number of samples
 *  @param out_data processed values
 *
 *  @return semi_period or -1
 *      semi_period = argmin(a(tau)),
 *      semi_period_min <= semi_period <= semi_period_max
 */
Integer32 calculate_semi_period(const Integer16* const data, Integer32 n, float **out_data)
{
    float *data_processed = *out_data;
    new_arr_primitive_s<float>(&data_processed, n);
    
    Integer32 sum = 0;
    Integer32 k = 0;
    for(Integer32 tau = 0; tau < n; ++tau) {
        sum = 0;
        for(Integer32 t = 0; t < n - tau; ++t) {
            sum += (Integer32)fabs(data[t + tau] - data[t]);
        }
        if((n - tau)) {
            data_processed[k++] = (1.0f / (n - tau) * sum);
        }
    }
    
    // prepare to calculate semi-period from calculated data
    Integer32 l = static_cast<Integer32>(floorf(0.1f * n));        // 10 % from len
    Integer32 semi_period = l;
    
    // find min without first and last l elements
    for(Integer32 p = l+1; p < n - l; ++p) {
        if(data_processed[p] < data_processed[semi_period] && data_processed[p])
            semi_period = p;
    }

    printf("semi_period: %i\n", semi_period);

    return data_processed[semi_period] != 0 ? semi_period : -1;
}

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
Integer32 integrate(Integer16 **container, Integer32 c_size, Integer32 begin, Integer32 step, const Binary * const info, Integer32 i_size)
{
    if(!step) {
        printf("ArgumentError");
        return 0;
    }
    Integer32 counter = 0;
    for(Integer32 i = begin, k = 0; i < c_size && k < i_size; i += step, ++k) {
        Binary *bits = 0;
        integerToBits((*container)[i], &bits);
        Integer16 sign = container[i] >= 0 ? 1 : -1;
        bits[0] = sign * info[k];
        ++counter;
        bitsToInteger(bits, (*container)[i]);
        delete_arr_primitive_s(&bits);
    }
    return counter;
}

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
Integer32 deintegrate(const Integer16 * const container, Integer32 size, Integer32 begin, Integer32 step, Binary **info)
{
    if(!step) {
        printf("ArgumentError");
        return 0;
    }
    new_arr_primitive_s(info, (int)(ceil((size-begin) / step)));
    Integer32 l = 0;
    Binary *ptr = *info;
    for(Integer32 i = begin; i < size; i += step, ++l) {
        Binary *bits = 0;
        integerToBits(container[i], &bits);
        *ptr = tmp_abs(bits[0]);
        ++ptr;
        delete_arr_primitive_s(&bits);
    }
    return l;
}
    
}