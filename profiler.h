//
//  profiler.h
//  project_euler
//
//  Created by Galarius on 19.05.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef project_euler_profiler_h
#define project_euler_profiler_h

#include <ctime>

static clock_t start;

#define CLOCK_START start = clock();
#define ELAPSED_TIME (std::clock() - start) / (double)CLOCKS_PER_SEC

#endif
