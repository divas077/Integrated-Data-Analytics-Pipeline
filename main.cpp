/*
 * FinTrack: Personal Expense & Budgeting CLI
 * C++17 | OOP | File I/O | CSV Persistence
 */

#include "ExpenseManager.h"
#include <iostream>
#include <limits>

// ANSI color codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

void printBanner() {
    std::cout << CYAN << BOLD;
    std::cout << R"(
  ______ _       _____              _    
 |  ____(_)     |_   _|            | |   
 | |__   _ _ __   | |  _ __ __ _  | |__ 
 |  __| | | '_ \  | | | '__/ _` | | '_ \
 | |    | | | | |_| |_| | | (_| | | | | |
 |_|    |_|_| |_|_____|_|  \__,_| |_| |_|
)" << RESET;
    std::cout << CYAN << "      Personal Expense & Budgeting CLI\n" << RESET;
    std::cout << "  ─────────────────────────────────────\n\n";
}

void printMenu() {
    std::cout << BOLD << "\n  [ MENU ]\n" << RESET;
    std::cout << "  1. Add Expense\n";
    std::cout << "  2. View All Expenses\n";
    std::cout << "  3. Monthly Summary Report\n";
    std::cout << "  4. Category Report\n";
    std::cout << "  5. Set Monthly Budget Limit\n";
    std::cout << "  6. Check Budget Status\n";
    std::cout << "  7. Delete Expense\n";
    std::cout << "  0. Exit\n";
    std::cout << "\n  Enter choice: ";
}

int getValidInt(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        std::cin >> val;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << RED << "  Invalid input. Try again.\n" << RESET;
        } else {
            std::cin.ignore();
            return val;
        }
    }
}

double getValidDouble(const std::string& prompt) {
    double val;
    while (true) {
        std::cout << prompt;
        std::cin >> val;
        if (std::cin.fail() || val < 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << RED << "  Invalid amount. Enter a positive number.\n" << RESET;
        } else {
            std::cin.ignore();
            return val;
        }
    }
}

std::string selectCategory() {
    std::cout << "\n  Categories:\n";
    std::cout << "  1. Food\n";
    std::cout << "  2. Travel\n";
    std::cout << "  3. Fees\n";
    std::cout << "  4. Shopping\n";
    std::cout << "  5. Health\n";
    std::cout << "  6. Other\n";
    int choice = getValidInt("  Select category (1-6): ");
    switch (choice) {
        case 1: return "Food";
        case 2: return "Travel";
        case 3: return "Fees";
        case 4: return "Shopping";
        case 5: return "Health";
        default: return "Other";
    }
}

int main() {
    printBanner();
    ExpenseManager manager("expenses.csv");

    int choice;
    do {
        printMenu();
        std::cin >> choice;
        std::cin.ignore();
        std::cout << "\n";

        switch (choice) {
            case 1: {
                double amount = getValidDouble("  Amount (₹): ");
                std::string category = selectCategory();
                std::string note;
                std::cout << "  Note/Description: ";
                std::getline(std::cin, note);
                std::string date;
                std::cout << "  Date (YYYY-MM-DD, leave blank for today): ";
                std::getline(std::cin, date);
                manager.addExpense(amount, category, note, date);
                std::cout << GREEN << "  ✓ Expense added successfully!\n" << RESET;
                break;
            }
            case 2:
                manager.displayAll();
                break;
            case 3: {
                std::string month;
                std::cout << "  Enter month (YYYY-MM, e.g. 2025-06): ";
                std::getline(std::cin, month);
                manager.monthlySummary(month);
                break;
            }
            case 4: {
                std::string category = selectCategory();
                std::string month;
                std::cout << "  Month (YYYY-MM, leave blank for all): ";
                std::getline(std::cin, month);
                manager.categoryReport(category, month);
                break;
            }
            case 5: {
                double limit = getValidDouble("  Set monthly budget limit (₹): ");
                manager.setBudgetLimit(limit);
                std::cout << GREEN << "  ✓ Budget limit set to ₹" << limit << "\n" << RESET;
                break;
            }
            case 6: {
                std::string month;
                std::cout << "  Check budget for month (YYYY-MM): ";
                std::getline(std::cin, month);
                manager.checkBudget(month);
                break;
            }
            case 7: {
                manager.displayAll();
                int id = getValidInt("  Enter ID to delete: ");
                manager.deleteExpense(id);
                break;
            }
            case 0:
                std::cout << CYAN << "  Goodbye! Stay financially fit. 💰\n\n" << RESET;
                break;
            default:
                std::cout << YELLOW << "  Invalid choice. Try again.\n" << RESET;
        }
    } while (choice != 0);

    return 0;
}
