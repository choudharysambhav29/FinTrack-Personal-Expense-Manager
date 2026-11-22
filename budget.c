#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "budget.h"

/**
 * Searches the budgets array for an entry matching the specified month (YYYY-MM).
 * Returns the array index if found, or -1 if not found.
 */
int find_budget_index(const Budget budgets[], int count, const char *month) {
    for (int i = 0; i < count; i++) {
        if (strcmp(budgets[i].month, month) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * Sums all expense amounts that occurred in the given month (YYYY-MM).
 * Compares the first 7 characters ("YYYY-MM") of each expense date.
 */
double calculate_monthly_spent(const Expense expenses[], int expense_count, const char *month) {
    double total = 0.0;
    for (int i = 0; i < expense_count; i++) {
        // Expense date is "YYYY-MM-DD", compare the first 7 characters
        if (strncmp(expenses[i].date, month, 7) == 0) {
            total += expenses[i].amount;
        }
    }
    return total;
}

/**
 * Displays the budget status report for a given month,
 * including total spent, remaining funds, usage percentage, and threshold warnings.
 */
static void print_budget_report(const char *month, double limit, double spent) {
    double remaining = limit - spent;
    double percentage = (limit > 0.0) ? ((spent / limit) * 100.0) : 0.0;

    printf("\n  ---------------------------------------------------\n");
    printf("  Budget Summary for Month : %s\n", month);
    printf("  ---------------------------------------------------\n");
    printf("  Monthly Budget Limit     : Rs. %.2f\n", limit);
    printf("  Total Amount Spent       : Rs. %.2f\n", spent);
    if (remaining >= 0.0) {
        printf("  Remaining Allowance      : Rs. %.2f\n", remaining);
    } else {
        printf("  Amount Over Budget       : Rs. %.2f (DEFICIT)\n", -remaining);
    }
    printf("  Budget Utilization       : %.1f%%\n", percentage);
    printf("  ---------------------------------------------------\n");

    // Warning indicators
    if (percentage >= 100.0) {
        printf("  STATUS: [ALERT - 100%%+ USAGE]\n");
        printf("  >>> You have EXCEEDED your monthly budget by Rs. %.2f!\n", spent - limit);
    } else if (percentage >= 80.0) {
        printf("  STATUS: [WARNING - 80%%+ USAGE]\n");
        printf("  >>> Caution! You have used %.1f%% of your budget (Rs. %.2f left).\n", percentage, remaining);
    } else {
        printf("  STATUS: [OK - WITHIN BUDGET]\n");
        printf("  >>> Spending is currently within safe limits.\n");
    }
    printf("  ---------------------------------------------------\n");
}

/**
 * Prompts the user to set or update a monthly budget limit for a specific month (YYYY-MM).
 * If a budget already exists for that month, it updates the limit.
 * Returns 1 on success, 0 on failure or cancellation.
 */
int set_monthly_budget(Budget budgets[], int *budget_count, const Expense expenses[], int expense_count) {
    printf("\n=======================================================\n");
    printf("                  SET MONTHLY BUDGET                   \n");
    printf("=======================================================\n");

    char month_input[32];
    while (1) {
        printf("  Enter Target Month (YYYY-MM, 0 to cancel): ");
        read_line(month_input, sizeof(month_input));
        if (strcmp(month_input, "0") == 0) {
            printf("  Operation cancelled.\n");
            return 0;
        }
        if (is_valid_month(month_input)) {
            break;
        }
        printf("  [!] Invalid format. Please enter as YYYY-MM (e.g., 2026-11).\n");
    }

    int idx = find_budget_index(budgets, *budget_count, month_input);
    if (idx != -1) {
        printf("  A budget for %s already exists with limit: Rs. %.2f\n", month_input, budgets[idx].limit);
        printf("  Do you want to update it? (1 = Yes, 2 = No): ");
        int confirm;
        read_int_range(&confirm, 1, 2);
        if (confirm != 1) {
            printf("  Budget update cancelled.\n");
            return 0;
        }

        printf("  Enter New Budget Limit (Rs.): ");
        read_positive_double(&budgets[idx].limit);
        printf("  [+] Budget for %s updated to Rs. %.2f successfully!\n", month_input, budgets[idx].limit);

        double spent = calculate_monthly_spent(expenses, expense_count, month_input);
        print_budget_report(month_input, budgets[idx].limit, spent);
        return 1;
    }

    // Creating a new budget record
    if (*budget_count >= MAX_BUDGETS) {
        printf("  [!] Error: Maximum budget limit reached (%d months).\n", MAX_BUDGETS);
        return 0;
    }

    Budget new_budget;
    strncpy(new_budget.month, month_input, MONTH_LEN - 1);
    new_budget.month[MONTH_LEN - 1] = '\0';

    printf("  Enter Budget Limit for %s (Rs.): ", month_input);
    read_positive_double(&new_budget.limit);

    budgets[*budget_count] = new_budget;
    (*budget_count)++;

    printf("  [+] Budget for %s set to Rs. %.2f successfully!\n", new_budget.month, new_budget.limit);

    double spent = calculate_monthly_spent(expenses, expense_count, new_budget.month);
    print_budget_report(new_budget.month, new_budget.limit, spent);

    return 1;
}

/**
 * Checks and displays the budget status for a user-specified month (YYYY-MM).
 * Calculates spent funds, remaining funds, and warning thresholds.
 */
void check_monthly_budget_status(const Budget budgets[], int budget_count, const Expense expenses[], int expense_count) {
    printf("\n=======================================================\n");
    printf("                CHECK MONTHLY BUDGET STATUS            \n");
    printf("=======================================================\n");

    char month_input[32];
    while (1) {
        printf("  Enter Month to Check (YYYY-MM, 0 to cancel): ");
        read_line(month_input, sizeof(month_input));
        if (strcmp(month_input, "0") == 0) {
            printf("  Operation cancelled.\n");
            return;
        }
        if (is_valid_month(month_input)) {
            break;
        }
        printf("  [!] Invalid format. Please enter as YYYY-MM (e.g., 2026-11).\n");
    }

    int idx = find_budget_index(budgets, budget_count, month_input);
    double spent = calculate_monthly_spent(expenses, expense_count, month_input);

    if (idx == -1) {
        printf("\n  [i] No budget has been set for month '%s'.\n", month_input);
        printf("  Total expenses recorded for %s: Rs. %.2f\n", month_input, spent);
        return;
    }

    print_budget_report(budgets[idx].month, budgets[idx].limit, spent);
}

/**
 * Displays an aligned summary table of all configured monthly budgets
 * along with their current spending, remaining amount, and alert status.
 */
void view_all_budgets(const Budget budgets[], int budget_count, const Expense expenses[], int expense_count) {
    printf("\n========================================================================================\n");
    printf("                               ALL CONFIGURED MONTHLY BUDGETS                           \n");
    printf("========================================================================================\n");

    if (budget_count == 0) {
        printf("  No monthly budgets configured yet. Use the Budget Manager to set one!\n");
        printf("========================================================================================\n");
        return;
    }

    printf(" %-7s | %11s | %11s | %11s | %7s | %-16s\n",
           "Month", "Budget(Rs.)", "Spent (Rs.)", "Remain(Rs.)", "Used %", "Status");
    printf("---------+-------------+-------------+-------------+---------+---------------------\n");

    for (int i = 0; i < budget_count; i++) {
        double spent = calculate_monthly_spent(expenses, expense_count, budgets[i].month);
        double remaining = budgets[i].limit - spent;
        double pct = (budgets[i].limit > 0.0) ? ((spent / budgets[i].limit) * 100.0) : 0.0;

        const char *status_str;
        if (pct >= 100.0) {
            status_str = "[ALERT] EXCEEDED";
        } else if (pct >= 80.0) {
            status_str = "[WARN] >= 80%";
        } else {
            status_str = "[OK] Normal";
        }

        printf(" %-7s | %11.2f | %11.2f | %11.2f | %6.1f%% | %-16s\n",
               budgets[i].month,
               budgets[i].limit,
               spent,
               remaining,
               pct,
               status_str);
    }
    printf("========================================================================================\n");
}
