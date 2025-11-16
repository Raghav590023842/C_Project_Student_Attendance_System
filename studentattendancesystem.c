//  12. Student Attendance System
// •	Store attendance records in a file.
// •	Generate monthly reports (percentage attendance).
// •	Search students with low attendance.
// •	Generate class summary reports.
// •	Password-protected admin access.

// Done by Raghav Vij (590023842) B1 Batch, Btech First Year, UPES Dehradun.


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int admlogin() {
    char savedPass[64] = "Raghav@2025";
    char inputPass[64];

    FILE *pf = fopen("adminpass.txt", "r");
    if (pf) {
        fscanf(pf, "%63s", savedPass);
        fclose(pf);
    }

    printf("Enter admin password: ");
    scanf("%63s", inputPass);

    if (strcmp(inputPass, savedPass) == 0) {
        printf("Login successful!\n");
        return 1;
    } else {
        printf("Incorrect password.\n");
        return 0;
    }
}

int addstudent(){
    FILE *fp = fopen("students.txt", "a");
    if(fp == NULL){
        printf("Error opening file.\n");
        return 0;
    }
    char name[50];
    int id;
    printf("Enter student ID: ");
    scanf("%d", &id);
    printf("Enter student name: ");
    scanf("%s", name);
    fprintf(fp, "%d %s 0 \n", id, name);
    fclose(fp);
    printf("----------------------------\n");
    printf("Student added successfully.\n");
    return 1;
}

int view(){
    FILE *fp = fopen("students.txt", "r");
    if(fp == NULL){
        printf("Error opening file.\n");
        return 0;
    }

    int id, attended;
    char name[50];

    FILE *tclass = fopen("totalclasses.txt", "r");
    int totalClasses = 0;
    fscanf(tclass, "%d", &totalClasses);
    fclose(tclass);

    printf("\nID\t   Name\t\tAttended    Total\n");
    printf("----------------------------------------------\n");

     while(fscanf(fp, "%d %49s %d", &id, name, &attended) == 3){

        float percentage = 0;
        if(totalClasses > 0)
            percentage = (attended * 100.0f) / totalClasses;

        char status[30] = "";
        if(percentage < 75)
            strcpy(status, "LOW ATTENDANCE!!!");

        printf("%-10d %-15s %-10d %-10d %s\n",
               id, name, attended, totalClasses, status);
    }

    fclose(fp);
    return 1;
}


int MarkAttendance() {

    FILE *fp = fopen("students.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Error opening students.txt/temp.txt\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return 0;
    }

    FILE *tclass = fopen("totalclasses.txt", "r");
    int totalClasses = 0;
    if (tclass) { fscanf(tclass, "%d", &totalClasses); fclose(tclass); }

    int ids[500], overall[500];
    char names[500][50];
    int n = 0;
    while (n < 500 && fscanf(fp, "%d %49s %d", &ids[n], names[n], &overall[n]) == 3) {
        n++;
    }
    fclose(fp);

    char month[8];
    printf("Enter month (YYYY-MM): ");
    scanf("%7s", month);

    char mfilename[40];   
    strcpy(mfilename, month);
    strcat(mfilename, ".txt");

    char ffilename[64]; 
    strcpy(ffilename, "monthlydata\\");
    strcat(ffilename, mfilename);

    FILE *mf = fopen(ffilename, "r+");
    if (!mf) {
        mf = fopen(ffilename, "w+");
        if (!mf) {
            printf("Error opening %s\n", mfilename);
            fclose(temp);
            return 0;
        }

        fprintf(mf, "CLASSES 0\n");
        for (int i = 0; i < n; i++) fprintf(mf, "%d %s 0\n", ids[i], names[i]);
        fflush(mf);
    }

    int presentMonth[500] = {0};
    int classesInMonth = 0;

    fseek(mf, 0, SEEK_SET);
    {
        char tag[16];
        if (fscanf(mf, "%15s %d", tag, &classesInMonth) != 2) classesInMonth = 0;

        int mid, mpresent; char mname[50];
        while (fscanf(mf, "%d %49s %d", &mid, mname, &mpresent) == 3) {
  
            for (int i = 0; i < n; i++) {
                if (ids[i] == mid) { presentMonth[i] = mpresent; break; }
            }
        }
    }

    printf("\n--- Mark Attendance (%s) ---\n", month);
    for (int i = 0; i < n; i++) {
        int status;
        printf("Student ID: %d, Name: %s   Present(1) / Absent(0): ", ids[i], names[i]);
        scanf("%d", &status);

        if (status == 1) {
            overall[i]++;        
            presentMonth[i]++;  
        }

        fprintf(temp, "%d %s %d\n", ids[i], names[i], overall[i]);
    }
    fclose(temp);

    totalClasses++;
    classesInMonth++;

    fclose(mf);
    mf = fopen(ffilename, "w");
    if (!mf) {
        printf("Error rewriting %s\n", ffilename);
        return 0;
    }
    fprintf(mf, "CLASSES %d\n", classesInMonth);
    for (int i = 0; i < n; i++) {
        fprintf(mf, "%d %s %d\n", ids[i], names[i], presentMonth[i]);
    }
    fclose(mf);

    tclass = fopen("totalclasses.txt", "w");
    if (tclass) { fprintf(tclass, "%d", totalClasses); fclose(tclass); }
    else { printf("Warning: couldn't update totalclasses.txt\n"); }

    remove("students.txt");
    rename("temp.txt", "students.txt");

    printf("\nAttendance marked for %s. (Total classes = %d)\n", month, totalClasses);
    return 1;
}


