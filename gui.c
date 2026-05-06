#include <gtk/gtk.h>
#include <string.h>
#include "movie.h"

/* Forward declarations */
static void clear_listbox(GtkWidget *listbox);
static void show_message(GtkWindow *parent, const char *title, const char *msg);
static void add_movie_to_listbox(const Movie *m, int index);

/* Main window widgets */
static GtkWidget *main_window;
static GtkWidget *listbox;
static GtkWidget *search_type_combo;
static GtkWidget *search_entry;

/* Helper: display a single movie in the listbox */
static void add_movie_to_listbox(const Movie *m, int index) {
    GtkWidget *row = gtk_list_box_row_new();
    char label_text[512];
    snprintf(label_text, sizeof(label_text),
             "%d. %s (%d) – %.1f ★  [%s]  📁 %s",
             index + 1, m->title, m->year, m->rating, m->genre, m->filepath);
    GtkWidget *label = gtk_label_new(label_text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
}

/* Clear all rows from the listbox */
static void clear_listbox(GtkWidget *listbox) {
    gtk_container_foreach(GTK_CONTAINER(listbox), (GtkCallback)gtk_widget_destroy, NULL);
}

/* Show a message dialog */
static void show_message(GtkWindow *parent, const char *title, const char *msg) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "%s", msg);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* --- Search logic (called when Search button is clicked) --- */
static void on_search(GtkWidget *button, gpointer data) {
    int search_type = gtk_combo_box_get_active(GTK_COMBO_BOX(search_type_combo));
    const char *text = gtk_entry_get_text(GTK_ENTRY(search_entry));

    if (strlen(text) == 0 && search_type != 3) {  // rating range handled separately
        show_message(GTK_WINDOW(main_window), "Info", "Please enter a search term.");
        return;
    }

    clear_listbox(listbox);

    int found = 0;
    switch (search_type) {
        case 0: { // Title
            char keyword[MAX_TITLE];
            strncpy(keyword, text, MAX_TITLE - 1);
            keyword[MAX_TITLE - 1] = '\0';
            found = search_movies_by_title_core(keyword, add_movie_to_listbox);
            break;
        }
        case 1: { // Genre
            char genre[MAX_GENRE];
            strncpy(genre, text, MAX_GENRE - 1);
            genre[MAX_GENRE - 1] = '\0';
            found = search_movies_by_genre_core(genre, add_movie_to_listbox);
            break;
        }
        case 2: { // Year
            int year = atoi(text);
            found = search_movies_by_year_core(year, add_movie_to_listbox);
            break;
        }
        case 3: { // Rating range – expects "min,max"
            char *comma = strchr(text, ',');
            if (!comma) {
                show_message(GTK_WINDOW(main_window), "Invalid format", "Use min,max (e.g., 3.5,8.0)");
                return;
            }
            float min_rating = atof(text);
            float max_rating = atof(comma + 1);
            found = search_movies_by_rating_core(min_rating, max_rating, add_movie_to_listbox);
            break;
        }
    }

    if (found == 0) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new("No matching movies found.");
        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
    }
    gtk_widget_show_all(listbox);
}

/* --- Show all movies (ignores search) --- */
static void on_show_all(GtkWidget *button, gpointer data) {
    clear_listbox(listbox);
    view_all_movies_core(add_movie_to_listbox);
    /* If no movies at all, show a message */
    GList *children = gtk_container_get_children(GTK_CONTAINER(listbox));
    if (children == NULL) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new("No movies in the collection.");
        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
    } else {
        g_list_free(children);
    }
    gtk_widget_show_all(listbox);
}

