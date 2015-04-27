//
//  Jonson.cpp
//  Jonson
//
//  Created by Galarius on 27.04.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#include "jonson_c.h"
#include <math.h>

using namespace jonson;

Jonson::Jonson(int* data, int n) :
    m_data(data), m_n(n), m_semi_period(0)
{ }

Jonson::~Jonson()
{ }

void Jonson::calculate()
{
    // Process
    float *data_processed = new float[m_n];
    int sum = 0;
    int k = 0;
    for(int tau = 0; tau < m_n; ++tau)
    {
        sum = 0;
        for(int t = 0; t < m_n - tau; ++t)
        {
            sum += (int)fabs((float)(m_data[t + tau] - m_data[t]));
        }
        
        if((m_n - tau)) {
            data_processed[k++] = (1.0f / (m_n - tau) * sum);
        }
    }
    // calculate semi-period from calculated data
    int l = (int)(floorf(0.1f * k));        // 10 % from len
    int dr_n = k - 2 * l;
    float *data_real = new float[dr_n];
    // remove first and last l elements
    for (int i = l; i < k - l; ++i) {
        data_real[i-l] = data_processed[i];
    }
    m_semi_period = 0;
    for(int i = 1; i < dr_n; ++i)
    {
        if(data_real[i] < data_real[m_semi_period])
            m_semi_period = i;
    }
    // clean up
    delete [] data_real;
    delete [] data_processed;
}

const int Jonson::getSemiPeriod() const
{
    return m_semi_period;
}
