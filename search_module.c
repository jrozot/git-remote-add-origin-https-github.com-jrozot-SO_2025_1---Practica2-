#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

#include "hash_module.h"

#define PORT 3550
#define BACKLOG 8

bool running = true;
FILE *code_hash;
FILE *csv;
FILE *main_index;

int serverfd, clientfd;
struct sockaddr_in server, client;
socklen_t lenclient;
char buffer[100];

void parse_product_line(const char *line, Product *p);

void cleanup_and_exit(int sig);

void display_product(const Product *p);

int main(void) {
    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    csv = fopen("Data/products.csv", "r");
    if (!csv)
    {
        perror("Error opening products.csv file");
        return 1;
    }

    // If main index is missing, attempt to write.
    main_index = fopen("Data/main_index.dat", "rb");
    if (!main_index) {
        perror("Error opening Data/main_index.dat file");
        write_main_index(csv);
        main_index = fopen("Data/main_index.dat", "rb");
    }
    
    // If hash table is missing, attempt to write.
    code_hash = fopen("Data/code_hash.dat","rb");
    if (!code_hash) {
        perror("Hash table missing");
        write_code_hashtable(csv);
        code_hash = fopen("Data/code_hash.dat","rb");
    }

    printf("Socket option SO_REUSEADDR set successfully.\n");

    serverfd = socket(AF_INET,SOCK_STREAM, 0);
    if ( serverfd == -1 ) perror("error: no descriptor obtained");

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero,8);

    int opt = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        return 1;
    }

    int err = bind( serverfd, (struct sockaddr*) &server, \
    sizeof(struct sockaddr_in) );
    if (err == -1 ) perror("error: failed to bind");

    long input, offset;
    
    while(1){

        printf("Listening...\n");
        err = listen( serverfd, BACKLOG);
        if ( err == -1 ) perror("error: failed to listen");

        lenclient = sizeof(struct sockaddr_in);
        clientfd = accept( serverfd, (struct sockaddr*) &client, &lenclient );
        if (clientfd < 0) { perror("accept"); continue; }

        while (running) {
            char *line = NULL;
            size_t len = 0;
            Product product;

            product.line_number = -1;

            ssize_t n = recv( clientfd, &input, sizeof(long), 0 );
            if (n <= 0) break;

            printf("search_module received: %ld\n", input);

            product.line_number = file_search(code_hash, input);

            if (product.line_number != -1){
                printf("Found at line: %ld\n", product.line_number);

                // Seek to the position of the offset in the index file
                if (fseek(main_index, product.line_number * sizeof(long int), SEEK_SET) != 0) {
                    perror("fseek failed on index file");
                    fclose(main_index);
                    return 1;
                }

                // Read the offset for the requested line
                if (fread(&offset, sizeof(long int), 1, main_index) != 1) {
                    fprintf(stderr, "Error reading offset for line %ld\n", product.line_number);
                    return 1;
                }

                // Open the CSV and seek to the offset
                if (!csv) {
                    perror("Error opening CSV file");
                    return 1;
                }

                // Seek to the offset in the csv file to read the line
                if (fseek(csv, offset, SEEK_SET) != 0) {
                    perror("fseek failed on CSV file");
                    fclose(csv);
                    return 1;
                }

                // Read and parse the line
                if (getline(&line, &len, csv) != -1) {
                    parse_product_line(line, &product);
                    printf("Product line succesfully parsed.\n");
                } else {
                    fprintf(stderr, "Failed to read line at offset %ld\n", offset);
                }
            }else{
                printf("Not found\n");
            }
            
            printf("Sending message to client...\n");
            display_product(&product);
            err = send( clientfd, &product , sizeof(Product), 0);
            if ( err == -1 ) perror("error: failed not send");

            free(line);
        }
    }
    cleanup_and_exit(0);
    return 0;
}

void parse_product_line(const char *line, Product *p) {
    extract_field(line, product_name, p->product_name);
    extract_field(line, brands, p->brands);
    extract_field(line, quantity, p->quantity);
    extract_field(line, main_category, p->main_category);
    extract_field(line, packaging, p->packaging);
    extract_field(line, countries_en, p->countries_en);
    extract_field(line, origins_en, p->origins_en);

    extract_field(line, nutriscore_grade, p->nutriscore_grade);
    extract_field(line, nova_group, p->nova_group);
    extract_field(line, additives_n, p->additives_n);
    extract_field(line, additives_tags, p->additives_tags);
    extract_field(line, allergens, p->allergens);
    extract_field(line, traces, p->traces);
    extract_field(line, environmental_score_grade, p->environmental_score_grade);

    extract_field(line, energy_kcal_100g, p->energy_kcal_100g);
    extract_field(line, fat_100g, p->fat_100g);
    extract_field(line, saturated_fat_100g, p->saturated_fat_100g);
    extract_field(line, sugars_100g, p->sugars_100g);
    extract_field(line, fiber_100g, p->fiber_100g);
    extract_field(line, proteins_100g, p->proteins_100g);
    extract_field(line, salt_100g, p->salt_100g);
}

void display_product(const Product *p) {
    printf("\n--- Product Info ---\n");
    printf("Name: %s\n", p->product_name);
    printf("Brand: %s\n", p->brands);
    printf("Quantity: %s\n", p->quantity);
    printf("Category: %s\n", p->main_category);
    printf("Packaging: %s\n", p->packaging);
    printf("Sold In: %s\n", p->countries_en);
    printf("Origin: %s\n", p->origins_en);
    printf("Nutri-Score: %s\n", p->nutriscore_grade);
    printf("NOVA Group: %s\n", p->nova_group);
    printf("Additives (n): %s\n", p->additives_n);
    printf("Additives: %s\n", p->additives_tags);
    printf("Allergens: %s\n", p->allergens);
    printf("Traces: %s\n", p->traces);
    printf("Eco Score: %s\n", p->environmental_score_grade);
    printf("Energy: %s kcal\n", p->energy_kcal_100g);
    printf("Fat: %s g\n", p->fat_100g);
    printf("Saturated Fat: %s g\n", p->saturated_fat_100g);
    printf("Sugars: %s g\n", p->sugars_100g);
    printf("Fiber: %s g\n", p->fiber_100g);
    printf("Proteins: %s g\n", p->proteins_100g);
    printf("Salt: %s g\n", p->salt_100g);
}

void cleanup_and_exit(int sig) {
    printf("\nReceived SIGINT. Cleaning up...\n");
    close(serverfd);
    fclose(code_hash);
    fclose(csv);
    fclose(main_index);
    exit(0);
}
