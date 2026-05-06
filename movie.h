#ifndef MOVIE_H
#define MOVIE_H

#define MAX_TITLE 100
#define MAX_GENRE 50
#define MAX_PATH 200

typedef struct {
    char title[MAX_TITLE];
    char genre[MAX_GENRE];
    int year;
    float rating;
    char filepath[MAX_PATH];
} Movie;

extern Movie *movies;
extern int count;
extern int capacity;

/* File handling */
void load_from_file(void);
void save_to_file(void);

/* Core operations – now use parameters instead of console I/O */
int add_movie_core(const char *title, const char *genre, int year, float rating, const char *filepath);
void view_all_movies_core(void (*display_func)(const Movie *m, int index));
int search_movies_by_title_core(const char *keyword, void (*found_func)(const Movie *m, int index));
int search_movies_by_genre_core(const char *keyword, void (*found_func)(const Movie *m, int index));
int search_movies_by_year_core(int year, void (*found_func)(const Movie *m, int index));
int search_movies_by_rating_core(float min_rating, float max_rating, void (*found_func)(const Movie *m, int index));
int update_movie_core(int id, const char *new_title, const char *new_genre, int new_year, float new_rating, const char *new_filepath);
int delete_movie_core(int id);

/* Utility functions (unchanged) */
void clear_input_buffer(void);
void to_lowercase(char *str);
void print_movie_table_header(void);
void print_movie(const Movie *m, int index);

#endif