#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LENGTH 16
#define MAX_NAME_LENGTH 64
#define LINE_BUFFER_SIZE 256

typedef struct {
    char id[MAX_ID_LENGTH];
    char name[MAX_NAME_LENGTH];
    double lab1;
    double lab2;
    double midterm;
    double final_exam;
    double total;
    char letter_grade[3];
} Student;

typedef struct {
    Student *items;
    size_t size;
    size_t capacity;
} StudentList;

void initialize_list(StudentList *list);
void free_list(StudentList *list);
int add_student(StudentList *list, Student student);
int load_students_from_csv(const char *filename, StudentList *list);
void calculate_grade(Student *student);
const char *letter_grade_from_total(double total);
void print_menu(void);
void print_student_table(const StudentList *list);
void print_summary(const StudentList *list);
void search_by_id(const StudentList *list);
void sort_by_total_descending(StudentList *list);
void save_report(const StudentList *list, const char *filename);
void read_line(char *buffer, size_t size);
void trim_newline(char *text);
int compare_total_descending(const void *a, const void *b);

int main(int argc, char *argv[]) {
    const char *input_file = "sample_students.csv";
    StudentList students;
    char choice[8];
    int running = 1;

    if (argc > 1) {
        input_file = argv[1];
    }

    initialize_list(&students);

    if (!load_students_from_csv(input_file, &students)) {
        fprintf(stderr, "Could not load student data from '%s'.\n", input_file);
        fprintf(stderr, "Usage: ./gradebook sample_students.csv\n");
        free_list(&students);
        return EXIT_FAILURE;
    }

    printf("Loaded %zu student record(s) from %s.\n", students.size, input_file);

    while (running) {
        print_menu();
        printf("Enter your choice: ");
        read_line(choice, sizeof(choice));

        switch (choice[0]) {
            case '1':
                print_student_table(&students);
                break;
            case '2':
                print_summary(&students);
                break;
            case '3':
                search_by_id(&students);
                break;
            case '4':
                sort_by_total_descending(&students);
                printf("Students sorted by total grade, highest to lowest.\n");
                break;
            case '5':
                save_report(&students, "grade_report.txt");
                break;
            case '0':
                running = 0;
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }

    free_list(&students);
    printf("Goodbye!\n");
    return EXIT_SUCCESS;
}

void initialize_list(StudentList *list) {
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

void free_list(StudentList *list) {
    free(list->items);
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

int add_student(StudentList *list, Student student) {
    Student *resized_items;
    size_t new_capacity;

    if (list->size == list->capacity) {
        new_capacity = (list->capacity == 0) ? 4 : list->capacity * 2;
        resized_items = realloc(list->items, new_capacity * sizeof(Student));

        if (resized_items == NULL) {
            return 0;
        }

        list->items = resized_items;
        list->capacity = new_capacity;
    }

    list->items[list->size] = student;
    list->size++;
    return 1;
}

int load_students_from_csv(const char *filename, StudentList *list) {
    FILE *file = fopen(filename, "r");
    char line[LINE_BUFFER_SIZE];
    int line_number = 0;

    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        Student student;
        int fields_read;
        line_number++;
        trim_newline(line);

        if (line_number == 1 && strstr(line, "id") != NULL) {
            continue;
        }

        if (strlen(line) == 0) {
            continue;
        }

        fields_read = sscanf(
            line,
            " %15[^,],%63[^,],%lf,%lf,%lf,%lf",
            student.id,
            student.name,
            &student.lab1,
            &student.lab2,
            &student.midterm,
            &student.final_exam
        );

        if (fields_read != 6) {
            fprintf(stderr, "Skipping invalid CSV line %d: %s\n", line_number, line);
            continue;
        }

        calculate_grade(&student);

        if (!add_student(list, student)) {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return list->size > 0;
}

void calculate_grade(Student *student) {
    student->total =
        (student->lab1 * 0.10) +
        (student->lab2 * 0.10) +
        (student->midterm * 0.30) +
        (student->final_exam * 0.50);

    strncpy(student->letter_grade, letter_grade_from_total(student->total), sizeof(student->letter_grade));
    student->letter_grade[sizeof(student->letter_grade) - 1] = '\0';
}

const char *letter_grade_from_total(double total) {
    if (total >= 90.0) return "A+";
    if (total >= 85.0) return "A";
    if (total >= 80.0) return "A-";
    if (total >= 77.0) return "B+";
    if (total >= 73.0) return "B";
    if (total >= 70.0) return "B-";
    if (total >= 65.0) return "C+";
    if (total >= 60.0) return "C";
    if (total >= 50.0) return "D";
    return "F";
}

void print_menu(void) {
    printf("\n=== C Gradebook Analyzer ===\n");
    printf("1. Show all students\n");
    printf("2. Show class summary\n");
    printf("3. Search student by ID\n");
    printf("4. Sort by total grade\n");
    printf("5. Save report to grade_report.txt\n");
    printf("0. Exit\n");
}

void print_student_table(const StudentList *list) {
    size_t i;

    printf("\n%-10s %-22s %6s %6s %8s %8s %8s %6s\n",
           "ID", "Name", "Lab1", "Lab2", "Midterm", "Final", "Total", "Grade");
    printf("--------------------------------------------------------------------------------\n");

    for (i = 0; i < list->size; i++) {
        const Student *s = &list->items[i];
        printf("%-10s %-22s %6.1f %6.1f %8.1f %8.1f %8.1f %6s\n",
               s->id, s->name, s->lab1, s->lab2, s->midterm, s->final_exam, s->total, s->letter_grade);
    }
}

void print_summary(const StudentList *list) {
    double sum = 0.0;
    double highest;
    double lowest;
    size_t i;

    if (list->size == 0) {
        printf("No students available.\n");
        return;
    }

    highest = list->items[0].total;
    lowest = list->items[0].total;

    for (i = 0; i < list->size; i++) {
        double total = list->items[i].total;
        sum += total;
        if (total > highest) highest = total;
        if (total < lowest) lowest = total;
    }

    printf("\nClass summary\n");
    printf("Students: %zu\n", list->size);
    printf("Average:  %.2f\n", sum / list->size);
    printf("Highest:  %.2f\n", highest);
    printf("Lowest:   %.2f\n", lowest);
}

void search_by_id(const StudentList *list) {
    char target_id[MAX_ID_LENGTH];
    size_t i;

    printf("Enter student ID: ");
    read_line(target_id, sizeof(target_id));

    for (i = 0; i < list->size; i++) {
        const Student *s = &list->items[i];
        if (strcmp(s->id, target_id) == 0) {
            printf("\nFound student:\n");
            printf("Name: %s\n", s->name);
            printf("Lab 1: %.1f, Lab 2: %.1f, Midterm: %.1f, Final: %.1f\n",
                   s->lab1, s->lab2, s->midterm, s->final_exam);
            printf("Total: %.1f, Letter Grade: %s\n", s->total, s->letter_grade);
            return;
        }
    }

    printf("No student found with ID '%s'.\n", target_id);
}

void sort_by_total_descending(StudentList *list) {
    qsort(list->items, list->size, sizeof(Student), compare_total_descending);
}

int compare_total_descending(const void *a, const void *b) {
    const Student *student_a = (const Student *)a;
    const Student *student_b = (const Student *)b;

    if (student_a->total < student_b->total) return 1;
    if (student_a->total > student_b->total) return -1;
    return strcmp(student_a->name, student_b->name);
}

void save_report(const StudentList *list, const char *filename) {
    FILE *file = fopen(filename, "w");
    size_t i;

    if (file == NULL) {
        printf("Could not create report file.\n");
        return;
    }

    fprintf(file, "C Gradebook Analyzer Report\n");
    fprintf(file, "===========================\n\n");
    fprintf(file, "%-10s %-22s %8s %6s\n", "ID", "Name", "Total", "Grade");
    fprintf(file, "------------------------------------------------\n");

    for (i = 0; i < list->size; i++) {
        const Student *s = &list->items[i];
        fprintf(file, "%-10s %-22s %8.1f %6s\n", s->id, s->name, s->total, s->letter_grade);
    }

    fclose(file);
    printf("Report saved to %s.\n", filename);
}

void read_line(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    trim_newline(buffer);
}

void trim_newline(char *text) {
    size_t length = strlen(text);
    while (length > 0 && (text[length - 1] == '\n' || text[length - 1] == '\r')) {
        text[length - 1] = '\0';
        length--;
    }
}
