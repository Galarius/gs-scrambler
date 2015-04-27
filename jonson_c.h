//
//  Jonson.h
//  Jonson
//
//  Created by Galarius on 27.04.15.
//  Copyright (c) 2015 galarius. All rights reserved.
//

#ifndef __Jonson__Jonson__
#define __Jonson__Jonson__

namespace jonson {
    
    class Jonson {
    public:
        explicit Jonson(int *data, int n);
        ~Jonson();
    public:
        void calculate();
        const int getSemiPeriod() const;
    private:
        int *m_data;
        int m_n;
        int m_semi_period;
    };
}

#endif /* defined(__Jonson__Jonson__) */
