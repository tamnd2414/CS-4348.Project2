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

#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Special device
#define BLOCK_SIZE (512)

uint i2b(uint inum){
  return (inum/IPB) + 2;
}

int main(int argc, char *argv[]){
	char *addr;
	int fssize;
	struct stat fsStat;
  struct dinode *dinodePtr;
  struct dinode *dinodePtr2;
  struct superblock *sbPtr;
  //struct dirent *dirPtr;

	if(argc < 2){
		fprintf(stderr, "Wrong usage!!!\n");
		exit(1);
	}

	int fsfd = open(argv[1], O_RDONLY);
	if(fsfd < 0){
		perror(argv[1]);
		exit(1);
	}

	if(fstat(fsfd, &fsStat) < 0)
		exit(1);

	fssize = fsStat.st_size;
	addr = mmap(NULL, fssize, PROT_READ, MAP_PRIVATE, fsfd, 0);
	if(addr == MAP_FAILED){
		perror("mmap failed");
		exit(1);
	}

  sbPtr = (struct superblock *) (addr + 1 * BLOCK_SIZE);
  printf("fs size %d, no. of blocks %d, no. of inodes %d \n", sbPtr->size, sbPtr->nblocks, sbPtr->ninodes);

  dinodePtr = (struct dinode *) (addr + i2b((uint)0)*BLOCK_SIZE);
  dinodePtr2 = (struct dinode *) (addr + i2b((uint)8)*BLOCK_SIZE);
  printf("begin addr %p, begin inode %p , offset %ld \n", addr, dinodePtr, (char *)dinodePtr -addr);
  printf("Root inode  size %d links %d address %p\n", dinodePtr[ROOTINO].size, dinodePtr[ROOTINO].nlink, (void *)&dinodePtr[ROOTINO]);
  printf("? 2th inode size %d links %d address %p\n", dinodePtr[2].size, dinodePtr[2].nlink, (void *)&dinodePtr[2]);
  printf("? 8th inode size %d links %d address %p\n", dinodePtr[8].size, dinodePtr[8].nlink, (void *)&dinodePtr[8]);
  printf("? 8th inode size %d links %d address %p\n", dinodePtr2[0].size, dinodePtr2[0].nlink, (void *)&dinodePtr2[0]);
  printf("DInode size: %ld\n", sizeof(struct dinode));

  int i;

  for(i = ROOTINO; i < sbPtr->ninodes; i++){
    if(dinodePtr[i].type == 0)
      continue;
    uint indirAddr = dinodePtr[i].addrs[NDIRECT];
    if(indirAddr == 0)
      continue;
    for(int j = ROOTINO; j < sbPtr->ninodes; j++){
      if(dinodePtr[j].type == 0 || j == i)
        continue;
      uint indirAddr2 = dinodePtr[j].addrs[NDIRECT];
      if(indirAddr2 == 0)
        continue;
      if(indirAddr2 == indirAddr){
        fprintf(stderr, "ERROR: indirect address used more than once\n");
        exit(1);
      }
    }
  }//end of for loop: case 7

  exit(0);
}//end of main
