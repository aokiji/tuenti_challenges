# Challenge 0 - Pizza love

We love pizza. You love pizza. And your friends love pizza too. You are hosting a party tomorrow, and you want to have enough pizza for everyone. Following the widely-known standards, every pizza is cut into 8 slices, and you know the maximum number of pizza slices that each person will eat. You want to know the minimum number of pizzas you need to order so that nobody goes hungry during the party.

## Input

In the first line, an integer T indicates the number of cases.

Each case consists of two lines. The first one contains a number N, the number of people attending the party. The second line contains N numbers, representing the maximum number of pizza slices that each guest eats (S).

## Output

For each case, a line starting with "Case #x: " followed by the minimum number of pizzas you need to order to ensure that nobody goes hungry.

## Limits

*   1 ≤ T ≤ 100
*   1 ≤ N ≤ 10000
*   1 ≤ S ≤ 100

# Sample Input

```
3
3
8 8 8
2
5 3
4
3 4 5 6
```

## Sample Output

```
Case #1: 3
Case #2: 1
Case #3: 3
```

In the first case, each of the three attendees eats an entire pizza, so the answer is 3.
In the second case, the two attendees eat 8 slices, so a single pizza is enough for them.
In the third case, the answer is 3:

* Attendees #1 and #3 eat an entire pizza together (3+5=8)
* Attendee #2 eats 4 slices of a second pizza
* Attendee #4 eats up to 6 pizza slices, but only 4 remain from the second pizza, so we need another one.

