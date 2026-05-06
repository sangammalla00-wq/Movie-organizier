/**
 * Utility functions for Movie Organizing System
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "movie.h"

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void to_lowercase(char *str) {
    for (; *str; ++str) *str = tolower(*str);
}

void print_movie_table_header() {
    printf("\n%-4s | %-30s | %-15s | %-6s | %-5s | %s\n",
           "ID", "Title", "Genre", "Year", "Rating", "File Path");
    printf("-----+--------------------------------+-----------------+--------+-------+------------------------------------\n");
}

void print_movie(const Movie *m, int index) {
    printf("%-4d | %-30s | %-15s | %-6d | %-5.1f | %s\n",
           index + 1, m->title, m->genre, m->year, m->rating, m->filepath);
}