/* --- Add Movie Dialog --- */
static void on_add_movie(GtkWidget *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Movie",
                                                    GTK_WINDOW(main_window),
                                                    GTK_DIALOG_MODAL,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content), grid);

    GtkWidget *title_entry = gtk_entry_new();
    GtkWidget *genre_entry = gtk_entry_new();
    GtkWidget *year_entry = gtk_entry_new();
    GtkWidget *rating_entry = gtk_entry_new();
    GtkWidget *path_entry = gtk_entry_new();

    int row = 0;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Title:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), title_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Genre:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), genre_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Year:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), year_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Rating (0-10):"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rating_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("File path:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), path_entry, 1, row, 1, 1);

    gtk_widget_show_all(content);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *title = gtk_entry_get_text(GTK_ENTRY(title_entry));
        const char *genre = gtk_entry_get_text(GTK_ENTRY(genre_entry));
        int year = atoi(gtk_entry_get_text(GTK_ENTRY(year_entry)));
        float rating = atof(gtk_entry_get_text(GTK_ENTRY(rating_entry)));
        const char *path = gtk_entry_get_text(GTK_ENTRY(path_entry));

        if (strlen(title) == 0) {
            show_message(GTK_WINDOW(main_window), "Error", "Title cannot be empty.");
        } else {
            if (add_movie_core(title, genre, year, rating, path)) {
                save_to_file();
                clear_listbox(listbox);
                GtkWidget *row = gtk_list_box_row_new();
                GtkWidget *label = gtk_label_new("Movie added. Use Search or Show All to see it.");
                gtk_container_add(GTK_CONTAINER(row), label);
                gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
                gtk_widget_show_all(listbox);
                show_message(GTK_WINDOW(main_window), "Success", "Movie added.");
            } else {
                show_message(GTK_WINDOW(main_window), "Error", "Memory allocation failed.");
            }
        }
    }
    gtk_widget_destroy(dialog);
}

/* --- Update Movie Dialog --- */
static void on_update_movie(GtkWidget *button, gpointer data) {
    if (count == 0) {
        show_message(GTK_WINDOW(main_window), "Info", "No movies to update.");
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Update Movie",
                                                    GTK_WINDOW(main_window),
                                                    GTK_DIALOG_MODAL,
                                                    "_Update", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content), grid);

    GtkWidget *id_spin = gtk_spin_button_new_with_range(1, count, 1);
    GtkWidget *title_entry = gtk_entry_new();
    GtkWidget *genre_entry = gtk_entry_new();
    GtkWidget *year_entry = gtk_entry_new();
    GtkWidget *rating_entry = gtk_entry_new();
    GtkWidget *path_entry = gtk_entry_new();

    int row = 0;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Movie ID:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_spin, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Title (leave blank to keep):"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), title_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Genre:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), genre_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Year (0 to keep):"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), year_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New Rating (negative to keep):"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rating_entry, 1, row, 1, 1);
    row++;
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("New File Path:"), 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), path_entry, 1, row, 1, 1);

    gtk_widget_show_all(content);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = gtk_spin_button_get_value(GTK_SPIN_BUTTON(id_spin));
        const char *new_title = gtk_entry_get_text(GTK_ENTRY(title_entry));
        const char *new_genre = gtk_entry_get_text(GTK_ENTRY(genre_entry));
        int new_year = atoi(gtk_entry_get_text(GTK_ENTRY(year_entry)));
        float new_rating = atof(gtk_entry_get_text(GTK_ENTRY(rating_entry)));
        const char *new_path = gtk_entry_get_text(GTK_ENTRY(path_entry));

        if (update_movie_core(id,
                              (strlen(new_title) ? new_title : NULL),
                              (strlen(new_genre) ? new_genre : NULL),
                              new_year,
                              (new_rating >= 0 ? new_rating : -1.0),
                              (strlen(new_path) ? new_path : NULL))) {
            save_to_file();
            clear_listbox(listbox);
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *label = gtk_label_new("Movie updated. Use Search or Show All to see changes.");
            gtk_container_add(GTK_CONTAINER(row), label);
            gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
            gtk_widget_show_all(listbox);
            show_message(GTK_WINDOW(main_window), "Success", "Movie updated.");
        } else {
            show_message(GTK_WINDOW(main_window), "Error", "Update failed.");
        }
    }
    gtk_widget_destroy(dialog);
}

