#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#define BUFFERSIZE 128
int main (int argc, char **argv) {
    
   int fd;
    char rule[BUFFERSIZE];
   char* file_name = "/proc/firewallExtension";
   fd = open( file_name , O_RDWR);
    if (fd == -1) {
        perror("Cannot open kenerl module.\n");
      exit (1);
    }
  if(argc==2&&argv[1][0]=='L'){
        int res;
        res = read(fd,rule, BUFFERSIZE);
        if (res <= 0) {
            fprintf(stderr, "Reading failed, result = %d\n", res);
            exit (1);
        }
        close (fd);
        return 0;
    }else if(argc==3&&argv[1][0]=='W'){
        char * buffer = NULL;
        int rres;
        int write_error;
        size_t n = 0;
        FILE *rule_fd  ; 
        rule_fd = fopen(argv[2],"r");
        rres =  getline(&buffer,&n,rule_fd);
        write(fd,"newRule",10);
        while(rres!=-1){
            
            printf("%s %zu\n",buffer,n );
            write_error = write(fd,buffer,n);
            if(write_error==-1){
                printf("write to kernel module error\n");
                return -1;
            }
            buffer=NULL;
            rres =  getline(&buffer,&n,rule_fd);
        }
        close (fd);
        return 0;
    }else{
        printf("arg count = %d is not correct \n",argc);
        close (fd);
        return -1;
    }
 /*   if(argv[1]=='L'){
        printf("%s\n", );
    }*/


    return 0;
    //printf ("The counters are %d and %d\n", counters[0], counters[1]);
}
    