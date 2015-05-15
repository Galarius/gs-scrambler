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
    new_arr_primitive_s(&data_processed, n);
    for(Integer32 tau = 0; tau < n; ++tau) {
        sum = 0;
        for(Integer32 t = 0; t < n - tau; ++t) {
            sum += (Integer32)fabs(data[t + tau] - data[t]);
        }
        if((n - tau)) {
            data_processed[k++] = (1.0f / (n - tau) * sum);
        }
    }
    // calculate semi-period from calculated data
    Integer32 l = (int)(floorf(0.1f * k));        // 10 % from len
    Integer32 dr_n = k - 2 * l;
    float_t *data_real = 0;
    new_arr_primitive_s(&data_real, dr_n);
    // remove first and last l elements
    for (Integer32 i = l; i < k - l; ++i) {
        data_real[i-l] = data_processed[i];
    }
    Integer32 semi_period = 0;
    for(Integer32 p = 1; p < dr_n; ++p) {
        if(data_real[p] < data_real[semi_period])
            semi_period = p;
    }
    // clean up
    delete_arr_primitive_s(&data_real);
    delete_arr_primitive_s(&data_processed);
    
    return semi_period;
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
Integer32 integrate(Integer16 **container, Integer32 size, Integer32 begin, Integer32 step, Integer16 * const info)
{
    if(!step) {
        printf("ArgumentError");
        return 0;
    }
    Integer16 *info_ptr = info;
    Integer32 counter = 0;
    for(Integer32 i = begin; i < size; i += step) {
        if(!info)
            break;
        Integer16 *bits = 0;
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
Integer32 deintegrate(const Integer16 * const container, Integer32 size, Integer32 begin, Integer32 step, Integer16 **info)
{
    if(!step) {
        printf("ArgumentError");
        return 0;
    }
    new_arr_primitive_s(info, (int)(ceil((size-begin) / step)));
    Integer32 l = 0;
    Integer16 *ptr = *info;
    for(Integer32 i = begin; i < size; i += step, ++l) {
        Integer16 *bits = 0;
        integerToBits(container[i], &bits);
        *ptr = tmp_abs(bits[0]);
        ++ptr;
        delete_arr_primitive_s(&bits);
    }
    return l;
}

//----------------------------------------------------------------
// Detector
//----------------------------------------------------------------

/**
 *  @param mark int16 array of bits
 *  @param size array size
 *  @param buffer_size size of accumulation container
 */
Detector::Detector(const Integer16 * const mark, Integer32 size, Integer32 bufferSize) : m_mark(0), m_accumulation_container(0), m_mark_size(size), m_accumulation_container_size(bufferSize), m_accumulation_container_fill_size(0)
{
    if(!mark || !size) {
        printf("ArgumentError");
        return;
    }
    
    new_arr_primitive_s(&m_mark, size);
    for (int i = 0; i < m_mark_size; ++i) {
        m_mark[i] = mark[i];
        printf("%i", mark[i]);
    }
    printf("\n");
    
    new_arr_primitive_s(&m_accumulation_container, bufferSize);
}

Detector::~Detector()
{
    delete_arr_primitive_s(&m_mark);
    delete_arr_primitive_s(&m_accumulation_container);
}

/**
 *  detector method
 *
 *  @param container container to scan
 *  @param size      size of the container
 *
 *  @return scan result
 */
bool Detector::detect(const Integer16 *const container, Integer32 size)
{
    Integer32 idx;
    if(m_accumulation_container_fill_size >= m_accumulation_container_size) {
        m_accumulation_container_fill_size = idx = 0;
    } else {
        idx = m_accumulation_container_fill_size;
    }
    
    for(Integer32 i = 0; i < size; ++i, ++idx)
    {
        Integer16 *bits = 0;
        integerToBits(container[i], &bits);
        m_accumulation_container[idx] = tmp_abs(bits[0]);
        delete_arr_primitive_s(&bits);
        ++m_accumulation_container_fill_size;
    }
    
    Integer32 pos;
    bool result = contains(m_mark, m_mark_size, m_accumulation_container, m_accumulation_container_fill_size, pos);
    return result;
}