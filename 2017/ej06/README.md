# Challenge 6 - The Tower

The Tower is made up of floors, each as wide as a continent, which are stacked infinitely one on top of another. Every regular starts at the second floor, the Floor of Test, and becomes a Ranker when they reach the last floor explored by Jahad and the Ten Great Families. To advance from one floor to the next, a regular has to pass a test imposed by the Guardian of that floor. Climbing The Tower is harder than it sounds and, of the millions of chosen regulars only a select few succeed in reaching the top, taking centuries on average to do so.

The Tower is all and everything and the answer to every question lies at its top. No one knows what may exist outside of it. No one, that is, except those who come from The Outside. Baam is an irregular from the outside world who opened the Door at the first floor, following his friend Rachel. As a Guide, you know all the ways through The Tower. You can see Baam's way too, how it will change the future and bring change to The Tower. It is your duty, as a Guide of The Tower, to help him become a Ranker and fulfill his destiny.

Every floor is connected to the next one and it takes Baam n years to pass the test of the nth floor and move on to the n+1th floor. There are some shortcuts that not everyone is brave (or foolish) enough to take, such as the Hell Train. To take a shortcut, one also has to take a test. You want Baam to become a Ranker as soon as possible, so you must find the fastest way to reach the last explored floor of The Tower. Once Baam arrives on a floor, he can go back to any lower floor to take a shortcut, and the amount of time taken to move between floors is negligible compared to the amount of time that Baam needs to pass a test.

Now, you may not understand everything written here, but don't worry; for now, you just need to discover how long it will take Baam to become a Ranker if he follows the optimum path in The Tower.

## Input

The first line will contain an integer C, the number of cases for our problem.
Each case consists of a line with two integers F and S, the last explored floor of The Tower and the number of shortcuts. A set of S lines with three integers A, B and Y follows, each indicating a shortcut from floor A to floor B with a test that takes Baam Y years to pass.

## Output

For each case, a line starting with "Case #x: " followed by the years it will take Baam to become a Ranker. Every line is followed by a new line character.

## Examples

```
Case 1:

2 0
```

```
Case 2:

4 2
2 4 5
1 3 3
```

```
Case 3:

9 3
3 5 2
4 8 6
5 8 20
```

```
Case 4:

10 8
1 6 6
2 4 1
3 5 5
3 6 5
7 8 3
7 10 10
2 6 2
4 7 4
```

In Case 1, there are only two floors with no shortcuts. The only path is (1 → 2), taking 1 year.
In Case 2, (1 → 2 → 3 → 4), (1 → 3 → 4) and (1 → 2 → 4) take 6 years.
In Case 3, the fastest path is (1 → 2 → 3 → 5 → 8 → 9), taking 19 years. Note that Baam will need to go down to the 4th floor after arriving at the 5th in order to take the shortcut (4 → 8).
In Case 4, the fastest path takes 16 years.

## Limits

*   1 ≤ F ≤ 231
*   0 ≤ Y ≤ 231
*   0 ≤ S ≤ 2500

## Sample Input

```
4
2 0
4 2
2 4 5
1 3 3
9 3
3 5 2
4 8 6
5 8 20
10 8
1 6 6
2 4 1
3 5 5
3 6 5
7 8 3
7 10 10
2 6 2
4 7 4
```

## Sample Output
```
Case #1: 1
Case #2: 6
Case #3: 19
Case #4: 16
```
