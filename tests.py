# -*- coding: utf-8 -*-
#!/usr/bin/env python

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from q_matrix import *
from stego_helper import *
from stego_core import *

import matplotlib.pyplot as plt

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
# M = message_to_matrix(msg_o)
# E = QMatrix.encode_matrix_message(M, 1)
# _M = QMatrix.decode_matrix_message(E, 1)
# msg = matrix_to_message(_M)
# #print msg
# M_bits = int_matrix_to_bits_matrix(E)
# QMatrix.pretty_print_matrix(M_bits)
# msg_matr_encoded_array = np.array(M_bits)
# message_to_proc_part = msg_matr_encoded_array.ravel()
# print len(message_to_proc_part)

# test 4
# q = QMatrix(10, 1)
# q.pretty_print()

#test 5.0

# msg_matr = message_to_matrix(msg_o)
# msg_matr_encoded = QMatrix.encode_matrix_message(msg_matr, 7)
#
# msg_matrix = QMatrix.decode_matrix_message(msg_matr_encoded, 7)
# msg = sh.matrix_to_message(msg_matrix)
# print msg_o == msg

# msg_matr = message_to_matrix(msg_o)
# msg_matr_encoded = QMatrix.encode_matrix_message(msg_matr, 7)
# print QMatrix.pretty_print_matrix(msg_matr_encoded)
# msg_matr_encoded_bits = int_matrix_to_bits_matrix(msg_matr_encoded)
# msg_matrix_encoded = bits_matrix_to_int_matrix(msg_matr_encoded_bits)
# print QMatrix.pretty_print_matrix(msg_matrix_encoded)

#test 5
# msg_matr = message_to_matrix(msg_o)
# msg_matr_encoded = QMatrix.encode_matrix_message(msg_matr, 7)
# print QMatrix.pretty_print_matrix(msg_matr_encoded)
# msg_matr_encoded_bits = int_matrix_to_bits_matrix(msg_matr_encoded)
# #print QMatrix.pretty_print_matrix(msg_matr_encoded_bits)
# msg_matr_encoded_array = np.array(msg_matr_encoded_bits)
# #print QMatrix.pretty_print_matrix(msg_matr_encoded_array.tolist())
# message_to_proc_part = msg_matr_encoded_array.ravel()
# #print message_to_proc_part
# mediate_length = len(message_to_proc_part)
#
# # message_to_proc_part = np.append(message_to_proc_part, [x for x in range(0, 100)])
# # bits = 8
# # if not mediate_length:
# #     mediate_length = mediate_length
# #     if mediate_length <= 0:
# #         raise AttributeError('Necessary argument not specified!')
# #
# # if len(message_to_proc_part) > mediate_length:
# #     message_to_proc_part = message_to_proc_part[:mediate_length]
# # elif message_to_proc_part < mediate_length:
# #     raise RuntimeError("Couldn't extract message with provided argument.")
#
# bits = 8
# s = int(math.sqrt(len(message_to_proc_part) / bits))
# msg_matrix_encoded_array = np.reshape(message_to_proc_part, (s, s, bits))
# #print QMatrix.pretty_print_matrix(msg_matrix_encoded_array.tolist())
# msg_matrix_encoded_bits = msg_matrix_encoded_array.tolist()
# #print QMatrix.pretty_print_matrix(msg_matr_encoded_bits)
# msg_matrix_encoded = sh.bits_matrix_to_int_matrix(msg_matrix_encoded_bits)
# print QMatrix.pretty_print_matrix(msg_matrix_encoded)
# msg_matrix = QMatrix.decode_matrix_message(msg_matrix_encoded, 7)
# msg = sh.matrix_to_message(msg_matrix)
# print msg_o == msg
# print msg

#test 6

