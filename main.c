#include "hospital.h"

int main(void) {
    printf("\n");
    printSeparator();
    printf("  SMART HOSPITAL PATIENT & RESOURCE ALLOCATION SYSTEM\n");
    printf("  CSC 1012 - University of Sri Jayewardenepura\n");
    printSeparator();

    ensureDataFolderExists();
    initializeSystem();
    loadBedStatusFromFile();
    loadPatientRecords();

    int running = 1;
    while (running) {
        displayMainMenu();
        int choice = getValidIntInput(1, 6);
        switch (choice) {
            case 1: registerNewPatient(); break;
            case 2: displayPatientsByPriority(); break;
            case 3:
                if (patientCount == 0)
                    printf("\n[INFO] No patients yet.\n");
                else {
                    printHeader("ALL PATIENTS");
                    for (int i = 0; i < patientCount; i++)
                        printf("%d. %s - %s (%s)\n", i+1,
                               patientIDs[i], patientNames[i],
                               specialtyNames[patientSpecialtyIDs[i]-1]);
                }
                break;
            case 4: generateReports(); break;
            case 5: displayBedOccupancyReport(); break;
            case 6:
                printf("\n[SYSTEM] Saving & exiting...\n");
                saveBedStatusToFile();
                printf("[SYSTEM] Goodbye!\n");
                running = 0;
                break;
        }
    }
    return 0;
}
