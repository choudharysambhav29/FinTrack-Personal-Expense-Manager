#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chart.h"
#include "analysis.h"

/**
 * Draws a horizontal ASCII bar chart of spending per category using '#' characters.
 * The bars are dynamically scaled so the category with the highest spending fills
 * MAX_BAR_WIDTH (40 characters). Displays amount and percentage next to each bar.
 * Supports viewing all-time spending or filtering by a chosen month (YYYY-MM).
 */
void draw_category_bar_chart(const Expense expenses[], int expense_count) {
    if (expense_count == 0) {
        printf("\n  [!] No expenses recorded yet. Cannot generate category chart.\n");
        return;
    }

    printf("\n=======================================================\n");
    printf("              CATEGORY SPENDING ASCII CHART            \n");
    printf("=======================================================\n");
    printf("  [1] All-Time Category Chart\n");
    printf("  [2] Filter Chart by Month (YYYY-MM)\n");
    printf("  [3] Back to Main Menu\n");
    printf("=======================================================\n");
    printf("  Enter choice (1-3): ");

    int choice;
    read_int_range(&choice, 1, 3);
    if (choice == 3) {
        return;
    }

    char filter_month[16];
    char *month_ptr = NULL;

    if (choice == 2) {
        while (1) {
            printf("  Enter Month to Filter (YYYY-MM, 0 to cancel): ");
            read_line(filter_month, sizeof(filter_month));
            if (strcmp(filter_month, "0") == 0) {
                printf("  Operation cancelled.\n");
                return;
            }
            if (is_valid_month(filter_month)) {
                month_ptr = filter_month;
                break;
            }
            printf("  [!] Invalid format. Example: 2026-11\n");
        }
    }

    CategorySummary summaries[NUM_CATEGORIES];
    double grand_total = 0.0;
    calculate_category_summary(expenses, expense_count, month_ptr, summaries, &grand_total);

    // Find the maximum category total to scale the bars
    double max_category_val = 0.0;
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        if (summaries[i].total > max_category_val) {
            max_category_val = summaries[i].total;
        }
    }

    printf("\n========================================================================================\n");
    if (month_ptr != NULL) {
        printf("               CATEGORY SPENDING BAR CHART FOR MONTH: %-10s               \n", month_ptr);
    } else {
        printf("                       ALL-TIME CATEGORY SPENDING BAR CHART                             \n");
    }
    printf("   Scale: Max category bar = %d characters ('#')                                      \n", MAX_BAR_WIDTH);
    printf("========================================================================================\n");
    printf(" %-14s | %-42s | %-20s\n", "Category", "Relative Spending Bar", "Amount & Share");
    printf("---------------+--------------------------------------------+---------------------------\n");

    if (grand_total <= 0.0 || max_category_val <= 0.0) {
        for (int i = 0; i < NUM_CATEGORIES; i++) {
            printf(" %-14s | [%-40s] | Rs. %10.2f (  0.0%%)\n", summaries[i].name, "", 0.0);
        }
        printf("---------------+--------------------------------------------+---------------------------\n");
        printf(" [i] Notice: No expenses recorded for this period.\n");
        printf("========================================================================================\n");
        return;
    }

    for (int i = 0; i < NUM_CATEGORIES; i++) {
        int bar_len = 0;
        if (max_category_val > 0.0 && summaries[i].total > 0.0) {
            bar_len = (int)((summaries[i].total / max_category_val) * MAX_BAR_WIDTH);
            if (bar_len == 0) {
                // Ensure small non-zero amounts show at least 1 character
                bar_len = 1;
            }
            if (bar_len > MAX_BAR_WIDTH) {
                bar_len = MAX_BAR_WIDTH;
            }
        }

        // Build bar string
        char bar_buf[MAX_BAR_WIDTH + 1];
        for (int b = 0; b < bar_len; b++) {
            bar_buf[b] = '#';
        }
        bar_buf[bar_len] = '\0';

        printf(" %-14s | [%-40s] | Rs. %10.2f (%5.1f%%)\n",
               summaries[i].name,
               bar_buf,
               summaries[i].total,
               summaries[i].percentage);
    }

    printf("---------------+--------------------------------------------+---------------------------\n");
    printf(" TOTAL SPEND   | %-42s | Rs. %10.2f (100.0%%)\n", "", grand_total);
    printf("========================================================================================\n");
}

