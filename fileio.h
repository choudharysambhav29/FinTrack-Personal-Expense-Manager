#ifndef FILEIO_H
#define FILEIO_H

#include "expense.h"
#include "budget.h"

// File persistence functions for expenses (CSV format)
int save_expenses_to_csv(const char *filename, const Expense expenses[], int count);
int load_expenses_from_csv(const char *filename, Expense expenses[], int *count);

// File persistence functions for budgets (Binary .dat format)
int save_budgets_to_file(const char *filename, const Budget budgets[], int count);
int load_budgets_from_file(const char *filename, Budget budgets[], int *count);

#endif // FILEIO_H
