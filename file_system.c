#include "file_system.h"




int find_empty_inode() {
    for (size_t i = 0; i < super_block.num_inodes; i++) {
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

int get_block_num(int file, int offset) {
    int togo = offset;
    int bn = inodes[file].first_block;
    while (togo > 0) {
        bn = disk_blocks[bn].next_block_num;
        togo--;
    }
    return bn;
}

// void write_byte(int fd, int pos, char* data) {
//     int relative_block = pos / 512;
//     int num_of_block = get_block_num(fd, relative_block);
//     int offset = pos % 512;
//     for (int i = 0; i < strlen(data); i++) {
//         disk_blocks[num_of_block].data[offset + i] = data[i];
//     }
// }
void write_byte(int fd, int pos, char data) { 
    /**
     * @brief Write a SINGLE byte into a disk block. 
     * The function calculates the correct relevant block (rb) that is needed to be accessed. 
     * if the position that is needed to be wrriten is out of the bounds of the file,
     * allocate a new disk block for it. 
     */
    int pos_ = pos;
    int rb = inodes[fd].first_block;
    while (pos_>=512) {
        pos_-=512;
        if (disk_blocks[rb].next_block_num==-1) {
            
            return -1;
        } else if (disk_blocks[rb].next_block_num == -2) { // the current block is the last block, so we allocate a new block
            disk_blocks[rb].next_block_num = find_empty_block(); 
            rb = disk_blocks[rb].next_block_num;
            disk_blocks[rb].next_block_num = -2; 
        } else {
            rb = disk_blocks[rb].next_block_num;
        }
    }
    if (pos>inodes[fd].size) {
        inodes[fd].size = pos+1;
    }
    disk_blocks[rb].data[pos] = data;
}
// void write_byte(int fd, int opos, char data) { 
//     /**
//      * @brief Write a SINGLE byte into a disk block. 
//      * The function calculates the correct relevant block (rb) that is needed to be accessed. 
//      * if the position that is needed to be wrriten is out of the bounds of the file,
//      * allocate a new disk block for it. 
//      */
//     int pos = opos;
//     int rb = inodes[fd].first_block;
//     while (pos>=512) {
//         pos-=512;
//         if (disk_blocks[rb].next_block_num==-1) {
//             return -1;
//         } else if (disk_blocks[rb].next_block_num == -2) { // the current block is the last block, so we allocate a new block
//             disk_blocks[rb].next_block_num = find_empty_block(); 
//             rb = disk_blocks[rb].next_block_num;
//             disk_blocks[rb].next_block_num = -2; 
//         } else {
//             rb = disk_blocks[rb].next_block_num;
//         }
//     }
//     if (opos>inodes[fd].size) {
//         inodes[fd].size = opos+1;
//     }
//     disk_blocks[rb].data[pos] = data;
// }


int allocate_file(int size, const char* name) {
    /**
     * @brief This function will allocate new inode and enough blocks for a new file. 
     * (One inode is allocated, the amount of needed blocks is calculated)
     * 
     */
    if (strlen(name)>7) {
        return -1;
    }
    int inode = find_empty_inode();
    if (inode == -1) {
        return -1;
    }
    int curr_block = find_empty_block();
    if (curr_block == -1) {
        return -1;
    }
    inodes[inode].size = size;
    inodes[inode].first_block = curr_block;
    disk_blocks[curr_block].next_block_num = -2; 
    strcpy(inodes[inode].name, name);
    if (size>512) {  // REQUIRES TESTS
        int allocated_size = -(3*512)/4;
        //int bn = size/BLOCK_SIZE;
        int next_block;
        while (allocated_size<size)
        {
            next_block = find_empty_block();
            if (next_block == -1) {
                return -1;
            }
            disk_blocks[curr_block].next_block_num = next_block;
            curr_block = next_block;
            allocated_size+=512;
        }
    }
    disk_blocks[curr_block].next_block_num = -2;

    return inode; 
}
// int  allocate_file( int size,const char * path){
//     int free_inode = find_empty_inode();
//     // if (free_inode == -1)
//     // {
//     //     return -1;
//     // }
    
//     int free_block = find_empty_block();
//     // if (free_block == -1)
//     // {
//     //     return -1;
//     // }
    
//     inodes[free_inode].size = size;
//     inodes[free_inode].first_block = free_block;
//     disk_blocks[free_block].next_block_num = -2;
//     strcpy(inodes[free_inode].name, path);
//         if (size>512) {  // REQUIRES TESTS
//         int allocated_size = -(3*512)/4;
//         //int bn = size/BLOCK_SIZE;
//         int next_block;
//         while (allocated_size<size)
//         {
//             next_block = find_empty_block();
//             if (next_block == -1) {
//                 return -1;
//             }
//             disk_blocks[free_block].next_block_num = next_block;
//             free_block = next_block;
//             allocated_size+=free_block;
//         }
//     }
//     disk_blocks[free_block].next_block_num = -2;
//     return free_inode;
// }


void mymkfs(int s) {
    /**
     * @brief Load into the memory a fresh 'install' of a ufs. (UNIX FILE SYSTEM)
     * The install will only include a root directory
     */
    int size_without_superblock = s - sizeof(struct superblock);
    super_block.num_inodes = (size_without_superblock/10)/(sizeof(struct inode)); 
    super_block.num_blocks = (size_without_superblock-size_without_superblock/10)/(sizeof(struct disk_block));
    inodes = malloc(super_block.num_inodes*sizeof(struct inode));
    disk_blocks = malloc(super_block.num_blocks*sizeof(struct disk_block));


    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        strcpy(inodes[i].name, "");
        inodes[i].first_block = -1;
        inodes[i].dir = 0; 
    }

    for (size_t i = 0; i < super_block.num_blocks; i++)
    {
        strcpy(disk_blocks[i].data, "");
        disk_blocks[i].next_block_num = -1;
    }

    // ############### CREATE ROOT FOLDER ###############
    int zerofd = allocate_file(sizeof(struct mydirent),  "root");
    if (zerofd != 0 ) {
        return -1;
    }
    inodes[zerofd].dir = 1;
    struct mydirent* rootdir = malloc(sizeof(struct mydirent));
    for (size_t i = 0; i < 10; i++)
    {
        rootdir->files[i] = -1;        
    }
    strcpy(rootdir->name, "root");
    rootdir->size = 0;
    char* rootdiraschar = (char*)rootdir;
    for (size_t i = 0; i < sizeof(struct mydirent); i++)
    {
        write_byte(zerofd, i, rootdiraschar[i]);
    }
    free(rootdir);
}

int mymkdir(const char *path, const char* name) {
    myDIR * dir_fd = myopendir(path);
    if (dir_fd == -1) {
        return -1;
    }
    if (inodes[dir_fd->fd].dir != 1)
    {
        return -1;
    } 
    int fd = dir_fd;
    int dirblock = inodes[fd].first_block;
    struct mydirent* currdir = (struct mydirent*)disk_blocks[dirblock].data;
    int newdirfd = allocate_file( sizeof(struct mydirent), name);
    currdir->files[currdir->size++] = newdirfd;
    inodes[newdirfd].dir = 1;
    struct mydirent* newdir = malloc(sizeof(struct mydirent));
    newdir->size = 0;
    for (size_t i = 0; i < 12; i++)
    {
        newdir->files[i] = -1;
    }
    
    char *newdiraschar = (char*)newdir;

    for (size_t i = 0; i < sizeof(struct mydirent); i++)
    {
        write_byte(newdirfd, i, newdiraschar[i]);        
    }
    strcpy(newdir->name, name);
    free(dir_fd);
    return newdirfd;
}


myDIR* myopendir(const char *pathname){
    char str[50];//to save the path
    strcpy(str, pathname);
    const char s[2] = "/";
    char *path;
    path = strtok(str,s);
    char current_path[12] = "";
    char lst_path[12] = "";
    while (path != NULL)
    {
        //printf(" % path\n ", token);//for test
        strcpy(lst_path,current_path);
        strcpy(current_path, path);//save in current path
        path = strtok(NULL, s);
    }
    //printf("%s\n", current_path);/for test
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
    myDIR* newdir = (myDIR*)malloc(sizeof(myDIR));
    newdir->fd = i; 
    return newdir;
    }
    myDIR* newdir = (myDIR*)malloc(sizeof(myDIR));
    int dir_fd = mymkdir(lst_path,current_path);

    return newdir;
}


