#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO1 "fifo1"
#define FIFO2 "fifo2"
#define MAX_BUF 1024

int main() {
    char sentence[MAX_BUF];

    // Create two FIFOs (named pipes)
    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    int fd1, fd2;

    if (fork()) {
        // Parent Process (Process1)
        fd1 = open(FIFO1, O_WRONLY);
        fd2 = open(FIFO2, O_RDONLY);

        while (1) {
            printf("Enter a sentence: ");
            fgets(sentence, MAX_BUF, stdin);

            // Write the sentence to the first pipe (FIFO1)
            write(fd1, sentence, strlen(sentence) + 1);

            // Read the result from the second pipe (FIFO2)
            read(fd2, sentence, MAX_BUF);
            printf("Received from Process2: %s", sentence);
        }

        // Close and unlink the FIFOs
        close(fd1);
        close(fd2);
        unlink(FIFO1);
        unlink(FIFO2);
    } else {
        // Child Process (Process2)
        fd1 = open(FIFO1, O_RDONLY);
        fd2 = open(FIFO2, O_WRONLY);

        while (1) {
            // Read the sentence from the first pipe (FIFO1)
            read(fd1, sentence, MAX_BUF);

            // Process the sentence (count characters, words, and lines)
            int char_count = 0, word_count = 0, line_count = 0;
            char* ptr = sentence;

            while (*ptr) {
                if (*ptr != ' ' && *ptr != '\n') {
                    char_count++;
                    if (*(ptr + 1) == ' ' || *(ptr + 1) == '\n')
                        word_count++;
                }

                if (*ptr == '\n')
                    line_count++;

                ptr++;
            }

            // Prepare the output
            snprintf(sentence, MAX_BUF, "Characters: %d, Words: %d, Lines: %d\n", char_count, word_count, line_count);

            // Write the result to the second pipe (FIFO2)
            write(fd2, sentence, strlen(sentence) + 1);
        }

        // Close the pipes
        close(fd1);
        close(fd2);
    }

    return 0;
}
