from libcpp cimport bool
cdef extern from "gsc_helper.h":
    cdef void str_to_short_arr "gsc::strToIntegerArray<short int>"(char *src, short int **dest, int size);
    cdef void short_arr_to_str "gsc::integerArrayToStr<short int>"(short int *src, char **dest, int size);
    cdef short int short_to_binary_arr "gsc::integerToBits<short int>"(short int x, bool **binary)
    cdef void binary_arr_to_short "gsc::bitsToInteger<short int>"(bool *binary, short int &x)
    cdef void delete_char_arr "gsc::delete_arr_primitive_s<char>"(char **arr)
