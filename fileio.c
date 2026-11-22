#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fileio.h"

/**
 * Saves all expense records to a CSV file.
 * Writes a standard header row followed by comma-separated records.
 * Returns 1 on success, 0 on failure.
 */
int save_expenses_to_csv(const char *filename, const Expense expenses[], int count) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("  [!] Error: Unable to open file '%s' for writing.\n", filename);
        return 0;
    }

    // Write CSV header
    fprintf(fp, "id,date,category,amount,note\n");

    // Write each expense record
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d,%s,%s,%.2f,%s\n",
                expenses[i].id,
                expenses[i].date,
                expenses[i].category,
                expenses[i].amount,
                expenses[i].note);
    }

    fclose(fp);
    return 1;
}

/**
 * Loads expense records from a CSV file into the expenses array.
 * Safely parses each line using comma delimiters, validates fields,
 * and skips headers or malformed records.
 * Returns the number of loaded records, or -1 if the file cannot be opened.
 */
int load_expenses_from_csv(const char *filename, Expense expenses[], int *count) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        // File does not exist yet; normal on initial launch
        return -1;
    }

    char line[256];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        // Strip trailing newline characters
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }

        // Skip blank lines
        if (len == 0) continue;

        // Skip CSV header line if present
        if (strncmp(line, "id,", 3) == 0 || strncmp(line, "ID,", 3) == 0) {
            continue;
        }

        if (*count >= MAX_EXPENSES) {
            printf("  [!] Warning: Reached maximum expense capacity (%d).\n", MAX_EXPENSES);
            break;
        }

        // Parse comma-separated fields: id,date,category,amount,note
        char *p1 = strchr(line, ',');
        if (!p1) continue;
        char *p2 = strchr(p1 + 1, ',');
        if (!p2) continue;
        char *p3 = strchr(p2 + 1, ',');
        if (!p3) continue;
        char *p4 = strchr(p3 + 1, ',');
        if (!p4) continue;

        // Null-terminate substrings
        *p1 = '\0';
        *p2 = '\0';
        *p3 = '\0';
        *p4 = '\0';

        char *str_id = line;
        char *str_date = p1 + 1;
        char *str_category = p2 + 1;
        char *str_amount = p3 + 1;
        char *str_note = p4 + 1;

        // Convert and populate Expense struct
        Expense exp;
        exp.id = atoi(str_id);
        if (exp.id <= 0) continue;

        strncpy(exp.date, str_date, DATE_LEN - 1);
        exp.date[DATE_LEN - 1] = '\0';

        strncpy(exp.category, str_category, CATEGORY_LEN - 1);
        exp.category[CATEGORY_LEN - 1] = '\0';

        exp.amount = atof(str_amount);
        if (exp.amount <= 0.0) continue;

        strncpy(exp.note, str_note, NOTE_LEN - 1);
        exp.note[NOTE_LEN - 1] = '\0';

        expenses[*count] = exp;
        (*count)++;
        loaded++;
    }

    fclose(fp);
    return loaded;
}

/**
 * Saves all monthly budget records to a binary file (budget.dat).
 * Writes record count followed by array of Budget structures using fwrite().
 * Returns 1 on success, 0 on failure.
 */
int save_budgets_to_file(const char *filename, const Budget budgets[], int count) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("  [!] Error: Unable to open '%s' for writing.\n", filename);
        return 0;
    }

    // Write count of records first
    if (fwrite(&count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }

    // Write Budget structs
    if (count > 0) {
        if (fwrite(budgets, sizeof(Budget), (size_t)count, fp) != (size_t)count) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

/**
 * Loads monthly budget records from a binary file (budget.dat) using fread().
 * Populates the budgets array with existing records.
 * Returns the number of loaded records, or -1 if the file cannot be opened.
 */
int load_budgets_from_file(const char *filename, Budget budgets[], int *count) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        // File does not exist yet; normal on initial launch
        return -1;
    }

    int file_count = 0;
    if (fread(&file_count, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    if (file_count < 0 || file_count > MAX_BUDGETS) {
        fclose(fp);
        return -1;
    }

    size_t read_items = fread(budgets, sizeof(Budget), (size_t)file_count, fp);
    fclose(fp);

    *count = (int)read_items;
    return *count;
}
