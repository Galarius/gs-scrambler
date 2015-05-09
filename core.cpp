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
Integer32 calculate_semi_period(Integer16* data, Integer32 n)
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
    delete_arr_primitive_s(data_real);
    delete_arr_primitive_s(data_processed);
    
    return semi_period;
}
