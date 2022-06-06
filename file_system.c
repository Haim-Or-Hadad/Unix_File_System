#include "file_system.h"


struct superblock super_block;
struct inode *inodes;
struct disk_block *disk_blocks;
size_t size_sb = sizeof(struct superblock);
size_t size_inode = sizeof(struct inode);
size_t size_db = sizeof(struct disk_block);

//create a new file system
void mymkfs(int s){

super_block.num_inodes = (((s-size_sb)/10) / size_inode);
super_block.num_blocks = ((s-size_sb)-((s-size_sb)/10)) / size_db ;
//super_block.size_blocks = sizeof(struct disk_block);
inodes = malloc(sizeof(struct inode) * super_block.num_inodes);
for (size_t i = 0; i < super_block.num_inodes; i++)
{
    strcpy(inodes[i].name, "inode");
    inodes[i].size = -1;
}
disk_blocks = malloc(sizeof(struct disk_block) * super_block.num_blocks);
for (size_t i = 0; i < super_block.num_blocks; i++)
{
    strcpy(disk_blocks[i].data, "block of disk");
    disk_blocks[i].next_block_num = -1;
}
}

//load a file system
int mymount(const char *source, const char *target,
const char *filesystemtype, unsigned long
mountflags, const void *data){
FILE *file;
file = fopen(target,"r");
//read superblock
fread(&super_block, size_sb, 1, file);
inodes = malloc(super_block.num_inodes*size_inode);
disk_blocks = malloc(super_block.num_blocks*size_db);
//read inodes
fread(inodes ,size_inode, 1,file);
//read disk
fread(disk_blocks, size_db, 1, file);
fclose(file);
}

//write the file system on disk-allocate file pointer
void sync_fs(char* name){
FILE *file;
file = fopen ( name , "w+");
fwrite(&super_block, size_sb, 1 , file);//superblock
fwrite(inodes, size_inode, super_block.num_inodes, file );//inode
fwrite(disk_blocks , size_db , super_block.num_blocks, file);//diskblock
fclose(file);
}

//return file discriptor
int allocate_file(const char name[8]){
int empty_in = find_empty_inode();
int empty_block = find_empty_block();

inodes[empty_in].first_block = empty_block;
disk_blocks[empty_block].next_block_num = -2;
}

int find_empty_inode(){
    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        if (inodes[i].first_block == -1)
        {
            return i;
        }
        return -1;
    }//find empty inode
}

int find_empty_block(){
    for (size_t i = 0; i < super_block.num_blocks; i++)
    {
        if (disk_blocks[i].next_block_num == -1)
        {
            return i;
        }
        return -1;
    }//find empty block
}

void print_fs(){
    printf("superblock info\n");
    printf("num inodes: %d\n", super_block.num_inodes);
    printf("num blocks: %d\n", super_block.num_blocks);
    printf("size blocks: %d\n", super_block.size_blocks);

    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        printf("inode num %ld = %s \n" , i , inodes[i].name);
    }
    
    for (size_t i = 0; i < super_block.num_blocks; i++)
    {
        printf("block num %ld = %s \n" , i , disk_blocks[i].data);
    }
}