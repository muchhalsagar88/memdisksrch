# memdisksrch
In memory vs Disk based searching

This assignment is about implementing variations of sequential and binary search access of data from in memory data structures as well as secondary storage. It deals with comparing the access times of the two searches in two different settings. 

The program accepts two binary files as input in the follwing format
./srch --<option> <key-file> <seek-file>

The key file conatins the key store which is to be searched while the seek file contains the key to be searched.

The different options for the search are:
1. mem-seq For in memory sequential search
2. mem-bin For in memory binary search
3. sec-seq For sequential search on the secondary storage
4. sec-bin For binary search on the secondary storage

The programs outputs the time taken to search for the keys in the key file.
