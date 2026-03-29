#pragma once
#include "Expense.h"
#include <string>
#include <vector>

/*
 * ExpenseManager — Handles all CRUD operations, CSV persistence,
 * budget enforcement, and summary report generation.
 */
class ExpenseManager {
public:
    explicit ExpenseManager(const std::string& csvFile);

    // Core CRUD
    void addExpense(double amount, const std::string& category,
                    const std::string& note, const std::string& date);
    void displayAll() const;
    void deleteExpense(int id);

    // Budget
    void setBudgetLimit(double limit);
    void checkBudget(const std::string& month) const;

    // Reports  — O(n) traversal
    void monthlySummary(const std::string& month) const;
    void categoryReport(const std::string& category, const std::string& month) const;

private:
    std::string            m_csvFile;
    std::vector<Expense>   m_expenses;
    double                 m_budgetLimit;
    int                    m_nextId;

    // Persistence helpers
    void loadFromCSV();
    void saveToCSV() const;

    // Utility
    std::string todayDate() const;
    double totalForMonth(const std::string& month) const;
    void printSeparator() const;
};
