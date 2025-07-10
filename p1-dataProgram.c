#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "hash_module.h"

#define QUERY_PIPE "query_pipe"
#define SEARCH_PIPE "search_pipe"

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

FILE *main_index, *csv;

void print_welcome_message();

void print_prompt_barcode();

void display_product_info(const char *line);

int main(void) {
    int query_fd = open(QUERY_PIPE, O_WRONLY);
    int search_fd = open(SEARCH_PIPE, O_RDONLY);

    if (query_fd == -1 || search_fd == -1) {
        perror("Failed to open FIFOs");
        return 1;
    }

    csv = fopen("Data/products.csv", "r");
    if (!csv){
        perror("Error opening products.csv file");
        return 1;
    }

    main_index = fopen("Data/main_index.dat", "rb");
    if (!main_index) {
        perror("Error opening Data/main_index.dat file");
        write_main_index(csv);
        main_index = fopen("Data/main_index.dat", "rb");
    }

    char buffer[32];
    long value;

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
        write(query_fd, &value, sizeof(long));

        // Wait for response
        long line_number, offset;
        ssize_t bytes_read = read(search_fd, &line_number, sizeof(long));
        if (bytes_read > 0) 
            printf("user_interface received: %ld\n", line_number);

        if (line_number == -1) {
            printf("Code %13ld not found in data file.\n", value);
        }else{    
            // Seek to the position of the offset in the index file
            if (fseek(main_index, line_number * sizeof(long int), SEEK_SET) != 0) {
                perror("fseek failed on index file");
                fclose(main_index);
                return 1;
            }

            // Read the offset for the requested line
            if (fread(&offset, sizeof(long int), 1, main_index) != 1) {
                fprintf(stderr, "Error reading offset for line %ld\n", line_number);
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

            // Read and print the line
            char *line = NULL;
            size_t len = 0;
            if (getline(&line, &len, csv) != -1) {
                display_product_info(line);
            } else {
                fprintf(stderr, "Failed to read line at offset %ld\n", offset);
            }
            free(line);
        }
    }

    close(query_fd);
    close(search_fd);

    fclose(main_index);
    fclose(csv);
    return 0;
}

void display_product_info(const char *line) {
    char field[512];

    // Clear screen
    printf("\033[2J\033[H");

    //
    // $ Product Overview
    //
    printf(COLOR_BOLD COLOR_CYAN "┏━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃ $ Product Overview   ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━┛\n" COLOR_RESET);

    extract_field(line, product_name, field);
    printf(COLOR_BOLD "  Name:          " COLOR_RESET "%s\n", field);

    extract_field(line, brands, field);
    printf(COLOR_BOLD "  Brand:         " COLOR_RESET "%s\n", field);

    extract_field(line, quantity, field);
    printf(COLOR_BOLD "  Quantity:      " COLOR_RESET "%s\n", field);

    extract_field(line, main_category, field);
    printf(COLOR_BOLD "  Category:      " COLOR_RESET "%s\n", field);

    extract_field(line, packaging, field);
    printf(COLOR_BOLD "  Packaging:     " COLOR_RESET "%s\n", field);

    extract_field(line, countries_en, field);
    printf(COLOR_BOLD "  Sold In:       " COLOR_RESET "%s\n", field);

    extract_field(line, origins_en, field);
    printf(COLOR_BOLD "  Origin:        " COLOR_RESET "%s\n", field);

    //
    // # Health Summary
    //
    printf("\n" COLOR_BOLD COLOR_GREEN "┏━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃ # Health Summary     ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━┛\n" COLOR_RESET);

    extract_field(line, nutriscore_grade, field);
    printf(COLOR_BOLD "  Nutri-Score:   " COLOR_RESET "%s\n", field);

    extract_field(line, nova_group, field);
    printf(COLOR_BOLD "  NOVA Group:    " COLOR_RESET "%s\n", field);

    extract_field(line, additives_n, field);
    printf(COLOR_BOLD "  Additives (n): " COLOR_RESET "%s\n", field);

    extract_field(line, additives_tags, field);
    printf(COLOR_BOLD "  Additives:     " COLOR_RESET "%s\n", field);

    extract_field(line, allergens, field);
    printf(COLOR_BOLD "  Allergens:     " COLOR_RESET "%s\n", field);

    extract_field(line, traces, field);
    printf(COLOR_BOLD "  Traces:        " COLOR_RESET "%s\n", field);

    extract_field(line, environmental_score_grade, field);
    printf(COLOR_BOLD "  Eco Score:     " COLOR_RESET "%s\n", field);

    //
    // % Nutritional Facts
    //
    printf("\n" COLOR_BOLD COLOR_YELLOW "┏━━━━━━━━━━━━━━━━━━━━━━┓\n");
    printf("┃ % Nutritional Facts  ┃\n");
    printf("┗━━━━━━━━━━━━━━━━━━━━━━┛\n" COLOR_RESET);

    printf(COLOR_BOLD "  Nutrient            per 100g\n" COLOR_RESET);
    printf("  ---------------------------------\n");

    extract_field(line, energy_kcal_100g, field);
    printf("  Energy (kcal):      %s\n", field);

    extract_field(line, fat_100g, field);
    printf("  Fat:                %s g\n", field);

    extract_field(line, saturated_fat_100g, field);
    printf("  Saturated Fat:      %s g\n", field);

    extract_field(line, sugars_100g, field);
    printf("  Sugars:             %s g\n", field);

    extract_field(line, fiber_100g, field);
    printf("  Fiber:              %s g\n", field);

    extract_field(line, proteins_100g, field);
    printf("  Protein:            %s g\n", field);

    extract_field(line, salt_100g, field);
    printf("  Salt:               %s g\n", field);

    printf(COLOR_GRAY "\n(Press Enter to continue...)" COLOR_RESET);
    getchar(); // pause until user hits Enter
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
