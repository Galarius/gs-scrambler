__author__ = 'galarius'

import math


class QMatrix:
    def __init__(self, p, n):
        """
        Create matrix with size (p+1)*(p+1) of power n
        """
        self.p = p
        self.n = n
        Q = [[1, 0],
             [1, 0]]

    def det(self):
        return (-1)**(self.p*self.n)

    def F(self, n):
        return (-1)**(abs(n)+1)*self.F(abs(n)) if n < 0 else (self.F(n-1)+self.F(n-1-self.p) if n > self.p+1 else (0 if n == 0 else 1))
        # if n >= 0:
        #     return self.F(n - 1) + self.F(n - self.p - 1) if 1 < n < self.p + 1 else (1 if n == 0 or n == 1 or n == self.p + 1 else (1 if n == -self.p-1 else 0))
        # else:
        #     return self.F(-n) if abs(n) % 2 == 0 else -self.F(-n)
        # Binne
        # return ((1.0+math.sqrt(5.0)**n) - (1.0-math.sqrt(5.0)**n))/math.sqrt(5.0)

q = QMatrix(1, 1)
m = []
for i in range(0, q.p+1):
    mm = []
    for j in range(0, q.p+1):
        n = q.n - q.p + j if i != 0 and i != q.p else ( q.n + 1 - j if i != q.p else q.n - j )
        mm.append(q.F(n))
    m.append(mm)

for i in range(0, q.p+1):
    print m[i]
print ''

q = QMatrix(2, 1)
print q.F(q.n+1), q.F(q.n), q.F(q.n-1)
print q.F(q.n-1), q.F(q.n-2), q.F(q.n-3)
print q.F(q.n), q.F(q.n-1), q.F(q.n-2)
