//
//  main.cpp
//  core
//
//  Created by Galarius on 06.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include <iostream>
#include "core.h"
#include "core_memory.h"

template <typename T>
void printArray(T *arr, int size)
{
    for(int i = 0; i < size; ++i)
        std::cout << arr[i] << " ";
    std::cout<<std::endl;
}

void tests()
{
    // test str_2_vec
    char src[] = "ABCDEFJHIGKLMNOPQRSTUVWXYZabsdefghijklmnopqrstuvwxyz";
    int size = (int)strlen(src);
    short int *dest = 0;
    str2vec_s(src, &dest, size);
    printArray(dest, size);
    // test vec_2_str
    char *src_back = 0;
    vec2str_s(dest, &src_back, size);
    printArray(src_back, size);
    delete_arr_primitive_s(dest);
    delete_arr_primitive_s(src_back);
    // test D2B
    int16_t i = 32700;
    int16_t *binary = 0;
    int16_t s = D2B(i, &binary);
    printArray(binary, s);
    // test B2D
    int16_t x;
    B2D(binary, x);
    printf("%i", x);
    std::cout<<std::endl;
    delete_arr_primitive_s(binary);
}

int main(int argc, const char * argv[]) {
    
    tests();
    
    return 0;
}
