#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip> // Include for formatting
#include <regex>  // For date validation
#include <set>  // For unique categories


using namespace std;

// Struct to store expense data
class Expense {
    public:
        string category;
        double amount;
};

vector<vector<pair<vector<int>, vector<int>>>> expenseData(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<string> essentialCategories = {"food", "regular_expenses", "work"};
vector<string> nonEssentialCategories = {"snacks", "travel", "fun", "extra"};

// Store expense data: date → category → list of expenses
// map<string, map<string, vector<Expense>>> expenseData;
const string filename = "filename.csv";
// Function declarations
bool isValidFile(const string &filename);
bool isValidCSV(const string &line);
void parseCSV(const string &filename);
void displayExpenses();
//void addExpense(const string &date, const string &category, const Expense &expense);
void addExpense();
void deleteExpense();
void updateExpense();
void saveToCSV(const string &filename);
bool validateDateFormat(const string &date);

#endif