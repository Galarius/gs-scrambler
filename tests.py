__author__ = 'galarius'

from q_matrix import *

M = message_to_matrix("n,dsnsndf,nsjdnfjsdfsdfsdf")
QMatrix.pretty_print_matrix(M)
E = QMatrix.encode_matrix_message(M, 1)
M = QMatrix.decode_matrix_message(E, 1)
QMatrix.pretty_print_matrix(E)
QMatrix.pretty_print_matrix(M)

# test 1
#q = QMatrix(1, 0)
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
