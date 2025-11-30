#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct student
{
    char rollNo[20];
    char name[50];
    int seatRow;
    int seatCol;
};

struct seat
{
    char rollNo[20];
    int isOccupied;
};

struct student **studentList = NULL;
struct seat **hallMap = NULL;
int totalStudents = 0;
int hallRows = 0;
int hallCols = 0;

void initializeHall()
{
    printf("Enter number of rows in the hall: ");
    scanf("%d", &hallRows);
    printf("Enter number of columns in the hall: ");
    scanf("%d", &hallCols);
    getchar();

    hallMap = (struct seat **)malloc(hallRows * sizeof(struct seat *));
    for(int i = 0; i < hallRows; i++)
    {
        hallMap[i] = (struct seat *)calloc(hallCols, sizeof(struct seat));
        for(int j = 0; j < hallCols; j++)
        {
            strcpy(hallMap[i][j].rollNo, "");
            hallMap[i][j].isOccupied = 0;
        }
    }

    printf("Hall initialized with %d rows and %d columns.\n", hallRows, hallCols);
}

void loadStudentData()
{
    FILE *fp;
    fp = fopen("students.dat", "rb");
    if(fp == NULL)
    {
        printf("\nNo existing data found. Starting fresh.\n");
        fp = fopen("students.dat", "wb");
        if(fp != NULL)
        {
            int zero = 0;
            fwrite(&zero, sizeof(int), 1, fp);
            fclose(fp);
        }
        return;
    }

    fread(&totalStudents, sizeof(int), 1, fp);

    if(totalStudents > 0)
    {
        studentList = (struct student **)malloc(totalStudents * sizeof(struct student *));

        for(int i = 0; i < totalStudents; i++)
        {
            studentList[i] = (struct student *)malloc(sizeof(struct student));
            fread(studentList[i], sizeof(struct student), 1, fp);

            int row = studentList[i]->seatRow;
            int col = studentList[i]->seatCol;
            strcpy(hallMap[row][col].rollNo, studentList[i]->rollNo);
            hallMap[row][col].isOccupied = 1;
        }

        printf("Loaded %d student(s) from file.\n", totalStudents);
    }

    fclose(fp);
}

