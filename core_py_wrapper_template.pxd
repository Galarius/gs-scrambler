cdef extern from "core.h":
    cdef void str2vec_short "strToIntegerArray<short int>"(char *src, short int **dest, int size);
    cdef void vec2str_short "integerArrayToStr<short int>"(short int *src, char **dest, int size);
    cdef short int d2b_short "integerToBits<short int>"(short int x, short int **binary)
    cdef void b2d_short "bitsToInteger<short int>"(short int *binary, short int &x)
    cdef void delete_char_arr "delete_arr_primitive_s<char>"(char **arr)