/* --- Delete Movie Dialog --- */
static void on_delete_movie(GtkWidget *button, gpointer data) {
    if (count == 0) {
        show_message(GTK_WINDOW(main_window), "Info", "No movies to delete.");
        return;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delete Movie",
                                                    GTK_WINDOW(main_window),
                                                    GTK_DIALOG_MODAL,
                                                    "_Delete", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 10);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(content), grid);

    GtkWidget *id_spin = gtk_spin_button_new_with_range(1, count, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Movie ID to delete:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_spin, 1, 0, 1, 1);

    gtk_widget_show_all(content);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = gtk_spin_button_get_value(GTK_SPIN_BUTTON(id_spin));
        if (delete_movie_core(id)) {
            save_to_file();
            clear_listbox(listbox);
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *label = gtk_label_new("Movie deleted. Use Search or Show All to update the list.");
            gtk_container_add(GTK_CONTAINER(row), label);
            gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
            gtk_widget_show_all(listbox);
            show_message(GTK_WINDOW(main_window), "Success", "Movie deleted.");
        } else {
            show_message(GTK_WINDOW(main_window), "Error", "Deletion failed.");
        }
    }
    gtk_widget_destroy(dialog);
}

/* --- Main Window --- */
static void activate(GtkApplication *app, gpointer user_data) {
    main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(main_window), "Movie Organizing System");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 900, 550);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    /* Top bar: search controls */
    GtkWidget *search_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), search_hbox, FALSE, FALSE, 0);

    GtkWidget *search_label = gtk_label_new("Search by:");
    gtk_box_pack_start(GTK_BOX(search_hbox), search_label, FALSE, FALSE, 0);

    search_type_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_type_combo), "Title");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_type_combo), "Genre");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_type_combo), "Year");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_type_combo), "Rating (min,max)");
    gtk_combo_box_set_active(GTK_COMBO_BOX(search_type_combo), 0);
    gtk_box_pack_start(GTK_BOX(search_hbox), search_type_combo, FALSE, FALSE, 0);

    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Enter search term...");
    gtk_box_pack_start(GTK_BOX(search_hbox), search_entry, TRUE, TRUE, 0);

    GtkWidget *search_btn = gtk_button_new_with_label("🔍 Search");
    gtk_box_pack_start(GTK_BOX(search_hbox), search_btn, FALSE, FALSE, 0);

    GtkWidget *show_all_btn = gtk_button_new_with_label("📋 Show All");
    gtk_box_pack_start(GTK_BOX(search_hbox), show_all_btn, FALSE, FALSE, 0);

    /* Button bar for Add/Update/Delete */
    GtkWidget *button_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), button_bar, FALSE, FALSE, 0);

    GtkWidget *add_btn = gtk_button_new_with_label("➕ Add Movie");
    GtkWidget *update_btn = gtk_button_new_with_label("✏️ Update");
    GtkWidget *delete_btn = gtk_button_new_with_label("🗑️ Delete");

    gtk_box_pack_start(GTK_BOX(button_bar), add_btn, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_bar), update_btn, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(button_bar), delete_btn, TRUE, TRUE, 0);

    /* Scrollable list of movies */
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    listbox = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled), listbox);

    /* Initial welcome message (empty list) */
    clear_listbox(listbox);
    GtkWidget *welcome_row = gtk_list_box_row_new();
    GtkWidget *welcome_label = gtk_label_new("Welcome! Use Search or Show All to see movies.");
    gtk_container_add(GTK_CONTAINER(welcome_row), welcome_label);
    gtk_list_box_insert(GTK_LIST_BOX(listbox), welcome_row, -1);
    gtk_widget_show_all(listbox);

    /* Signal connections */
    g_signal_connect(search_btn, "clicked", G_CALLBACK(on_search), NULL);
    g_signal_connect(show_all_btn, "clicked", G_CALLBACK(on_show_all), NULL);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_movie), NULL);
    g_signal_connect(update_btn, "clicked", G_CALLBACK(on_update_movie), NULL);
    g_signal_connect(delete_btn, "clicked", G_CALLBACK(on_delete_movie), NULL);

    /* Load data from file but don't display it automatically */
    load_from_file();

    gtk_widget_show_all(main_window);
}

/* --- Main entry point --- */
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.movie.organizer", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    save_to_file();
    free(movies);
    return status;
}