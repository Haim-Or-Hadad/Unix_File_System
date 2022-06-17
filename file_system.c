#include "file_system.h"

struct myopenfile open_files[10];
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

int  allocate_file(const char * path, int size){
int free_inode = -1;
int free_block = -1;
for (size_t i = 0; i < super_block.num_inodes; i++)
{
    if (inodes[i].first_block == -1) {
        free_inode =  i;
    }       
}
for (size_t i = 0; i < super_block.num_blocks; i++)
{
    if (disk_blocks[i].next_block_num == -1) {
        free_block = i;
   }
}
memcmp(inodes[free_inode].name , path, strlen(path));
inodes[free_inode].first_block = free_block;
inodes[free_inode].size = size;
disk_blocks[free_block].next_block_num = -1;
return free_inode;
}

int mymkdir(const char *path, const char* name) {
    myDIR* dirp = myopendir(path);
    int fd = dirp->fd;
    if (fd==-1) {
        return -1;
    }
    int dirblock = inodes[fd].first_block;
    struct mydirent* currdir = (struct mydirent*)disk_blocks[dirblock].data;
    int newdirfd = allocate_file(path, sizeof(struct mydirent));
    currdir->files[currdir->size++] = newdirfd;
    inodes[newdirfd].dir = 1;
    struct mydirent* newdir = malloc(sizeof(struct mydirent));
    newdir->size = 0;
    for (size_t i = 0; i < 15; i++)
    {
        newdir->files[i] = -1;
    }
    
    char *newdiraschar = (char*)newdir;

    for (size_t i = 0; i < sizeof(struct mydirent); i++)
    {
        writebyte(newdirfd, i, newdiraschar[i]);        
    }
    strcpy(newdir->name, name);
    myclosedir(dirp);
    return newdirfd;
}


myDIR* myopendir(const char *pathname){
char str[50];//to save the path
    memcpy(str, pathname, strlen(pathname));///copy the pathname to str
    char *path = strtok(str,"/");
    char current_path[12];
    char lst_path[12];
    while (path != NULL)
    {
        //printf(" % path\n ", token);//for test
        strcpy(lst_path,current_path);
        strcpy(current_path, path);//save in current path
        path = strtok(NULL, str);
    }
    //now we search the path name in inodes
    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        if (strcmp(inodes[i].name, current_path) == 0 )
        {
            if (inodes[i].dir != 1)
            {
                return -1;
            }
    }
    return i;
    }
    int my_fd = myopendir(lst_path);
    if (my_fd == -1) {
        return -1;
    }
    if (inodes[my_fd].dir != 1) {
        perror("DIR_NOT_FILE");
        return -1;
    }
    int d_b = inodes[my_fd].first_block;
    struct mydirent *currdir = (struct mydirent *) disk_blocks[d_b].data;
    int newdirfd = allocate_file(current_path, sizeof(struct mydirent));
    currdir->files[currdir->size++] = newdirfd;
    inodes[newdirfd].dir = 1;
    struct mydirent *newdir = malloc(sizeof(struct mydirent));
    newdir->size = 0;
    for (size_t i = 0; i < 12; i++) {
        newdir->files[i] = -1;
    }

    char *new_dir2 = (char *) newdir;
    write_byte(newdirfd, 0, new_dir2);
    open_files[my_fd].pos += (sizeof(struct mydirent));
    strcpy(newdir->name, pathname);
    return newdirfd;
}


int myopen(const char *pathname, int flags)
{
    char str[50];//to save the path
    memcpy(str, pathname, strlen(pathname));///copy the pathname to str
    char *path = strtok(str,"/");
    char current_path[12];
    char lst_path[12];
    while (path != NULL)
    {
        //printf(" % path\n ", token);//for test
        strcmp(current_path, path);//save in current path
    }
    //now we search the path name in inodes
    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        if (strcmp(inodes[i].name, current_path) == 0 )
        {
        open_files[i].fd = i;//save the num of file
        open_files[i].pos = 0; // position of the lseek!
        return i;
        }
    }
    // if file not exist
    int p_fd = allocate_file(pathname,1);
    int d_fd =  myopendir(lst_path); 
    struct mydirent *current_dir;
    if (inodes[d_fd].dir != 1) {
        current_dir =  NULL;
    }
    current_dir = (struct mydirent *) disk_blocks[inodes[d_fd].first_block].data;
    current_dir->files[current_dir->size++] = p_fd;
    open_files[p_fd].fd = p_fd;
    open_files[p_fd].pos = 0;
    return p_fd;
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