struct mydirent *myreaddir(myDIR* dir_fd) {
    if (inodes[dir_fd->fd].dir != 1) {
        return NULL;
    }
    return (struct mydirent *) disk_blocks[inodes[dir_fd->fd].first_block].data;
}

int myopen(const char *pathname, int flags)
{
    char str[90];//to save the path
    strcpy(str, pathname);///copy the pathname to str
    const char s[2] = "/";
    char *path;
    path  = strtok(str,s);
    char current_path[12] = "";
    char lst_path[12] = "";
    while (path != NULL)
    {
        if (path != NULL)
        {
            strcpy(lst_path, current_path);
            strcpy(current_path, path);
        }
    path = strtok(NULL, s);
    }
    //now we search the path name in inodes
    for (size_t i = 0; i < super_block.num_inodes; i++)
    {
        if (!strcmp(inodes[i].name, current_path))
        {
        if (inodes[i].dir!=0) {
            return -1;}
        
        open_files[i].fd = i;//save the num of file
        open_files[i].pos = 0; // position of the lseek!
        return i;
        }
        }
        if (flags != 1 )
        {
            return -1;
        }
        
    int newfd = allocate_file(1, current_path);
    // printf("%s%d\n", current_path, newfd);
    myDIR * dir_fd = myopendir(lst_path);
    struct mydirent *currdir = myreaddir(dir_fd);
    currdir->files[currdir->size++] = newfd;
    open_files[newfd].fd = newfd;
    open_files[newfd].pos = 0;
    //myclosedir(dirp);
    return newfd;


    }
    