# chunk = [-2, -2, -2, -2, -2, -2, -2, -2, -2, -558, -1148, -1447, -2301, -2773, -4330, -4967, -5268, -6075, -5877, -6765, -7304, -8164, -10718, -11392, -13023, -13420, -12585, -12678, -10764, -10531, -9724, -9130, -10022, -9046, -11343, -10007, -10278, -8687, -5354, -3708, 2668, 4172, 8559, 9821, 8361, 9422, 4638, 5498, 4456, 4777, 4924, 4887, 6269, 6040, 3552, 2574, 1627, 519, -862, -2298, -3872, -5539, -6783, -8176, -7649, -9060, -11107, -12526, -14230, -15551, -15068, -15656, -18226, -19141, -21816, -21989, -21993, -21887, -21849, -21820, -21707, -21432, -20088, -18553, -18385, -17458, -16071, -15104, -10122, -9128, -6223, -4688, 920, 851, 100, 1320, 554, 1288, 2177, 2231, 5611, 6639, 7468, 7658, 8470, 8011, 8606, 8812, 6505, 6388, 6752, 6869, 9637, 9766, 5554, 5975, 5710, 5320, 9399, 8741, 1186, 3330, 2802, 549, 4329, 5754, 2090, 4053, -301, -2866, 9358, 9618, 11688, 13039, -683, -689, 173, -103, 1065, 272, 7405, 9356, 1314, -572, -347, -2159, -467, 2664, 10262, 8335, -3359, -4481, -4330, -2668, -5792, -9003, 5817, 5832, 4069, 4249, -3218, -3740, -8254, -10047, -6599, -5940, -97, -184, -11575, -13917, -14993, -13693, -14200, -12315, -1071, -6022, -3389, -38, -3251, -4253, -2127, -1936, -2225, -1756, -4118, -3730, 683, -800, -3004, -1210, -1366, -706, 625, 779, 1683, -549, -359, 4510, 3577, 756, 2727, 2845, 3372, 5002, 4248, 4504, 5972, 6514, 7204, 7640, 5300, 6636, 9105, 8830, 10737, 9287, -4265, 24, 13022, 8711, 9450, 14301, 13635, 14097, 14507, 12344, 10347, 11681, 2340, 1586, 3930, 4145, 7557, 8106, 1770, 4630, 8701, 6064, 6091, 6425, 3825, 8525, 10816, 5218, 6094, 8509, 7102, 6160, 17932, 16065, 12920, 15090, 6501, 8055, 12871, 11722, 10454, 9120, 7971, 8960, 8188, 7695, 13387, 9642, 5172, 7218, 179, 1424, 1462, -3715, 8717, 10673, 9429, 11618, -7, -3818, -838, 810, 5744, 6164, 3930, 2949, -3792, -6125, -13523, -9350, 5358, 2297, 3153, -1078, -14135, -5342, -8203, -17296, -11699, -7355, -4702, -2110, -13617, -20138, -13650, -12657, -14070, -12139, -16897, -16577, -11922, -13807, -15099, -15485, -19739, -14488, -10006, -15333, -14603, -12887, -13511, -12888, -12860, -14311, -13148, -11282, -11896, -11514, -11237, -11382, -7695, -6156, -11292, -10206, -7631, -7882, -3930, -6184, -10618, -4077, -98, -4922, -4279, -3366, -4095, -781, -732, -1282, 186, 1017, 1216, 1574, 1777, 1585, 3216, 3976, 6189, 6803, 2148, 4906, 10668, 7510, 5478, 8465, 8638, 10676, 12833, 5726, 5620, 16581, 14241, 9108, 12024, 10041, 12026, 15762, 11684, 10595, 12598, 11836, 15298, 15786, 12336, 12940, 9605, 12269, 18267, 11731, 8418, 14232, 14245, 10140, 10255, 9275, 9358, 16829, 12526, 4451, 9967, 11098, 8600, 9597, 7314, 6141, 8432, 7506, 5845, 5742, 3969, 4718, 6952, 4453, 3666, 2208, -8, 3936, 3496, -1533, -674, 771, -1318, -2775, -2001, -1393, -2676, -2688, -5238, -6700, -3385, -4646, -9003, -8337, -8428, -9281, -8194, -8932, -8525, -11124, -12793, -8636, -5706, -9984, -7027, -9947, -8829, -3174, -7468, -9385, -9025, -11735, -10089, -9436, -15059, -14309, -10159, -10382, -7536, -9981, -14210, -9783, -7468, -10891, -11096, -11312, -12557, -7756, -7135, -12147, -10011, -8472, -10996, -11671, -9464, -10949, -8045, -6435, -12413, -10761, -8240, -8351, -7266, -7901, -12006, -7766, -4744, -8462, -2908, -1291, -3566, -4165, -1198, 127, 441, 1215, 1947, 2770, 4121, 5675, 1029, 383, 494, 43, -5269, -1735, 8218, 5672, 8567, 9258, 7448, 12080, 14533, 10444, 11335, 11594, 11773, 17255, 15518, 12279, 14900, 14472, 16588, 19358, 14335, 13975, 22908, 22286, 13629, 16855, 18737, 17247, 18938, 18087, 17509, 21147, 23444, 19672, 20213, 18741, 17867, 18880, 19235, 24151, 18868, 12176, 20287, 22695, 15978, 16271, 18138, 16876, 17904, 17024, 13348, 15527, 18735, 13988, 10954, 14672, 14343, 10855, 10887, 14281, 12737, 7888, 9971, 12889, 8783, 5510, 7203, 5810, 6632, 7104, 2917, 3142, 8135, 5492, -276, 2082, 2295, 729, 1132, -1341, -3107, -587, -1139, -2936, -2851, -4341, -4506, -3851, -5335, -6441, -6201, -6671, -7450, -7980, -10195, -10892, -8667, -9039, -5987, -9458, -17502, -10896, -7989, -12941, -12350, -12936, -13924, -10310, -11809, -19555, -16243, -11019, -14268, -12919, -14393, -16608, -13502, -13556, -16293, -16152, -16442, -16360, -12735, -14485, -16223, -14360, -14189, -13726, -12813, -17341, -18348, -12926, -13603, -10648, -11472, -17456, -12711, -9829, -13632, -14251, -12479, -12899, -9070, -7923, -10096, -8991, -11713, -9562, -5705, -8976, -10532, -7325, -6957, -3946, -1948, -7811, -5962, -1640, -3980, -6611, -2605, 1432, -2001, -2140, 473, 364, -1101, -448, 3209, 2661, 1274, 2878, 3269, 4061, 5647, 3883, 3366, 8286, 8747, 373, 2527, 12056, 8216, 5523, 8608, 8413, 8965, 10207, 8123, 7574, 10188, 10098, 11330, 11672, 7976, 8734, 9203, 10640, 13382, 10643, 8169, 13471, 11182, 4987, 7448, 10195, 10909, 12628, 9860, 9735, 9806, 9487, 8509, 7996, 11151, 10970, 6428, 8497, 13367, 10784, 10649, 11491, 9722, 11198, 12381, 10280, 10008, 13298, 11860, 8977, 10635, 11797, 11692, 9403, 9343, 8921, 8547, 9892, 6195, 6545, 10567, 9585, 7570, 6624, 2680, 5067, 6673, 6116, 8003, 2510, 1288, 6590, 4518, 915, 1574, -389, 1259, 5632, 340, -1742, 2257, 1866, -1553, -1461, -1115, -1853, -424, -2294, -5867, -3774, -3261, -4701, -4232, -4190, -4392, -7434, -7567, -5462, -7495, -10010, -8688, -7874, -8706, -7824, -10303, -11425, -9499, -10356, -11933, -12023, -13247, -13620, -13859, -13522, -12145, -14637, -15538, -14006, -14745, -17928, -17589, -13422, -15444, -17530, -16880, -18699, -18025, -17819, -19759, -20243, -16549, -16397, -19270, -18665, -21333, -20512, -18313, -20331, -20714, -21251, -22582, -19230, -18530, -20802, -19904, -20647, -20822, -21076, -21077, -20985, -20925, -20640, -19610, -18676, -19557, -19180, -20402, -20440, -20279, -19388, -15758, -17924, -18900, -18771, -20579, -16011, -14996, -17900, -16723, -17991, -17642, -15796, -15223, -11999, -14679, -16371, -14693, -15814, -12916, -12374, -14233, -12623, -10046, -11066, -12472, -9891, -8128, -10958, -12076, -9787, -10168, -7056, -6488, -9201, -6994, -4473, -5601, -5925, -5234, -5916, -3923, -2965, -3213, -2405, -3476, -2548, 520, -308, -918, -19, -703, 1254, 2620, 1862, 2587, 1402, 1806, 5197, 4765, 5093, 5489, 4308, 5782, 6263, 6756, 7769, 8547, 9131, 6701, 7870, 11349, 9584, 7519, 10058, 11158, 11586, 13481, 11001, 10833, 12388, 12045, 13896, 13523, 11612, 12873, 12967, 14470, 17018, 13047, 12205, 16160, 15715, 15500, 15852, 12943, 14335, 16217, 15416, 16634, 15389, 14546, 16622, 16591, 14822, 15200, 15980, 15736, 16702, 16529, 16604, 15252, 13126, 14863, 14389, 15276, 15970, 16077, 16263, 14317, 13988, 11277, 13879, 17369, 12743, 11865, 13891, 12898, 11950, 12060, 12987, 12572, 12679, 11212, 7689, 10520, 11655, 11160, 11793, 10651, 10306, 10752, 9976, 7639, 8261, 11676, 9433, 9461, 9759, 9319, 7559, 6522, 8234, 6341, 4234, 4126, 2281, 2730, 5142, 2557, 1528, 3612, 3866, 429, 94, 2624, 710, -1074, -286, -1305, -1948, -163, -705, -629, -1824, -2531, -1516, -1574, -859, -2418, -8986, -4757, 344, -6373, -8154, -5204, -5117]
# print len(chunk)
# msg_o = 'H ey'
# msg_o = "In the field of audio steganography, fundamental spread spectrum (SS) techniques attempts to distribute secret data throughout the frequency spectrum of the audio signal to the maximum possible level."
# core = StegoCore(msg_o, 1, StegoMode.Hide)
#core.process(chunk)
#
# core = StegoCore('', 1, StegoMode.Recover)
# core.process(chunk)
#
# print core.recover_message()



msg_o = "@&&^%**<>@&&^%**<>"
M = message_to_matrix(msg_o)
QMatrix.pretty_print_matrix(M)
E = QMatrix.encode_matrix_message(M, 11)
QMatrix.pretty_print_matrix(E)
_M = QMatrix.decode_matrix_message(E, 11)
msg = matrix_to_message(_M)
print msg








