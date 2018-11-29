#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>
#include "types.h"
#include "fs.h"

#define BLOCK_SIZE (512)

int nblocks = 995;
int ninodes = 200;
int size = 1024;

uint
i2b(uint inum)
{
    return (inum / IPB) + 2;
}


int
main(int argc, char *argv[])
{
    int fsfd;
    char *addr;
    struct dinode *dip;
    struct superblock *sb;
    //struct dirent *rde;
    struct stat fileStat;

    if(argc < 2){
        fprintf(stderr, "Usage: sample fs.img ...\n");
        exit(1);
    }

    fsfd = open(argv[1], O_RDONLY);
    if(fsfd < 0){
        perror(argv[1]);
        exit(1);
    }

    //get the size of the file
    if(fstat(fsfd,&fileStat) < 0)
        return 1;

    /* Dont hard code the size of file. Use fstat to get the size */
    addr = mmap(NULL, fileStat.st_size , PROT_READ, MAP_PRIVATE, fsfd, 0);
    if (addr == MAP_FAILED){
        perror("mmap failed");
        exit(1);
    }
    /* read the super block */
    sb = (struct superblock *) (addr + 1 * BLOCK_SIZE);
    printf("fs size %d, no. of blocks %d, no. of inodes %d \n", sb->size, sb->nblocks, sb->ninodes);

    /* read the inodes */
    dip = (struct dinode *) (addr + i2b((uint)0)*BLOCK_SIZE);
    printf("begin addr %p, begin inode %p , offset %ld \n", addr, dip, (char *)dip -addr);


    for(int i = 0; i < sb->ninodes; i++){
        printf("Dinode %d Root inode  size %d links %d type %d \n", i+1, dip[i+1].size, dip[i+1].nlink, dip[i+1].type);

        /*
        //check for case 3
        if((i+1) == 1){
            if(dip[i+1].size == 0) {
                printf("ERROR: root directory does not exist\n");
                exit(1);
            }

            rde = (struct dirent *) (addr + (dip[ROOTINO].addrs[0])*BLOCK_SIZE);

            int x = rde->inum;
            rde++;
            int x2 = rde->inum;

            if(x != x2){
                printf("ERROR: root directory does not exist\n");
                exit(1);
            }
        }

        //check for case 1
        if(dip[i+1].type != 0 && dip[i+1].type != 1 && dip[i+1].type != 2 && dip[i+1].type != 3) {
            printf("ERROR: bad inode\n");
            exit(1);
        }

        //check for case 4
        if(dip[i+1].type == 1){
            rde = (struct dirent *) (addr + (dip[i+1].addrs[0])*BLOCK_SIZE);
            if(strcmp(rde->name ,".") == 0){
                rde++;
                if(strcmp(rde->name ,"..") == 0){}
                else{
                    printf("ERROR: directory not properly formatted.\n");
                    exit(1);
                }
            }
            else{
                printf("ERROR: directory not properly formatted.\n");
                exit(1);
            }
        }


        //check for case 5
        if(dip[i+1].type != 0) {
            for (int j = 0; j < NDIRECT; j++) {
                if (dip[i + 1].addrs[j] != 0) {
                    int blocknum = dip[i + 1].addrs[j];
                    uchar *bitmap = (uchar *) (addr + BBLOCK(blocknum, sb->ninodes) * BLOCK_SIZE);
                    int bi = blocknum % BPB;
                    int m = 1 << (bi % 8);
                    if ((bitmap[bi / 8] & m) == 0) {
                        printf("ERROR: address used by inode but marked free in bitmap\n");
                        exit(1);
                    }
                    else {}
                }
            }
        }
*/
    }

    //case6
    for(int i=1; i <= sb->nblocks; i++) {

        int blocknum = i;
        uchar *bitmap = (uchar *) (addr + BBLOCK(blocknum, sb->ninodes) * BLOCK_SIZE);
        int bi = blocknum % BPB;
        int m = 1 << (bi % 8);

        if ((bitmap[bi / 8] & m) == 0){
            continue;
        }
        printf("used bitmap %d block number %d\n", (bitmap[bi / 8] & m), i);
        for(int j = 1; j <= sb->ninodes; j++){
            for(int z=0; z<NDIRECT; z++){
                if(dip[j].addrs[z] == (i)){
                    printf("matches\n");
                }
            }

            uint *indirect = (uint *) (dip[j].addrs[NDIRECT]*BLOCK_SIZE + addr);
            for(int z = 0; z<= NINDIRECT; z++, indirect++){
                if(*(indirect) == i){
                    printf("matches\n");
                }
            }
        }
    }

    exit(0);

}

