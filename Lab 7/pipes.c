#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//* -> 5 with only one pipe. (it might've worked, but it shouldn't because each end of the pipe
//should be
//* associated with the opposite type of stream, stdin/stdout????)

#define BUFFER_SIZE (128)

// Helper function to send messages
// Retries to send whatever was not sent in the beginning
void send_msg(int tx, char const *str) {
	size_t len = strlen(str);
	size_t written = 0;

	while (written < len) {
		ssize_t ret = write(tx, str + written, len - written);
		if (ret < 0) {
			fprintf(stderr, "[ERR]: write failed: %s, content: %s\n", strerror(errno), str);
			exit(EXIT_FAILURE);
		}

		written += ret;
	}
}

int child_main(int rx, int tx) {
	while (true) {
		char buffer[BUFFER_SIZE];
		ssize_t ret = read(rx, buffer, BUFFER_SIZE - 1);

		if (ret == 0) {
			// ret == 0 signals EOF
			fprintf(stderr, "[INFO]: parent closed the pipe\n");
            close(tx);
			break;
		} else if (ret == -1) {
			// ret == -1 signals error
			fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "[INFO]: parent sent %zd B\n", ret);
		//*write(rx, "1", 2);
		buffer[ret] = 0;
		fputs(buffer, stdout);
		send_msg(tx, "akw");
	}

    close(tx);
	close(rx);
	return 0;
}

int parent_main(int tx, int rx) {
	// The parent likes classic rock:
	// https://www.youtube.com/watch?v=btPJPFnesV4
	size_t ret;
	char buffer[BUFFER_SIZE];

	send_msg(tx, "It's the eye of the tiger\n");
    ret = read(rx, buffer, BUFFER_SIZE - 1);
    buffer[ret] = 0;
    fputs(buffer, stdout);
    
	send_msg(tx, "It's the thrill of the fight\n");
    ret = read(rx, buffer, BUFFER_SIZE - 1);
    buffer[ret] = 0;
    fputs(buffer, stdout);

	send_msg(tx, "Rising up to the challenge of our rival\n");
	ret = read(rx, buffer, BUFFER_SIZE - 1);
    buffer[ret] = 0;
    fputs(buffer, stdout);

	fprintf(stderr, "[INFO]: closing pipe\n");
    close(rx);
	close(tx);

	// Parent waits for the child
	wait(NULL);
	return 0;
}

int main() {
	int filedes[2];
	int pipes[2];
	if (pipe(filedes) != 0) {
		fprintf(stderr, "[ERR]: pipe() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

    if (pipe(pipes) != 0) {
		fprintf(stderr, "[ERR]: pipe() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fork() == 0) {
		// We need to close the ends we are not using
		// Otherwise, the child will be perpetually
		// Waiting for a message that will never come
		close(filedes[1]);
		close(pipes[0]);
		return child_main(filedes[0], pipes[1]);
	} else {
		close(filedes[0]);
		close(pipes[1]);
		return parent_main(filedes[1], pipes[0]);
	}
}
