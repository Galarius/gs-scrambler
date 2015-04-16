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
        for i in range(0, p+1):
            mm = []
            for j in range(0, p+1):
                if p == 1:
                    k = n - p + j if i != 0 and i != p else (n + 1 - j if i != p else n - j)
                else:
                    k = n + 1 - j if i == 0 else n - (p-1) + (i-1) - j
                mm.append(self.fib(k))
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
        return (-1)**(self.p*self.n)

    def fib(self, n):
        """
        Fibonacci extended to negative range.
        """
        if n == 0:
            return 0
        elif n > self.p+1:
            return self.fib(n-1) + self.fib(n-self.p-1)
        elif n > 0 or n == -self.p:
            return 1
        elif n >= -self.p+1:
            return 0
        else:
            return self.fib(n+self.p+1) - self.fib(n+self.p)


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


q = QMatrix(2, 1)
q.pretty_print()
q = QMatrix(2, -1)
q.pretty_print()
M = message_to_matrix("kkjljkl")
QMatrix.pretty_print_matrix(M)
E = QMatrix.encode_matrix_message(M, 1)
M = QMatrix.decode_matrix_message(E, 1)
QMatrix.pretty_print_matrix(E)
QMatrix.pretty_print_matrix(M)

# test 1
# q = QMatrix(1, 0)
# print [q.fib(x) for x in range(-8, 9)][::-1]
# q = QMatrix(2, 0)
# print [q.fib(x) for x in range(-8, 9)][::-1]
# q = QMatrix(3, 0)
# print [q.fib(x) for x in range(-8, 9)][::-1]
# q = QMatrix(4, 0)
# print [q.fib(x) for x in range(-8, 9)][::-1]
# q = QMatrix(5, 0)
# print [q.fib(x) for x in range(-8, 9)][::-1]
# test 2
# q = QMatrix(2, 2)
# q.pretty_print()
# print q.fib(3), q.fib(2), q.fib(1)
# print q.fib(1), q.fib(0), q.fib(-1)
# print q.fib(2), q.fib(1), q.fib(0)
# print ""
# q = QMatrix(2, -1)
# q.pretty_print()
# print q.fib(0), q.fib(-1), q.fib(-2)
# print q.fib(-2), q.fib(-3), q.fib(-4)
# print q.fib(-1), q.fib(-2), q.fib(-3)
# print ""
# q = QMatrix(3, 3)
# q.pretty_print()
# print q.fib(4), q.fib(3), q.fib(2), q.fib(1)
# print q.fib(1), q.fib(0), q.fib(-1), q.fib(-2)
# print q.fib(2), q.fib(1), q.fib(0), q.fib(-1)
# print q.fib(3), q.fib(2), q.fib(1), q.fib(0)
# print ""
# q = QMatrix(3, -1)
# q.pretty_print()
# print q.fib(0), q.fib(-1), q.fib(-2), q.fib(-3)
# print q.fib(-3), q.fib(-4), q.fib(-5), q.fib(-6)
# print q.fib(-2), q.fib(-3), q.fib(-4), q.fib(-5)
# print q.fib(-1), q.fib(-2), q.fib(-3), q.fib(-4)
