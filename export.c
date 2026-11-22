#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "export.h"
#include "analysis.h"

/**
 * Escapes special HTML characters in user-entered strings (notes/descriptions)
 * to prevent malformed HTML rendering.
 */
static void write_escaped_html(FILE *fp, const char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '&':  fprintf(fp, "&amp;"); break;
            case '<':  fprintf(fp, "&lt;"); break;
            case '>':  fprintf(fp, "&gt;"); break;
            case '"':  fprintf(fp, "&quot;"); break;
            case '\'': fprintf(fp, "&#39;"); break;
            default:   fputc(str[i], fp); break;
        }
    }
}

/**
 * Generates an interactive, standalone HTML financial dashboard report.
 * Uses modern CSS styling and the Chart.js CDN for responsive data visualization.
 * Writes summary cards, category doughnut chart, monthly bar chart,
 * 10 most recent transactions, and monthly budget utilization badges.
 */
int export_html_dashboard(const char *filename,
                          const Expense expenses[], int expense_count,
                          const Budget budgets[], int budget_count) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("  [!] Error: Unable to create '%s' for writing.\n", filename);
        return 0;
    }

    // 1. Calculate overall financial statistics
    CategorySummary summaries[NUM_CATEGORIES];
    double grand_total = 0.0;
    calculate_category_summary(expenses, expense_count, NULL, summaries, &grand_total);

    double avg_per_expense = (expense_count > 0) ? (grand_total / (double)expense_count) : 0.0;

    int max_cat_idx = 0;
    for (int i = 1; i < NUM_CATEGORIES; i++) {
        if (summaries[i].total > summaries[max_cat_idx].total) {
            max_cat_idx = i;
        }
    }

    // 2. Aggregate monthly totals ordered chronologically
    char months[MAX_EXPENSES][MONTH_LEN];
    double month_totals[MAX_EXPENSES];
    int month_count = 0;

    for (int i = 0; i < expense_count; i++) {
        char exp_m[MONTH_LEN];
        strncpy(exp_m, expenses[i].date, 7);
        exp_m[7] = '\0';

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

    // Sort months chronologically
    for (int i = 0; i < month_count - 1; i++) {
        for (int j = 0; j < month_count - i - 1; j++) {
            if (strcmp(months[j], months[j + 1]) > 0) {
                char tmp_m[MONTH_LEN];
                strcpy(tmp_m, months[j]);
                strcpy(tmp_m, months[j + 1]);
                strcpy(months[j + 1], tmp_m);

                double tmp_t = month_totals[j];
                month_totals[j] = month_totals[j + 1];
                month_totals[j + 1] = tmp_t;
            }
        }
    }

    // 3. Write HTML Header & CSS
    fprintf(fp, "<!DOCTYPE html>\n");
    fprintf(fp, "<html lang=\"en\">\n");
    fprintf(fp, "<head>\n");
    fprintf(fp, "  <meta charset=\"UTF-8\">\n");
    fprintf(fp, "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fp, "  <title>FinTrack &ndash; Financial Dashboard</title>\n");
    fprintf(fp, "  <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n");
    fprintf(fp, "  <style>\n");
    fprintf(fp, "    :root {\n");
    fprintf(fp, "      --primary: #4f46e5;\n");
    fprintf(fp, "      --primary-light: #e0e7ff;\n");
    fprintf(fp, "      --bg-page: #f8fafc;\n");
    fprintf(fp, "      --bg-card: #ffffff;\n");
    fprintf(fp, "      --text-main: #0f172a;\n");
    fprintf(fp, "      --text-muted: #64748b;\n");
    fprintf(fp, "      --border: #e2e8f0;\n");
    fprintf(fp, "      --success: #10b981;\n");
    fprintf(fp, "      --warning: #f59e0b;\n");
    fprintf(fp, "      --danger: #ef4444;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    * { box-sizing: border-box; margin: 0; padding: 0; }\n");
    fprintf(fp, "    body {\n");
    fprintf(fp, "      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;\n");
    fprintf(fp, "      background-color: var(--bg-page);\n");
    fprintf(fp, "      color: var(--text-main);\n");
    fprintf(fp, "      padding: 30px 20px;\n");
    fprintf(fp, "      line-height: 1.5;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    .container { max-width: 1200px; margin: 0 auto; }\n");
    fprintf(fp, "    header {\n");
    fprintf(fp, "      margin-bottom: 25px;\n");
    fprintf(fp, "      display: flex;\n");
    fprintf(fp, "      justify-content: space-between;\n");
    fprintf(fp, "      align-items: center;\n");
    fprintf(fp, "      flex-wrap: wrap;\n");
    fprintf(fp, "      gap: 15px;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    h1 { font-size: 1.85rem; font-weight: 700; color: var(--text-main); }\n");
    fprintf(fp, "    .subtitle { color: var(--text-muted); font-size: 0.95rem; margin-top: 4px; }\n");
    fprintf(fp, "    .badge-app {\n");
    fprintf(fp, "      background: var(--primary-light);\n");
    fprintf(fp, "      color: var(--primary);\n");
    fprintf(fp, "      padding: 6px 14px;\n");
    fprintf(fp, "      border-radius: 9999px;\n");
    fprintf(fp, "      font-size: 0.85rem;\n");
    fprintf(fp, "      font-weight: 600;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    .kpi-grid {\n");
    fprintf(fp, "      display: grid;\n");
    fprintf(fp, "      grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));\n");
    fprintf(fp, "      gap: 18px;\n");
    fprintf(fp, "      margin-bottom: 25px;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    .card {\n");
    fprintf(fp, "      background: var(--bg-card);\n");
    fprintf(fp, "      border: 1px solid var(--border);\n");
    fprintf(fp, "      border-radius: 12px;\n");
    fprintf(fp, "      padding: 20px;\n");
    fprintf(fp, "      box-shadow: 0 1px 3px rgba(0,0,0,0.04);\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    .kpi-card .kpi-label { color: var(--text-muted); font-size: 0.85rem; font-weight: 600; text-transform: uppercase; letter-spacing: 0.05em; }\n");
    fprintf(fp, "    .kpi-card .kpi-value { font-size: 1.8rem; font-weight: 700; color: var(--text-main); margin-top: 8px; }\n");
    fprintf(fp, "    .kpi-card .kpi-desc { color: var(--text-muted); font-size: 0.82rem; margin-top: 4px; }\n");
    fprintf(fp, "    .charts-grid {\n");
    fprintf(fp, "      display: grid;\n");
    fprintf(fp, "      grid-template-columns: repeat(auto-fit, minmax(460px, 1fr));\n");
    fprintf(fp, "      gap: 20px;\n");
    fprintf(fp, "      margin-bottom: 25px;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    .chart-container { position: relative; height: 320px; width: 100%%; margin-top: 15px; }\n");
    fprintf(fp, "    .section-title { font-size: 1.15rem; font-weight: 600; color: var(--text-main); margin-bottom: 4px; }\n");
    fprintf(fp, "    .section-subtitle { font-size: 0.85rem; color: var(--text-muted); margin-bottom: 12px; }\n");
    fprintf(fp, "    table {\n");
    fprintf(fp, "      width: 100%%;\n");
    fprintf(fp, "      border-collapse: collapse;\n");
    fprintf(fp, "      font-size: 0.9rem;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    th, td { padding: 12px 14px; text-align: left; border-bottom: 1px solid var(--border); }\n");
    fprintf(fp, "    th { background-color: #f1f5f9; color: #475569; font-weight: 600; font-size: 0.82rem; text-transform: uppercase; letter-spacing: 0.04em; }\n");
    fprintf(fp, "    tr:hover { background-color: #f8fafc; }\n");
    fprintf(fp, "    .amount-col { font-family: monospace; font-weight: 600; text-align: right; }\n");
    fprintf(fp, "    .badge-status {\n");
    fprintf(fp, "      display: inline-block;\n");
    fprintf(fp, "      padding: 3px 8px;\n");
    fprintf(fp, "      border-radius: 6px;\n");
    fprintf(fp, "      font-size: 0.78rem;\n");
    fprintf(fp, "      font-weight: 600;\n");
    fprintf(fp, "    }\n");
    fprintf(fp, "    .badge-ok { background: #dcfce7; color: #166534; }\n");
    fprintf(fp, "    .badge-warn { background: #fef3c7; color: #92400e; }\n");
    fprintf(fp, "    .badge-danger { background: #fee2e2; color: #991b1b; }\n");
    fprintf(fp, "    .progress-bar-bg { background: #e2e8f0; height: 8px; border-radius: 9999px; overflow: hidden; margin-top: 5px; }\n");
    fprintf(fp, "    .progress-bar-fill { height: 100%%; border-radius: 9999px; }\n");
    fprintf(fp, "    .empty-state { text-align: center; padding: 50px 20px; color: var(--text-muted); }\n");
    fprintf(fp, "    footer { margin-top: 35px; text-align: center; color: var(--text-muted); font-size: 0.85rem; }\n");
    fprintf(fp, "  </style>\n");
    fprintf(fp, "</head>\n");
    fprintf(fp, "<body>\n");
    fprintf(fp, "  <div class=\"container\">\n");

    // 4. Header Bar
    fprintf(fp, "    <header>\n");
    fprintf(fp, "      <div>\n");
    fprintf(fp, "        <h1>FinTrack &ndash; Expense & Budget Dashboard</h1>\n");
    fprintf(fp, "        <div class=\"subtitle\">Personal Financial Health, Categorical Breakdown & Monthly Analytics</div>\n");
    fprintf(fp, "      </div>\n");
    fprintf(fp, "      <div class=\"badge-app\">FinTrack v1.0 &bull; C-Generated</div>\n");
    fprintf(fp, "    </header>\n");

    // 5. Summary KPI Cards
    fprintf(fp, "    <div class=\"kpi-grid\">\n");
    fprintf(fp, "      <div class=\"card kpi-card\">\n");
    fprintf(fp, "        <div class=\"kpi-label\">Total Spent</div>\n");
    fprintf(fp, "        <div class=\"kpi-value\">Rs. %.2f</div>\n", grand_total);
    fprintf(fp, "        <div class=\"kpi-desc\">Cumulative expenditure to date</div>\n");
    fprintf(fp, "      </div>\n");

    fprintf(fp, "      <div class=\"card kpi-card\">\n");
    fprintf(fp, "        <div class=\"kpi-label\">Total Transactions</div>\n");
    fprintf(fp, "        <div class=\"kpi-value\">%d</div>\n", expense_count);
    fprintf(fp, "        <div class=\"kpi-desc\">Individual expenses recorded</div>\n");
    fprintf(fp, "      </div>\n");

    fprintf(fp, "      <div class=\"card kpi-card\">\n");
    fprintf(fp, "        <div class=\"kpi-label\">Top Category</div>\n");
    if (expense_count > 0 && summaries[max_cat_idx].total > 0.0) {
        fprintf(fp, "        <div class=\"kpi-value\" style=\"font-size: 1.45rem;\">%s</div>\n", summaries[max_cat_idx].name);
        fprintf(fp, "        <div class=\"kpi-desc\">Rs. %.2f (%.1f%% share)</div>\n",
                summaries[max_cat_idx].total, summaries[max_cat_idx].percentage);
    } else {
        fprintf(fp, "        <div class=\"kpi-value\" style=\"font-size: 1.45rem;\">None</div>\n");
        fprintf(fp, "        <div class=\"kpi-desc\">No spending recorded</div>\n");
    }
    fprintf(fp, "      </div>\n");

    fprintf(fp, "      <div class=\"card kpi-card\">\n");
    fprintf(fp, "        <div class=\"kpi-label\">Average Spend / Expense</div>\n");
    fprintf(fp, "        <div class=\"kpi-value\">Rs. %.2f</div>\n", avg_per_expense);
    fprintf(fp, "        <div class=\"kpi-desc\">Across all %d transactions</div>\n", expense_count);
    fprintf(fp, "      </div>\n");
    fprintf(fp, "    </div>\n");

    // 6. Charts Section (Category Doughnut + Monthly Bar)
    if (expense_count > 0) {
        fprintf(fp, "    <div class=\"charts-grid\">\n");
        fprintf(fp, "      <div class=\"card\">\n");
        fprintf(fp, "        <div class=\"section-title\">Category Spending Breakdown</div>\n");
        fprintf(fp, "        <div class=\"section-subtitle\">Distribution across predefined spending categories</div>\n");
        fprintf(fp, "        <div class=\"chart-container\">\n");
        fprintf(fp, "          <canvas id=\"categoryChart\"></canvas>\n");
        fprintf(fp, "        </div>\n");
        fprintf(fp, "      </div>\n");

        fprintf(fp, "      <div class=\"card\">\n");
        fprintf(fp, "        <div class=\"section-title\">Monthly Spending Overview</div>\n");
        fprintf(fp, "        <div class=\"section-subtitle\">Total expenditure per month in chronological sequence</div>\n");
        fprintf(fp, "        <div class=\"chart-container\">\n");
        fprintf(fp, "          <canvas id=\"monthlyChart\"></canvas>\n");
        fprintf(fp, "        </div>\n");
        fprintf(fp, "      </div>\n");
        fprintf(fp, "    </div>\n");
    } else {
        fprintf(fp, "    <div class=\"card empty-state\">\n");
        fprintf(fp, "      <h3>No Expense Data Available Yet</h3>\n");
        fprintf(fp, "      <p style=\"margin-top: 8px;\">Start recording your expenses in FinTrack to view interactive visual charts and monthly trends!</p>\n");
        fprintf(fp, "    </div>\n");
    }

    // 7. Monthly Budgets Table
    fprintf(fp, "    <div class=\"card\" style=\"margin-bottom: 25px;\">\n");
    fprintf(fp, "      <div class=\"section-title\">Monthly Budget Tracking</div>\n");
    fprintf(fp, "      <div class=\"section-subtitle\">Status, limits, remaining allowances, and usage alerts</div>\n");

    if (budget_count == 0) {
        fprintf(fp, "      <p style=\"color: var(--text-muted); font-size: 0.9rem; padding: 10px 0;\">No monthly budgets configured yet. Use Option [6] in FinTrack to set your budget limits.</p>\n");
    } else {
        fprintf(fp, "      <div style=\"overflow-x: auto;\">\n");
        fprintf(fp, "        <table>\n");
        fprintf(fp, "          <thead>\n");
        fprintf(fp, "            <tr>\n");
        fprintf(fp, "              <th>Month</th>\n");
        fprintf(fp, "              <th style=\"text-align: right;\">Budget Limit (Rs.)</th>\n");
        fprintf(fp, "              <th style=\"text-align: right;\">Total Spent (Rs.)</th>\n");
        fprintf(fp, "              <th style=\"text-align: right;\">Remaining (Rs.)</th>\n");
        fprintf(fp, "              <th>Utilization</th>\n");
        fprintf(fp, "              <th>Status</th>\n");
        fprintf(fp, "            </tr>\n");
        fprintf(fp, "          </thead>\n");
        fprintf(fp, "          <tbody>\n");

        for (int b = 0; b < budget_count; b++) {
            double spent = calculate_monthly_spent(expenses, expense_count, budgets[b].month);
            double remaining = budgets[b].limit - spent;
            double pct = (budgets[b].limit > 0.0) ? ((spent / budgets[b].limit) * 100.0) : 0.0;
            double bar_width = (pct > 100.0) ? 100.0 : pct;

            const char *badge_class;
            const char *badge_text;
            const char *bar_color;

            if (pct >= 100.0) {
                badge_class = "badge-danger";
                badge_text = "OVER BUDGET";
                bar_color = "var(--danger)";
            } else if (pct >= 80.0) {
                badge_class = "badge-warn";
                badge_text = "WARNING >= 80%";
                bar_color = "var(--warning)";
            } else {
                badge_class = "badge-ok";
                badge_text = "WITHIN BUDGET";
                bar_color = "var(--success)";
            }

            fprintf(fp, "            <tr>\n");
            fprintf(fp, "              <td style=\"font-weight: 600;\">%s</td>\n", budgets[b].month);
            fprintf(fp, "              <td class=\"amount-col\">Rs. %.2f</td>\n", budgets[b].limit);
            fprintf(fp, "              <td class=\"amount-col\">Rs. %.2f</td>\n", spent);
            if (remaining >= 0.0) {
                fprintf(fp, "              <td class=\"amount-col\" style=\"color: #166534;\">Rs. %.2f</td>\n", remaining);
            } else {
                fprintf(fp, "              <td class=\"amount-col\" style=\"color: #991b1b;\">-Rs. %.2f</td>\n", -remaining);
            }
            fprintf(fp, "              <td style=\"min-width: 140px;\">\n");
            fprintf(fp, "                <div style=\"display: flex; justify-content: space-between; font-size: 0.8rem;\"><span>%.1f%%</span></div>\n", pct);
            fprintf(fp, "                <div class=\"progress-bar-bg\"><div class=\"progress-bar-fill\" style=\"width: %.1f%%; background: %s;\"></div></div>\n",
                    bar_width, bar_color);
            fprintf(fp, "              </td>\n");
            fprintf(fp, "              <td><span class=\"badge-status %s\">%s</span></td>\n", badge_class, badge_text);
            fprintf(fp, "            </tr>\n");
        }

        fprintf(fp, "          </tbody>\n");
        fprintf(fp, "        </table>\n");
        fprintf(fp, "      </div>\n");
    }
    fprintf(fp, "    </div>\n");

    // 8. 10 Most Recent Expenses Table
    fprintf(fp, "    <div class=\"card\">\n");
    fprintf(fp, "      <div class=\"section-title\">Recent Transactions</div>\n");
    fprintf(fp, "      <div class=\"section-subtitle\">Up to 10 most recently added expenses</div>\n");

    if (expense_count == 0) {
        fprintf(fp, "      <p style=\"color: var(--text-muted); font-size: 0.9rem; padding: 10px 0;\">No transactions recorded yet.</p>\n");
    } else {
        fprintf(fp, "      <div style=\"overflow-x: auto;\">\n");
        fprintf(fp, "        <table>\n");
        fprintf(fp, "          <thead>\n");
        fprintf(fp, "            <tr>\n");
        fprintf(fp, "              <th>ID</th>\n");
        fprintf(fp, "              <th>Date</th>\n");
        fprintf(fp, "              <th>Category</th>\n");
        fprintf(fp, "              <th style=\"text-align: right;\">Amount (Rs.)</th>\n");
        fprintf(fp, "              <th>Note</th>\n");
        fprintf(fp, "            </tr>\n");
        fprintf(fp, "          </thead>\n");
        fprintf(fp, "          <tbody>\n");

        int start_idx = expense_count - 1;
        int end_idx = (expense_count > 10) ? (expense_count - 10) : 0;

        for (int i = start_idx; i >= end_idx; i--) {
            fprintf(fp, "            <tr>\n");
            fprintf(fp, "              <td style=\"font-weight: 600;\">#%d</td>\n", expenses[i].id);
            fprintf(fp, "              <td>%s</td>\n", expenses[i].date);
            fprintf(fp, "              <td><span class=\"badge-status\" style=\"background: #e2e8f0; color: #334155;\">%s</span></td>\n",
                    expenses[i].category);
            fprintf(fp, "              <td class=\"amount-col\">Rs. %.2f</td>\n", expenses[i].amount);
            fprintf(fp, "              <td>");
            write_escaped_html(fp, expenses[i].note);
            fprintf(fp, "</td>\n");
            fprintf(fp, "            </tr>\n");
        }

        fprintf(fp, "          </tbody>\n");
        fprintf(fp, "        </table>\n");
        fprintf(fp, "      </div>\n");
    }
    fprintf(fp, "    </div>\n");

    // 9. Footer
    fprintf(fp, "    <footer>\n");
    fprintf(fp, "      FinTrack &ndash; Personal Expense Manager &bull; College Project C11 Console Application\n");
    fprintf(fp, "    </footer>\n");

    // 10. Chart.js Initialization Script
    if (expense_count > 0) {
        fprintf(fp, "  <script>\n");

        // Category Doughnut Chart
        fprintf(fp, "    const catCtx = document.getElementById('categoryChart').getContext('2d');\n");
        fprintf(fp, "    new Chart(catCtx, {\n");
        fprintf(fp, "      type: 'doughnut',\n");
        fprintf(fp, "      data: {\n");
        fprintf(fp, "        labels: [");
        for (int i = 0; i < NUM_CATEGORIES; i++) {
            fprintf(fp, "'%s'%s", summaries[i].name, (i < NUM_CATEGORIES - 1 ? ", " : ""));
        }
        fprintf(fp, "],\n");
        fprintf(fp, "        datasets: [{\n");
        fprintf(fp, "          data: [");
        for (int i = 0; i < NUM_CATEGORIES; i++) {
            fprintf(fp, "%.2f%s", summaries[i].total, (i < NUM_CATEGORIES - 1 ? ", " : ""));
        }
        fprintf(fp, "],\n");
        fprintf(fp, "          backgroundColor: [\n");
        fprintf(fp, "            '#f87171', '#fb923c', '#facc15', '#4ade80',\n");
        fprintf(fp, "            '#60a5fa', '#a78bfa', '#94a3b8'\n");
        fprintf(fp, "          ],\n");
        fprintf(fp, "          borderWidth: 2,\n");
        fprintf(fp, "          borderColor: '#ffffff'\n");
        fprintf(fp, "        }]\n");
        fprintf(fp, "      },\n");
        fprintf(fp, "      options: {\n");
        fprintf(fp, "        responsive: true,\n");
        fprintf(fp, "        maintainAspectRatio: false,\n");
        fprintf(fp, "        plugins: {\n");
        fprintf(fp, "          legend: { position: 'right' },\n");
        fprintf(fp, "          tooltip: {\n");
        fprintf(fp, "            callbacks: {\n");
        fprintf(fp, "              label: function(context) {\n");
        fprintf(fp, "                let val = context.parsed || 0;\n");
        fprintf(fp, "                return ' ' + context.label + ': Rs. ' + val.toFixed(2);\n");
        fprintf(fp, "              }\n");
        fprintf(fp, "            }\n");
        fprintf(fp, "          }\n");
        fprintf(fp, "        }\n");
        fprintf(fp, "      }\n");
        fprintf(fp, "    });\n");

        // Monthly Bar Chart
        fprintf(fp, "    const monthCtx = document.getElementById('monthlyChart').getContext('2d');\n");
        fprintf(fp, "    new Chart(monthCtx, {\n");
        fprintf(fp, "      type: 'bar',\n");
        fprintf(fp, "      data: {\n");
        fprintf(fp, "        labels: [");
        for (int m = 0; m < month_count; m++) {
            fprintf(fp, "'%s'%s", months[m], (m < month_count - 1 ? ", " : ""));
        }
        fprintf(fp, "],\n");
        fprintf(fp, "        datasets: [{\n");
        fprintf(fp, "          label: 'Total Spent (Rs.)',\n");
        fprintf(fp, "          data: [");
        for (int m = 0; m < month_count; m++) {
            fprintf(fp, "%.2f%s", month_totals[m], (m < month_count - 1 ? ", " : ""));
        }
        fprintf(fp, "],\n");
        fprintf(fp, "          backgroundColor: '#6366f1',\n");
        fprintf(fp, "          borderRadius: 6\n");
        fprintf(fp, "        }]\n");
        fprintf(fp, "      },\n");
        fprintf(fp, "      options: {\n");
        fprintf(fp, "        responsive: true,\n");
        fprintf(fp, "        maintainAspectRatio: false,\n");
        fprintf(fp, "        scales: {\n");
        fprintf(fp, "          y: {\n");
        fprintf(fp, "            beginAtZero: true,\n");
        fprintf(fp, "            ticks: {\n");
        fprintf(fp, "              callback: function(val) { return 'Rs. ' + val; }\n");
        fprintf(fp, "            }\n");
        fprintf(fp, "          }\n");
        fprintf(fp, "        },\n");
        fprintf(fp, "        plugins: {\n");
        fprintf(fp, "          legend: { display: false },\n");
        fprintf(fp, "          tooltip: {\n");
        fprintf(fp, "            callbacks: {\n");
        fprintf(fp, "              label: function(context) {\n");
        fprintf(fp, "                return ' Spent: Rs. ' + context.parsed.y.toFixed(2);\n");
        fprintf(fp, "              }\n");
        fprintf(fp, "            }\n");
        fprintf(fp, "          }\n");
        fprintf(fp, "        }\n");
        fprintf(fp, "      }\n");
        fprintf(fp, "    });\n");

        fprintf(fp, "  </script>\n");
    }

    fprintf(fp, "  </div>\n");
    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");

    fclose(fp);
    return 1;
}
