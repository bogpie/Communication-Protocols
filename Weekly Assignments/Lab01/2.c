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
 
    for(int i = argc-1 ; i >=1 ; --i)
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


        int lineCount = 0;
        
        // parcurgem in ordine inversa sirul, caracter cu caracter

         int index = size-1;

 
        while(index>=0)
        {
            //parcurgem cate o linie, calculandu-i lungimea
            int lineCount=0;
 
            while(buf[index]!=10 && index>=0)
            {
                ++lineCount;
                --index;
            }
            --lineCount;
 
            buf[index+1+lineCount+1] = '\0';

            printf("%s",buf + index + 1);
            if(lineCount!=-1)
            {
                printf("\n");
            }
            --index;
        }
    


        close(input);
    }

}