#ifndef EXPENSE_H
#define EXPENSE_H

// Maximum limits and buffer sizes
#define MAX_EXPENSES    1000
#define DATE_LEN        11      // "YYYY-MM-DD" + '\0'
#define MONTH_LEN       8       // "YYYY-MM" + '\0'
#define CATEGORY_LEN    25
#define NOTE_LEN        100
#define NUM_CATEGORIES  7

// Array of predefined category names accessible across modules
extern const char *CATEGORIES[NUM_CATEGORIES];

// Expense structure representing a single financial record
typedef struct {
    int id;                      // Unique sequential identifier
    char date[DATE_LEN];         // Date in YYYY-MM-DD format
    char category[CATEGORY_LEN]; // Category name from predefined list
    double amount;               // Expense amount in currency units
    char note[NOTE_LEN];         // Brief description or memo
} Expense;

// Core expense management functions
int get_next_expense_id(const Expense expenses[], int count);
int add_expense(Expense expenses[], int *count);
void view_all_expenses(const Expense expenses[], int count);
int edit_expense(Expense expenses[], int count);
int delete_expense(Expense expenses[], int *count);
int find_expense_index_by_id(const Expense expenses[], int count, int id);
void display_categories(void);

// Input validation and helper functions
int is_valid_date(const char *date);
int is_valid_month(const char *month);
int read_line(char *buffer, int max_len);
int read_int_range(int *out_val, int min_val, int max_val);
int read_positive_double(double *out_val);

#endif // EXPENSE_H
