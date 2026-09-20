#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_PATIENTS 200
#define MAX_NAME_LENGTH 50
#define NUM_SPECIALTIES 4
#define NUM_WARDS 4
#define MAX_BEDS_PER_WARD 20

/* Specialty lookup data */
extern const int specialtyIDs[NUM_SPECIALTIES];
extern const char *specialtyNames[NUM_SPECIALTIES];
extern const double specialtyFees[NUM_SPECIALTIES];
extern const int specialtyTimes[NUM_SPECIALTIES];
extern const int specialtyCaps[NUM_SPECIALTIES];

/* Ward lookup data */
extern const int wardIDs[NUM_WARDS];
extern const char *wardNames[NUM_WARDS];
extern const double wardDailyRates[NUM_WARDS];
extern const int wardCapacities[NUM_WARDS];

/* Patient parallel arrays */
extern char patientNames[MAX_PATIENTS][MAX_NAME_LENGTH];
extern int patientAges[MAX_PATIENTS];
extern int patientUrgency[MAX_PATIENTS];
extern int patientSpecialtyIDs[MAX_PATIENTS];
extern int patientWardIDs[MAX_PATIENTS];
extern int patientDaysAdmitted[MAX_PATIENTS];
extern int patientBedNumbers[MAX_PATIENTS];
extern double patientFinalBills[MAX_PATIENTS];
extern char patientIDs[MAX_PATIENTS][10];
extern int patientCount;

extern int bedOccupancy[NUM_WARDS][MAX_BEDS_PER_WARD];
extern int specialtyQueueCounts[NUM_SPECIALTIES];

/* Function prototypes */
void ensureDataFolderExists(void);
void initializeSystem(void);
void initializeBedOccupancy(void);
void loadBedStatusFromFile(void);
void saveBedStatusToFile(void);

void displayMainMenu(void);
void displaySpecialtyMenu(void);
void displayWardMenu(void);

void registerNewPatient(void);
int findAvailableBed(int wardIndex);
void assignBedToPatient(int patientIndex, int wardIndex);
double calculateEmergencySurcharge(double baseFee, int urgencyLevel);
double calculateAgeSubsidy(double grossTotal, int age);
double calculateWaitingTime(int specialtyIndex);
void displayBill(int patientIndex);

void displayPatientsByPriority(void);
void sortPatientsByPriority(int sortedIndices[], int count);

void generateReports(void);
void displayUrgencyStatistics(void);
void displayRevenueStatistics(void);
void displayBedOccupancyReport(void);
void displayHighestPayingPatient(void);

void appendPatientRecord(int patientIndex);
void loadPatientRecords(void);

void clearInputBuffer(void);
int getValidIntInput(int min, int max);
void getValidStringInput(char *buffer, int maxLength);
void printSeparator(void);
void printHeader(const char *title);
void generatePatientID(char *buffer, int index);

#endif
