//
//  gsc_buffer.h
//  core
//
//  Created by Galarius on 30.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __core__gsc_buffer__
#define __core__gsc_buffer__

#include <vector>

namespace gsc {

template <typename T>
class Buffer
{
public:
    Buffer(const T* const data, size_t size);
    Buffer(size_t size);
    Buffer(const Buffer &other);
    Buffer(Buffer &&other);
    Buffer();
    ~Buffer() throw();
    
    Buffer& operator=(const Buffer &other);
    Buffer& operator=(Buffer &&other);
public:
    T* get_ptr(size_t offset = 0) const;
    
public:
    std::vector<T> buf_;
};
    
}

/* Include implementation for template class */
#include "gsc_buffer.h"

#endif /* defined(__core__gsc_buffer__) */
