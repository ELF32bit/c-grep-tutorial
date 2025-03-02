1) Basic grep implementation like it's 1990.
* strlen(), toupper(), isalpha() are not designed to work with multibyte characters 😃️.
* UTF-8 is the most common text encoding where each character is 1 to 4 bytes.
```
In **the** beginning God created **the** heaven and **the** earth.
And God said, Let there be light: and there was light.
**the**;**The**;**THE**;TheThe;TheTheThe;th
```

2) More advanced grep implementation that prints individual lines.
* C headers are introduced to structure the program.
* asprintf() is used to create colored lines.
```
1: In **the** beginning God created **the** heaven and **the** earth.
3: And **the** evening and **the** morning were **the** third day.
5: **the**;**The**;**THE**;TheThe;TheTheThe;th
```

3) Even more advanced grep implementation with multithreading.
