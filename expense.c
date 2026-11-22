#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expense.h"

// List of the 7 predefined expense categories
const char *CATEGORIES[NUM_CATEGORIES] = {
    "Food",
    "Travel",
    "Shopping",
    "Bills",
    "Entertainment",
    "Health",
    "Other"
};

/**
 * Safely reads a line of text from standard input.
 * Strips trailing newline characters and flushes any leftover input
 * in the buffer if the user input exceeded the buffer size.
 */
int read_line(char *buffer, int max_len) {
    if (!fgets(buffer, max_len, stdin)) {
        buffer[0] = '\0';
        return 0;
    }

    // Check if newline character is present
    size_t len = strlen(buffer);
    if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        // Strip trailing newline/carriage return
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
            buffer[len - 1] = '\0';
            len--;
        }
    } else {
        // Line was longer than buffer, flush the rest of the line
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            // Discard remaining characters
        }
    }
    return 1;
}

/**
 * Prompts the user to enter an integer within a specified [min_val, max_val] range.
 * Continues prompting until valid numeric input within the bounds is provided.
 */
int read_int_range(int *out_val, int min_val, int max_val) {
    char line[64];
    while (1) {
        if (!read_line(line, sizeof(line)) || strlen(line) == 0) {
            printf("  Error: Input cannot be empty. Please enter a number (%d - %d): ", min_val, max_val);
            continue;
        }

        char *endptr;
        long val = strtol(line, &endptr, 10);

        // Skip whitespace after number
        while (isspace((unsigned char)*endptr)) endptr++;

        if (*endptr != '\0') {
            printf("  Error: Non-numeric input detected. Enter a number (%d - %d): ", min_val, max_val);
            continue;
        }

        if (val < min_val || val > max_val) {
            printf("  Error: Out of range. Please enter a number between %d and %d: ", min_val, max_val);
            continue;
        }

        *out_val = (int)val;
        return 1;
    }
}

/**
 * Prompts the user to enter a positive double amount (greater than 0.0).
 * Validates that the input is purely numeric and strictly positive.
 */
int read_positive_double(double *out_val) {
    char line[64];
    while (1) {
        if (!read_line(line, sizeof(line)) || strlen(line) == 0) {
            printf("  Error: Amount cannot be empty. Enter positive amount (Rs.): ");
            continue;
        }

        char *endptr;
        double val = strtod(line, &endptr);

        // Skip trailing whitespace
        while (isspace((unsigned char)*endptr)) endptr++;

        if (*endptr != '\0') {
            printf("  Error: Invalid numeric format. Enter positive amount (e.g., 25.50): Rs. ");
            continue;
        }

        if (val <= 0.0) {
            printf("  Error: Amount must be strictly greater than 0. Enter positive amount (Rs.): ");
            continue;
        }

        *out_val = val;
        return 1;
    }
}

/**
 * Checks whether a given year is a leap year.
 * Returns 1 if leap year, 0 otherwise.
 */
static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
 * Validates whether a date string conforms to the 'YYYY-MM-DD' calendar format.
 * Checks string length, hyphen positions, digit characters, and valid day counts.
 * Returns 1 if valid, 0 otherwise.
 */
int is_valid_date(const char *date) {
    if (date == NULL || strlen(date) != 10) {
        return 0;
    }

    // Format must be YYYY-MM-DD
    if (date[4] != '-' || date[7] != '-') {
        return 0;
    }

    // Verify all other characters are digits
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) {
            return 0;
        }
    }

    int year, month, day;
    if (sscanf(date, "%4d-%2d-%2d", &year, &month, &day) != 3) {
        return 0;
    }

    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;

    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (is_leap_year(year)) {
        days_in_month[1] = 29;
    }

    if (day < 1 || day > days_in_month[month - 1]) {
        return 0;
    }

    return 1;
}

/**
 * Validates whether a month string conforms to the 'YYYY-MM' format.
 * Returns 1 if valid, 0 otherwise.
 */
int is_valid_month(const char *month) {
    if (month == NULL || strlen(month) != 7) {
        return 0;
    }

    if (month[4] != '-') {
        return 0;
    }

    for (int i = 0; i < 7; i++) {
        if (i == 4) continue;
        if (!isdigit((unsigned char)month[i])) {
            return 0;
        }
    }

    int year, m;
    if (sscanf(month, "%4d-%2d", &year, &m) != 2) {
        return 0;
    }

    if (year < 1900 || year > 2100) return 0;
    if (m < 1 || m > 12) return 0;

    return 1;
}

/**
 * Displays the list of predefined categories with numbers for user selection.
 */
