//
//  Jonson.h
//  Jonson
//
//  Created by Galarius on 27.04.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//
/*
    Alter-Jonson formula:
        a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
    n - total number of samples,

        is used to determine semi-periods for discrete functions.
            semi_period = argmin(a(tau)),
        semi_period_min <= semi_period <= semi_period_max
*/

#ifndef __Jonson__H__
#define __Jonson__H__

namespace jonson {
    
    class Jonson {
    public:
        explicit Jonson(short int *data, int n);  // init with data pointer and data size
        ~Jonson();
    public:
        void calculate();                   // calculate semi-period
        const int getSemiPeriod() const;    // get processing result
    private:
        short int *m_data;        // pointer to data which will be processed
        int m_n;            // data size
        int m_semi_period;  // processing result
    };
}

#endif /* defined(__Jonson__H__) */
