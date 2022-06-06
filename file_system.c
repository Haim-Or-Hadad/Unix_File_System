#include "file_system.h"


struct superblock super_block;
struct inode *inodes;
struct disk_block *disk_blocks;
size_t size_sb = sizeof(struct superblock);
size_t size_inode = sizeof(struct inode);
size_t size_db = sizeof(struct disk_block);

//create a new file system
void create_fs(){
super_block.num_inodes = 10;
super_block.num_blocks = 100;
super_block.size_blocks = sizeof(struct disk_block);

inodes = malloc(sizeof(struct inode) * super_block.num_inodes);
for (size_t i = 0; i < super_block.num_inodes; i++)
{
    strcpy(inodes[i].name, "inode");
    inodes[i].size = -1;
}

disk_blocks = malloc(sizeof(struct disk_block) * super_block.num_blocks);
for (size_t i = 0; i < super_block.num_blocks; i++)
{
    //strcpy(inodes[i].name, "");
    disk_blocks[i].next_block_num = -1;
}

}
//load a file system
void mount_fs(){

}
//write the file systen on disk-allocate file pointer
void sync_fs(char* name){
FILE *file;
file = fopen ( name , "w+");
fwrite(&super_block, size_sb, 1 , file);//superblock

for (size_t i = 0; i < super_block.num_inodes; i++)
{
    fwrite(&inodes[i], size_inode, 1, file );
}//inode

for (size_t i = 0; i < super_block.num_blocks; i++)
{
    fwrite(&disk_blocks[i], size_db , 1, file);
}//diskblock

fclose(file);
}