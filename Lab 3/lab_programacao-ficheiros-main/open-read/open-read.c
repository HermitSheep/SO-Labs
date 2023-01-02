/*
 * open-read.c
 *
 * Simple example of opening and reading to a file.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
   /*
    *
    * the attributes are:
    * - O_RDONLY: open the file for reading
    *
    */
   FILE* fd = fopen("test.txt", "r");  // "r" read
   if (fd < 0){
      fprintf(stderr, "open error: %s\n", strerror(errno));
      return -1;
   }

   char buffer[128];
   memset(buffer,0,sizeof(buffer));

   /* read the contents of the file */
   int bytes_read = fread(&buffer, sizeof(char), 20, fd); // &buffer primeiro bit do buffer - sizeof(char) tamanho de cada casa - 20 nº de casas a ser lidas
   if (bytes_read < 0){
      fprintf(stderr, "read error: %s\n", strerror(errno));
      return -1;
   }

   printf("%s",buffer);

   /* close the file */
   fclose(fd);

   return 0;
}
