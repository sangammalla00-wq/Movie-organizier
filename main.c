#include <stdio.h>
#include <stdlib.h>
#include "movie.h"

int main() {
    load_from_file();   

    int choice;
    do {
        printf("\n========================================\n");
        printf("        MOVIE ORGANIZING SYSTEM        \n");
        printf("========================================\n");
        printf("1. Add Movie\n");
        printf("2. View All Movies\n");
        printf("3. Search Movies\n");
        printf("4. Update Movie\n");
        printf("5. Delete Movie\n");
        printf("6. Exit\n");
        printf("----------------------------------------\n");
        printf("Enter your choice (1-6): ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
            case 1: add_movie(); break;
            case 2: view_all_movies(); break;
            case 3: search_movies(); break;
            case 4: update_movie(); break;
            case 5: delete_movie(); break;
            case 6: 
                save_to_file();
                printf("Data saved. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter 1-6.\n");
        }
    } while (choice != 6);

    // Free allocated memory
    free(movies);
    return 0;
}