/**
 * Draws an ASCII bar chart of total spending per month, ordered chronologically.
 * Aggregates all expenses by YYYY-MM, sorts them in ascending date order,
 * scales the bars relative to the highest monthly spend, and prints amounts.
 */
void draw_monthly_trend_chart(const Expense expenses[], int expense_count) {
    if (expense_count == 0) {
        printf("\n  [!] No expenses recorded yet. Cannot generate monthly trend chart.\n");
        return;
    }

    // Extract unique months and accumulate totals
    char months[MAX_EXPENSES][MONTH_LEN];
    double month_totals[MAX_EXPENSES];
    int month_count = 0;

    for (int i = 0; i < expense_count; i++) {
        char exp_m[MONTH_LEN];
        strncpy(exp_m, expenses[i].date, 7);
        exp_m[7] = '\0';

        // Check if month already recorded
        int found_idx = -1;
        for (int m = 0; m < month_count; m++) {
            if (strcmp(months[m], exp_m) == 0) {
                found_idx = m;
                break;
            }
        }

        if (found_idx != -1) {
            month_totals[found_idx] += expenses[i].amount;
        } else {
            strncpy(months[month_count], exp_m, MONTH_LEN - 1);
            months[month_count][MONTH_LEN - 1] = '\0';
            month_totals[month_count] = expenses[i].amount;
            month_count++;
        }
    }

    // Sort months chronologically (ascending YYYY-MM) using simple bubble sort
    for (int i = 0; i < month_count - 1; i++) {
        for (int j = 0; j < month_count - i - 1; j++) {
            if (strcmp(months[j], months[j + 1]) > 0) {
                // Swap months
                char temp_m[MONTH_LEN];
                strcpy(temp_m, months[j]);
                strcpy(months[j], months[j + 1]);
                strcpy(months[j + 1], temp_m);

                // Swap totals
                double temp_tot = month_totals[j];
                month_totals[j] = month_totals[j + 1];
                month_totals[j + 1] = temp_tot;
            }
        }
    }

    // Find highest monthly spend to scale the bars
    double max_month_val = 0.0;
    double all_months_total = 0.0;
    for (int m = 0; m < month_count; m++) {
        if (month_totals[m] > max_month_val) {
            max_month_val = month_totals[m];
        }
        all_months_total += month_totals[m];
    }

    printf("\n========================================================================================\n");
    printf("                              MONTHLY SPENDING TREND CHART                              \n");
    printf("   Scale: Highest month spend (Rs. %.2f) = %d characters ('#')                       \n",
           max_month_val, MAX_BAR_WIDTH);
    printf("========================================================================================\n");
    printf(" %-7s | %-42s | %-18s\n", "Month", "Spending Trend Bar", "Total Spent (Rs.)");
    printf("---------+--------------------------------------------+---------------------------------\n");

    for (int m = 0; m < month_count; m++) {
        int bar_len = 0;
        if (max_month_val > 0.0 && month_totals[m] > 0.0) {
            bar_len = (int)((month_totals[m] / max_month_val) * MAX_BAR_WIDTH);
            if (bar_len == 0) {
                bar_len = 1;
            }
            if (bar_len > MAX_BAR_WIDTH) {
                bar_len = MAX_BAR_WIDTH;
            }
        }

        char bar_buf[MAX_BAR_WIDTH + 1];
        for (int b = 0; b < bar_len; b++) {
            bar_buf[b] = '#';
        }
        bar_buf[bar_len] = '\0';

        printf(" %-7s | [%-40s] | Rs. %10.2f\n",
               months[m],
               bar_buf,
               month_totals[m]);
    }

    printf("---------+--------------------------------------------+---------------------------------\n");
    printf(" TOTAL   | %d month(s) recorded%*s | Rs. %10.2f\n",
           month_count, 22, "", all_months_total);
    printf("========================================================================================\n");

    if (month_count == 1) {
        printf("  [i] Note: Only 1 month of records available. Add expenses from other months to see trends!\n");
    }
}
