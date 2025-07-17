#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "hash_module.h"

#define PORT 3550 
#define BACKLOG 8

// ANSI color codes
#define COLOR_RESET     "\033[0m"
#define COLOR_BOLD      "\033[1m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_RED       "\033[31m"
#define COLOR_WHITE     "\033[37m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_GRAY      "\033[90m"

struct sockaddr_in server, client;
socklen_t lenclient;
int fd;
char buffer[100];
char ipAddress[15];

FILE *main_index, *csv;

void print_welcome_message();

void print_prompt_barcode();

void display_product_info_struct(const Product *p);

void cleanup_and_exit(int sig);

//int main(void) {
    
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_ip_address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];

    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    char buffer[32];
    long value;
    Product product;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    client.sin_addr.s_addr = inet_addr(server_ip);
    bzero(server.sin_zero, 8);

    int err = connect( fd, (struct sockaddr*) &client, sizeof(struct sockaddr) );
    if ( err == -1 ){
        perror("error: failed to connect");
        return 1;
    }

    print_welcome_message();

    while (1) {
        print_prompt_barcode();

        if (!fgets(buffer, sizeof(buffer), stdin)) break;

        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) break;

        if (strlen(buffer) == 0 || strlen(buffer) >= 14 || strspn(buffer, "0123456789") != strlen(buffer)) {
            printf("Invalid input. Please enter only digits.\n");
            continue;
        }

        value = atol(buffer);

        // Pass query to search module
        err = send( fd, &value, sizeof(long), 0 );
        if ( err == -1 ) perror("error: failed to send");

        // Wait for response
        ssize_t n = recv( fd, &product, sizeof(Product), 0);

        if (n <= 0) {
            printf("Server closed connection.\n");
            break;
        }

        if (product.line_number == -1) {
            printf("Code %13ld not found in data file.\n", value);
        }else{    
            // Read and print the line
            display_product_info_struct(&product);
        }
    }


    close(fd);
    return 0;
}

void display_product_info_struct(const Product *p) {
    printf("\033[2J\033[H"); // Clear screen

    // Product Overview
    printf(COLOR_BOLD COLOR_CYAN "┏━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃ $ Product Overview   ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━┛\n" COLOR_RESET);
    printf(COLOR_BOLD "  Name:          " COLOR_RESET "%s\n", p->product_name);
    printf(COLOR_BOLD "  Brand:         " COLOR_RESET "%s\n", p->brands);
    printf(COLOR_BOLD "  Quantity:      " COLOR_RESET "%s\n", p->quantity);
    printf(COLOR_BOLD "  Category:      " COLOR_RESET "%s\n", p->main_category);
    printf(COLOR_BOLD "  Packaging:     " COLOR_RESET "%s\n", p->packaging);
    printf(COLOR_BOLD "  Sold In:       " COLOR_RESET "%s\n", p->countries_en);
    printf(COLOR_BOLD "  Origin:        " COLOR_RESET "%s\n", p->origins_en);

    // Health Summary
    printf("\n" COLOR_BOLD COLOR_GREEN "┏━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃ # Health Summary     ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━┛\n" COLOR_RESET);
    printf(COLOR_BOLD "  Nutri-Score:   " COLOR_RESET "%s\n", p->nutriscore_grade);
    printf(COLOR_BOLD "  NOVA Group:    " COLOR_RESET "%s\n", p->nova_group);
    printf(COLOR_BOLD "  Additives (n): " COLOR_RESET "%s\n", p->additives_n);
    printf(COLOR_BOLD "  Additives:     " COLOR_RESET "%s\n", p->additives_tags);
    printf(COLOR_BOLD "  Allergens:     " COLOR_RESET "%s\n", p->allergens);
    printf(COLOR_BOLD "  Traces:        " COLOR_RESET "%s\n", p->traces);
    printf(COLOR_BOLD "  Eco Score:     " COLOR_RESET "%s\n", p->environmental_score_grade);

    // Nutritional Facts
    printf("\n" COLOR_BOLD COLOR_YELLOW "┏━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃ X Nutritional Facts  ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━┛\n" COLOR_RESET);
    printf(COLOR_BOLD "  Nutrient            per 100g\n" COLOR_RESET);
    printf("  ---------------------------------\n");
    printf("  Energy (kcal):      %s\n", p->energy_kcal_100g);
    printf("  Fat:                %s g\n", p->fat_100g);
    printf("  Saturated Fat:      %s g\n", p->saturated_fat_100g);
    printf("  Sugars:             %s g\n", p->sugars_100g);
    printf("  Fiber:              %s g\n", p->fiber_100g);
    printf("  Protein:            %s g\n", p->proteins_100g);
    printf("  Salt:               %s g\n", p->salt_100g);

    printf(COLOR_GRAY "\n(Press Enter to continue...)" COLOR_RESET);
    getchar();
}

void print_welcome_message() {
    printf("\033[2J\033[H"); // Clear screen
    printf("\n");
    printf("\033[1;36m"); // Bold Cyan
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃        Welcome        ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━┛\n");
    printf("\033[0m"); // Reset color
}

void print_prompt_barcode() {
    printf("\n");
    printf("\033[1;33m"); // Bold Yellow
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃    Please enter a barcode:   ┃\n");
    printf("┃     (EAN or type 'exit')     ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
    printf("\033[0m"); // Reset color
    printf("> ");
}

void cleanup_and_exit(int sig) {
    printf("\nReceived SIGINT. Cleaning up...\n");
    close(fd);
    exit(0);
}
