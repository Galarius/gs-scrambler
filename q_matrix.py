__author__ = 'galarius'

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
                k = n-p+i-j if i != 0 else n+1-j
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
        p = len(M)-1
        Q = QMatrix(p, n)
        E = QMatrix.mult(M, Q.Q)
        return E

    @staticmethod
    def decode_matrix_message(E, n):
        p = len(E)-1
        Q = QMatrix(p, -n)
        M = QMatrix.mult(E, Q.Q)
        return M
