# Challenge 3 - Favourite primes

As you may have already heard, archeologists working on Samos have made a fascinating discovery. It seems that Pythagoreans had a very intriguing daily ceremony: each of them selected one of the first 25 prime numbers, they multiplied those primes that were chosen and wrote down the result on a rock. They repeated this same routine for almost 100 years! We have scanned the rock and you can find the numbers here.

The archeologists heading this research have put out an international call to analyze these numbers, as they believe that extremely valuable historical information can be extracted from those numbers. Specifically, they want to know which prime numbers were most popular in different time periods. Please help us to make this discovery!

## Input

The first line contains the number of cases.
The rest of the file contains 1 line per case, containing 2 numbers: the 0-indexed beginning (inclusive) and end (exclusive) of the time period to be checked.

## Output

N lines each containing the number of repetitions of the most popular prime followed by the most popular prime. If there is more than one number with the same number of repetitions, all of them should appear, in ascending order, separated by spaces.

## Sample Input

```
2
0 1
0 2
```

## Sample Output

```
11 7 11 41 71
21 11
```

