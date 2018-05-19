# Code Compression and Decompression Algorithm

This repository contains source code for variable length dictionary based code compression 
and decompression algorithm. A 3-bit dictionary is used based on frequency of binary codes.
The most frequent instruction have index 000. The input consists of 32-bit binaries. The most
beneficial pattern is chosen for compression.If two formats produce exactly the same compression, 
the one that has more priority is chosen. 

Priority for different formats:
1. Run length encoding
2. Bit-mask based compression
3. 1-bit mismatch
4. 2-bit consecutive mismatch
5. 2-bit mismatch anywhere
6. Direct matching
7. Original binary

### Files Included

* SIM.c     	 - Code compression and decompression logic using dictionary based encoding techniques.
* original.txt   - This file contains input instructions for compression.
* cout.txt       - This file has the expected compressed output.
* dout.txt       - This file has the expected decompressed output.
* compressed.txt - This file contains input binaries for decompression.  

### Note

Please use parameters "1" and "2" to indicate compression, and decompression respectively.\n
./SIM 1 for compression <br />
./SIM 2 for decompression <br />  	