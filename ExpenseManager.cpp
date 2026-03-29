#include "ExpenseManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <map>
#include <cmath>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

static const char* CSV_HEADER = "id,amount,category,note,date\n";

ExpenseManager::ExpenseManager(const std::string& csvFile)
    : m_csvFile(csvFile), m_budgetLimit(0.0), m_nextId(1)
{
    loadFromCSV();
}

std::string ExpenseManager::todayDate() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_info = std::localtime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
    return std::string(buf);
}

void ExpenseManager::loadFromCSV() {
    std::ifstream file(m_csvFile);
    if (!file.is_open()) return;
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.rfind("#budget,", 0) == 0) {
            m_budgetLimit = std::stod(line.substr(8));
            continue;
        }
        std::istringstream ss(line);
        std::string token;
        Expense e;
        try {
            std::getline(ss, token, ','); e.id       = std::stoi(token);
            std::getline(ss, token, ','); e.amount   = std::stod(token);
            std::getline(ss, token, ','); e.category = token;
            std::getline(ss, token, ','); e.note     = token;
            std::getline(ss, token, ','); e.date     = token;
            m_expenses.push_back(e);
            m_nextId = std::max(m_nextId, e.id + 1);
        } catch (...) {}
    }
}

void ExpenseManager::saveToCSV() const {
    std::ofstream file(m_csvFile, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << RED << "  [ERROR] Cannot write to " << m_csvFile << RESET << "\n";
        return;
    }
    file << CSV_HEADER;
    if (m_budgetLimit > 0.0)
        file << "#budget," << std::fixed << std::setprecision(2) << m_budgetLimit << "\n";
    for (const auto& e : m_expenses) {
        file << e.id << ","
             << std::fixed << std::setprecision(2) << e.amount << ","
             << e.category << "," << e.note << "," << e.date << "\n";
    }
}

void ExpenseManager::printSeparator() const {
    std::cout << "  " << std::string(68, '-') << "\n";
}

void ExpenseManager::addExpense(double amount, const std::string& category,
                                const std::string& note, const std::string& date)
{
    std::string d = date.empty() ? todayDate() : date;
    m_expenses.emplace_back(m_nextId++, amount, category, note, d);
    saveToCSV();
    std::string month = d.substr(0, 7);
    double total = totalForMonth(month);
    if (m_budgetLimit > 0.0 && total > m_budgetLimit) {
        std::cout << BOLD << RED
                  << "\n  !! BUDGET ALERT: Exceeded Rs."
                  << std::fixed << std::setprecision(2) << m_budgetLimit
                  << " for " << month << "! Total: Rs." << total << "\n" << RESET;
    }
}

void ExpenseManager::displayAll() const {
    if (m_expenses.empty()) {
        std::cout << YELLOW << "  No expenses recorded yet.\n" << RESET;
        return;
    }
    printSeparator();
    std::cout << BOLD << std::left
              << "  " << std::setw(5)  << "ID"
              << std::setw(12) << "Amount"
              << std::setw(12) << "Category"
              << std::setw(25) << "Note"
              << std::setw(12) << "Date" << RESET << "\n";
    printSeparator();
    for (const auto& e : m_expenses) {
        std::cout << "  " << std::left << std::setw(5) << e.id
                  << std::right << std::setw(8) << std::fixed << std::setprecision(2) << e.amount << "    "
                  << std::left << std::setw(12) << e.category
                  << std::setw(25) << e.note << std::setw(12) << e.date << "\n";
    }
    printSeparator();
    std::cout << BOLD << "  Total entries: " << m_expenses.size() << RESET << "\n";
}

void ExpenseManager::deleteExpense(int id) {
    auto it = std::find_if(m_expenses.begin(), m_expenses.end(),
                           [id](const Expense& e){ return e.id == id; });
    if (it == m_expenses.end()) {
        std::cout << RED << "  Expense with ID " << id << " not found.\n" << RESET;
        return;
    }
    m_expenses.erase(it);
    saveToCSV();
    std::cout << GREEN << "  Expense #" << id << " deleted.\n" << RESET;
}

void ExpenseManager::setBudgetLimit(double limit) {
    m_budgetLimit = limit;
    saveToCSV();
}

double ExpenseManager::totalForMonth(const std::string& month) const {
    double total = 0.0;
    for (const auto& e : m_expenses)
        if (e.date.substr(0, 7) == month)
            total += e.amount;
    return total;
}