int Monthlyreport() {
    char month[8];
    printf("Enter month (YYYY-MM): ");
    scanf("%7s", month);

    char mfilename[40], path[64];
    strcpy(mfilename, month);
    strcat(mfilename, ".txt");
    strcpy(path, "monthlydata\\");
    strcat(path, mfilename);

    FILE *mf = fopen(path, "r");
    if (!mf) {
        printf("No data for %s\n", month);
        return 0;
    }

    int totalClasses = 0;
    char tag[16];
    if (fscanf(mf, "%15s %d", tag, &totalClasses) != 2) {
        printf("Bad file format in %s\n", path);
        fclose(mf);
        return 0;
    }

    printf("\nMonthly Report for %s\n", month);
    printf("%-10s %-15s %-10s %-10s\n", "ID", "Name", "Attended", "Percentage");
    printf("---------------------------------------------------\n");

    int id, present;
    char name[50];
    while (fscanf(mf, "%d %49s %d", &id, name, &present) == 3) {
        float pct = (totalClasses > 0) ? (present * 100.0f / totalClasses) : 0.0f;
        printf("%-10d %-15s %-10d %-9.2f%%\n", id, name, present, pct);
    }

    fclose(mf);
    return 1;
}

int LowAttendance() {
    float threshold=75.0;
    

    FILE *fp = fopen("students.txt", "r");
    if (!fp) {
        printf("Error opening students.txt\n");
        return 0;
    }

    FILE *tclass = fopen("totalclasses.txt", "r");
    int totalClasses = 0;
    if (tclass) { fscanf(tclass, "%d", &totalClasses); fclose(tclass); }

    printf("\nStudents with attendance below %.2f%%:\n", threshold);
    printf("%-10s %-15s %-10s %-10s\n", "ID", "Name", "Attended", "Percentage");
    printf("---------------------------------------------------\n");

    int id, attended;
    char name[50];
    while (fscanf(fp, "%d %49s %d", &id, name, &attended) == 3) {
        float pct = (totalClasses > 0) ? (attended * 100.0f / totalClasses) : 0.0f;
        if (pct < threshold) {
            printf("%-10d %-15s %-10d %-9.2f%%\n", id, name, attended, pct);
        }
    }

    fclose(fp);
    return 1;
}

int ClassSummary() {
    FILE *fp = fopen("students.txt", "r");
    if (!fp) {
        printf("Error opening students.txt\n");
        return 0;
    }

    FILE *tclass = fopen("totalclasses.txt", "r");
    int totalClasses = 0;
    if (tclass) { fscanf(tclass, "%d", &totalClasses); fclose(tclass); }

    int totalStudents = 0, totalAttendance = 0;

    int id, attended;
    char name[50];
    while (fscanf(fp, "%d %49s %d", &id, name, &attended) == 3) {
        totalStudents++;
        totalAttendance += attended;
    }

    fclose(fp);

    float avgAttendance = (totalStudents > 0) ? (totalAttendance * 100.0f / (totalStudents * totalClasses)) : 0.0f;

    printf("\nClass Summary Report:\n");
    printf("Total Students: %d\n", totalStudents);
    printf("Total Classes Held: %d\n", totalClasses);
    printf("Average Attendance: %.2f%%\n", avgAttendance);

    return 1;
}

int Quickdata() {
    FILE *fp = fopen("students.txt", "a");
    if (fp == NULL) {
        printf("Error opening students.txt\n");
        return 0;
    }

    int n;
    printf("Enter number of students to add quickly: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid count.\n");
        fclose(fp);
        return 0;
    }

    int id;
    char name[50];

    for (int i = 0; i < n; i++) {
        printf("\nStudent %d\n", i + 1);

        printf("Enter student ID: ");
        scanf("%d", &id);

        printf("Enter student name: ");
        scanf("%49s", name);   

        fprintf(fp, "%d %s 0 \n", id, name);
    }

    fclose(fp);
    printf("\n----------------------------\n");
    printf("Quick addition completed for %d student(s).\n", n);
    return 1;
}

