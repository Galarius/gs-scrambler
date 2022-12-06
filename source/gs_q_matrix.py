# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

import numpy as np

class QMatrix:
    """
    Generalized Q-matrix of fibonacci numbers
    """
    def __init__(self, p, n):
        """
        Init q-matrix with size (p+1)*(p+1) of power n
        """
        self.p = p
        self.n = n
        self.Q = np.empty((self.p+1, self.p+1), dtype=np.int16)
        # create matrix
        self.create()

    def create(self):
        """
        Q-matrix creation method
        """
        for i in range(0, self.p+1):
            for j in range(0, self.p+1):
                k = self.n-self.p+i-j if i != 0 else self.n+1-j
                self.Q[i][j] = self.fib(k)

    def det(self):
        """
        Determinant of the matrix
        """
        return (-1)**(self.p*self.n)

    def fib(self, n):
        """
        Fibonacci numbers extended to negative range.
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
    def mult(a, b):
        """
        Multiply a matrix with matrix b.
        Assuming sizes are correct
        :param a: left matrix
        :param b: other (right) matrix
        :return:  multiplication
        """
        zip_b = zip(*b)
        return [[sum(ele_a*ele_b for ele_a, ele_b in zip(row_a, col_b))
               for col_b in zip_b] for row_a in a]

    @staticmethod
    def encode_matrix_message(M, n):
        """
        Encode matrix M with Q-matrix of power n
        :param M: matrix to encode
        :param n: power of Q-Matrix
        :return:  encoded matrix M
        """
        p = len(M)-1
        Q = QMatrix(p, n)
        E = M.dot(Q.Q)
        return E

    @staticmethod
    def decode_matrix_message(E, n):
        """
        Decode matrix E with Q-matrix of power n
        :param E: matrix to decode
        :param n: power of Q-Matrix
        :return:  decoded matrix E
        """
        p = len(E)-1
        Q = QMatrix(p, -n)
        M = E.dot(Q.Q)
        return M

    def pretty_print(self):
        """
        Print current Q-matrix
        """
        for i in range(0, self.p+1):
            print(self.Q[i])
        print('')

    @staticmethod
    def pretty_print_matrix(M):
        """
        Print 'M'
        """
        for i in range(0, len(M)):
            print(M[i])
        print('')
