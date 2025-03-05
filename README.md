## 1. Getopt().

## 2. Basic grep implementation like it's 1990.
* strlen(), toupper(), isalpha() are not designed to work with multibyte characters 😃️.
* UTF-8 is the most common text encoding where each character is 1 to 4 bytes.
<table><tr><td>
In <ins>the</ins> beginning God created <ins>the</ins> heaven and <ins>the</ins> earth.<br>
And God said, Let there be light: and there was light.<br>
<ins>the</ins>;<ins>The</ins>;<ins>THE</ins>;TheThe;TheTheThe;th
</table></tr></td>

## 3. Wide character grep implementation supporting more languages.
* Wide characters are compiler-dependent and therefore not very portable.
* External libraries are required to properly support various text encodings.
<table><tr><td>
בראשית ברא אלהים <ins>את</ins>👋️ השמים ואת👋️ הארץ<br>
<ins>את</ins>👋️ואת👋️ואת👋️<ins>את</ins>👋️<ins>את</ins>👋️👋️ואת
</table></tr></td>

## 4. More advanced grep implementation printing individual lines.
* C headers are introduced to structure the program.
* getline() is used to read file line by line without limitations.
* asprintf() is used to easily create generic strings.
<table><tr><td>
1: In <ins>the</ins> beginning God created <ins>the</ins> heaven and <ins>the</ins> earth.<br>
3: And <ins>the</ins> evening and <ins>the</ins> morning were <ins>the</ins> third day.<br>
5: <ins>the</ins>;<ins>The</ins>;<ins>THE</ins>;TheThe;TheTheThe;th<br>
Matches found: 9
</table></tr></td>

## 5. Even more advanced grep implementation with multithreading.
* Run the program multiple times and observe different output order.
* Threads receive jobs from a special thread-safe queue.
* Multiple files can now be provided as input.
<table><tr><td>
[3] ../examples/3-leviticus.txt: 2435<br>
[2] ../examples/3-exodus.txt: 3113<br>
[4] ../examples/3-numbers.txt: 3500<br>
[5] ../examples/3-deuteronomy.txt: 2166<br>
[1] ../examples/3-genesis.txt: 2458<br>
Matches found: 13672
</table></tr></td>
