# Challenge 5 - Ghost in the HTTP

https://52.49.91.111:8443

## Solution

Download the data given in ranges. Decode it to construct an image with 4017-8120.

```
curl -D - -H 'range: bytes=4017-8120' -k https://52.49.91.111:8443/ghost
```

Inspecting the response it says *You found me. Pushing my token, did you get it?*

It got me thinking of HTTP/2 PUSH

```
nghttp -H 'range: bytes=4017-8120' -v https://52.49.91.111:8443/ghost
```
