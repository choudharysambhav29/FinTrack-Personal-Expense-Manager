#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "analysis.h"

/**
 * Checks whether a given year is a leap year.
 */
static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
 * Returns the total calendar days in a specified month of a given year.
 */
static int get_calendar_days_in_month(int year, int month) {
    if (month < 1 || month > 12) return 30;
    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (is_leap_year(year)) {
        days_in_month[1] = 29;
    }
    return days_in_month[month - 1];
}

/**
 * Derives the calendar month immediately preceding the given 'YYYY-MM' string.
 * Handles year rollback (e.g., 2026-01 rollbacks to 2025-12).
 */
static void get_previous_month(const char *curr_month, char *prev_month) {
    int year, month;
    if (sscanf(curr_month, "%4d-%2d", &year, &month) == 2) {
        if (month == 1) {
            year--;
            month = 12;
        } else {
            month--;
        }
        snprintf(prev_month, MONTH_LEN, "%04d-%02d", year, month);
    } else {
        strcpy(prev_month, "Unknown");
    }
}

/**
 * Sums all expense amounts matching a specific month (YYYY-MM).
 */
static double get_month_total(const Expense expenses[], int count, const char *month) {
    double total = 0.0;
    for (int i = 0; i < count; i++) {
        if (strncmp(expenses[i].date, month, 7) == 0) {
            total += expenses[i].amount;
        }
    }
    return total;
}

/**
 * Counts the number of unique calendar dates on which at least one expense occurred.
 * Can be filtered by a specific month (YYYY-MM) or all-time (if filter_month is NULL).
 */
static int count_unique_active_days(const Expense expenses[], int count, const char *filter_month) {
    char unique_dates[MAX_EXPENSES][DATE_LEN];
    int unique_count = 0;

    for (int i = 0; i < count; i++) {
        if (filter_month != NULL && strncmp(expenses[i].date, filter_month, 7) != 0) {
            continue;
        }

        int already_seen = 0;
        for (int d = 0; d < unique_count; d++) {
            if (strcmp(unique_dates[d], expenses[i].date) == 0) {
                already_seen = 1;
                break;
            }
        }

        if (!already_seen) {
            strncpy(unique_dates[unique_count], expenses[i].date, DATE_LEN - 1);
            unique_dates[unique_count][DATE_LEN - 1] = '\0';
            unique_count++;
        }
    }
    return unique_count;
}

/**
 * Aggregates expense totals, counts, and percentage shares for each of the
 * predefined categories. Can filter by a specific month (YYYY-MM) or analyze all-time
 * if filter_month is NULL.
 */
void calculate_category_summary(const Expense expenses[], int expense_count,
                                const char *filter_month,
                                CategorySummary summaries[NUM_CATEGORIES],
                                double *grand_total) {
    *grand_total = 0.0;

    // Initialize summaries for each predefined category
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        strncpy(summaries[i].name, CATEGORIES[i], CATEGORY_LEN - 1);
        summaries[i].name[CATEGORY_LEN - 1] = '\0';
        summaries[i].total = 0.0;
        summaries[i].percentage = 0.0;
        summaries[i].count = 0;
    }

    // Accumulate amounts
    for (int i = 0; i < expense_count; i++) {
        // If filtering by month, skip non-matching expenses
        if (filter_month != NULL && strncmp(expenses[i].date, filter_month, 7) != 0) {
            continue;
        }

        // Match category
        for (int c = 0; c < NUM_CATEGORIES; c++) {
            if (strcmp(expenses[i].category, CATEGORIES[c]) == 0) {
                summaries[c].total += expenses[i].amount;
                summaries[c].count++;
                *grand_total += expenses[i].amount;
                break;
            }
        }
    }

    // Compute percentage share for each category
    if (*grand_total > 0.0) {
        for (int i = 0; i < NUM_CATEGORIES; i++) {
            summaries[i].percentage = (summaries[i].total / *grand_total) * 100.0;
        }
    }
}

/**
 * Interactive menu to view category-wise spending.
 * Allows the user to view all-time spending or filter by a specific calendar month.
 */