void ExpenseManager::checkBudget(const std::string& month) const {
    if (m_budgetLimit <= 0.0) {
        std::cout << YELLOW << "  No budget limit set. Use option 5 to set one.\n" << RESET;
        return;
    }
    double total   = totalForMonth(month);
    double percent = (total / m_budgetLimit) * 100.0;
    double left    = m_budgetLimit - total;
    printSeparator();
    std::cout << BOLD << "  Budget Status - " << month << "\n" << RESET;
    printSeparator();
    std::cout << "  Monthly Limit : Rs." << std::fixed << std::setprecision(2) << m_budgetLimit << "\n";
    std::cout << "  Total Spent   : Rs." << total << "\n";
    if (total > m_budgetLimit) {
        std::cout << BOLD << RED
                  << "  Over Budget By: Rs." << std::fabs(left)
                  << "  (" << std::setprecision(1) << percent << "% used)\n"
                  << "  !! LIMIT EXCEEDED\n" << RESET;
    } else {
        int barWidth = 40;
        int filled   = static_cast<int>((percent / 100.0) * barWidth);
        std::string bar(filled, '#');
        bar += std::string(barWidth - filled, '-');
        std::string color = (percent > 80) ? YELLOW : GREEN;
        std::cout << color
                  << "  Remaining: Rs." << left
                  << "  (" << std::setprecision(1) << percent << "% used)\n"
                  << "  [" << bar << "]\n" << RESET;
    }
    printSeparator();
}

void ExpenseManager::monthlySummary(const std::string& month) const {
    std::map<std::string, double> catTotals;
    double grandTotal = 0.0;
    int count = 0;
    for (const auto& e : m_expenses) {
        if (e.date.substr(0, 7) == month) {
            catTotals[e.category] += e.amount;
            grandTotal += e.amount;
            ++count;
        }
    }
    if (count == 0) {
        std::cout << YELLOW << "  No expenses found for " << month << ".\n" << RESET;
        return;
    }
    printSeparator();
    std::cout << BOLD << "  Monthly Summary - " << month << "\n" << RESET;
    printSeparator();
    std::cout << BOLD << std::left << std::setw(20) << "  Category"
              << std::right << std::setw(14) << "Amount (Rs.)"
              << std::setw(10) << "Share %" << RESET << "\n";
    printSeparator();
    for (std::map<std::string, double>::const_iterator it = catTotals.begin();
         it != catTotals.end(); ++it) {
        const std::string& cat = it->first;
        double amt = it->second;
        double share = (amt / grandTotal) * 100.0;
        int barLen = static_cast<int>(share / 5);
        std::string bar(barLen, '*');
        std::cout << "  " << std::left << std::setw(18) << cat
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2) << amt
                  << std::setw(8) << std::setprecision(1) << share << "%  "
                  << CYAN << bar << RESET << "\n";
    }
    printSeparator();
    std::cout << BOLD << "  " << std::left << std::setw(18) << "TOTAL"
              << std::right << std::setw(10) << std::fixed << std::setprecision(2) << grandTotal
              << "  (" << count << " transactions)" << RESET << "\n";
    printSeparator();
    if (m_budgetLimit > 0.0) {
        double pct = (grandTotal / m_budgetLimit) * 100.0;
        if (pct > 100.0)
            std::cout << BOLD << RED << "  !! Over budget by Rs." << grandTotal - m_budgetLimit << "\n" << RESET;
        else
            std::cout << GREEN << "  OK Within budget. " << std::setprecision(1) << pct
                      << "% of Rs." << m_budgetLimit << " used.\n" << RESET;
    }
}

void ExpenseManager::categoryReport(const std::string& category,
                                    const std::string& month) const
{
    double total = 0.0;
    int count = 0;
    printSeparator();
    std::cout << BOLD << "  Category Report - " << category;
    if (!month.empty()) std::cout << " (" << month << ")";
    std::cout << "\n" << RESET;
    printSeparator();
    std::cout << BOLD << std::left << std::setw(5) << "  ID"
              << std::setw(12) << "Amount" << std::setw(25) << "Note" << "Date\n" << RESET;
    printSeparator();
    for (const auto& e : m_expenses) {
        if (e.category != category) continue;
        if (!month.empty() && e.date.substr(0, 7) != month) continue;
        std::cout << "  " << std::left << std::setw(5) << e.id
                  << std::right << std::setw(8) << std::fixed << std::setprecision(2) << e.amount << "    "
                  << std::left << std::setw(25) << e.note << e.date << "\n";
        total += e.amount;
        ++count;
    }
    printSeparator();
    std::cout << BOLD << "  Total in " << category << ": Rs."
              << std::fixed << std::setprecision(2) << total
              << "  (" << count << " transactions)\n" << RESET;
    printSeparator();
}