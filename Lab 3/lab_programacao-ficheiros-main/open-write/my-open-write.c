/*
 * open-write.c
 *
 * Simple example of opening and writing to a file.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(char *argv[]) {
  /*
   * the attributes are:
   * - O_CREAT: create the file if it does not exist
   * - O_TRUNC: erase the contents of the file
   * - O_WRONLY: open the file for writting
   *
   * the permissions are applied if the file is created:
   * - S_IRUSR: reading, user
   * - S_IWUSR: writting, user
   *    FILE* fd = fopen("test.txt", O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR |
   S_IWUSR);

   */
  FILE *fd = fopen("test.txt", "w+"); // 0600 é as permições
  if (fd < 0) {
    fprintf(stderr, "open error: %s\n", strerror(errno));
    return -1;
  }

  char buffer[] = "Hello World!\n";

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
