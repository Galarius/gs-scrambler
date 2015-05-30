//
//  core.h
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//
//  Implementation file for template functions.

#define SIGN(x) ((x) == (0) ? (0) : ((x) > (0) ? (1) : (-1)))

/**
 *  template abs
 */
template <typename IntegerType>
static inline IntegerType tmp_abs(IntegerType x)
{
    return (x < 0 ? -x : x);
}

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
size_t strToIntegerArray(char *const src, size_t size, IntegerType **dest)
{
    assert(src  && "ArgumentsError");
    assert(!(*dest) && "ArgumentsError");
    
    size_t d_size = strlen(src);
    new_arr_primitive_s(dest, d_size);
    for(size_t  i = 0; i < d_size; ++i) {
        (*dest)[i] =  static_cast<IntegerType>(static_cast<unsigned char>(src[i]));
    }
    return d_size;
}
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
size_t strToIntegerArray(wchar_t *const src, size_t size, IntegerType **dest)
{
    assert(src  && "ArgumentsError");
    assert(!(*dest) && "ArgumentsError");
    
    size_t d_size = wcslen(src);
    new_arr_primitive_s(dest, d_size);
    for(size_t  i = 0; i < d_size; ++i) {
        (*dest)[i] =  static_cast<IntegerType>(static_cast<unsigned short>(src[i]));
    }
    return d_size;
}
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
void integerArrayToStr(IntegerType *src, size_t size, char **dest)
{
    assert(src  && "ArgumentsError");
    assert(!(*dest) && "ArgumentsError");
    
    new_arr_primitive_s(dest, size + 1);
    for(size_t  i = 0; i < size; ++i) {
        (*dest)[i] = static_cast<char>(src[i]);
    }
    (*dest)[size] = '\0';
}
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
void integerArrayToStr(IntegerType *src, size_t size, wchar_t **dest)
{
    assert(src  && "ArgumentsError");
    assert(!(*dest) && "ArgumentsError");
    
    new_arr_primitive_s(dest, size + 1);
    for(size_t  i = 0; i < size; ++i) {
        (*dest)[i] = static_cast<wchar_t>(src[i]);
    }
    (*dest)[size] = '\0';
}
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
size_t integerToBits(IntegerType x, BinaryType **binary)
{
    assert(!(*binary) && "ArgumentsError");
    
    size_t sizeInBits = sizeof(IntegerType) * 8;
    new_arr_primitive_s(binary, sizeInBits);
    IntegerType sign = SIGN(x);
    for(size_t i = 0; i < sizeInBits; ++i) {
        (*binary)[i] = tmp_abs(x) % 2;
        x = floor(tmp_abs(x) / 2);
    }
    for(size_t i = 0; i < sizeInBits; ++i) {
        (*binary)[i] *= sign;
    }
    return sizeInBits;
}
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
void bitsToInteger(BinaryType *binary, IntegerType &x)
{
    assert(binary && "ArgumentsError");
    
    x = 0;
    size_t sizeInBits = sizeof(IntegerType) * 8;
    for(size_t i = 0; i < sizeInBits; ++i) {
        x += binary[i] * pow(2, i);
    }
}

//------------------------------------------------------------------------------------
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
bool contains(const IntegerType* small, size_t size_small, IntegerType *big, size_t size_big, size_t &out_pos)
{
    assert(size_small && "Zero size array.");
    assert(size_small <= size_big && "ArgumentsError");
    
    bool matches = false;
    out_pos = SIZE_T_MAX;
    for (size_t i = 0; i < size_big-size_small+1; ++i) {
        matches = true;
        for (size_t j = 0; j < size_small; ++j) {
            if (big[i+j] != small[j]) {
                matches = false;
                break;
            }
        }
        if(matches) {
            out_pos = i;
            break;
        }
    }
    return matches;
}
//------------------------------------------------------------------------------------
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
size_t dynamicStep(IntegerType *src, size_t size, size_t k)
{
    size_t res = 0;
    for(size_t i = 0; i < size; ++i)
        res += src[i];
    return k * res;
}
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
template<typename IntegerType>
size_t calculate_semi_period(const IntegerType* const data, size_t n)
{
    float *data_processed = 0;
    size_t semi_period = calculate_semi_period(data, n, &data_processed);
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
template<typename IntegerType>
size_t calculate_semi_period(const IntegerType* const data, size_t n, float **out_data)
{
//    for(size_t i = 0; i < n; ++i)
//        printf("%i ", data[i]);
//    printf("\n");
    
    new_arr_primitive_s<float>(out_data, n);
    float *data_processed = *out_data;
    
    size_t sum = 0;
    size_t k = 0;
    for(size_t tau = 0; tau < n; ++tau) {
        sum = 0;
        for(size_t t = 0; t < n - tau; ++t) {
            sum += (size_t)fabs(data[t + tau] - data[t]);
        }
        if((n - tau)) {
            data_processed[k++] = (1.0f / (n - tau) * sum);
        }
    }
    
    // prepare to calculate semi-period from calculated data
    size_t l = static_cast<size_t>(floorf(0.1f * n));        // 10 % from len
    size_t semi_period = l;
    
    // find min without first and last l elements
    for(size_t p = l+1; p < n - l; ++p) {
        if(data_processed[p] < data_processed[semi_period] && data_processed[p]) {
            semi_period = p;
        }
    }
    
    printf("semi_period: %zu\n", semi_period);
    
    return data_processed[semi_period] != 0 ? semi_period : -1;
}
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
size_t integrate(IntegerType **container, size_t c_size, size_t begin, size_t step, const BinaryType * const info, size_t i_size)
{
    assert(step > 0 && "ArgumentError");

    size_t counter = 0;
    for(size_t i = begin, k = 0; i < c_size && k < i_size; i += step, ++k) {
        BinaryType *bits = 0;
        integerToBits((*container)[i], &bits);
        IntegerType sign = container[i] >= 0 ? 1 : -1;
        bits[0] = sign * info[k];
        ++counter;
        bitsToInteger(bits, (*container)[i]);
        delete_arr_primitive_s(&bits);
    }
    return counter;
}
//------------------------------------------------------------------------------------
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
size_t deintegrate(const IntegerType * const container, size_t size, size_t begin, size_t step, BinaryType **info)
{
    assert(step > 0 && "ArgumentError");
    
    new_arr_primitive_s(info, (int)(ceil((size-begin) / step)));
    size_t l = 0;
    BinaryType *ptr = *info;
    for(size_t i = begin; i < size; i += step, ++l) {
        BinaryType *bits = 0;
        integerToBits(container[i], &bits);
        *ptr = tmp_abs(bits[0]);
        ++ptr;
        delete_arr_primitive_s(&bits);
    }
    return l;
}
//------------------------------------------------------------------------------------
