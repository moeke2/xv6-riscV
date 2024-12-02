1. Use hd to print the struct superblock.:

```bash
$ hd -s 1024 -n 1024 forensics.img
00000400  40 30 20 10 d0 07 00 00  a2 07 00 00 c8 00 00 00  |@0 .............|
00000410  1e 00 00 00 02 00 00 00  20 00 00 00 2d 00 00 00  |........ ...-...|
00000420  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000800
```

2. Determine the total number of inodes in this image and which disk block these inodes start on.
    We have 0xc8 inodes that start at block 0x20 (4th and 6th uint in the struct above, little endian).

3. Calculate the size of a struct dinode. (Hint: a short is 2 bytes.)
    A dinode has 4 shorts, 1 uint and NDIRECT+1 uint addresses ==> 4*2 + 1*4 + 13*4 = 64 byte

4. Now use hd to view the disk block where the inodes start. The output should start like this:
<addr> 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
<addr> 01 00 00 00 00 00 02 00  00 04 00 00 2e 00 00 00  |................|
<addr> 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|

    4.1) On which data block is the root directory stored? Print this data block with hd for verification: at this point, you should see the names of the files in your root directory appear in the output of hd (for example, README, cat, echo, etc.). 

        The root dir inode is at 0x20 * 1024 --> root dir data block is at 0x2e * 1024
        ```bash
        hd -s 32768 -n 1024 forensics.img
        hd -n 47104 -s 1024 forensics.img

        ```
    4.2) The last entry in the root directory (hint: struct dirent) is another folder. What is the name of this directory? Which inode does this directory refer to?
        name down

        Inode 0x15.
    4.3) Use hd to only print the inode of this folder. What parameters do you use? The output should start like this:
    <addr>  01 00 00 00 00 00 02 00  50 00 00 00 d4 02 00 00  |........P.......|
    <addr>  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
    *
    <addr>  01 00 00 00 00 00 02 00  30 00 00 00 d5 02 00 00  |........0.......|
        inode start is 0x20 which is the block number: 
        0x20*1024 = 32768 is the byte offset for inode start
        We look for inode 0x15, each inode is 64 byte --> 1344 byte offset
        32768 + 1344 = 34112 is the byte offset of inode 0x15

        ```bash
        hd -n 64 -s 34112 forensics.img
        00008540  01 00 00 00 00 00 02 00  50 00 00 00 d4 02 00 00  |........P.......|
        00008550  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
        *
        00008580
        ```

        4.3.1--- Use hd to only print the first data block of this folder. What parameters do you use?
            0x2D4 is the address of the first data block
            0x2d4 * 1024 = 741376
            ```bash
            $ hd -s 741376 -n 1024 forensics.img
            000b5000  15 00 2e 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            000b5010  01 00 2e 2e 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            000b5020  16 00 74 68 65 00 00 00  00 00 00 00 00 00 00 00  |..the...........|
            000b5030  19 00 66 69 6c 65 00 00  00 00 00 00 00 00 00 00  |..file..........|
            000b5040  1a 00 64 69 72 65 63 74  69 6f 6e 00 00 00 00 00  |..direction.....|
            000b5050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            *
            000b5400
            ```
        4.3.2--- What type of inode is the last entry in this folder? On which data block(s) is the content stored? How large is the entry?
            inode 0x1a, data block 0x2d8, size 0xa

            inode 1a:
            inode offset 32768 + entry 0x1a * 64 = 34432
            ```bash
            $ hd -n 64 -s 34432 forensics.img
            00008680  02 00 00 00 00 00 01 00  0a 00 00 00 d8 02 00 00  |................|
            00008690  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            *
            000086c0
            ```
        4.3.3 Use dd to extract the file from steps 7 and 8 from the file system. Run man dd to see which parameters you need. Hint: set bs=1.
            data block 2d8: 0x2d8 * 1024 = 745472
            ```bash
            hd -n 64 -s 745472 forensics.img
            000b6000  57 52 4f 4e 47 5f 57 41  59 0a 00 00 00 00 00 00  |WRONG_WAY.......|
            000b6010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
            *
            000b6040
            ```


            ```bash
            $ dd if=forensics.img of=direction bs=1 count=10 skip=745472
            ```
            Leads to "Wrong Way" file.


