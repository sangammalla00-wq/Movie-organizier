#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "movie.h"

/* Add movie – receives all data from caller */
int add_movie_core(const char *title, const char *genre, int year, float rating, const char *filepath) {
    if (count >= capacity) {
        capacity = (capacity == 0) ? 5 : capacity * 2;
        movies = (Movie*)realloc(movies, capacity * sizeof(Movie));
        if (!movies) return 0;  // failure
    }

    Movie *new = &movies[count];
    strncpy(new->title, title, MAX_TITLE - 1);
    new->title[MAX_TITLE - 1] = '\0';
    strncpy(new->genre, genre, MAX_GENRE - 1);
    new->genre[MAX_GENRE - 1] = '\0';
    new->year = year;
    new->rating = rating;
    strncpy(new->filepath, filepath, MAX_PATH - 1);
    new->filepath[MAX_PATH - 1] = '\0';

    count++;
    return 1;  // success
}

/* View all – calls display_func for each movie */
void view_all_movies_core(void (*display_func)(const Movie *m, int index)) {
    for (int i = 0; i < count; i++) {
        display_func(&movies[i], i);
    }
}

/* Search by title – returns number of matches, calls found_func for each */
int search_movies_by_title_core(const char *keyword, void (*found_func)(const Movie *m, int index)) {
    int found = 0;
    char key_lower[MAX_TITLE];
    strncpy(key_lower, keyword, MAX_TITLE - 1);
    key_lower[MAX_TITLE - 1] = '\0';
    to_lowercase(key_lower);

    for (int i = 0; i < count; i++) {
        char title_lower[MAX_TITLE];
        strcpy(title_lower, movies[i].title);
        to_lowercase(title_lower);
        if (strstr(title_lower, key_lower)) {
            found_func(&movies[i], i);
            found++;
        }
    }
    return found;
}

int search_movies_by_genre_core(const char *keyword, void (*found_func)(const Movie *m, int index)) {
    int found = 0;
    char key_lower[MAX_GENRE];
    strncpy(key_lower, keyword, MAX_GENRE - 1);
    key_lower[MAX_GENRE - 1] = '\0';
    to_lowercase(key_lower);

    for (int i = 0; i < count; i++) {
        char genre_lower[MAX_GENRE];
        strcpy(genre_lower, movies[i].genre);
        to_lowercase(genre_lower);
        if (strstr(genre_lower, key_lower)) {
            found_func(&movies[i], i);
            found++;
        }
    }
    return found;
}

int search_movies_by_year_core(int year, void (*found_func)(const Movie *m, int index)) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (movies[i].year == year) {
            found_func(&movies[i], i);
            found++;
        }
    }
    return found;
}

int search_movies_by_rating_core(float min_rating, float max_rating, void (*found_func)(const Movie *m, int index)) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (movies[i].rating >= min_rating && movies[i].rating <= max_rating) {
            found_func(&movies[i], i);
            found++;
        }
    }
    return found;
}

/* Update movie – only updates fields that are not NULL */
int update_movie_core(int id, const char *new_title, const char *new_genre, int new_year, float new_rating, const char *new_filepath) {
    if (id < 1 || id > count) return 0;
    Movie *m = &movies[id - 1];

    if (new_title != NULL && strlen(new_title) > 0)
        strncpy(m->title, new_title, MAX_TITLE - 1);
    if (new_genre != NULL && strlen(new_genre) > 0)
        strncpy(m->genre, new_genre, MAX_GENRE - 1);
    if (new_year > 0)
        m->year = new_year;
    if (new_rating >= 0.0 && new_rating <= 10.0)
        m->rating = new_rating;
    if (new_filepath != NULL && strlen(new_filepath) > 0)
        strncpy(m->filepath, new_filepath, MAX_PATH - 1);

    return 1;
}

/* Delete movie */
int delete_movie_core(int id) {
    if (id < 1 || id > count) return 0;
    for (int i = id - 1; i < count - 1; i++) {
        movies[i] = movies[i + 1];
    }
    count--;
    return 1;
}