void view_category_summary_menu(const Expense expenses[], int expense_count) {
    if (expense_count == 0) {
        printf("\n  [!] No expenses recorded yet. Please add expenses first.\n");
        return;
    }

    printf("\n=======================================================\n");
    printf("                CATEGORY SPENDING SUMMARY              \n");
    printf("=======================================================\n");
    printf("  [1] View All-Time Category Summary\n");
    printf("  [2] Filter Category Summary by Month (YYYY-MM)\n");
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

    printf("\n========================================================================================\n");
    if (month_ptr != NULL) {
        printf("                      CATEGORY BREAKDOWN FOR MONTH: %-10s                 \n", month_ptr);
    } else {
        printf("                           ALL-TIME CATEGORY BREAKDOWN                                  \n");
    }
    printf("========================================================================================\n");
    printf(" %-15s | %-12s | %16s | %10s\n", "Category", "Transactions", "Total Spent(Rs.)", "Share (%)");
    printf("-----------------+--------------+------------------+------------\n");

    int total_transactions = 0;
    for (int i = 0; i < NUM_CATEGORIES; i++) {
        printf(" %-15s | %-12d | %16.2f | %9.1f%%\n",
               summaries[i].name,
               summaries[i].count,
               summaries[i].total,
               summaries[i].percentage);
        total_transactions += summaries[i].count;
    }

    printf("-----------------+--------------+------------------+------------\n");
    printf(" TOTAL           | %-12d | %16.2f | %9.1f%%\n",
           total_transactions,
           grand_total,
           (grand_total > 0.0 ? 100.0 : 0.0));
    printf("========================================================================================\n");
}

/**
 * Conducts and prints in-depth spending pattern analysis:
 * 1. Highest spending category (amount and share)
 * 2. Average spend per day (stating explicitly how days are counted)
 * 3. Top 3 largest individual expenses with date, category, and amount
 * 4. Month-over-month (MoM) % change with explicit "increase" or "decrease" label
 * Robustly handles zero data, single-month datasets, and zero denominator cases.
 */
