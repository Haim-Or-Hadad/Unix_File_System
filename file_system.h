#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief meta information about the filesystem
 * number of inodes
 * number of disk blocks
 * size of disk blocks
 */
struct superblock
{
int num_inodes;
int num_blocks;
int size_blocks;
};

/**
 * @brief  inode is an index node. 
 * It serves as a unique identifier for a specific piece of metadata on a given filesystem.
 * the size of the inode
 * the name of the inode(file)
 */
struct inode
{
int size;
char name[8]; 
};

/**
 * @brief file allocation table
 * next_block_num: next block number in the disk
 * data: the data in the block
 */
struct disk_block{
int next_block_num;
char data[512];
};


/**
 * @brief get size and create a file system of s size
 * @param s
 */
void mymkfs(int s);
/**
 * @brief get disk and install all the dick on the file system
 * @param source 
 * @param target 
 * @param filesystemtype 
 * @param mountflags 
 * @param data 
 * @return int 
 */
int mymount(const char *source, const char *target,
const char *filesystemtype, unsigned long
mountflags, const void *data);

/**
 * @brief open the file if exist . if no create new file with pathname!
 * 
 * @param pathname 
 * @param flags 
 * @return int 
 */
int myopen(const char *pathname, int flags);

/**
 * @brief close the file from my table
 * func open get path and permissions , find the file in the inodes table 
 * for easy acsses open has open file system , when file one open
 * so when i open firstly file i write the number in the table of file discrpetor(index 3).
 * @param myfd 
 * @return int 
 */
int myclose(int myfd);

/**
 * @brief read the data
 * 
 * @param myfd 
 * @param buf 
 * @param count 
 * @return ssize_t 
 */
size_t myread(int myfd, void *buf, size_t count);

size_t mywrite(int myfd, const void *buf, size_t count);

/**
 * @brief when file open i have a pointer to start of the file.
 * when i read something from the file the pointer transfer to 
 * the next place. 
 * for any fd the pointer move just for him!.
 * tosleek : to know where the pointer in file.
 * @param myfd 
 * @param offset 
 * @param whence 
 * @return off_t 
 */
//off_t mylseek(int myfd, off_t offset, int whence);

//myDIR *myopendir(const char *name);
//struct mydirent *myreaddir(myDIR *dirp);
//int myclosedir(myDIR *dirp);

//create a new file system
void create_fs();
//load a file system
void mount_fs();
//write the file systen on disk
void sync_fs(char*);