void display_categories(void) {
    printf("  Available Categories:\n");
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        printf("    [%d] %-15s", i + 1, CATEGORIES[i]);
        if ((i + 1) % 3 == 0 || i == NUM_CATEGORIES - 1) {
            printf("\n");
        }
    }
}

/**
 * Calculates the next unique sequential ID for a new expense record.
 * Finds the current maximum ID in the array and adds 1.
 */
int get_next_expense_id(const Expense expenses[], int count) {
    int max_id = 0;
    for (int i = 0; i < count; i++) {
        if (expenses[i].id > max_id) {
            max_id = expenses[i].id;
        }
    }
    return max_id + 1;
}

/**
 * Linear search to find the array index of an expense by its ID.
 * Returns the 0-based array index, or -1 if not found.
 */
int find_expense_index_by_id(const Expense expenses[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (expenses[i].id == id) {
            return i;
        }
    }
    return -1;
}

/**
 * Prompts the user to enter details for a new expense and appends it to the array.
 * Validates date, category selection, positive amount, and note text.
 * Returns 1 on success, 0 if capacity limit is reached.
 */
int add_expense(Expense expenses[], int *count) {
    if (*count >= MAX_EXPENSES) {
        printf("\n  [!] Error: Expense storage is full (maximum %d records).\n", MAX_EXPENSES);
        return 0;
    }

    printf("\n=======================================================\n");
    printf("                  ADD NEW EXPENSE                      \n");
    printf("=======================================================\n");

    Expense new_exp;
    new_exp.id = get_next_expense_id(expenses, *count);
    printf("  Generated Expense ID : #%d\n", new_exp.id);

    // 1. Enter Date
    char input_date[32];
    while (1) {
        printf("  Enter Date (YYYY-MM-DD): ");
        read_line(input_date, sizeof(input_date));
        if (is_valid_date(input_date)) {
            strncpy(new_exp.date, input_date, DATE_LEN - 1);
            new_exp.date[DATE_LEN - 1] = '\0';
            break;
        }
        printf("  [!] Invalid date format or calendar date. Example: 2026-11-19\n");
    }

    // 2. Select Category
    printf("\n");
    display_categories();
    printf("  Select Category (1-%d): ", NUM_CATEGORIES);
    int cat_choice;
    read_int_range(&cat_choice, 1, NUM_CATEGORIES);
    strncpy(new_exp.category, CATEGORIES[cat_choice - 1], CATEGORY_LEN - 1);
    new_exp.category[CATEGORY_LEN - 1] = '\0';

    // 3. Enter Amount
    printf("  Enter Amount (Rs.): ");
    read_positive_double(&new_exp.amount);

    // 4. Enter Note
    printf("  Enter Note / Description: ");
    read_line(new_exp.note, NOTE_LEN);
    if (strlen(new_exp.note) == 0) {
        strcpy(new_exp.note, "General");
    }

    // Save into array
    expenses[*count] = new_exp;
    (*count)++;

    printf("\n  [+] Expense #%d added successfully!\n", new_exp.id);
    return 1;
}

/**
 * Displays all recorded expenses in an aligned tabular console view.
 * If no expenses exist, prints an informative message.
 */
void view_all_expenses(const Expense expenses[], int count) {
    printf("\n========================================================================================\n");
    printf("                                   ALL EXPENSES RECORDED                                \n");
    printf("========================================================================================\n");

    if (count == 0) {
        printf("  No expenses recorded yet. Use Option 1 to add your first expense!\n");
        printf("========================================================================================\n");
        return;
    }

    printf(" %-5s | %-10s | %-15s | %11s | %-32s\n", "ID", "Date", "Category", "Amount(Rs.)", "Note");
    printf("-------+------------+-----------------+------------+------------------------------------\n");

    double grand_total = 0.0;
    for (int i = 0; i < count; i++) {
        printf(" #%-4d | %-10s | %-15s | %10.2f | %-32s\n",
               expenses[i].id,
               expenses[i].date,
               expenses[i].category,
               expenses[i].amount,
               expenses[i].note);
        grand_total += expenses[i].amount;
    }

    printf("-------+------------+-----------------+------------+------------------------------------\n");
    printf(" TOTAL | %d record(s)%*s | %10.2f |\n", count, 18, "", grand_total);
    printf("========================================================================================\n");
}

/**
 * Edits an existing expense by its unique ID.
 * Allows the user to modify Date, Category, Amount, or Note.
 * Returns 1 on success, 0 if cancelled or not found.
 */
