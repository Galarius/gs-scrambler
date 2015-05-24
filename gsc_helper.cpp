//
//  core.cpp
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "gsc_helper.h"

#include <cstdlib>
#include <thread>
#include <mutex>

namespace gsc {

/**
 *  Protect resource from multithreaded access
 */
static std::mutex locker;

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
    float_t *data_processed = 0;
    Integer32 semi_period = calculate_semi_period(data, n, &data_processed);
    delete_arr_primitive_s(&data_processed);
    return semi_period;
}

/**
 *  Thread jonson core function to process data
 *
 *  @param start          start idx
 *  @param end            end idx
 *  @param data           original data
 *  @param n              data size
 *  @param data_processed processed data
 */
static void thread_jonson_core(Integer32 start, Integer32 end, const Integer16* const data, Integer32 n, float *data_processed)
{
    std::lock_guard<std::mutex> lg(locker);
    // compute sum
    // loops inverted for efficiency
    for(Integer32 t = 0; t < n; ++t) {
        for(Integer32 tau = start; tau < end; ++tau) {
            if(t >= n - tau)
                break;
            data_processed[tau] += abs(data[t + tau] - data[t]);
        }
    }
    // normalize
    for(Integer32 tau = start; tau < end; ++tau)
        data_processed[tau] /= (n - tau);
};

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
Integer32 calculate_semi_period(const Integer16* const data, Integer32 n, float **out_data)
{
    // get number of cores
    Integer32 cores = std::thread::hardware_concurrency();
    std::thread *threads = new std::thread[cores];
    float *data_processed = *out_data;
    new_arr_primitive_s<float>(&data_processed, n);
    
    // process per core
    Integer32 w = n / cores;
    Integer32 extra = n % cores;
    Integer32 beg = 0;
    Integer32 end = 0;
    for (Integer32 i = 0; i < cores; ++i) {
        beg = i * w;
        end = beg + w;
        if(i == cores-1)
            end += extra;
        threads[i] = std::thread(thread_jonson_core, beg, end, data, n, data_processed);
    }
    
    // prepare to calculate semi-period from calculated data
    Integer32 l = static_cast<Integer32>(floorf(0.1f * n));        // 10 % from len
    Integer32 semi_period = l;
    
    // join threads
    for (Integer32 i = 0; i < cores; ++i)
        threads[i].join();
    
    delete []threads;
    
    // find min without first and last l elements
    for(Integer32 p = l+1; p < n - l; ++p) {
        if(data_processed[p] < data_processed[semi_period] && data_processed[p])
            semi_period = p;
    }
    return data_processed[semi_period] != 0 ? semi_period : -1;
}

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
Integer32 integrate(Integer16 **container, Integer32 size, Integer32 begin, Integer32 step, Binary * const info)
{
    if(!step) {
        printf("ArgumentError");
        return 0;
    }
    Binary *info_ptr = info;
    Integer32 counter = 0;
    for(Integer32 i = begin; i < size; i += step) {
        if(!info)
            break;
        Binary *bits = 0;
        integerToBits((*container)[i], &bits);
        Integer16 sign = container[i] >= 0 ? 1 : -1;
        bits[0] = sign * (*info_ptr);
        ++info_ptr;
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