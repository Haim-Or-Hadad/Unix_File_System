#include "file_system.h"

struct myopenfile open_files[10];
struct superblock super_block;
struct inode *inodes;
struct disk_block *disk_blocks;
size_t size_sb = sizeof(struct superblock);
size_t size_inode = sizeof(struct inode);
size_t size_db = sizeof(struct disk_block);

int find_empty_inode() {
    for (int i = 0; i < super_block.num_inodes; i++) {
        if (inodes[i].first_block == -1) {
            return i;
        }
    }
    return -1;
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


void write_byte(int fd, int pos, char* data) {
    int relative_block = pos / 512;
    int block_num = inodes[fd].first_block;
    while (relative_block > 0) {
        block_num = disk_blocks[block_num].next_block_num;
        relative_block--;
    }
    int offset = pos % 512;
    for (int i = 0; i < strlen(data); i++) {
        disk_blocks[block_num].data[offset + i] = data[i];
    }
}

int  allocate_file(const char * path, int size){
int free_inode = find_empty_inode();
int free_block = find_empty_block();
strcpy(inodes[free_inode].name, path);
inodes[free_inode].first_block = free_block;
inodes[free_inode].size = size;
disk_blocks[free_block].next_block_num = -1;
return free_inode;
}
//create a new file system
void mymkfs(int s){
int size_without_sb = s - sizeof(struct superblock);
super_block.num_inodes = (size_without_sb/10) / size_inode;
super_block.num_blocks = (size_without_sb-size_without_sb/10) / size_db ;
super_block.size_blocks = sizeof(struct disk_block);
inodes = malloc(sizeof(struct inode) * super_block.num_inodes);
for (size_t i = 0; i < super_block.num_inodes; i++)
{
    strcpy(inodes[i].name, "empty");
    inodes[i].size = -1;
    inodes[i].dir = 0; //0 mean is file
    inodes[i].first_block = -1;
}
disk_blocks = malloc(sizeof(struct disk_block) * super_block.num_blocks);
for (size_t i = 0; i < super_block.num_blocks; i++)
{
    strcpy(disk_blocks[i].data, "e_block");
    disk_blocks[i].next_block_num = -1;
}
int root_fd=allocate_file("root", sizeof(struct mydirent));
inodes[root_fd].dir = 1;//1 is dir
struct mydirent *root_dir = malloc(sizeof(struct mydirent));
strcpy(root_dir->name, "root");
root_dir->size = 0;
for (size_t i = 0; i < 15; i++) {//init files in dir be -1=empty
        root_dir->files[i] = -1;
}
char* dir_of_root_char = (char*)root_dir;
write_byte(root_fd, 0, dir_of_root_char);
open_files[root_fd].pos += (sizeof(dir_of_root_char));
free(root_dir);
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
        write_byte(newdirfd, i, newdiraschar[i]);        
    }
    strcpy(newdir->name, name);
    myclosedir(dirp);
    return newdirfd;
}


myDIR* myopendir(const char *pathname){
    char str[50];//to save the path
    strcpy(str, pathname);
    const char s[2] = "/";
    char *path = strtok(str,s);
    char current_path[12] = "";
    char lst_path[12] = "";
    while (path != NULL)
    {
        //printf(" % path\n ", token);//for test
        strcpy(lst_path,current_path);
        strcpy(current_path, path);//save in current path
        path = strtok(NULL, s);
    }
    //now we search the path name in inodes
    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        if (!strcmp(inodes[i].name, current_path))
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
    int newdirfd = allocate_file(sizeof(struct mydirent),current_path);
    currdir->files[currdir->size++] = newdirfd;
    inodes[newdirfd].dir = 1;
    struct mydirent *newdir = malloc(sizeof(struct mydirent));
    newdir->size = 0;
    for (size_t i = 0; i < 15; i++) {
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
// int allocate_file(const char name[8]){
// int empty_in = find_empty_inode();
// int empty_block = find_empty_block();

// inodes[empty_in].first_block = empty_block;
// disk_blocks[empty_block].next_block_num = -2;
//}

int myclosedir(myDIR* myfd){
    free(myfd);
}



size_t myread(int myfd, void *buf, size_t count){
    char* buffer = malloc(count+1);
    int rb= inodes[myfd].first_block;
    int curr_pos=open_files[myfd].pos;
    for (size_t i = 0; i < count; i++)
    {
        while (curr_pos>=size_db) {
            curr_pos-=size_db;
            rb = disk_blocks[rb].next_block_num;
    }
        int curr_pos=open_files[myfd].pos;
        buffer[i] = disk_blocks[rb].data[curr_pos];
        open_files[myfd].pos++;
    }
    buffer[count] = '\0';
    strncpy(buf, buffer, count);
    free(buffer);
    

    return open_files[myfd].pos;
}

size_t mywrite(int myfd, const void *buf, size_t count) {
    if (inodes[myfd].dir == 1) {
        return -1;
    }
    char *buffer = (char *) buf;
    write_byte(myfd, open_files[myfd].pos, buffer);
    open_files[myfd].pos += (count);
    return open_files[myfd].pos;
}

int mylseek(int myfd, off_t offset, int whence){
    if (whence==SEEK_CUR) {
        open_files[myfd].pos += offset;
    } 
    else if (whence==SEEK_END) {
        open_files[myfd].pos = inodes[myfd].size+offset;
    } 
    else if (whence==SEEK_SET) {
        open_files[myfd].pos = offset;
    }
    if (open_files[myfd].pos<0) {
        open_files[myfd].pos = 0;
    }
    return open_files[myfd].pos;
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


void printdir(const char* pathname) {
    /**
     * @brief Function used for debugging, print information about a directory with a given apth.
     */
    myDIR* dirp = myopendir(pathname);
    int fd = dirp->fd;
    printf("NAME OF DIRECTORY: %s\n", inodes[fd].name);
    struct mydirent* currdir = (struct mydirent*)disk_blocks[inodes[fd].first_block].data;
    for (size_t i = 0; i < currdir->size; i++)
    {
        printf("file number %ld: %s, ",i, inodes[currdir->files[i]].name);
    }
    myclosedir(dirp);
    printf("\nDONE\n");
}