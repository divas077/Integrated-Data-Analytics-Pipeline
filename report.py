#!/usr/bin/env python3
"""
report.py — FinTrack Python Data Layer
Reads the expenses.csv produced by the C++ engine and generates:
  - Monthly summary tables
  - Category breakdowns
  - Budget analysis
  - Rich ASCII visualizations
Usage:
  python report.py
  python report.py --month 2025-06
  python report.py --category Food
  python report.py --export report_2025_06.json
"""

import csv
import json
import argparse
import os
import sys
from datetime import datetime
from collections import defaultdict
from pathlib import Path

# ── ANSI colours ──────────────────────────────────────────────────────────────
RESET   = "\033[0m"
BOLD    = "\033[1m"
RED     = "\033[31m"
GREEN   = "\033[32m"
YELLOW  = "\033[33m"
CYAN    = "\033[36m"
MAGENTA = "\033[35m"

CSV_FILE = "expenses.csv"

# ── Data Loading ──────────────────────────────────────────────────────────────
def load_expenses(csv_path: str) -> tuple[list[dict], float]:
    """Parse CSV into list of expense dicts. Also reads budget metadata."""
    expenses    = []
    budget_limit = 0.0

    if not Path(csv_path).exists():
        print(f"{RED}[ERROR] {csv_path} not found. Run the C++ app first.{RESET}")
        sys.exit(1)

    with open(csv_path, newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        next(reader, None)          # skip header

        for row in reader:
            if not row:
                continue
            # Budget metadata row
            if row[0].startswith('#budget'):
                try:
                    budget_limit = float(row[1])
                except (IndexError, ValueError):
                    pass
                continue
            try:
                expenses.append({
                    'id'      : int(row[0]),
                    'amount'  : float(row[1]),
                    'category': row[2].strip(),
                    'note'    : row[3].strip(),
                    'date'    : row[4].strip(),
                    'month'   : row[4][:7],
                })
            except (IndexError, ValueError):
                pass    # skip malformed rows

    return expenses, budget_limit


# ── Helpers ───────────────────────────────────────────────────────────────────
def bar(value: float, max_val: float, width: int = 30, char: str = '█') -> str:
    filled = int((value / max_val) * width) if max_val else 0
    return char * filled + '░' * (width - filled)


def separator(width: int = 70) -> None:
    print("  " + "─" * width)


def fmt(amount: float) -> str:
    return f"₹{amount:,.2f}"


# ── Report: Monthly Summary ───────────────────────────────────────────────────
def monthly_summary(expenses: list[dict], month: str, budget: float) -> None:
    rows = [e for e in expenses if e['month'] == month]
    if not rows:
        print(f"{YELLOW}  No data for {month}.{RESET}")
        return

    by_cat: dict[str, float] = defaultdict(float)
    for e in rows:
        by_cat[e['category']] += e['amount']

    grand_total = sum(by_cat.values())
    max_val     = max(by_cat.values())

    separator()
    print(f"{BOLD}  📊  Monthly Summary — {month}{RESET}")
    separator()
    print(f"{BOLD}  {'Category':<18} {'Amount':>12} {'Share':>8}  Chart{RESET}")
    separator()

    for cat, amt in sorted(by_cat.items(), key=lambda x: -x[1]):
        share  = (amt / grand_total) * 100
        b      = bar(amt, max_val, 20)
        colour = CYAN if share < 40 else (YELLOW if share < 70 else RED)
        print(f"  {cat:<18} {fmt(amt):>12} {share:>7.1f}%  {colour}{b}{RESET}")

    separator()
    print(f"{BOLD}  {'TOTAL':<18} {fmt(grand_total):>12}   ({len(rows)} transactions){RESET}")
    separator()

    # Budget analysis
    if budget > 0:
        pct  = (grand_total / budget) * 100
        left = budget - grand_total
        print()
        if grand_total > budget:
            print(f"{BOLD}{RED}  ⚠  Over budget by {fmt(abs(left))} ({pct:.1f}% of {fmt(budget)}){RESET}")
        else:
            colour = YELLOW if pct > 80 else GREEN
            print(f"{colour}  ✓ Budget used: {pct:.1f}%  |  Remaining: {fmt(left)}{RESET}")
            print(f"  [{bar(grand_total, budget, 40)}]")
        separator()


# ── Report: Category Drill-down ───────────────────────────────────────────────
def category_report(expenses: list[dict], category: str, month: str = '') -> None:
    rows = [e for e in expenses
            if e['category'].lower() == category.lower()
            and (not month or e['month'] == month)]

    if not rows:
        print(f"{YELLOW}  No entries for category '{category}'.{RESET}")
        return

    total = sum(e['amount'] for e in rows)
    separator()
    label = f"{category}" + (f" — {month}" if month else "")
    print(f"{BOLD}  🗂  Category Report: {label}{RESET}")
    separator()
    print(f"{BOLD}  {'ID':>4}  {'Date':<12} {'Amount':>12}  Note{RESET}")
    separator()

    for e in sorted(rows, key=lambda x: x['date']):
        print(f"  {e['id']:>4}  {e['date']:<12} {fmt(e['amount']):>12}  {e['note']}")

    separator()
    print(f"{BOLD}  Total in {category}: {fmt(total)}  ({len(rows)} transactions){RESET}")
    separator()


# ── Report: All-time overview ─────────────────────────────────────────────────
def overview(expenses: list[dict], budget: float) -> None:
    if not expenses:
        print(f"{YELLOW}  No expenses found.{RESET}")
        return

    months: dict[str, float] = defaultdict(float)
    for e in expenses:
        months[e['month']] += e['amount']

    separator()
    print(f"{BOLD}  📈  FinTrack — All-Time Overview{RESET}")
    separator()
    print(f"  Total records  : {len(expenses)}")
    print(f"  Total spent    : {fmt(sum(e['amount'] for e in expenses))}")
    print(f"  Months tracked : {len(months)}")
    if budget:
        print(f"  Monthly budget : {fmt(budget)}")
    separator()

    print(f"{BOLD}  Month-by-Month:{RESET}")
    max_val = max(months.values())
    for m, amt in sorted(months.items()):
        colour = YELLOW if (budget and amt > budget) else CYAN
        print(f"  {m}  {fmt(amt):>12}  {colour}{bar(amt, max_val, 25)}{RESET}")

    separator()


# ── Export: JSON ──────────────────────────────────────────────────────────────
def export_json(expenses: list[dict], budget: float, out_path: str) -> None:
    data = {
        "generated_at"   : datetime.now().isoformat(),
        "budget_limit"   : budget,
        "total_expenses" : len(expenses),
        "grand_total"    : sum(e['amount'] for e in expenses),
        "expenses"       : expenses,
    }
    with open(out_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=2, ensure_ascii=False)
    print(f"{GREEN}  ✓ Exported to {out_path}{RESET}")


# ── Entry Point ───────────────────────────────────────────────────────────────
def main() -> None:
    parser = argparse.ArgumentParser(description="FinTrack Python Report Layer")
    parser.add_argument('--csv',      default=CSV_FILE,  help="Path to expenses CSV")
    parser.add_argument('--month',    default='',        help="Filter by YYYY-MM")
    parser.add_argument('--category', default='',        help="Category drill-down")
    parser.add_argument('--export',   default='',        help="Export to JSON file")
    args = parser.parse_args()

    expenses, budget = load_expenses(args.csv)

    print(f"\n{CYAN}{BOLD}  FinTrack — Python Reporting Layer{RESET}\n")

    if args.export:
        export_json(expenses, budget, args.export)
        return

    if args.category:
        category_report(expenses, args.category, args.month)
    elif args.month:
        monthly_summary(expenses, args.month, budget)
    else:
        overview(expenses, budget)
        if expenses:
            # Auto-show current month summary
            cur_month = datetime.now().strftime('%Y-%m')
            if any(e['month'] == cur_month for e in expenses):
                print()
                monthly_summary(expenses, cur_month, budget)

    print()


if __name__ == '__main__':
    main()
