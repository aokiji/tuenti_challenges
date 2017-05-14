# Challenge 8 - Uni code to rule them all

We need to process lots of data. As a big problem with data is how you recollect it, one of the most important steps is cleaning and normalizing it.

In this case, we need to clean integers, just integers. We asked our data collectors to only use integers with decimal notation. We thought this would give us clean data… but that’s not the case, there are invalid numbers, and a lot of valid numbers with strange characters.

Please, help us to clean this data.

## Input

The first line will contain an integer C, the number of cases for our problem.
Each case consists of a line with some characters. Some of them will be valid numbers. A number is valid if all its characters are decimal digits (Nd), and there are no other characters or spaces inside the number. Only one number is allowed per line. Spaces (Zs) are allowed at the beginning and end of each line.
## Output

For each case, a line starting with "Case #x: " followed by the the number in hexadecimal format or, if the number is not valid, the string “N/A”. Every line is followed by a new line character.
## Limits

   * 0 ≤ Number ≤ 104097 - 1
   * 1 ≤ Number of lines ≤ 250

## Sample Input
```
11
12345
 12345  
 12345  a
   789꧔
123 45
1234abc  
٨8٩
۵ ۷
٦ମ2
O５
०５
```

## Sample Output

```
Case #1: 3039
Case #2: 3039
Case #3: N/A
Case #4: 1ed6
Case #5: N/A
Case #6: N/A
Case #7: 379
Case #8: N/A
Case #9: N/A
Case #10: N/A
Case #11: 5
```