int myfclose(int the_fd){
    open_files[the_fd].pos = -1;
    open_files[the_fd].fd = -1;
}

//load a file system
// int mymount(const char *source, const char *target,
// const char *filesystemtype, unsigned long
// mountflags, const void *data){
// FILE *file;
// file = fopen(target,"r");
// //read superblock
// fread(&super_block, size_sb, 1, file);
// inodes = malloc(super_block.num_inodes*size_inode);
// disk_blocks = malloc(super_block.num_blocks*size_db);
// //read inodes
// fread(inodes ,size_inode, 1,file);
// //read disk
// fread(disk_blocks, size_db, 1, file);
// fclose(file);
// }

//write the file system on disk-allocate file pointer
// void sync_fs(char* name){
// FILE *file;
// file = fopen ( name , "w+");
// fwrite(&super_block, size_sb, 1 , file);//superblock
// fwrite(inodes, size_inode, super_block.num_inodes, file );//inode
// fwrite(disk_blocks , size_db , super_block.num_blocks, file);//diskblock
// fclose(file);
// }

//return file discriptor
// int allocate_file(const char name[8]){
// int empty_in = find_empty_inode();
// int empty_block = find_empty_block();

// inodes[empty_in].first_block = empty_block;
// disk_blocks[empty_block].next_block_num = -2;
//}

// int myclosedir(myDIR* myfd){
//     free(myfd);
// }



// size_t myread(int myfd, void *buf, size_t count){
//     char* buffer = malloc(count+1);
//     if (open_files[myfd].fd != myfd) { 
//         return -1;
//     }
//     if (inodes[myfd].dir==1) {
//         return -1;
//     }
//     for (size_t i = 0; i < count; i++)
//     {
//         int pos_ = open_files[myfd].pos;
//         int start_block = inodes[myfd].first_block;
//         while(pos_ >= 512){
//             pos_ -= 512;
//             start_block = disk_blocks[start_block].next_block_num;
//         if (inodes[myfd].first_block == -1)
//         {
//             return -1;
//         }
//         if (inodes[myfd].first_block == -2)
//         {
//             return -1;
//         }
        
//         }
        
//         buffer[i] = disk_blocks[start_block].data[pos_];
//         open_files[myfd].pos++;
//     }
//     buffer[count] = '\0';
//     strncpy(buf, buffer, count);
//     free(buffer);
    
//     return open_files[myfd].pos;
    
// }
char read_byte(int fd, int pos) {
    /**
     * @brief Read a SINGLE byte from a disk block. 
     * The function calculates the correct relevant block (rb) that is needed to be accessed. 
     * The single byte is @return 'ed.
     */
    int rb = inodes[fd].first_block;
    while (pos>=512) {
        pos-=512;
        rb = disk_blocks[rb].next_block_num;
        if (rb==-1) {
            return -1;
        } else if (rb == -2) {
            return -1;
        }
    }
    return disk_blocks[rb].data[pos];
}

size_t myread(int myfd, void *buf, size_t count) {
    /**
     * @brief Read a chunk of bytes from the given file into @param buf.
     * @return the current position of the file pointer. 
     * 
     */
    if (inodes[myfd].dir==1) {
        return -1;
    }
    if (open_files[myfd].fd != myfd) { 
        return -1;
    }
    char* buffer = malloc(count+1);
    for (size_t i = 0; i < count; i++)
    {
        buffer[i] = read_byte(myfd, open_files[myfd].pos);
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
    open_files[myfd].pos++;
    for (size_t i = 0; i < count; i++)
    {
        write_byte(myfd, open_files[myfd].pos, buffer[i]);
        open_files[myfd].pos++;
    }
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


// void printdir(const char* pathname) {
//     /**
//      * @brief Function used for debugging, print information about a directory with a given apth.
//      */
//     myDIR* dirp = myopendir(pathname);
//     int fd = dirp->fd;
//     printf("NAME OF DIRECTORY: %s\n", inodes[fd].name);
//     struct mydirent* currdir = (struct mydirent*)disk_blocks[inodes[fd].first_block].data;
//     for (size_t i = 0; i < currdir->size; i++)
//     {
//         printf("file number %ld: %s, ",i, inodes[currdir->files[i]].name);
//     }
//     myclosedir(dirp);
//     printf("\nDONE\n");
// }