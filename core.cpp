//
//  core.cpp
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "core.h"
#include "core_memory.h"
#include "core_types.h"

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
Integer32 calculate_semi_period(const Integer16* const data, Integer32 n)
{
    // process
    float_t *data_processed = 0;
    Integer32 sum = 0;
    Integer32 k = 0;
    new_arr_primitive_s<float_t>(&data_processed, n);
    for(Integer32 tau = 0; tau < n; ++tau) {
        sum = 0;
        for(Integer32 t = 0; t < n - tau; ++t) {
            sum += (Integer32)fabs((float_t)(data[t + tau] - data[t]));
        }
        if((n - tau)) {
            data_processed[k++] = (1.0f / (n - tau) * sum);
        }
    }
    // calculate semi-period from calculated data
    Integer32 l = (int)(floorf(0.1f * k));        // 10 % from len
    Integer32 dr_n = k - 2 * l;
    float_t *data_real = 0;
    new_arr_primitive_s<float_t>(&data_real, dr_n);
    // remove first and last l elements
    for (Integer32 i = l; i < k - l; ++i) {
        data_real[i-l] = data_processed[i];
    }
    Integer32 semi_period = 0;
    for(Integer32 p = 1; p < dr_n; ++p)
    {
        if(data_real[p] < data_real[semi_period])
            semi_period = p;
    }
    // clean up
    delete_arr_primitive_s(&data_real);
    delete_arr_primitive_s(&data_processed);
    
    return semi_period;
}

/**
 *  Integrate data from 'stream' to 'container' of size 'size' begining with 'begin' with step 'step'.
 *
 *  @param container container to modify
 *  @param size      container size
 *  @param begin     integration start pos
 *  @param step      integration step
 *  @param stream    message (encoded) to integrate
 */
void integrate(Integer16 **container, Integer32 size, Integer32 begin, Integer32 step, Integer16 *stream)
{
    if(!step) {
        printf("ArgumentError");
        return;
    }
    for(Integer32 i = begin; i < size; i += step) {
        if(!stream)
            break;
        Integer16 *bits = 0;
        D2B((*container)[i], &bits);
        Integer16 sign = container[i] >= 0 ? 1 : -1;
        bits[0] = sign * (*stream);
        ++stream;
        B2D(bits, (*container)[i]);
        delete_arr_primitive_s(&bits);
    }
}

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
Integer32 deintegrate(const Integer16 * const container, Integer32 size, Integer32 begin, Integer32 step, Integer16 **stream)
{
    if(!step) {
        printf("ArgumentError");
        return 0;
    }
    Integer32 l = (int)(ceil((size-begin) / step));
    new_arr_primitive_s(stream, l);
    Integer16 *ptr = *stream;
    for(Integer32 i = begin; i < size; i += step) {
        Integer16 *bits = 0;
        D2B(container[i], &bits);
        *ptr = gabs(bits[0]);
        ++ptr;
    }
    return l;
}