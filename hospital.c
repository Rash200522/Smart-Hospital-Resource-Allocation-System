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
    printf("  ================================================================\n");
    printf("                                                                  \n");
    printf("     %s v%s                                   ║\n", APP_NAME, APP_VERSION);
    printf("     CSC 1012 - University of Sri Jayewardenepura                 \n");
    printf("                                                                  \n");
    printf("  ================================================================\n");
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

    double waitTime = specialtyQueueCounts[specIdx] * specialtyTimes[specIdx];

    printf("\n");
    printSeparator();
    printf("                 SMART HOSPITAL ADMISSION & BILL\n");
    printSeparator();

    printf("  %-28s : %s\n", "Patient ID", patientIDs[idx]);
    printf("  %-28s : %s\n", "Patient Name", patientNames[idx]);

    if (age < 5 || age > 65)
        printf("  %-28s : %d Years (15%% Subsidy Eligible)\n", "Age", age);
    else
        printf("  %-28s : %d Years\n", "Age", age);

    printf("  %-28s : %s\n", "Specialty", specialtyNames[specIdx]);

    if (wardID > 0) {
        char wardBed[50];
        sprintf(wardBed, "%s (Bed #%02d)", wardNames[wardID - 1], patientBedNumbers[idx]);
        printf("  %-28s : %s\n", "Assigned Ward", wardBed);
    } else {
        printf("  %-28s : %s\n", "Assigned Ward", "Not Admitted (OPD)");
    }

    const char *urgencyText[] = {"Level 1 (Normal)", "Level 2 (Urgent)", "Level 3 (Critical)"};
    printf("  %-28s : %s\n", "Urgency Level", urgencyText[urgency - 1]);

    printSeparator();

    printf("  %-28s : LKR %10.2f\n", "Base Consultation Fee", baseFee);

    double surPct = (urgency == 3) ? 50.0 : (urgency == 2) ? 25.0 : 0.0;
    char surLine[30];
    sprintf(surLine, "LKR %.2f (%.0f%%)", surcharge, surPct);
    printf("  %-28s : %s\n", "Emergency Surcharge", surLine);

    char wardLine[30];
    sprintf(wardLine, "LKR %.2f", wardCost);
    char wardLabel[50];
    sprintf(wardLabel, "Ward Stay Cost (%d Days)", patientDaysAdmitted[idx]);
    printf("  %-28s : %s\n", wardLabel, wardLine);

    printf("  %-28s : LKR %10.2f\n", "Gross Total Bill", gross);

    if (discount > 0) {
        char discLine[30];
        sprintf(discLine, "LKR -%.2f (15%%)", discount);
        printf("  %-28s : %s\n", "Age Subsidy Discount", discLine);
    } else {
        printf("  %-28s : LKR 0.00 (0%%)\n", "Age Subsidy Discount");
    }

    printSeparator();
    printf("  %-28s : LKR %10.2f\n", "FINAL PAYABLE AMOUNT", finalAmt);
    printSeparator();

    if (urgency == 3)
        printf("  Estimated Waiting Time : 0.00 mins (Immediate Attention)\n");
    else
        printf("  Estimated Waiting Time : %.2f mins\n", waitTime);

    printSeparator();
    printf("\n");
}

/* priority sorting */

void sortPatientsByPriority(int sortedIndices[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            int a = sortedIndices[j];
            int b = sortedIndices[j + 1];
            int swap = 0;
            if (patientUrgency[a] < patientUrgency[b]) swap = 1;
            else if (patientUrgency[a] == patientUrgency[b] && a > b) swap = 1;
            if (swap) {
                int t = sortedIndices[j];
                sortedIndices[j] = sortedIndices[j + 1];
                sortedIndices[j + 1] = t;
            }
        }
    }
}

void displayPatientsByPriority(void) {
    if (patientCount == 0) {
        printf("\n[INFO] No patients yet.\n");
        return;
    }
    printHeader("PATIENTS BY PRIORITY");
    int sorted[MAX_PATIENTS];
    for (int i = 0; i < patientCount; i++) sorted[i] = i;
    sortPatientsByPriority(sorted, patientCount);

    printf("%-5s %-12s %-25s %-6s %-10s %-25s\n",
           "Rank", "ID", "Name", "Age", "Urgency", "Specialty");
    printSeparator();
    for (int i = 0; i < patientCount; i++) {
        int idx = sorted[i];
        const char *ut[] = {"NORMAL", "URGENT", "CRITICAL"};
        printf("%-5d %-12s %-25s %-6d %-10s %-25s\n",
               i + 1, patientIDs[idx], patientNames[idx],
               patientAges[idx], ut[patientUrgency[idx] - 1],
               specialtyNames[patientSpecialtyIDs[idx] - 1]);
    }
    printSeparator();
}

/* REPORTS */
void displayUrgencyStatistics(void) {
    printf("\n--- Patient Statistics by Urgency ---\n");
    int counts[3] = {0, 0, 0};
    for (int i = 0; i < patientCount; i++)
        counts[patientUrgency[i] - 1]++;
    printf("Total Registered: %d\n\n", patientCount);
    printf("  Level 1 (Normal):   %3d (%.1f%%)\n", counts[0],
           patientCount ? counts[0]*100.0/patientCount : 0);
    printf("  Level 2 (Urgent):   %3d (%.1f%%)\n", counts[1],
           patientCount ? counts[1]*100.0/patientCount : 0);
    printf("  Level 3 (Critical): %3d (%.1f%%)\n", counts[2],
           patientCount ? counts[2]*100.0/patientCount : 0);
}

