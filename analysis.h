#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "expense.h"

// Structure to hold category summary aggregation
typedef struct {
    char name[CATEGORY_LEN];
    double total;
    double percentage;
    int count;
} CategorySummary;

// Aggregates totals and percentage shares across categories
void calculate_category_summary(const Expense expenses[], int expense_count,
                                const char *filter_month,
                                CategorySummary summaries[NUM_CATEGORIES],
                                double *grand_total);

// Interactive view to display category summary table (all-time or filtered by month)
void view_category_summary_menu(const Expense expenses[], int expense_count);

// Performs comprehensive spending pattern analysis
void run_spending_pattern_analysis(const Expense expenses[], int expense_count);

#endif // ANALYSIS_H
