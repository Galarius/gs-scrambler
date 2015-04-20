__author__ = 'galarius'

from q_matrix import *

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

# test 3
msg_o = "In the field of audio steganography, fundamental spread spectrum (SS) techniques attempts to distribute secret data throughout the frequency spectrum of the audio signal to the maximum possible level."
M = message_to_matrix(msg_o)
E = QMatrix.encode_matrix_message(M, 1)
_M = QMatrix.decode_matrix_message(E, 1)
msg = matrix_to_message(_M)
#print msg
M_bits = int_matrix_to_bits_matrix(E)
QMatrix.pretty_print_matrix(M_bits)

# test 4
# q = QMatrix(10, 1)
# q.pretty_print()