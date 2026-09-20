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
    printf("  ╔══════════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                              ║\n");
    printf("  ║   %s v%s                                   ║\n", APP_NAME, APP_VERSION);
    printf("  ║   CSC 1012 - University of Sri Jayewardenepura               ║\n");
    printf("  ║                                                              ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════╝\n");
    printSeparator();

    printf("  [1]  Register New Patient\n");
    printf("  [2]  View Patients by Priority (Emergency Triage)\n");
    printf("  [3]  View All Registered Patients\n");
    printf("  [4]  Generate Reports & Analytics\n");
    printf("  [5]  View Bed Occupancy Status\n");
    printf("  [6]  Save & Exit\n");
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

//billing calculation
double calculateEmergencySurcharge(double baseFee, int urgencyLevel) {
    switch (urgencyLevel) {
        case 3: return baseFee * 0.50;
        case 2: return baseFee * 0.25;
        default: return 0.0;
    }
}

double calculateAgeSubsidy(double grossTotal, int age) {
    if (age < 5 || age > 65) return grossTotal * 0.15;
    return 0.0;
}

double calculateWaitingTime(int specialtyIndex) {
    return specialtyQueueCounts[specialtyIndex] * (double)specialtyTimes[specialtyIndex];
}



//bed management
int findAvailableBed(int wardIndex) {
    for (int i = 0; i < wardCapacities[wardIndex]; i++)
        if (bedOccupancy[wardIndex][i] == 0) return i;
    return -1;
}

void assignBedToPatient(int patientIndex, int wardIndex) {
    for (int i = 0; i < wardCapacities[wardIndex]; i++) {
        if (bedOccupancy[wardIndex][i] == 0) {
            bedOccupancy[wardIndex][i] = 1;
            patientBedNumbers[patientIndex] = i + 1;
            return;
        }
    }
}


/* patients registration  */
void registerNewPatient(void) {
    if (patientCount >= MAX_PATIENTS) {
        printf("\n[ERROR] Maximum patients reached!\n");
        return;
    }

    printHeader("NEW PATIENT REGISTRATION");
    int idx = patientCount;
    generatePatientID(patientIDs[idx], idx);
    printf("  Patient ID: %s\n\n", patientIDs[idx]);

    printf("  Patient Name: ");
    getValidStringInput(patientNames[idx], MAX_NAME_LENGTH);

    printf("  Patient Age (0-120): ");
    patientAges[idx] = getValidIntInput(0, 120);

    printf("\n  Urgency Level:\n    1-Normal  2-Urgent  3-Critical\n  Select (1-3): ");
    patientUrgency[idx] = getValidIntInput(1, 3);

    displaySpecialtyMenu();
    printf("\n  Select Specialty ID (1-%d): ", NUM_SPECIALTIES);
    int specChoice = getValidIntInput(1, NUM_SPECIALTIES);
    patientSpecialtyIDs[idx] = specChoice;
    int specIdx = specChoice - 1;

    printf("\n  Admitted to ward? (1=Yes, 0=No): ");
    int isAdmitted = getValidIntInput(0, 1);

    if (isAdmitted == 1) {
        displayWardMenu();
        printf("\n  Select Ward ID (1-%d): ", NUM_WARDS);
        int wardChoice = getValidIntInput(1, NUM_WARDS);
        int wardIdx = wardChoice - 1;

        int bedNum = findAvailableBed(wardIdx);
        if (bedNum == -1) {
            printf("\n[ERROR] No beds in %s! Registered as OPD.\n", wardNames[wardIdx]);
            patientWardIDs[idx] = 0;
            patientDaysAdmitted[idx] = 0;
            patientBedNumbers[idx] = -1;
        } else {
            patientWardIDs[idx] = wardChoice;
            printf("  Days Admitted: ");
            patientDaysAdmitted[idx] = getValidIntInput(1, 365);
            assignBedToPatient(idx, wardIdx);
            printf("\n  [SUCCESS] Bed #%02d in %s\n",
                   patientBedNumbers[idx], wardNames[wardIdx]);
        }
    } else {
        patientWardIDs[idx] = 0;
        patientDaysAdmitted[idx] = 0;
        patientBedNumbers[idx] = -1;
        printf("\n  Registered as OPD.\n");
    }

    /* Calculate bill */
    double baseFee = specialtyFees[specIdx];
    double surcharge = calculateEmergencySurcharge(baseFee, patientUrgency[idx]);
    double wardCost = 0.0;
    if (patientWardIDs[idx] > 0) {
        wardCost = patientDaysAdmitted[idx] * wardDailyRates[patientWardIDs[idx] - 1];
    }
    double gross = baseFee + surcharge + wardCost;
    double discount = calculateAgeSubsidy(gross, patientAges[idx]);
    patientFinalBills[idx] = gross - discount;

    displayBill(idx);
    specialtyQueueCounts[specIdx]++;
    patientCount++;

    printf("\n[SYSTEM] Patient registered. Total: %d\n", patientCount);
}

/* bill display */
void displayBill(int idx) {
    int specIdx = patientSpecialtyIDs[idx] - 1;
    int urgency = patientUrgency[idx];
    int age = patientAges[idx];
    int wardID = patientWardIDs[idx];

    double baseFee = specialtyFees[specIdx];
    double surcharge = calculateEmergencySurcharge(baseFee, urgency);
    double wardCost = 0.0;
    if (wardID > 0)
        wardCost = patientDaysAdmitted[idx] * wardDailyRates[wardID - 1];
    double gross = baseFee + surcharge + wardCost;
    double discount = calculateAgeSubsidy(gross, age);
    double finalAmt = gross - discount;

    double waitTime = (specialtyQueueCounts[specIdx]) * specialtyTimes[specIdx];

    printSeparator();
    printf("           SMART HOSPITAL ADMISSION & BILL\n");
    printSeparator();
    printf("Patient ID: %s\n", patientIDs[idx]);
    printf("Patient Name: %s\n", patientNames[idx]);
    if (age < 5 || age > 65)
        printf("Age: %d Years (15%% Subsidy Eligible)\n", age);
    else
        printf("Age: %d Years\n", age);

    printf("Specialty: %s\n", specialtyNames[specIdx]);
    if (wardID > 0)
        printf("Assigned Ward: %s (Bed #%02d)\n",
               wardNames[wardID - 1], patientBedNumbers[idx]);
    else
        printf("Assigned Ward: Not Admitted (OPD)\n");

    const char *urgencyText[] = {"Level 1 (Normal)", "Level 2 (Urgent)", "Level 3 (Critical)"};
    printf("Urgency Level: %s\n", urgencyText[urgency - 1]);

    printf("Base Consultation Fee: LKR %.2f\n", baseFee);

    double surPct = (urgency == 3) ? 50.0 : (urgency == 2) ? 25.0 : 0.0;
    printf("Emergency Surcharge: LKR %.2f (%.0f%%)\n", surcharge, surPct);

    printf("Ward Stay Cost (%d Days): LKR %.2f\n", patientDaysAdmitted[idx], wardCost);
    printf("Gross Total Bill: LKR %.2f\n", gross);

    if (discount > 0)
        printf("Age Subsidy Discount: LKR -%.2f (15%%)\n", discount);
    else
        printf("Age Subsidy Discount: LKR 0.00 (0%%)\n");

    printf("Final Payable Amount: LKR %.2f\n", finalAmt);

    if (urgency == 3)
        printf("Estimated Waiting Time: 0.00 mins (Immediate Attention)\n");
    else
        printf("Estimated Waiting Time: %.2f mins\n", waitTime);

    printSeparator();
}
