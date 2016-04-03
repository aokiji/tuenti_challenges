import sys
import re
import base64

slice_re = re.compile("(\d+)((?:L|B)R?)")

b64string = sys.stdin.readline().rstrip()
decoded_string = ''.join([bin(x)[2:].zfill(8) for x in bytearray(base64.standard_b64decode(b64string))])

first_idx = 0
num_cases = int(sys.stdin.readline().rstrip())
for case in range(0, num_cases):
    cpu_slice = sys.stdin.readline().rstrip()
    matches = slice_re.search(cpu_slice)
    num_bits = int(matches.group(1))
    case_bits = []
    while num_bits > 0:
        advance = min(8, num_bits)
        case_bits.append(decoded_string[first_idx:(first_idx + advance)])
        num_bits = num_bits - 8
        first_idx = first_idx + advance
    if 'L' in matches.group(2):
        case_bits.reverse()
    bitstr = ''.join(case_bits)
    if 'R' in matches.group(2):
        bitstr = bitstr[::-1]
    print int(bitstr, 2)
    