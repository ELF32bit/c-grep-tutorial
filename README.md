1) Basic grep implementation like it's 1990.
* strlen(), toupper(), isalpha() are not designed to work with multibyte characters 😃️.
* UTF-8 is the most common text encoding where each character is 1 to 4 bytes.
> In <ins>the</ins> beginning God created <ins>the</ins> heaven and <ins>the</ins> earth.
> And God said, Let there be light: and there was light.
> <ins>the</ins>;<ins>The</ins>;<ins>THE</ins>;TheThe;TheTheThe;th

2) More advanced grep implementation that prints individual lines.
* C headers are introduced to structure the program.
* asprintf() is used to create colored lines.
> 1: In <ins>the</ins> beginning God created <ins>the</ins> heaven and <ins>the</ins> earth.
> 3: And <ins>the</ins> evening and <ins>the</ins> morning were <ins>the</ins> third day.
> 5: <ins>the</ins>;<ins>The</ins>;<ins>THE</ins>;TheThe;TheTheThe;th

3) Even more advanced grep implementation with multithreading.
