# Challenge 9 - X Correlate all the things

## Problem

At Tuenti we are developing our own voice recognition system, and in order to match different patterns to a wave, we use an algorithm that gives a score to each pattern-wave pairing. If the score is high enough, we consider that pattern to be found.

The problem we have now is that the algorithm is CPU intensive and the feature is becoming quite popular, so we need it to be as fast as possible while being single threaded. Can you help us?

The algorithm returns a score for a pair of vectors defining a pattern and a wave, the score being the maximum of the normalized discrete Cross-correlation between the pattern and all the subvectors of the wave times the subvector size. The algorithm code in c++ is supplied here.

## Input

The input file starts with 2 numbers in a single line:

* P, the length of the pattern vector
* W, the length of the wave vector

The next P+1+W lines will contain the two vectors, first the pattern and then the wave, separated by a single empty line.

## Output

Output one single line with the matching score, rounded to the nearest 0.0001 (The code must manage double-precision, though).

## Limits

100 <= P, W <= 3000
0.0 <= wave, pattern <= 15.0

## Sample input
```
10 20
1.0
2.0
3.0
4.0
5.0
6.0
7.0
8.0
9.0
10.0

3.0
2.0
4.0
3.0
5.0
6.0
7.0
8.0
9.0
10.0
11.0
12.0
13.0
1.0
2.0
3.0
2.0
1.0
3.0
4.0
```

## Sample Output
```
9.9645
```

