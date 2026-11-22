#ifndef CHART_H
#define CHART_H

#include "expense.h"

// Maximum width in characters for horizontal ASCII bars
#define MAX_BAR_WIDTH 40

// Draws a horizontal ASCII bar chart of spending per category scaled to the largest category
void draw_category_bar_chart(const Expense expenses[], int expense_count);

// Draws a horizontal ASCII bar chart of total spending per month ordered chronologically
void draw_monthly_trend_chart(const Expense expenses[], int expense_count);

#endif // CHART_H
