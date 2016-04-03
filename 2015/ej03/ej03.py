#!/usr/bin/python
import sys

known_primes = (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97)

def prime_factors(number):
    primes = []
    cont = True
    while cont:
        cont = False
        for prime in reversed(known_primes):
            if number % prime == 0:
                primes.append(prime)
                number /= prime
                cont = True
    return primes



numbers_file = open('numbers.txt', 'r')
numbers = [long(line.rstrip()) for line in numbers_file]
numbers_file.close()

num_cases = int(sys.stdin.readline().rstrip())
for case in range(0, int(num_cases)):
    indices = [int(i) for i in sys.stdin.readline().rstrip().split(" ")]
    numbers_to_process = [numbers[index] for index in range(indices[0], indices[1])]
    primes_count = dict(zip(known_primes, [0 for p in known_primes]))
    for number in numbers_to_process:
        primes = prime_factors(number)
        for prime in primes:
            primes_count[prime] += 1
    maximum = max(primes_count.itervalues())
    output = [maximum]
    for k, v in primes_count.iteritems():
        if v == maximum:
            output.append(k)
    print ' '.join(str(i) for i in output)