void run_spending_pattern_analysis(const Expense expenses[], int expense_count) {
    if (expense_count == 0) {
        printf("\n  [!] No expenses recorded yet. Cannot perform spending pattern analysis.\n");
        return;
    }

    printf("\n=======================================================\n");
    printf("              SPENDING PATTERN ANALYSIS                \n");
    printf("=======================================================\n");
    printf("  [1] All-Time Spending Pattern Analysis\n");
    printf("  [2] Analyze Specific Month (YYYY-MM)\n");
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
            printf("  Enter Month to Analyze (YYYY-MM, 0 to cancel): ");
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

    // 1. Category calculations & Grand total
    CategorySummary summaries[NUM_CATEGORIES];
    double grand_total = 0.0;
    calculate_category_summary(expenses, expense_count, month_ptr, summaries, &grand_total);

    // Collect matching expense pointers for top 3 search
    const Expense *matching[MAX_EXPENSES];
    int matching_count = 0;
    for (int i = 0; i < expense_count; i++) {
        if (month_ptr == NULL || strncmp(expenses[i].date, month_ptr, 7) == 0) {
            matching[matching_count++] = &expenses[i];
        }
    }

    printf("\n========================================================================================\n");
    if (month_ptr != NULL) {
        printf("               SPENDING PATTERN INSIGHTS FOR MONTH: %-10s                 \n", month_ptr);
    } else {
        printf("                         ALL-TIME SPENDING PATTERN INSIGHTS                             \n");
    }
    printf("========================================================================================\n");

    if (matching_count == 0 || grand_total <= 0.0) {
        printf("  [i] No expenses found for this selection.\n");
        printf("========================================================================================\n");
        return;
    }

    // A. Highest Spending Category
    int max_cat_idx = 0;
    for (int i = 1; i < NUM_CATEGORIES; i++) {
        if (summaries[i].total > summaries[max_cat_idx].total) {
            max_cat_idx = i;
        }
    }

    printf("\n  1. HIGHEST SPENDING CATEGORY:\n");
    printf("  --------------------------------------------------------------------------------------\n");
    if (summaries[max_cat_idx].total > 0.0) {
        printf("  Category       : %s\n", summaries[max_cat_idx].name);
        printf("  Total Spent    : Rs. %.2f\n", summaries[max_cat_idx].total);
        printf("  Category Share : %.1f%% of overall spending\n", summaries[max_cat_idx].percentage);
        printf("  Transactions   : %d record(s)\n", summaries[max_cat_idx].count);
    } else {
        printf("  No spending recorded across any category (all Rs. 0.00).\n");
    }

    // B. Daily Average Spend (Explaining day counting methods clearly)
    int active_days = count_unique_active_days(expenses, expense_count, month_ptr);
    printf("\n  2. DAILY AVERAGE SPEND:\n");
    printf("  --------------------------------------------------------------------------------------\n");
    printf("  Total Spending in Period : Rs. %.2f\n", grand_total);
    if (active_days > 0) {
        double avg_per_active_day = grand_total / (double)active_days;
        printf("  Method A (Active Days)   : Days are counted as unique dates with at least one expense.\n");
        printf("                             Found %d active spending day(s).\n", active_days);
        printf("                             Daily Average = Rs. %.2f / active day\n", avg_per_active_day);
    }

    if (month_ptr != NULL) {
        int y, m;
        sscanf(month_ptr, "%4d-%2d", &y, &m);
        int cal_days = get_calendar_days_in_month(y, m);
        double avg_per_calendar_day = grand_total / (double)cal_days;
        printf("  Method B (Calendar Days) : Days are counted as total calendar days in %s (%d days).\n",
               month_ptr, cal_days);
        printf("                             Daily Average = Rs. %.2f / calendar day\n", avg_per_calendar_day);
    }

    // C. Top 3 Largest Expenses
    printf("\n  3. TOP LARGEST EXPENSES:\n");
    printf("  --------------------------------------------------------------------------------------\n");
    // Sort pointers to matching expenses in descending order of amount
    for (int i = 0; i < matching_count - 1; i++) {
        for (int j = 0; j < matching_count - i - 1; j++) {
            if (matching[j]->amount < matching[j + 1]->amount) {
                const Expense *tmp = matching[j];
                matching[j] = matching[j + 1];
                matching[j + 1] = tmp;
            }
        }
    }

    int display_top = (matching_count < 3) ? matching_count : 3;
    if (display_top == 1) {
        printf("  (Only 1 expense record found in this period)\n");
    } else if (display_top == 2) {
        printf("  (Only 2 expense records found in this period)\n");
    } else {
        printf("  Top 3 single largest transactions:\n");
    }

    printf("  Rank | Date       | Category        | Amount (Rs.) | Note\n");
    printf("  -----+------------+-----------------+--------------+---------------------------------\n");
    for (int k = 0; k < display_top; k++) {
        printf("  #%-3d | %-10s | %-15s | Rs. %8.2f | %s\n",
               k + 1,
               matching[k]->date,
               matching[k]->category,
               matching[k]->amount,
               matching[k]->note);
    }

    // D. Month-over-Month (MoM) Change Percentage
    printf("\n  4. MONTH-OVER-MONTH (MoM) CHANGE ANALYSIS:\n");
    printf("  --------------------------------------------------------------------------------------\n");

    if (month_ptr != NULL) {
        // Specific chosen month
        char prev_m[MONTH_LEN];
        get_previous_month(month_ptr, prev_m);

        double curr_total = grand_total;
        double prev_total = get_month_total(expenses, expense_count, prev_m);

        printf("  Current Month  : %s (Total Spend: Rs. %.2f)\n", month_ptr, curr_total);
        printf("  Previous Month : %s (Total Spend: Rs. %.2f)\n", prev_m, prev_total);

        if (prev_total == 0.0) {
            if (curr_total == 0.0) {
                printf("  MoM Change     : 0.0%% (no change - both months have zero spending)\n");
            } else {
                printf("  MoM Change     : N/A (Previous month %s had Rs. 0.00 spending; base is zero)\n", prev_m);
                printf("                   New spending in %s: Rs. %.2f\n", month_ptr, curr_total);
            }
        } else {
            double diff = curr_total - prev_total;
            double pct = (diff / prev_total) * 100.0;
            if (diff > 0.0) {
                printf("  MoM Change     : +%.1f%% (increase of Rs. %.2f compared to %s)\n",
                       pct, diff, prev_m);
            } else if (diff < 0.0) {
                printf("  MoM Change     : %.1f%% (decrease of Rs. %.2f compared to %s)\n",
                       pct, -diff, prev_m);
            } else {
                printf("  MoM Change     : 0.0%% (no change in spending compared to %s)\n", prev_m);
            }
        }
    } else {
        // All-time: collect distinct chronological months
        char all_months[MAX_EXPENSES][MONTH_LEN];
        double month_totals[MAX_EXPENSES];
        int m_count = 0;

        for (int i = 0; i < expense_count; i++) {
            char m_str[MONTH_LEN];
            strncpy(m_str, expenses[i].date, 7);
            m_str[7] = '\0';

            int f = -1;
            for (int m = 0; m < m_count; m++) {
                if (strcmp(all_months[m], m_str) == 0) {
                    f = m;
                    break;
                }
            }
            if (f != -1) {
                month_totals[f] += expenses[i].amount;
            } else {
                strncpy(all_months[m_count], m_str, MONTH_LEN - 1);
                all_months[m_count][MONTH_LEN - 1] = '\0';
                month_totals[m_count] = expenses[i].amount;
                m_count++;
            }
        }

        // Sort chronologically
        for (int i = 0; i < m_count - 1; i++) {
            for (int j = 0; j < m_count - i - 1; j++) {
                if (strcmp(all_months[j], all_months[j + 1]) > 0) {
                    char tmp_m[MONTH_LEN];
                    strcpy(tmp_m, all_months[j]);
                    strcpy(tmp_m, all_months[j + 1]);
                    strcpy(all_months[j + 1], tmp_m);

                    double tmp_t = month_totals[j];
                    month_totals[j] = month_totals[j + 1];
                    month_totals[j + 1] = tmp_t;
                }
            }
        }

        if (m_count <= 1) {
            printf("  Only %d month of data recorded (%s).\n",
                   m_count, (m_count == 1 ? all_months[0] : "none"));
            printf("  At least two consecutive calendar months are required to compute Month-over-Month change.\n");
        } else {
            printf("  Sequential Month-over-Month Breakdown:\n");
            printf("  Month     | Total Spend (Rs.) | Change vs Prior Month\n");
            printf("  ----------+-------------------+--------------------------------------------\n");
            printf("  %-9s | Rs. %13.2f | (Base month)\n", all_months[0], month_totals[0]);

            for (int m = 1; m < m_count; m++) {
                double prev_val = month_totals[m - 1];
                double curr_val = month_totals[m];
                double diff = curr_val - prev_val;

                if (prev_val == 0.0) {
                    if (curr_val == 0.0) {
                        printf("  %-9s | Rs. %13.2f | 0.0%% (no change)\n", all_months[m], curr_val);
                    } else {
                        printf("  %-9s | Rs. %13.2f | N/A (prior month was Rs. 0.00)\n", all_months[m], curr_val);
                    }
                } else {
                    double pct = (diff / prev_val) * 100.0;
                    if (diff > 0.0) {
                        printf("  %-9s | Rs. %13.2f | +%5.1f%% (increase)\n", all_months[m], curr_val, pct);
                    } else if (diff < 0.0) {
                        printf("  %-9s | Rs. %13.2f | %5.1f%% (decrease)\n", all_months[m], curr_val, pct);
                    } else {
                        printf("  %-9s | Rs. %13.2f |   0.0%% (no change)\n", all_months[m], curr_val);
                    }
                }
            }
        }
    }

    printf("========================================================================================\n");
}