int findAvailableSeat(int *row, int *col)
{
    for(int i = 0; i < hallRows; i++)
    {
        for(int j = 0; j < hallCols; j++)
        {
            if(hallMap[i][j].isOccupied == 0)
            {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }
    return 0;
}

int findStudentIndex(char rollNo[20])
{
    int count = 0;
    for(int i = 0; i < totalStudents; i++)
    {
        count = 0;
        for(int j = 0; j < strlen(rollNo); j++)
        {
            if(studentList[i]->rollNo[j] == rollNo[j])
            {
                count++;
            }
        }
        
        if(count == strlen(rollNo))
        {
            return i;
        }
    }
    return -1;
}

void saveStudentData()
{
    FILE *fp;
    fp = fopen("students.txt", "w");
    if(fp == NULL)
    {
        printf("Error: Cannot save data!\n");
        return;
    }

    fprintf(fp, "========================================\n");
    fprintf(fp, "  STUDENT SEATING ALLOCATION RECORD\n");
    fprintf(fp, "========================================\n\n");
    fprintf(fp, "Total Students: %d\n", totalStudents);
    fprintf(fp, "Hall Dimensions: %d Rows x %d Columns\n\n", hallRows, hallCols);
    fprintf(fp, "----------------------------------------\n\n");

    for(int i = 0; i < totalStudents; i++)
    {
        fprintf(fp, "Student %d:\n", i + 1);
        fprintf(fp, "  Roll No    : %s\n", studentList[i]->rollNo);
        fprintf(fp, "  Name       : %s\n", studentList[i]->name);
        fprintf(fp, "  Seat Row   : %d\n", studentList[i]->seatRow + 1);
        fprintf(fp, "  Seat Column: %d\n\n", studentList[i]->seatCol + 1);
    }

    fprintf(fp, "----------------------------------------\n");
    fprintf(fp, "End of Record\n");
    fprintf(fp, "========================================\n");

    fclose(fp);

    FILE *fpBinary;
    fpBinary = fopen("students.dat", "wb");
    if(fpBinary == NULL)
    {
        printf("Error: Cannot save binary data!\n");
        return;
    }

    fwrite(&totalStudents, sizeof(int), 1, fpBinary);

    for(int i = 0; i < totalStudents; i++)
    {
        fwrite(studentList[i], sizeof(struct student), 1, fpBinary);
    }

    fclose(fpBinary);
}

void logAction(char action[200])
{
    FILE *fp;
    fp = fopen("allocation_log.txt", "a");
    if(fp == NULL)
    {
        printf("Warning: Cannot write to log file!\n");
        fp = fopen("allocation_log.txt", "w");
        if(fp == NULL)
        {
            return;
        }
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    fprintf(fp, "[%s] %s\n", timestamp, action);

    fclose(fp);
}

void allocateSeat()
{
    int numStudents;
    printf("\nHow many students do you want to allocate seats for? ");
    scanf("%d", &numStudents);
    getchar();

    int s = 0;
    while(s < numStudents)
    {
        char rollNo[20];
        char name[50];
        int row;
        int col;
        int validSeat = 0;
        char choice;

        printf("\n--- Student %d ---\n", s + 1);
        printf("Enter Roll No: ");
        fgets(rollNo, sizeof(rollNo), stdin);
        rollNo[strcspn(rollNo, "\n")] = 0;

        printf("Enter Name: ");
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;

        if(findStudentIndex(rollNo) != -1)
        {
            printf("Error: Student with Roll No %s already allocated! Please enter another student.\n", rollNo);
            continue;
        }

        while(validSeat == 0)
        {
            printf("Enter Row Number (1 to %d): ", hallRows);
            scanf("%d", &row);
            printf("Enter Column Number (1 to %d): ", hallCols);
            scanf("%d", &col);
            getchar();

            row = row - 1;
            col = col - 1;

            if(row < 0 || row >= hallRows || col < 0 || col >= hallCols)
            {
                printf("Error: Invalid seat position! Please try again.\n");
            }
            else if(hallMap[row][col].isOccupied == 1)
            {
                printf("Error: Seat at Row %d, Column %d is already occupied! Please choose another seat.\n", row + 1, col + 1);
            }
            else
            {
                validSeat = 1;
            }
        }

        studentList = (struct student **)realloc(studentList, (totalStudents + 1) * sizeof(struct student *));
        studentList[totalStudents] = (struct student *)malloc(sizeof(struct student));

        strcpy(studentList[totalStudents]->rollNo, rollNo);
        strcpy(studentList[totalStudents]->name, name);
        studentList[totalStudents]->seatRow = row;
        studentList[totalStudents]->seatCol = col;

        strcpy(hallMap[row][col].rollNo, rollNo);
        hallMap[row][col].isOccupied = 1;

        totalStudents++;

        saveStudentData();

        char logMsg[200];
        sprintf(logMsg, "ALLOCATED: Roll No: %s, Name: %s, Seat: Row %d, Col %d", rollNo, name, row + 1, col + 1);
        logAction(logMsg);

        printf("Seat allocated successfully at Row %d, Column %d!\n", row + 1, col + 1);

        s++;

        if(s < numStudents)
        {
            printf("\nDo you want to continue or Save and Exit? (C to Continue / E to Exit): ");
            scanf("%c", &choice);
            getchar();

            if(choice == 'E' || choice == 'e')
            {
                printf("Data saved. Returning to main menu...\n");
                return;
            }
        }
    }
}

void deallocateSeat()
{
    int numStudents;
    printf("\nHow many students do you want to deallocate? ");
    scanf("%d", &numStudents);
    getchar();

    int s = 0;
    while(s < numStudents)
    {
        char rollNo[20];
        char choice;

        printf("\n--- Student %d ---\n", s + 1);
        printf("Enter Roll No to deallocate: ");
        fgets(rollNo, sizeof(rollNo), stdin);
        rollNo[strcspn(rollNo, "\n")] = 0;

        int index = findStudentIndex(rollNo);
        if(index == -1)
        {
            printf("Error: Student with Roll No %s not found! Please enter another roll number.\n", rollNo);
            continue;
        }

        int row = studentList[index]->seatRow;
        int col = studentList[index]->seatCol;
        char name[50];
        strcpy(name, studentList[index]->name);

        char logMsg[200];
        sprintf(logMsg, "DEALLOCATED: Roll No: %s, Name: %s, Seat: Row %d, Col %d", rollNo, name, row + 1, col + 1);
        logAction(logMsg);

        strcpy(hallMap[row][col].rollNo, "");
        hallMap[row][col].isOccupied = 0;

        free(studentList[index]);

        for(int i = index; i < totalStudents - 1; i++)
        {
            studentList[i] = studentList[i + 1];
        }

        totalStudents--;

        if(totalStudents > 0)
        {
            studentList = (struct student **)realloc(studentList, totalStudents * sizeof(struct student *));
        }
        else
        {
            free(studentList);
            studentList = NULL;
        }

        saveStudentData();

        printf("Seat deallocated successfully!\n");

        s++;

        if(s < numStudents)
        {
            printf("\nDo you want to continue or Save and Exit? (C to Continue / E to Exit): ");
            scanf("%c", &choice);
            getchar();

            if(choice == 'E' || choice == 'e')
            {
                printf("Data saved. Returning to main menu...\n");
                return;
            }
        }
    }
}

void displayHallStatus()
{
    printf("\n--- Hall Status ---\n");
    printf("Legend: [RowCol(X)] = Occupied, [RowCol(O)] = Available\n\n");

    for(int i = 0; i < hallRows; i++)
    {
        for(int j = 0; j < hallCols; j++)
        {
            if(hallMap[i][j].isOccupied == 1)
            {
                printf("[%d%d(X)] ", i + 1, j + 1);
            }
            else
            {
                printf("[%d%d(O)] ", i + 1, j + 1);
            }
        }
        printf("\n");
    }

    printf("\nTotal Seats: %d\n", hallRows * hallCols);
    printf("Occupied: %d\n", totalStudents);
    printf("Available: %d\n", (hallRows * hallCols) - totalStudents);
}

void searchStudent()
{
    char rollNo[20];

    printf("\n--- Search Student ---\n");
    printf("Enter Roll No to search: ");
    fgets(rollNo, sizeof(rollNo), stdin);
    rollNo[strcspn(rollNo, "\n")] = 0;

    int index = findStudentIndex(rollNo);
    if(index == -1)
    {
        printf("Student with Roll No %s not found!\n", rollNo);
        return;
    }

    printf("\n--- Student Details ---\n");
    printf("Roll No: %s\n", studentList[index]->rollNo);
    printf("Name: %s\n", studentList[index]->name);
    printf("Seat Location: Row %d, Column %d\n", studentList[index]->seatRow + 1, studentList[index]->seatCol + 1);
}

void viewAuditLog()
{
    FILE *fp = fopen("allocation_log.txt", "r");
    if(fp == NULL)
    {
        printf("No audit log found.\n");
        return;
    }

    printf("\n--- Audit Log ---\n");
    char line[300];
    while(fgets(line, sizeof(line), fp))
    {
        printf("%s", line);
    }

    fclose(fp);
}

void freeMemory()
{
    for(int i = 0; i < totalStudents; i++)
    {
        free(studentList[i]);
    }
    if(studentList != NULL)
    {
        free(studentList);
    }

    for(int i = 0; i < hallRows; i++)
    {
        free(hallMap[i]);
    }
    free(hallMap);
}

int main()
{
    int choice;

    printf("=== Dynamic Student Seating Allocator ===\n\n");

    initializeHall();
    loadStudentData();

    do
    {
        printf("\n--- Main Menu ---\n");
        printf("1. Allocate Seat\n");
        printf("2. Deallocate Seat\n");
        printf("3. Display Hall Status\n");
        printf("4. Search Student\n");
        printf("5. View Audit Log\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        if(choice == 1)
        {
            allocateSeat();
        }
        else if(choice == 2)
        {
            deallocateSeat();
        }
        else if(choice == 3)
        {
            displayHallStatus();
        }
        else if(choice == 4)
        {
            searchStudent();
        }
        else if(choice == 5)
        {
            viewAuditLog();
        }
        else if(choice == 6)
        {
            saveStudentData();
            freeMemory();
            printf("Data saved. Exiting...\n");
        }
        else
        {
            printf("Invalid choice! Try again.\n");
        }
    }
    while(choice != 6);

    return 0;
}
