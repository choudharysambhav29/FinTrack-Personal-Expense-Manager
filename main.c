#include <stdio.h>
#include <stdlib.h>
#include "expense.h"
#include "budget.h"
#include "analysis.h"
#include "chart.h"
#include "export.h"
#include "fileio.h"

#define EXPENSE_FILE "expenses.csv"
#define BUDGET_FILE  "budget.dat"
#define HTML_REPORT  "report.html"

/**
 * Displays the main menu for FinTrack console application.
 * Shows available options, current record counts, and guides user interaction.
 */
void display_menu(int expense_count, int budget_count) {
    printf("\n=======================================================\n");
    printf("         FINTRACK - PERSONAL EXPENSE MANAGER           \n");
    printf("=======================================================\n");
    printf("  [1]  Add New Expense\n");
    printf("  [2]  View All Expenses (%d recorded)\n", expense_count);
    printf("  [3]  Edit Expense by ID\n");
    printf("  [4]  Delete Expense by ID\n");
    printf("  [5]  Category Spending Summary (Totals & %% Share)\n");
    printf("  [6]  Monthly Budget Manager (%d configured)\n", budget_count);
    printf("  [7]  Category Spending Bar Chart (ASCII #)\n");
    printf("  [8]  Monthly Spending Trend Chart (ASCII #)\n");
    printf("  [9]  Spending Pattern Analysis & Insights\n");
    printf("  [10] Export HTML Dashboard ('%s')\n", HTML_REPORT);
    printf("  [11] Save All Data to Files\n");
    printf("  [0]  Save & Exit\n");
    printf("=======================================================\n");
    printf("  Enter choice (0-11): ");
}

/**
 * Sub-menu for managing monthly budgets (setting limits, checking status, viewing all).
 */
void handle_budget_menu(Budget budgets[], int *budget_count, const Expense expenses[], int expense_count) {
    int in_budget_menu = 1;
    while (in_budget_menu) {
        printf("\n=======================================================\n");
        printf("                MONTHLY BUDGET MANAGER                 \n");
        printf("=======================================================\n");
        printf("  [1] Set / Update Monthly Budget Limit\n");
        printf("  [2] Check Monthly Budget Status & Warning Alerts\n");
        printf("  [3] View All Configured Monthly Budgets\n");
        printf("  [4] Back to Main Menu\n");
        printf("=======================================================\n");
        printf("  Enter choice (1-4): ");

        int choice;
        read_int_range(&choice, 1, 4);

        switch (choice) {
            case 1:
                if (set_monthly_budget(budgets, budget_count, expenses, expense_count)) {
                    // Auto-save budget changes
                    save_budgets_to_file(BUDGET_FILE, budgets, *budget_count);
                }
                break;
            case 2:
                check_monthly_budget_status(budgets, *budget_count, expenses, expense_count);
                break;
            case 3:
                view_all_budgets(budgets, *budget_count, expenses, expense_count);
                break;
            case 4:
                in_budget_menu = 0;
                break;
            default:
                break;
        }
    }
}

/**
 * Main application entry point.
 * Initializes storage, loads existing records from files if present,
 * and drives the interactive menu loop.
 */
int main(void) {
    Expense expenses[MAX_EXPENSES];
    int expense_count = 0;

    Budget budgets[MAX_BUDGETS];
    int budget_count = 0;

    printf("\n=======================================================\n");
    printf("             Welcome to FinTrack v1.0                  \n");
    printf("=======================================================\n");

    // Load existing expenses from expenses.csv if present
    int exp_loaded = load_expenses_from_csv(EXPENSE_FILE, expenses, &expense_count);
    if (exp_loaded > 0) {
        printf("  [i] Loaded %d existing expense(s) from '%s'.\n", exp_loaded, EXPENSE_FILE);
    } else {
        printf("  [i] Starting with empty expenses ('%s' not found or empty).\n", EXPENSE_FILE);
    }

    // Load existing budgets from budget.dat if present
    int bud_loaded = load_budgets_from_file(BUDGET_FILE, budgets, &budget_count);
    if (bud_loaded > 0) {
        printf("  [i] Loaded %d monthly budget(s) from '%s'.\n", bud_loaded, BUDGET_FILE);
    } else {
        printf("  [i] Starting with no budgets configured ('%s' not found).\n", BUDGET_FILE);
    }

    int running = 1;
    while (running) {
        display_menu(expense_count, budget_count);
        int choice;
        read_int_range(&choice, 0, 11);

        switch (choice) {
            case 1:
                if (add_expense(expenses, &expense_count)) {
                    // Auto-save on modification
                    save_expenses_to_csv(EXPENSE_FILE, expenses, expense_count);
                }
                break;

            case 2:
                view_all_expenses(expenses, expense_count);
                break;

            case 3:
                if (edit_expense(expenses, expense_count)) {
                    // Auto-save on modification
                    save_expenses_to_csv(EXPENSE_FILE, expenses, expense_count);
                }
                break;

            case 4:
                if (delete_expense(expenses, &expense_count)) {
                    // Auto-save on modification
                    save_expenses_to_csv(EXPENSE_FILE, expenses, expense_count);
                }
                break;

            case 5:
                view_category_summary_menu(expenses, expense_count);
                break;

            case 6:
                handle_budget_menu(budgets, &budget_count, expenses, expense_count);
                break;

            case 7:
                draw_category_bar_chart(expenses, expense_count);
                break;

            case 8:
                draw_monthly_trend_chart(expenses, expense_count);
                break;

            case 9:
                run_spending_pattern_analysis(expenses, expense_count);
                break;

            case 10:
                if (export_html_dashboard(HTML_REPORT, expenses, expense_count, budgets, budget_count)) {
                    printf("\n  [+] Successfully exported financial dashboard to '%s'!\n", HTML_REPORT);
                    printf("  [i] Open '%s' in any web browser to view your interactive charts and summary.\n", HTML_REPORT);
                }
                break;

            case 11: {
                int s1 = save_expenses_to_csv(EXPENSE_FILE, expenses, expense_count);
                int s2 = save_budgets_to_file(BUDGET_FILE, budgets, budget_count);
                if (s1 && s2) {
                    printf("  [+] Successfully saved %d expense(s) and %d budget(s) to disk.\n",
                           expense_count, budget_count);
                }
                break;
            }

            case 0:
                printf("\n  Saving records to disk before exiting...\n");
                save_expenses_to_csv(EXPENSE_FILE, expenses, expense_count);
                save_budgets_to_file(BUDGET_FILE, budgets, budget_count);
                printf("  [+] All data saved successfully. Thank you for using FinTrack. Goodbye!\n\n");
                running = 0;
                break;

            default:
                printf("  [!] Unknown option. Please choose between 0 and 11.\n");
                break;
        }
    }

    return 0;
}
