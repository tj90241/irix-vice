SGI VICE library for IRIX

Tested on a SGI O2 box. Doesn't quite work.

MSP IRAM access seems to work, the "go" flag seems correct, and yet the $PC
bounces all over the place instead of sitting in a loop like it should.

I based my findings off this kernel commit:
http://www.linux-mips.org/archives/linux-mips/2002-12/txthaIaqLzUsn.txt

