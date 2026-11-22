#ifndef EXPORT_H
#define EXPORT_H

#include "expense.h"
#include "budget.h"

/**
 * Generates an interactive, standalone HTML financial dashboard (report.html)
 * complete with summary KPI cards, Chart.js category doughnut chart,
 * monthly spending bar chart, a table of the 10 most recent transactions,
 * and monthly budget utilization tracking.
 * Returns 1 on success, 0 on failure.
 */
int export_html_dashboard(const char *filename,
                          const Expense expenses[], int expense_count,
                          const Budget budgets[], int budget_count);

#endif // EXPORT_H
