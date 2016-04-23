import array
import sys

def read_data():
    data_file = open('sheet.data', 'r')
    M, N = [int(dim) for dim in data_file.readline().rstrip().split(' ')]
    data = []
    for y in range(0, M):
         data.append([int(dim) for dim in data_file.readline().rstrip().split()])
    return data, M, N

def sum_rectangle(data, y0, x0, M, N = None):
    if N is None:
        N = M
    return sum(sum(row[x0:x0+N]) for row in data[y0:y0+M])

def quality(data, M, N, y0, x0, y1, x1, K):
    maxq = 0
    # cache of sum of columns
    sum_up = []
    sum_down = []
    for x in range(x0, x1 + 1):
        sum_up.append(sum_rectangle(data, y0, x, K, 1))
        sum_down.append(sum_rectangle(data, y0 + K + 1, x, K, 1))        
        
    for y in range(y0 + K, y1 - K + 1):
        sumq = None
        for x in range(x0 + K, x1 - K + 1):
            xi = x - x0 
            if sumq is None:
                sumq = sum(sum_up[xi - K: xi])
                sumq += sum(sum_down[xi + 1: xi + K + 1])
            else:
                sumq += sum_up[xi - 1] - sum_up[xi - K - 1] + sum_down[xi + K] - sum_down[xi]  
            if sumq > maxq:
                maxq = sumq
        # update sum_up and sum_down
        if y != y1 - K:
            for x in range(x0, x1 + 1):
                xi = x - x0
                sum_up[xi] += data[y][x] - data[y - K][x]
                sum_down[xi] += data[y + K + 1][x] - data[y + 1][x]
    return maxq

data, M, N = read_data()
T = int(sys.stdin.readline().rstrip())
for t in range(0, T):
    y0, x0, y1, x1, K = [int(x) for x in sys.stdin.readline().rstrip().split()]
    print "Case %s: %s" % (t+1, quality(data, M, N, y0, x0, y1, x1, K))