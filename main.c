#include "hospital.h"

int main(void) {
    ensureDataFolderExists();
    initializeSystem();

    int running = 1;
    while (running) {
        displayMainMenu();
        int choice = getValidIntInput(1, 6);
        switch (choice) {
            case 1: printf("\n[Registration coming soon]\n"); break;
            case 2: printf("\n[Priority view coming soon]\n"); break;
            case 3: printf("\n[Patient list coming soon]\n"); break;
            case 4: printf("\n[Reports coming soon]\n"); break;
            case 5: displayWardMenu(); break;
            case 6:
                printf("\n[SYSTEM] Exiting...\n");
                running = 0;
                break;
        }
    }
    return 0;
}
