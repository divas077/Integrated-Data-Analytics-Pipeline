#  Integrated Data Analytics Pipeline

> A multi-language command line application for tracking personal expenses, managing budgets, and generating financial reports — built with C++17 and Python 3.

---

## What It Does

FinTrack lets you manage your daily expenses directly from the terminal. You can add expenses, set a monthly budget, get warnings when you exceed it, and generate detailed category-wise reports. All data is saved to a CSV file that you can open directly in Excel.

---

## Features

| Feature | Description |
|---|---|
| Add Expense | Record amount, category, note and date |
| View All | Display all expenses in a formatted table |
| Monthly Summary | Category-wise breakdown with percentage share |
| Category Report | Filter expenses by category and month |
| Budget Alerts | Red warning when monthly limit is exceeded |
| CSV Persistence | Auto-saves every change to expenses.csv |
| Python Reports | Rich terminal reports + JSON export |
| Budget Tracker | Visual progress bar showing spending vs limit |

---

## Project Structure

```
fintrack/
│
├── main.cpp            # CLI menu, user input, display
├── Expense.h           # Expense data structure (struct)
├── ExpenseManager.h    # Class declaration (blueprint)
├── ExpenseManager.cpp  # All logic — CRUD, File I/O, reports
├── report.py           # Python reporting layer
├── Makefile            # Build automation
└── expenses.csv        # Auto-generated when you add first expense
```

---

## How The Files Work Together

```
main.cpp          →   takes input from user, calls functions
ExpenseManager    →   handles all logic and calculations
expenses.csv      →   C++ writes here, Python reads from here
report.py         →   Python reads CSV and generates reports
```

The C++ engine and Python layer communicate entirely through the CSV file. No direct connection between them — clean separation of concerns.

---

## Tech Stack

| Technology | Purpose |
|---|---|
| C++17 | Core logic, OOP, File I/O |
| Python 3 | Data reporting, JSON export |
| STL — vector | Storing expenses in memory |
| STL — map | Category-wise aggregation |
| STL — fstream | Reading and writing CSV file |
| csv, json, argparse | Python standard libraries |

---

## OOP Concepts Used

- **Class** — `ExpenseManager` handles all operations
- **Struct** — `Expense` represents one transaction
- **Encapsulation** — public functions, private data members
- **Constructor** — initializer list, default constructor
- **Const correctness** — read-only functions marked const

---

## Getting Started

### Requirements

- g++ compiler with C++17 support
- MinGW (Windows) or GCC (Linux/macOS)
- Python 3.x
- make / mingw32-make

### Installation

**Clone the repository:**
```bash
git clone https://github.com/yourusername/fintrack.git
cd fintrack
```

### Build

**Linux / macOS:**
```bash
make
./fintrack
```

**Windows:**
```bash
mingw32-make
fintrack.exe
```

---

## Running The Program

### C++ CLI (add and manage expenses):
```bash
fintrack.exe          # Windows
./fintrack            # Linux/macOS
```

### Python Reports (after adding some expenses):
```bash
python3 report.py                        # all-time overview
python3 report.py --month 2026-03        # monthly summary
python3 report.py --category Food        # food expenses only
python3 report.py --export data.json     # export to JSON
```

---

## Menu Options

```
[1]  Add New Expense
[2]  View All Expenses
[3]  Monthly Summary Report
[4]  Category Report
[5]  Set Monthly Budget Limit
[6]  Check Budget Status
[7]  Delete Expense
[0]  Exit
```

---

## Categories

```
Food      Travel      Fees
Shopping  Health      Other
```

---

## CSV Format

Every expense is saved in this format:
```
id,amount,category,note,date
1,450.00,Food,Office lunch,2026-03-30
2,1200.00,Travel,Uber to airport,2026-03-31
3,3500.00,Shopping,New shoes,2026-04-01
#budget,25000.00
```

The `#budget` line stores your monthly limit. This file opens directly in Microsoft Excel or Google Sheets.

---

## Sample Output

### Adding an expense:
```
Amount (Rs.): 450
Category: Food
Note: Office lunch
Date (blank for today):

[OK] Expense added and saved to expenses.csv!
```

### Monthly summary:
```
  --------------------------------------------------------------------
  Monthly Summary - 2026-03
  --------------------------------------------------------------------
  Category             Amount (Rs.)  Share %
  --------------------------------------------------------------------
  Food                     6200.00    33.6%  **************
  Travel                   4800.00    26.0%  **********
  Shopping                 3950.00    21.4%  ********
  Fees                     2100.00    11.4%  ****
  Health                   1400.00     7.6%  ***
  --------------------------------------------------------------------
  TOTAL                   18450.00   (12 transactions)
  OK Within budget. 73.8% of Rs.25000.00 used.
```

### Budget alert:
```
!! BUDGET ALERT: Exceeded Rs.25000.00 for 2026-03!
   Total spent: Rs.26450.00
```

---

## Algorithm

All reports use **O(n) single-pass traversal** — the program loops through expenses exactly once to calculate all totals simultaneously. No nested loops, no repeated scanning.

---

## What I Learned Building This

- Multi-language system design (C++ + Python)
- Object Oriented Programming in C++
- File I/O and CSV data persistence
- STL containers — vector, map
- Input validation and error handling
- Command line argument parsing in Python
- Separation of concerns across files

---

## Future Improvements

- [ ] Web frontend (HTML/CSS/JS)
- [ ] Search expenses by date range
- [ ] Export monthly report as PDF
- [ ] Recurring expense tracking
- [ ] Multiple currency support
- [ ] Graphical charts in Python (matplotlib)

---

## Author

**Divas**
Second Year CS Student

---

## License

This project is open source and available under the MIT License.
