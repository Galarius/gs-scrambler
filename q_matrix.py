__author__ = 'galarius'

import math
from stego_helper import *

class QMatrix:
    def __init__(self, p, n):
        """
        Create matrix with size (p+1)*(p+1) of power n
        """
        self.p = p
        self.n = n
        self.Q = []
        for i in range(0, self.p+1):
            mm = []
            for j in range(0, self.p+1):
                if self.p == 1:
                    n = self.n - self.p + j if i != 0 and i != self.p else (self.n + 1 - j if i != self.p else self.n - j)
                else:
                    n = self.n - self.p + j if i != 0 and i != self.p else (self.n + 1 - j if i != self.p else self.n - j)
                mm.append(self.fib(n))
            self.Q.append(mm)

    def pretty_print(self):
        for i in range(0, self.p+1):
            print self.Q[i]
        print ''

    @staticmethod
    def pretty_print_matrix(M):
        for i in range(0, len(M)):
            print M[i]
        print ''

    def det(self):
        return (-1)**(self.p*self.slotActivationDatan)

    def fib(self, n):
        """
        Fibonacci extended to negative range.

        ToDo: optimize (memorization, direct calculation)
        """
        # return (-1)**(abs(n)+1)*self.fib(abs(n)) if n < -self.p else (0 if n < 0 else (self.fib(n-1) +
        #                          self.fib(n-self.p-1) if n > self.p+1 else (0 if n == 0 else 1)))
        if n == 0:
            return 0
        elif n > self.p+1:
            return self.fib(n-1) + self.fib(n-self.p-1)
        elif n > 0 or n == -self.p:
            return 1
        elif n > -self.p:
            return 0
        else:
            return (-1)**(abs(n)+1)*self.fib(abs(n))


    @staticmethod
    def mult(a,b):
        zip_b = zip(*b)
        return [[sum(ele_a*ele_b for ele_a, ele_b in zip(row_a, col_b))
               for col_b in zip_b] for row_a in a]

    @staticmethod
    def encode_matrix_message(M, n):
        p = int(math.sqrt(len(M)))+1
        Q = QMatrix(p, n)
        E = QMatrix.mult(M, Q.Q)
        return E

    @staticmethod
    def decode_matrix_message(E, n):
        p = int(math.sqrt(len(E)))+1
        Q = QMatrix(p, -n)
        M = QMatrix.mult(E, Q.Q)
        return M


q = QMatrix(2, 2)
# q.pretty_print()
#q = QMatrix(2, -1)
#q.pretty_print()
# M = message_to_matrix("kkjljkl")
# QMatrix.pretty_print_matrix(M)
# E = QMatrix.encode_matrix_message(M, 1)
# M = QMatrix.decode_matrix_message(E, 1)
# QMatrix.pretty_print_matrix(E)
# QMatrix.pretty_print_matrix(M)

# test

q = QMatrix(1, 0)
print [q.fib(x) for x in range(-8, 9)]
q = QMatrix(2, 0)
print [q.fib(x) for x in range(-8, 9)]
q = QMatrix(3, 0)
print [q.fib(x) for x in range(-8, 9)]
q = QMatrix(4, 0)
print [q.fib(x) for x in range(-8, 9)]
q = QMatrix(5, 0)
print [q.fib(x) for x in range(-8, 9)]

q = QMatrix(2, 2)
print q.fib(3), q.fib(2), q.fib(1)
print q.fib(1), q.fib(0), q.fib(-1)
print q.fib(2), q.fib(1), q.fib(0)
print ""
q = QMatrix(2, -1)
print q.fib(0), q.fib(-1), q.fib(-2)
print q.fib(-2), q.fib(-3), q.fib(-4)
print q.fib(-1), q.fib(-2), q.fib(-3)
print ""
q = QMatrix(3, 3)
print q.fib(4), q.fib(3), q.fib(2), q.fib(1)
print q.fib(1), q.fib(0), q.fib(-1), q.fib(-2)
print q.fib(2), q.fib(1), q.fib(0), q.fib(-1)
print q.fib(3), q.fib(2), q.fib(1), q.fib(0)
print ""
q = QMatrix(3, -1)
print q.fib(0), q.fib(-1), q.fib(-2), q.fib(-3)
print q.fib(-3), q.fib(-4), q.fib(-5), q.fib(-6)
print q.fib(-2), q.fib(-3), q.fib(-4), q.fib(-5)
print q.fib(-1), q.fib(-2), q.fib(-3), q.fib(-4)