void displayRevenueStatistics(void) {
    printf("\n--- Revenue Statistics ---\n");
    double gross = 0, discounts = 0, revenue = 0;
    for (int i = 0; i < patientCount; i++) {
        int si = patientSpecialtyIDs[i] - 1;
        double bf = specialtyFees[si];
        double sc = calculateEmergencySurcharge(bf, patientUrgency[i]);
        double wc = 0;
        if (patientWardIDs[i] > 0)
            wc = patientDaysAdmitted[i] * wardDailyRates[patientWardIDs[i]-1];
        double g = bf + sc + wc;
        double d = calculateAgeSubsidy(g, patientAges[i]);
        gross += g;
        discounts += d;
        revenue += (g - d);
    }
    printf("Total Gross Billing:  LKR %.2f\n", gross);
    printf("Total Discounts:      LKR %.2f\n", discounts);
    printf("Total Net Revenue:    LKR %.2f\n", revenue);
}

void displayBedOccupancyReport(void) {
    printf("\n--- Bed Occupancy Report ---\n\n");
    printf("%-28s %-8s %-10s %-10s %-12s\n",
           "Ward", "Total", "Occupied", "Available", "%");
    printSeparator();
    int tBeds = 0, tOcc = 0;
    for (int i = 0; i < NUM_WARDS; i++) {
        int cap = wardCapacities[i], occ = 0;
        for (int j = 0; j < cap; j++)
            if (bedOccupancy[i][j] == 1) occ++;
        printf("%-28s %-8d %-10d %-10d %.1f%%\n",
               wardNames[i], cap, occ, cap - occ,
               cap ? occ*100.0/cap : 0);
        tBeds += cap; tOcc += occ;
    }
    printSeparator();
    printf("%-28s %-8d %-10d %-10d %.1f%%\n",
           "TOTAL", tBeds, tOcc, tBeds - tOcc,
           tBeds ? tOcc*100.0/tBeds : 0);
}

void displayHighestPayingPatient(void) {
    printf("\n--- Highest Paying Patient ---\n");
    if (patientCount == 0) { printf("No patients.\n"); return; }
    int hi = 0;
    for (int i = 1; i < patientCount; i++)
        if (patientFinalBills[i] > patientFinalBills[hi]) hi = i;
    printf("Patient ID:   %s\n", patientIDs[hi]);
    printf("Patient Name: %s\n", patientNames[hi]);
    printf("Total Bill:   LKR %.2f\n", patientFinalBills[hi]);
}

void generateReports(void) {
    if (patientCount == 0) { printf("\n[INFO] No data.\n"); return; }
    printHeader("REPORTS");
    printf("  [1] Urgency Statistics\n");
    printf("  [2] Revenue Statistics\n");
    printf("  [3] Bed Occupancy\n");
    printf("  [4] Highest Paying Patient\n");
    printf("  [5] All Reports\n");
    printf("  [0] Back\n");
    printf("  Choice: ");
    int c = getValidIntInput(0, 5);
    switch (c) {
        case 1: displayUrgencyStatistics(); break;
        case 2: displayRevenueStatistics(); break;
        case 3: displayBedOccupancyReport(); break;
        case 4: displayHighestPayingPatient(); break;
        case 5:
            displayUrgencyStatistics();
            displayRevenueStatistics();
            displayBedOccupancyReport();
            displayHighestPayingPatient();
            break;
    }
}

/* FILE HANDLING  */
void loadBedStatusFromFile(void) {
    FILE *f = fopen("data/beds_status.txt", "r");
    if (f == NULL) {
        printf("[INFO] No previous bed data.\n");
        return;
    }
    int w, b, s, loaded = 0;
    while (fscanf(f, "%d %d %d", &w, &b, &s) == 3) {
        if (w >= 0 && w < NUM_WARDS && b >= 0 && b < MAX_BEDS_PER_WARD) {
            bedOccupancy[w][b] = s;
            loaded++;
        }
    }
    fclose(f);
    printf("[SYSTEM] Loaded %d bed statuses.\n", loaded);
}

void saveBedStatusToFile(void) {
    FILE *f = fopen("data/beds_status.txt", "w");
    if (f == NULL) { printf("[ERROR] Cannot save!\n"); return; }
    for (int i = 0; i < NUM_WARDS; i++)
        for (int j = 0; j < wardCapacities[i]; j++)
            fprintf(f, "%d %d %d\n", i, j, bedOccupancy[i][j]);
    fclose(f);
    printf("[SYSTEM] Bed status saved.\n");
}

void appendPatientRecord(int idx) {
    FILE *f = fopen("data/patient_records.txt", "a");
    if (f == NULL) return;
    time_t now = time(NULL);
    char ts[30];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(f, "========================================\n");
    fprintf(f, "Time: %s\n", ts);
    fprintf(f, "ID: %s | Name: %s | Age: %d\n",
            patientIDs[idx], patientNames[idx], patientAges[idx]);
    fprintf(f, "Urgency: %d | Specialty: %s\n",
            patientUrgency[idx],
            specialtyNames[patientSpecialtyIDs[idx]-1]);
    fprintf(f, "Final Amount: LKR %.2f\n", patientFinalBills[idx]);
    fprintf(f, "========================================\n\n");
    fclose(f);
}

void loadPatientRecords(void) {
    FILE *f = fopen("data/patient_records.txt", "r");
    if (f == NULL) { printf("[INFO] No previous records.\n"); return; }
    char line[256];
    int cnt = 0;
    while (fgets(line, sizeof(line), f))
        if (strstr(line, "ID:") != NULL) cnt++;
    fclose(f);
    printf("[SYSTEM] Found %d historical records.\n", cnt);
}
