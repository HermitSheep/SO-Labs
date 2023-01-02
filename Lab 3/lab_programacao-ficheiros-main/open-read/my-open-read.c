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


int writea(char buffer[]) {
  FILE *fd = fopen("test_res.txt", "w+"); // 0600 é as permições
  if (fd < 0) {
    fprintf(stderr, "open error: %s\n", strerror(errno));
    return -1;
  }

  /* write a string to the file */
  int bytes_written = fwrite(&buffer, sizeof(char), 13, fd);
  if (bytes_written < 0) {
    fprintf(stderr, "write error: %s\n", strerror(errno));
    return -1;
  }

  /* close the file */
  fclose(fd);

  return 0;
}


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
    int bytes_read = 0;
    int read = 0;
    do {
        //i++;
        //printf("bytes read: %d, cycle: %d\n", read, i);
        bytes_read = fread(&buffer, sizeof(char), sizeof(buffer), fd); // fread devolve o numero de bits lidos
        if (bytes_read < 0){
            fprintf(stderr, "read error: %s\n", strerror(errno));
            return -1;
        }
        writea(buffer);
        read += bytes_read;
        memset(buffer, 0, sizeof(buffer));  //otherwise, on the last write, some residuals in the buffer still get printed. This could be solved by only printing the amount of characters that was read, but in this context it's overly complicated.
    }
    while (bytes_read > 0);

    printf("bytes read: %d\n", read);

   /* close the file */
   fclose(fd);

   return 0;
}

