#ifndef HOSPITAL_H
#define HOSPITAL_H

#define APP_VERSION "1.0.0"
#define APP_NAME    "Smart Hospital System"

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

//function prototype
//creates data forlder if it missing
void ensureDataFolderExists(void);

//initialize all data structure
void initializeSystem(void);

//sets all beds to available
void initializeBedOccupancy(void);

//reads beds occupy data
void loadBedStatusFromFile(void);

//current bed occupancy
void saveBedStatusToFile(void);

//display main menu
void displayMainMenu(void);

//display doctor with fees and time
void displaySpecialtyMenu(void);

//display wards with rates and availability
void displayWardMenu(void);

//handle patient registration process
void registerNewPatient(void);

//finds available beds
int findAvailableBed(int wardIndex);

//records occupied beds
void assignBedToPatient(int patientIndex, int wardIndex);

//calculate emegency syrcharge
double calculateEmergencySurcharge(double baseFee, int urgencyLevel);

//Calculates 15% age subsidy for age<5 or age>65
double calculateAgeSubsidy(double grossTotal, int age);

//calculate estimated waiting time
double calculateWaitingTime(int specialtyIndex);

//display formatted bill
void displayBill(int patientIndex);

//display patients sorts by urgency
void displayPatientsByPriority(void);

//Sorts patient indices by priority using bubble sort
void sortPatientsByPriority(int sortedIndices[], int count);

//display report menu
void generateReports(void);

//display urgency level statistic
void displayUrgencyStatistics(void);

//display revenue
void displayRevenueStatistics(void);

//display bed occupancy
void displayBedOccupancyReport(void);

//display highest paying patient info
void displayHighestPayingPatient(void);

//Appends a patient record to data/patient_records.txt
void appendPatientRecord(int patientIndex);

// Reads and counts past patient records
void loadPatientRecords(void);

//clear input buffer
void clearInputBuffer(void);

//reads integer
int getValidIntInput(int min, int max);

//reads non empty string without digit
void getValidStringInput(char *buffer, int maxLength

 //prints horizontal line
void printSeparator(void);

//prints centered header with seperator
void printHeader(const char *title);

//generate patient id
void generatePatientID(char *buffer, int index);

#endif
