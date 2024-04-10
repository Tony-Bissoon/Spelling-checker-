#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <linux/limits.h>

#define MAX_WORD_LEN 100
#define DICT_INIT_SIZE 100000

typedef struct {
    char word[MAX_WORD_LEN];
    int line;
    int col;
} Word;

char **dict;
int dict_size = 0;
int dict_capacity = DICT_INIT_SIZE;

void load_dictionary(const char *dict_file);
int binary_search(const char *word);
void process_file(const char *file_path);
void extract_words(const char *buffer, size_t buffer_size, const char *file_path);
int is_valid_word_char(char c);
void traverse_directory(const char *dir_path);
int check_spelling(Word words[], int num_words, const char *file_path);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dictionary> <file/dir> [file/dir ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    load_dictionary(argv[1]);

    for (int i = 2; i < argc; i++) {
        struct stat st;
        if (stat(argv[i], &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                traverse_directory(argv[i]);
            } else {
                process_file(argv[i]);
            }
        } else {
            fprintf(stderr, "Error: Failed to open %s\n", argv[i]);
        }
    }

    for (int i = 0; i < dict_size; i++) {
        free(dict[i]);
    }
    free(dict);
    return EXIT_SUCCESS;
}

void load_dictionary(const char *dict_file) {
    FILE *fp = fopen(dict_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open dictionary file %s\n", dict_file);
        exit(EXIT_FAILURE);
    }

    dict = malloc(dict_capacity * sizeof(char *));

    char word[MAX_WORD_LEN];
    while (fscanf(fp, "%s", word) != EOF) {
        if (dict_size == dict_capacity) {
            dict_capacity *= 2;
            dict = realloc(dict, dict_capacity * sizeof(char *));
        }
        dict[dict_size] = malloc(strlen(word) + 1);
        strcpy(dict[dict_size], word);
        dict_size++;
    }

    fclose(fp);

    qsort(dict, dict_size, sizeof(char *), (int (*)(const void *, const void *))strcmp);
}

int binary_search(const char *word) {
    int left = 0, right = dict_size - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcasecmp(word, dict[mid]); // Use strcasecmp for case-insensitive comparison
        if (cmp == 0) {
            return 1;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return 0;
}

void process_file(const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open %s\n", file_path);
        return;
    }

    char buffer[BUFSIZ];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        extract_words(buffer, bytes_read, file_path);
    }

    fclose(fp);
}

void extract_words(const char *buffer, size_t buffer_size, const char *file_path) {
    Word words[BUFSIZ];
    int num_words = 0;
    int line = 1, col = 1;

    for (size_t i = 0; i < buffer_size; i++) {
        char c = buffer[i];

        if (is_valid_word_char(c)) {
            int word_start = i;
            while (i < buffer_size && is_valid_word_char(buffer[i])) {
                i++;
            }
            int word_end = i;

            strncpy(words[num_words].word, buffer + word_start, word_end - word_start);
            words[num_words].word[word_end - word_start] = '\0';

            words[num_words].line = line;
            words[num_words].col = col;
            num_words++;

            col += word_end - word_start + 1;
        } else if (c == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }
    }

    check_spelling(words, num_words, file_path);
}

int is_valid_word_char(char c) {
    return isalnum(c) || c == '\'' || c == '-';
}

void traverse_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Error: Failed to open directory %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

            struct stat st;
            if (stat(path, &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    traverse_directory(path);
                } else if (S_ISREG(st.st_mode) && strstr(entry->d_name, ".txt") != NULL) {
                    process_file(path);
                }
            } else {
                fprintf(stderr, "Error: Failed to stat %s\n", path);
            }
        }
    }

    closedir(dir);
}

int check_spelling(Word words[], int num_words, const char *file_path) {
    int errors_found = 0;

    for (int i = 0; i < num_words; i++) {
        Word word = words[i];
        int found = binary_search(word.word);

        if (!found) {
            printf("%s (%d,%d): %s\n", file_path, word.line, word.col, word.word);
            errors_found = 1;
        }
    }

    return errors_found;
}


