#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>
#include "hash_module.h"

#define QUERY_PIPE "query_pipe"
#define SEARCH_PIPE "search_pipe"

int query_fd = -1;
int search_fd = -1;
bool running = true;
FILE *code_hash;
FILE *csv;

void cleanup_and_exit(int sig) {
    printf("\nReceived SIGINT. Cleaning up...\n");
    if (query_fd != -1) close(query_fd);
    if (search_fd != -1) close(search_fd);
    unlink(QUERY_PIPE);
    unlink(SEARCH_PIPE);
    fclose(code_hash);
    exit(0);
}

int main() {
    signal(SIGINT, cleanup_and_exit);

    // Create FIFOs
    mkfifo(QUERY_PIPE, 0666);
    mkfifo(SEARCH_PIPE, 0666);

    printf("search_module: Waiting for connections...\n");

    // Open FIFOs
    query_fd = open(QUERY_PIPE, O_RDONLY);
    search_fd = open(SEARCH_PIPE, O_WRONLY);

    csv = fopen("Data/products.csv", "r");
    if (!csv)
    {
        perror("Error opening products.csv file");
        return 1;
    }

    // If hash table is missing, attempt to write.
    code_hash = fopen("Data/code_hash.dat","rb");
    if (!code_hash) {
        perror("Hash table missing");
        write_code_hashtable(csv);
        code_hash = fopen("Data/code_hash.dat","rb");
    }

    fclose(csv);

    long input, result;

    while (running) {
        result = -1;

        ssize_t bytes_read = read(query_fd, &input, sizeof(long));
        if (bytes_read <= 0) continue;

        printf("search_module received: %ld\n", input);

        result = file_search(code_hash, input);
        if (result != -1){
            printf("Found at line: %ld\n", result);
        }else{
            printf("Not found\n");
        }

        // Echo back the same number
        write(search_fd, &result, sizeof(long));
    }

    cleanup_and_exit(0);
    return 0;
}
