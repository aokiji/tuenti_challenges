#!/usr/bin/env python

import sys
from math import ceil

input = sys.stdin
cases = int(input.readline())
for case in range(cases):
    N,M = (int(x) for x in input.readline().split())
    tortillas = ceil(N/2) + ceil(M/2)
    print(f"Case #{case+1}: {tortillas}")
