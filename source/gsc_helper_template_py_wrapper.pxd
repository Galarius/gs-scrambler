cdef extern from "stdint.h" nogil:
    ctypedef   signed char  int8_t
    ctypedef   signed short int16_t

cdef extern from "gsc_helper.h":
    cdef size_t str_to_short_arr "gsc::strToIntegerArray<int16_t>"(char *src, size_t size, int16_t **dest) except+
    cdef void short_arr_to_str "gsc::integerArrayToStr<int16_t>"(int16_t *src, size_t size, char **dest) except+
    cdef size_t short_to_binary_arr "gsc::integerToBits<int16_t, int8_t>"(int16_t x, int8_t **binary) except+
    cdef void binary_arr_to_short "gsc::bitsToInteger<int8_t, int16_t>"(int8_t *binary, int16_t &x) except+
    cdef void delete_char_arr "gsc::delete_arr_primitive_s<char>"(char **arr) except+
    cdef size_t calculate_semi_period_short "gsc::calculate_semi_period<int16_t>" (const int16_t* data, size_t n, float **out_data) except+