void reset() {
    FILE *fp = fopen("students.txt", "w");
    if (fp != NULL) {
        fclose(fp);
    }

    FILE *tclass = fopen("totalclasses.txt", "w");
    if (tclass != NULL) {
        fprintf(tclass, "0");
        fclose(tclass);
    }

    printf("Attendance data has been reset.\n");
}

int changeadmpass() {

    if (!admlogin()) return 0;

    char newPass[64], confirm[64];

    printf("Enter new admin password: ");
    scanf("%63s", newPass);

    printf("Confirm new admin password: ");
    scanf("%63s", confirm);

    if (strcmp(newPass, confirm) != 0) {
        printf("Passwords do not match.\n");
        return 0;
    }

    FILE *pf = fopen("adminpass.txt", "w");
    if (!pf) {
        printf("Error opening adminpass.txt for writing.\n");
        return 0;
    }
    fprintf(pf, "%s", newPass);
    fclose(pf);

    printf("Admin password changed successfully.\n");
    return 1;
}
void about() {
    printf("\nStudent Attendance System\n");
    printf("Developed by Raghav Vij aka me :)\n");
    printf("I am a Btech first year student at UPES Dehradun.\n");
    printf("I am in B1 Batch and my SAP ID is 590023842.\n");
    printf("I love coding and problem-solving.\n");
    printf("This project is developed as a part of my curriculum.\n");
    printf("Feel free to reach out for any queries or collaborations!\n");
    printf("You can Contact me at:+91-7717444915\n");
    printf("----------------------------\n");
    printf("Version 1.0\n");
    printf("This system helps manage student attendance records efficiently.\n");
    printf("Thank you for using the Student Attendance System!\n\n");
    printf("----------------------------\n");
}
void feedback() {
    char feedback[256];
    printf("Enter your feedback/suggestions(Please include your name also!):\n");
    getchar(); 
    fgets(feedback, sizeof(feedback), stdin);

    FILE *ff = fopen("feedback.txt", "a");
    if (ff) {
        fprintf(ff, "%s\n", feedback);
        fclose(ff);
        printf("Thank you for your feedback!\n");
    } else {
        printf("Error saving feedback.\n");
    }
}
void delstudentrec() {
    int delId;
    printf("Enter student ID to delete: ");
    scanf("%d", &delId);

    FILE *fp = fopen("students.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Error opening files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    int id, attended;
    char name[50];
    int found = 0;

    while (fscanf(fp, "%d %49s %d", &id, name, &attended) == 3) {
        if (id == delId) {
            found = 1;
            continue; 
        }
        fprintf(temp, "%d %s %d\n", id, name, attended);
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove("students.txt");
        rename("temp.txt", "students.txt");
        printf("Student with ID %d deleted successfully.\n", delId);
    } else {
        remove("temp.txt");
        printf("Student with ID %d not found.\n", delId);
    }
}
void editstudentrec() {
    int editId;
    printf("Enter student ID to update: ");
    scanf("%d", &editId);

    FILE *fp = fopen("students.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Error opening files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    int id, attended;
    char name[50];
    int found = 0;

    while (fscanf(fp, "%d %49s %d", &id, name, &attended) == 3) {
        if (id == editId) {
            found = 1;
            printf("Enter new name for student ID %d: ", id);
            scanf("%49s", name);
            printf("Enter new attendance count for student ID %d: ", id);
            scanf("%d", &attended);
        }
        fprintf(temp, "%d %s %d\n", id, name, attended);
    }

    fclose(fp);
    fclose(temp);

    if (found) {
        remove("students.txt");
        rename("temp.txt", "students.txt");
        printf("Student with ID %d updated successfully.\n", editId);
    } else {
        remove("temp.txt");
        printf("Student with ID %d not found.\n", editId);
    }
}
void help(){
    printf("\n--- Help / Instructions ---\n");
    printf("1. Add student (admin): Add a new student to the system. Requires admin login.\n");
    printf("2. View all students: Display a list of all students with their attendance records.\n");
    printf("3. Mark attendance for class (admin): Record attendance for a class session. Requires admin login.\n");
    printf("4. Generate monthly report (percentage): View attendance percentages for a specified month.\n");
    printf("5. Search students with low attendance: Find students whose attendance is below a certain threshold.\n");
    printf("6. Generate class summary report: Get an overview of class attendance statistics.\n");
    printf("7. Add students (quick) (admin): Quickly add multiple students. Requires admin login.\n");
    printf("8. Reset attendance data (admin): Clear all attendance records. Requires admin login.\n");
    printf("9. Change admin password (admin): Update the admin password. Requires current password.\n");
    printf("10. Backup data (admin): Backup attendance data to a secure location. (Feature not implemented yet)\n");
    printf("11. Restore data from backup (admin): Restore attendance data from a backup. (Feature not implemented yet)\n");
    printf("12. Delete student record (admin): Remove a student's record from the system. Requires admin login.\n");
    printf("13. Update student details (admin): Modify a student's information and attendance count. Requires admin login.\n");
    printf("14. View attendance history of a student: View detailed attendance history for a specific student. (Feature not implemented yet)\n");
    printf("15. Export reports to CSV (admin): Export attendance reports in CSV format. (Feature not implemented yet)\n");
    printf("16. Import student data from CSV (admin): Import student records from a CSV file. (Feature not implemented yet)\n");
    printf("17. Help/Instructions: View this help message.\n");
    printf("18. About: Learn more about the Student Attendance System and its developer.\n");
    printf("19. Feedback: Provide feedback or suggestions for improvement.\n");
    printf("0. Exit: Close the application.\n");
    printf("----------------------------\n\n");
    printf("\nFor admin functions, ensure you have the correct password.\n");
    printf("Contact the developer for any issues or support.\n");
    printf("----------------------------\n\n");
}

int backup(){
    FILE *fs, *fb;
    char ch;

    fs = fopen("students.txt", "r");
    fb = fopen("backup/students_backup.txt", "w");

    if (fs && fb) {
        while ((ch = fgetc(fs)) != EOF)
            fputc(ch, fb);
        fclose(fs);
        fclose(fb);
    }

    fs = fopen("totalclasses.txt", "r");
    fb = fopen("backup/totalclasses_backup.txt", "w");

    if (fs && fb) {
        while ((ch = fgetc(fs)) != EOF)
            fputc(ch, fb);
        fclose(fs);
        fclose(fb);
    }

    printf("\nBackup created successfully inside 'backup' folder.\n");

    return 1;
}
int restore(){
    FILE *fs, *fb;
    char ch;

    fb = fopen("backup/students_backup.txt", "r");
    fs = fopen("students.txt", "w");

    if (fs && fb) {
        while ((ch = fgetc(fb)) != EOF)
            fputc(ch, fs);
        fclose(fs);
        fclose(fb);
    }

    fb = fopen("backup/totalclasses_backup.txt", "r");
    fs = fopen("totalclasses.txt", "w");

    if (fs && fb) {
        while ((ch = fgetc(fb)) != EOF)
            fputc(ch, fs);
        fclose(fs);
        fclose(fb);
    }

    printf("\nData restored successfully from backup.\n");

    return 1;
}
int stuhist() {

    int searchId;
    printf("Enter student ID: ");
    scanf("%d", &searchId);

    FILE *fp = fopen("students.txt", "r");
    if (!fp) {
        printf("Error opening students.txt\n");
        return 0;
    }

    int id, attended;
    char name[50];
    int found = 0;

    while (fscanf(fp, "%d %49s %d", &id, name, &attended) == 3) {
        if (id == searchId) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("\nStudent not found.\n");
        return 0;
    }

    FILE *tclass = fopen("totalclasses.txt", "r");
    int totalClasses = 0;
    if (tclass) {
        fscanf(tclass, "%d", &totalClasses);
        fclose(tclass);
    }

    char month[8];
    printf("Enter month (YYYY-MM): ");
    scanf("%7s", month);

    char filename[60];
    strcpy(filename, "monthlydata\\");
    strcat(filename, month);
    strcat(filename, ".txt");

    FILE *mf = fopen(filename, "r");
    if (!mf) {
        printf("\nNo attendance record found for %s\n", month);
        return 0;
    }

    int mid, presentMonth;
    char stuName[50];

    printf("\nAttendance History for %s (ID: %d)\n", name, searchId);
    printf("--------------------------------------\n");

    char skipline[100];
    fgets(skipline, sizeof(skipline), mf);

    while (fscanf(mf, "%d %49s %d", &mid, stuName, &presentMonth) == 3) {
        if (mid == searchId) {
            printf("Month   : %s\n", month);
            printf("Present : %d / %d classes\n", presentMonth, totalClasses);
            fclose(mf);
            return 1;
        }
    }

    printf("\nNo attendance found for this month for this student.\n");
    fclose(mf);
    return 1;
}

int exportcsv() {
    
    char month[8];
    printf("Enter month (YYYY-MM): ");
    scanf("%7s", month);

    char inname[60];
    strcpy(inname, "monthlydata\\");
    strcat(inname, month);
    strcat(inname, ".txt");

    FILE *mf = fopen(inname, "r");
    if (!mf) {
        printf("No monthly file found: %s\n", inname);
        return 0;
    }

    FILE *tclass = fopen("totalclasses.txt", "r");
    int totalClasses = 0;
    if (tclass) {
        fscanf(tclass, "%d", &totalClasses);
        fclose(tclass);
    }
    char skipline[100];
    fgets(skipline, sizeof(skipline), mf);
    char outname[60];
    strcpy(outname, "exports\\");
    strcat(outname, month);
    strcat(outname, ".csv");

    FILE *out = fopen(outname, "w");
    if (!out) {
        printf("Cannot create: %s\n", outname);
        fclose(mf);
        return 0;
    }

    fprintf(out, "ID,Name,Present,Total,Percentage\n");

    int id, present;
    char name[50];

    while (fscanf(mf, "%d %49s %d", &id, name, &present) == 3) {
        float pct = (totalClasses > 0) ? (present * 100.0f / totalClasses) : 0.0f;
        fprintf(out, "%d,%s,%d,%d,%.2f\n", id, name, present, totalClasses, pct);
    }

    fclose(mf);
    fclose(out);

    printf("Exported CSV: %s\n", outname);
    return 1;
}

int importcsv() {

    char filename[50];
    printf("Enter CSV file name (example: data.csv): ");
    scanf("%49s", filename);

    FILE *csv = fopen(filename, "r");
    if (!csv) {
        printf("Error opening %s\n", filename);
        return 0;
    }

    FILE *fp = fopen("students.txt", "a");
    if (!fp) {
        printf("Error opening students.txt\n");
        fclose(csv);
        return 0;
    }

    int id, attended;
    char name[50];

    char skip[200];
    fgets(skip, sizeof(skip), csv);

    while (fscanf(csv, "%d,%49[^,],%d", &id, name, &attended) == 3) {
        fprintf(fp, "%d %s %d\n", id, name, attended);
    }

    fclose(csv);
    fclose(fp);

    printf("CSV Imported Successfully into students.txt\n");
    return 1;
}

int main(){
    int choice;
    printf("=== Student Attendance System ===\n");
    printf("~Developed by Raghav Vij\n");
    while (1) {
        printf("\nMenu:\n");
        printf("1. Add student (admin)\n");
        printf("2. View all students\n");
        printf("3. Mark attendance for class (admin)\n");
        printf("4. Generate monthly report (percentage)\n");
        printf("5. Search students with low attendance\n");
        printf("6. Generate class summary report\n");
        printf("7. Add students (quick)\n");
        printf("8. Reset attendance data (admin)\n");
        printf("9. Change admin password (admin)\n");
        printf("10. Backup data (admin)\n");
        printf("11. Restore data from backup (admin)\n");
        printf("12. Delete student record (admin)\n");
        printf("13. Update student details (admin)\n");
        printf("14. View attendance history of a student\n");
        printf("15. Export reports to CSV (admin)\n");
        printf("16. Import student data from CSV (admin)\n");
        printf("17. Help/Instructions\n");
        printf("18. About\n");
        printf("19. Feedback\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                if (admlogin()) 
                    {addstudent();}
                break;
            case 2:
                view();
                break;
            case 3:
                if (admlogin()) 
                    {MarkAttendance();}
                break;
            case 4:
                Monthlyreport();
                break;
            case 5:
                LowAttendance();
                break;
            case 6:
                ClassSummary();
                break;
            case 7:
                if (admlogin()) 
                    {Quickdata();}
                break;
            case 8:
                if (admlogin()) 
                    {reset();}
                break;
            case 9:
                changeadmpass();
                break;
            case 10:
                if (admlogin()) 
                    {backup();}
                break;
            case 11:
                if (admlogin()) 
                    {restore();}
                break;
            case 12:
                if (admlogin()) 
                    {delstudentrec();}
                break;
            case 13:
                if (admlogin()) 
                    {editstudentrec();}
                break;
            case 14:
                stuhist();
                break;
            case 15:
                if (admlogin()) 
                    {exportcsv();}
                break;
            case 16:
                 if (admlogin()) 
                     {importcsv();}
                 break;
            case 17:
                help();
                break;
            case 18:
                about();
                break;
            case 19:
                feedback();
                break;
            case 0:
                printf("Exiting. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
