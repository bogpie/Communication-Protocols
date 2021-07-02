#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <string.h> 

void fatal(char * errorMsg)
{
    perror(errorMsg);
    exit(0);
}

int main (int argc, char* argv[])
{
    printf("test cu doua fisiere : cat input input => :\n\n" );
    if(argc <= 1) 
    {
        fatal("Not enough arguments");
    }
 
    for(int i = 1 ; i < argc ; ++i)
    {
        int input = open(argv[i],O_RDONLY,S_IRUSR);

        if (input < 0)
        {
            perror("Cannot open");
        }
        int start =  lseek(input,(size_t)0,SEEK_SET);
        int end =  lseek(input,(size_t)0,SEEK_END);
        lseek(input,(size_t)0,SEEK_SET);
        int size = end - start;
        char* buf = malloc (size);

        if(buf!=NULL)
        {
            memcpy(buf,"\0",sizeof(buf));
        }
        else fatal("Malloc error");
        
        fflush(stdout);
    
        int noBytes;
        noBytes = read (input,buf,size);
        if(noBytes<0)
        {
            fatal("Read error");
        }
        printf("%s",buf);
 

        close(input);
    }

}