
#include <stdio.h>
#include <stdlib.h>
#include "movie.h"
Movie *movies = NULL;
int count = 0;
int capacity = 0;

void load_from_file() {
    FILE *fp = fopen("movies.dat", "rb");
    if (!fp) {
        
        return;
    }
    int stored_count;
    if (fread(&stored_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return;
    }

    movies = (Movie*)malloc(stored_count * sizeof(Movie));
    if (!movies) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        exit(1);
    }
    capacity = stored_count;
    count = stored_count;

    
    fread(movies, sizeof(Movie), count, fp);
    fclose(fp);
}

void save_to_file() {
    FILE *fp = fopen("movies.dat", "wb");
    if (!fp) {
        printf("Error: Cannot open file for writing!\n");
        return;
    }

    
    fwrite(&count, sizeof(int), 1, fp);
    
    fwrite(movies, sizeof(Movie), count, fp);
    fclose(fp);
}