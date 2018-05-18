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
