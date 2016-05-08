# Challenge 10 - Container madness

Don't you always find that you lose your keys when you need them the most? We always try to keep them in a safe place and think "I'll remember where they are", but this isn't always the case. Please, help us to find our keys. They must be around here somewhere, but we can't find them.

## Input

The last place we saw them was in this file: container.zip

## Output

All of our keys, in order

## Sample output
```
KEY1: This is the first key
KEY2: This is the second key
.
.
.
KEYN: This is the nth key
```

## Solution
```
unzip -l container.zip | grep KEY # get KEY1
unzip container.zip
unzip -c tuenti.docx | grep --text -o "KEY2: [^ <]*" # get KEY2
unzip tuenti.docx tuenti.mp3
eyeD3 tuenti.mp3  | grep KEY3 -A 1 # get KEY3
eyeD3 --write-image . tuenti.mp3 
identify -verbose FRONT_COVER.png | grep KEY # get KEY4
identify -verbose FRONT_COVER.png | grep NextLevel | perl -MMIME::Base64 -e 'use Path::Tiny; my $in = <STDIN>; chomp($in); $in =~ s/^\s*NextLevel:\s+//; path('out')->spew(MIME::Base64::decode($in))'
pdfinfo out  | grep KEY5 # get KEY5
pdfdetach out -saveall
mkvmerge --identify video.mkv
mkvextract tracks video.mkv 1:track1
subp2png -n track1
tesseract track10001.png stdout # almost good ocr for KEY6
mkvextract attachments video.mkv 1:video_attachment
grep KEY video_attachment # get KEY7
cat video_attachment | perl -MConvert::UU -MIO::All -e 'use Path::Tiny; my $in = io("-")->all; chomp($in); path('uudecode')->spew(Convert::UU::uudecode($in))'
gzip -d uudecode -c | cpio -idm . # get KEY8
gpg --output lastlevel --decrypt LastLevel.txt.gpg # get KEY9 with passphrase MWPcxRK5LFoF7xn5324OjsuybKa2Rzjl as read with gzip -d uudecode -c
```

