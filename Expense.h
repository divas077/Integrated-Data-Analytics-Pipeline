#pragma once
#include <string>

/*
 * Expense — Core data structure representing a single financial transaction.
 * Attributes: id, amount, category, note, date
 */
struct Expense {
    int         id;
    double      amount;
    std::string category;
    std::string note;
    std::string date;       // Format: YYYY-MM-DD

    Expense() = default;
    Expense(int id, double amount, const std::string& category,
            const std::string& note, const std::string& date)
        : id(id), amount(amount), category(category), note(note), date(date) {}
};
