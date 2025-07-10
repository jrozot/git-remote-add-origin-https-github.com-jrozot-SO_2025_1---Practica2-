#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include "hash_module.h"

void extract_field(const char *line, int field_index, char *field) {
    const char *start = line;
    const char *end;
    int current_index = 0;

    // Traverse to the desired field
    while (current_index < field_index && *start != '\0') {
        if ((start = strchr(start, '\t')) == NULL) {
            field[0] = '\0'; // Field not found
            return;
        }
        start++; // Move past the tab
        current_index++;
    }

    // Find end of field
    end = strchr(start, '\t');
    if (!end) {
        // Last field or no tab at all
        strncpy(field, start, 255);
        field[255] = '\0';
    } else {
        size_t len = end - start;
        if (len > 255) len = 255;
        strncpy(field, start, len);
        field[len] = '\0';
    }
}

void write_main_index(FILE* csv)
{
    perror("Error opening index file");
    printf("Writing main index...\n");
    FILE *index = fopen("Data/main_index.dat", "wb");

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    long int offset;

    while ((read = getline(&line, &len, csv)) != -1) {
        // Get current offset at start of the line
        offset = ftell(csv) - read;
        // Write offset to index file
        fwrite(&offset, sizeof(long int), 1, index);
    }

    free(line);
    fclose(csv);
    fclose(index);

    printf("Indexing complete.\n");
}

// Simple hash function using modulo
unsigned int hash(long code) {
    return (unsigned int)(code % TABLE_SIZE);
}

// Create a hash table
HashTable* create_table() {
    HashTable* table = malloc(sizeof(HashTable));
    table->buckets = calloc(TABLE_SIZE, sizeof(Node*));
    return table;
}

// Insert (EAN code, line number) into the hash table
void insert(HashTable* table, long ean_code, long line_number) {
    unsigned int idx = hash(ean_code);
    Node* new_node = malloc(sizeof(Node));
    new_node->ean_code = ean_code;
    new_node->line_number = line_number;
    new_node->next = table->buckets[idx];
    table->buckets[idx] = new_node;
}

// Free memory
void free_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Node* current = table->buckets[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}

void save_table_to_file(HashTable* table, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    long* bucket_offsets = calloc(TABLE_SIZE, sizeof(long));  // initially 0
    fseek(file, TABLE_SIZE * sizeof(long), SEEK_SET);         // reserve space for bucket offsets

    // Save nodes and record offsets
    for (int i = 0; i < TABLE_SIZE; ++i) {
        Node* current = table->buckets[i];
        long last_offset = 0;
        while (current != NULL) {
            FileNode fnode;
            fnode.ean_code = current->ean_code;
            fnode.line_number = current->line_number;
            fnode.next_offset = 0;

            long current_offset = ftell(file);
            if (last_offset != 0) {
                // update previous node's next_offset
                fseek(file, last_offset + 2 * sizeof(long), SEEK_SET);
                fwrite(&current_offset, sizeof(long), 1, file);
                fseek(file, 0, SEEK_END);  // go back to end for writing
            } else {
                bucket_offsets[i] = current_offset;
            }

            fwrite(&fnode, sizeof(FileNode), 1, file);

            last_offset = current_offset;
            current = current->next;
        }
    }

    // Write the bucket offset table at the beginning
    fseek(file, 0, SEEK_SET);
    fwrite(bucket_offsets, sizeof(long), TABLE_SIZE, file);

    free(bucket_offsets);
    fclose(file);
}

long file_search(FILE* file, long ean_code)
{
    unsigned int idx = hash(ean_code);
    fseek(file, idx * sizeof(long), SEEK_SET);

    long offset;
    fread(&offset, sizeof(long), 1, file);

    while (offset != 0) {
        FileNode node;
        fseek(file, offset, SEEK_SET);
        fread(&node, sizeof(FileNode), 1, file);

        if (node.ean_code == ean_code) {
            return node.line_number;
        }

        offset = node.next_offset;
    }

    return -1;  // Not found
}

void write_code_hashtable(FILE* csv)
{
    printf("Data/code_hash.dat not found\n");

    printf("Creating hash table...\n");

    HashTable* table = create_table();

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    long int line_number = 0;

    while((read = getline(&line, &len, csv)) != -1){
        char field_arr[256];
        extract_field(line, code, field_arr);
        insert(table, atol(field_arr), line_number);
        line_number ++;
    }

    save_table_to_file(table, "Data/code_hash.dat");
    printf("Hash table succesfuly stored.\n");
}
