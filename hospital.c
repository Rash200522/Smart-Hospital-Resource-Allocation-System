#include "hospital.h"

//lookup data
const int specialtyIDs[NUM_SPECIALTIES] = {1, 2, 3, 4};
const char *specialtyNames[NUM_SPECIALTIES] = {
    "General Practice (OPD)", "Paediatrics", "Cardiology", "Neurology"
};
const double specialtyFees[NUM_SPECIALTIES] = {1500.00, 2500.00, 4500.00, 5000.00};
const int specialtyTimes[NUM_SPECIALTIES] = {15, 20, 30, 30};
const int specialtyCaps[NUM_SPECIALTIES] = {30, 20, 12, 10};

const int wardIDs[NUM_WARDS] = {1, 2, 3, 4};
const char *wardNames[NUM_WARDS] = {
    "General Ward", "Paediatric Ward", "Surgical Ward", "ICU (Intensive Care Unit)"
};
const double wardDailyRates[NUM_WARDS] = {3000.00, 6000.00, 12000.00, 25000.00};
const int wardCapacities[NUM_WARDS] = {20, 10, 10, 5};

//global data
char patientNames[MAX_PATIENTS][MAX_NAME_LENGTH];
int patientAges[MAX_PATIENTS];
int patientUrgency[MAX_PATIENTS];
int patientSpecialtyIDs[MAX_PATIENTS];
int patientWardIDs[MAX_PATIENTS];
int patientDaysAdmitted[MAX_PATIENTS];
int patientBedNumbers[MAX_PATIENTS];
double patientFinalBills[MAX_PATIENTS];
char patientIDs[MAX_PATIENTS][10];
int patientCount = 0;

int bedOccupancy[NUM_WARDS][MAX_BEDS_PER_WARD];
int specialtyQueueCounts[NUM_SPECIALTIES];

//initialization
void ensureDataFolderExists(void) {
    FILE *test = fopen("data/beds_status.txt", "a");
    if (test == NULL) {
        #ifdef _WIN32
            system("mkdir data");
        #else
            system("mkdir -p data");
        #endif
        test = fopen("data/beds_status.txt", "a");
    }
    if (test != NULL) fclose(test);
}

void initializeSystem(void) {
    initializeBedOccupancy();
    for (int i = 0; i < NUM_SPECIALTIES; i++) specialtyQueueCounts[i] = 0;
    for (int i = 0; i < MAX_PATIENTS; i++) {
        patientNames[i][0] = '\0';
        patientAges[i] = 0;
        patientUrgency[i] = 0;
        patientSpecialtyIDs[i] = 0;
        patientWardIDs[i] = 0;
        patientDaysAdmitted[i] = 0;
        patientBedNumbers[i] = -1;
        patientFinalBills[i] = 0.0;
        patientIDs[i][0] = '\0';
    }
    printf("[SYSTEM] Hospital system initialized.\n");
}

void initializeBedOccupancy(void) {
    for (int i = 0; i < NUM_WARDS; i++)
        for (int j = 0; j < MAX_BEDS_PER_WARD; j++)
            bedOccupancy[i][j] = 0;
}

//utility functions

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getValidIntInput(int min, int max) {
    int value;
    char buffer[100];
    char extra;

    while (1) {

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            /* Handle Ctrl+D / EOF gracefully */
            printf("\n[ERROR] Input stream closed. Exiting.\n");
            exit(1);
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) {
            printf("  [ERROR] Input cannot be empty. Try again: ");
            continue;
        }

        //Strict parsing: reject "12abc", accept "12" only if followed by nothing
        if (sscanf(buffer, "%d %c", &value, &extra) != 1) {
            printf("  [ERROR] '%s' is not a valid integer. Try again: ", buffer);
            continue;
        }

        //range check
        if (value < min || value > max) {
            printf("  [ERROR] Value must be between %d and %d. Try again: ", min, max);
            continue;
        }

        return value;
    }
}

void getValidStringInput(char *buffer, int maxLength) {
    while (1) {
        if (fgets(buffer, maxLength, stdin) == NULL) {
            printf("\n[ERROR] Input stream closed. Exiting.\n");
            exit(1);
        }

        //remove new line
        buffer[strcspn(buffer, "\n")] = '\0';


        if (strlen(buffer) == 0) {
            printf("  [ERROR] Input cannot be empty. Try again: ");
            continue;
        }


        int hasNonSpace = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isspace((unsigned char)buffer[i])) {
                hasNonSpace = 1;
                break;
            }
        }
        if (!hasNonSpace) {
            printf("  [ERROR] Input cannot be only spaces. Try again: ");
            continue;
        }


        int hasDigit = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (isdigit((unsigned char)buffer[i])) {
                hasDigit = 1;
                break;
            }
        }
        if (hasDigit) {
            printf("  [ERROR] Name should not contain digits. Try again: ");
            continue;
        }

        return;
    }
}

void printSeparator(void) {
    printf("================================================================================\n");
}

void printHeader(const char *title) {
    printf("\n");
    printSeparator();
    printf("  %s\n", title);
    printSeparator();
}

void generatePatientID(char *buffer, int index) {
    sprintf(buffer, "PAT-%d", 1001 + index);
}


//menu function
void displayMainMenu(void) {
    printf("\n");
    printSeparator();
    printf("       SMART HOSPITAL PATIENT & RESOURCE ALLOCATION SYSTEM\n");
    printSeparator();
    printf("  [1] Register New Patient\n");
    printf("  [2] View Patients by Priority\n");
    printf("  [3] View All Registered Patients\n");
    printf("  [4] Generate Reports & Analytics\n");
    printf("  [5] View Bed Occupancy Status\n");
    printf("  [6] Save & Exit\n");
    printSeparator();
    printf("  Enter your choice (1-6): ");
}

void displaySpecialtyMenu(void) {
    printf("\n--- Available Specialties ---\n");
    printf("%-4s %-28s %-18s %-15s %-10s\n",
           "ID", "Specialty Name", "Fee", "Time/Patient", "Cap");
    printSeparator();
    for (int i = 0; i < NUM_SPECIALTIES; i++) {
        printf("%-4d %-28s LKR %-14.2f %-15d %-10d\n",
               specialtyIDs[i], specialtyNames[i], specialtyFees[i],
               specialtyTimes[i], specialtyCaps[i]);
    }
}

void displayWardMenu(void) {
    printf("\n--- Available Wards ---\n");
    printf("%-4s %-28s %-18s %-12s %-10s\n",
           "ID", "Ward Name", "Daily Rate", "Capacity", "Available");
    printSeparator();
    for (int i = 0; i < NUM_WARDS; i++) {
        int avail = 0;
        for (int j = 0; j < wardCapacities[i]; j++)
            if (bedOccupancy[i][j] == 0) avail++;
        printf("%-4d %-28s LKR %-15.2f %-12d %-10d\n",
               wardIDs[i], wardNames[i], wardDailyRates[i],
               wardCapacities[i], avail);
    }
}
