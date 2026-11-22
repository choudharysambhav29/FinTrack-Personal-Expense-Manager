#ifndef BUDGET_H
#define BUDGET_H

#include "expense.h"

#define MAX_BUDGETS 120    // Capacity for monthly budgets (up to 10 years)

// Budget structure for a specific calendar month
typedef struct {
    char month[MONTH_LEN]; // Target month formatted as "YYYY-MM"
    double limit;          // Spending limit for this month
} Budget;

// Core budget management functions
int find_budget_index(const Budget budgets[], int count, const char *month);
double calculate_monthly_spent(const Expense expenses[], int expense_count, const char *month);
int set_monthly_budget(Budget budgets[], int *budget_count, const Expense expenses[], int expense_count);
void check_monthly_budget_status(const Budget budgets[], int budget_count, const Expense expenses[], int expense_count);
void view_all_budgets(const Budget budgets[], int budget_count, const Expense expenses[], int expense_count);

#endif // BUDGET_H