int edit_expense(Expense expenses[], int count) {
    if (count == 0) {
        printf("\n  [!] No expenses available to edit.\n");
        return 0;
    }

    printf("\n=======================================================\n");
    printf("                    EDIT EXPENSE                       \n");
    printf("=======================================================\n");
    printf("  Enter the Expense ID to edit (0 to cancel): ");
    
    int id;
    read_int_range(&id, 0, 999999);
    if (id == 0) {
        printf("  Edit operation cancelled.\n");
        return 0;
    }

    int idx = find_expense_index_by_id(expenses, count, id);
    if (idx == -1) {
        printf("  [!] Expense ID #%d not found.\n", id);
        return 0;
    }

    printf("\n  Current Record Details:\n");
    printf("  --------------------------------------\n");
    printf("  ID       : #%d\n", expenses[idx].id);
    printf("  Date     : %s\n", expenses[idx].date);
    printf("  Category : %s\n", expenses[idx].category);
    printf("  Amount   : Rs. %.2f\n", expenses[idx].amount);
    printf("  Note     : %s\n", expenses[idx].note);
    printf("  --------------------------------------\n");

    printf("\n  Select field to modify:\n");
    printf("    [1] Date\n");
    printf("    [2] Category\n");
    printf("    [3] Amount\n");
    printf("    [4] Note\n");
    printf("    [5] Cancel\n");
    printf("  Enter choice (1-5): ");

    int choice;
    read_int_range(&choice, 1, 5);

    switch (choice) {
        case 1: {
            char new_date[32];
            while (1) {
                printf("  Enter New Date (YYYY-MM-DD): ");
                read_line(new_date, sizeof(new_date));
                if (is_valid_date(new_date)) {
                    strncpy(expenses[idx].date, new_date, DATE_LEN - 1);
                    expenses[idx].date[DATE_LEN - 1] = '\0';
                    break;
                }
                printf("  [!] Invalid date format. Example: 2026-11-19\n");
            }
            printf("  [+] Date updated successfully!\n");
            break;
        }
        case 2: {
            display_categories();
            printf("  Select New Category (1-%d): ", NUM_CATEGORIES);
            int cat_choice;
            read_int_range(&cat_choice, 1, NUM_CATEGORIES);
            strncpy(expenses[idx].category, CATEGORIES[cat_choice - 1], CATEGORY_LEN - 1);
            expenses[idx].category[CATEGORY_LEN - 1] = '\0';
            printf("  [+] Category updated to '%s'!\n", expenses[idx].category);
            break;
        }
        case 3: {
            printf("  Enter New Amount (Rs.): ");
            read_positive_double(&expenses[idx].amount);
            printf("  [+] Amount updated to Rs. %.2f!\n", expenses[idx].amount);
            break;
        }
        case 4: {
            printf("  Enter New Note / Description: ");
            read_line(expenses[idx].note, NOTE_LEN);
            if (strlen(expenses[idx].note) == 0) {
                strcpy(expenses[idx].note, "General");
            }
            printf("  [+] Note updated successfully!\n");
            break;
        }
        case 5:
        default:
            printf("  Edit operation cancelled.\n");
            return 0;
    }

    return 1;
}

/**
 * Deletes an existing expense by its unique ID.
 * Asks for user confirmation before shifting subsequent records left.
 * Returns 1 on success, 0 if cancelled or not found.
 */
int delete_expense(Expense expenses[], int *count) {
    if (*count == 0) {
        printf("\n  [!] No expenses available to delete.\n");
        return 0;
    }

    printf("\n=======================================================\n");
    printf("                   DELETE EXPENSE                      \n");
    printf("=======================================================\n");
    printf("  Enter the Expense ID to delete (0 to cancel): ");

    int id;
    read_int_range(&id, 0, 999999);
    if (id == 0) {
        printf("  Delete operation cancelled.\n");
        return 0;
    }

    int idx = find_expense_index_by_id(expenses, *count, id);
    if (idx == -1) {
        printf("  [!] Expense ID #%d not found.\n", id);
        return 0;
    }

    printf("\n  Target Record to Delete:\n");
    printf("  #%d | %s | %s | Rs. %.2f | %s\n",
           expenses[idx].id,
           expenses[idx].date,
           expenses[idx].category,
           expenses[idx].amount,
           expenses[idx].note);

    printf("  Are you sure you want to delete this expense? (1 = Yes, 2 = No): ");
    int confirm;
    read_int_range(&confirm, 1, 2);

    if (confirm != 1) {
        printf("  Deletion aborted.\n");
        return 0;
    }

    // Shift elements left to overwrite deleted item and maintain array continuity
    for (int i = idx; i < *count - 1; i++) {
        expenses[i] = expenses[i + 1];
    }
    (*count)--;

    printf("  [+] Expense #%d deleted successfully.\n", id);
    return 1;
}
