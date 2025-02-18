#include "aps.h"

// Function to validate if a file exists
bool isValidFile(const string &filename) {
    ifstream file(filename);
    return file.good();
}

// Function to validate CSV format (basic check)
bool isValidCSV(const string &line) {
    int commaCount = count(line.begin(), line.end(), ',');
    return commaCount == 3; // Expecting 4 fields (date, name, category, amount)
}

void parseCSV(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;
    vector<string> categories; // Store category headers

    // Read the header row (category names)
    if (getline(file, line)) {
        stringstream ss(line);
        string category;
        
        getline(ss, category, ','); // Skip first column (Date)
        while (getline(ss, category, ',')) {
            categories.push_back(category);
        }
    }

    // Read each row of data
    while (getline(file, line)) {
        stringstream ss(line);
        string date, amountStr;
        
        getline(ss, date, ','); // Extract date

        // Iterate through all categories in the row
        for (size_t i = 0; i < categories.size(); i++) {
            if (!getline(ss, amountStr, ',')) {
                continue;
            }

            try {
                double amount = stod(amountStr);
                Expense newExpense = {categories[i], amount};
                expenseData[date][categories[i]].push_back(newExpense);
            } catch (exception &e) {
                cerr << "Warning: Invalid amount in row: " << line << endl;
            }
        }
    }
    file.close();
}

// Function to check if the date is in YYYY-MM-DD format
bool validateDateFormat(const string &date) {
    regex datePattern(R"(\d{4}-\d{2}-\d{2})"); // Regular expression for YYYY-MM-DD
    return regex_match(date, datePattern);
}

void displayExpenses() {
    if (expenseData.empty()) {
        cout << "No expenses recorded." << endl;
        return;
    }

    cout << "\n---------------------------------------------\n";
    cout << "|    Date    |   Category   |    Amount ($)  |\n";
    cout << "---------------------------------------------\n";

    for (const auto &dateEntry : expenseData) {
        for (const auto &categoryEntry : dateEntry.second) {
            for (const auto &exp : categoryEntry.second) {
                cout << "| " << setw(10) << left << dateEntry.first
                     << "| " << setw(12) << left << categoryEntry.first
                     << "| " << setw(12) << fixed << setprecision(2) << exp.amount << " |\n";
            }
        }
    }
    cout << "---------------------------------------------\n";
}

// Function to add a new expense with user input
void addExpense() {
    string date;
    cout << "Enter the date (YYYY-MM-DD): ";
    cin >> date;

    // Validate Date Format
    if (!validateDateFormat(date)) {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }

    vector<string> categories = {"food", "travel", "work", "regular_expenses", "fun", "extra", "snacks"};
    map<string, double> expenseEntry;

    // Take input for each category
    for (const auto &category : categories) {
        double amount;
        cout << "Enter amount for " << category << ": ";
        cin >> amount;
        expenseEntry[category] = amount;
    }

    // Store in expenseData
    for (const auto &entry : expenseEntry) {
        Expense newExpense = {entry.first, entry.second};  // category, amount
        if (expenseData[date][entry.first].empty()) {
            expenseData[date][entry.first] = {newExpense};  // Initialize if empty
        } else {
            expenseData[date][entry.first].push_back(newExpense);  // Append new expense
        }
    }

    // Append to CSV file
    ofstream file("filename.csv", ios::app);  // Append mode
    if (file.is_open()) {
        file << "\n";
        file << date;
        for (const auto &category : categories) {
            file << "," << expenseEntry[category];
        }
        file.close();
        cout << "Expense added successfully!\n";
    } else {
        cout << "Error opening file.\n";
    }
}

void updateExpense() {
    string date, category;
    double newAmount;
    
    // Take user input
    cout << "Enter the date (YYYY-MM-DD): ";
    cin >> date;
    cout << "Enter the category: ";
    cin >> category;
    cout << "Enter the new amount: ";
    cin >> newAmount;

    // ✅ Check if the date exists in `expenseData`
    if (expenseData.find(date) != expenseData.end() &&
        expenseData[date].find(category) != expenseData[date].end()) {
        
        // ✅ Update amount for all expenses in the given category
        for (auto &exp : expenseData[date][category]) {
            exp.amount = newAmount;
        }

        cout << "Expense updated successfully in memory!" << endl;

        // ✅ Save updated `expenseData` to the CSV file
        saveToCSV("filename.csv");

    } else {
        cout << "Expense not found!" << endl;
    }
}

void deleteExpenses() {
    string filename = "filename.csv";
    string date;
    cout << "Enter the date (YYYY-MM-DD) to delete all expenses: ";
    cin >> date;

    // Validate Date Format
    if (!validateDateFormat(date)) {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }

    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error: Cannot open file " << filename << " for reading.\n";
        return;
    }

    ofstream outFile("temp.csv");
    if (!outFile.is_open()) {
        cerr << "Error: Cannot open temporary file for writing.\n";
        inFile.close();
        return;
    }

    string line;
    bool found = false;
    bool isFirstLine = true;

    // Read the first line (header) and keep it
    if (getline(inFile, line)) {
        outFile << line << "\n"; // Keep header row
    }

    // Read the file line by line and write all lines except the ones with the given date
    while (getline(inFile, line)) {
        stringstream ss(line);
        string recordDate;
        getline(ss, recordDate, ',');

        if (recordDate == date) {
            found = true; // Mark that the date was found and will be deleted
            continue; // Skip writing this line to the new file
        }

        // ✅ Ensure no extra blank lines in CSV
        if (!isFirstLine) {
            outFile << "\n"; // Add newline only before new entries (not at the end)
        }

        outFile << line; // Write the remaining lines
        isFirstLine = false;
    }

    inFile.close();
    outFile.close();

    // Replace the old file with the new one
    if (found) {
        remove(filename.c_str());
        rename("temp.csv", filename.c_str());
        cout << "Expenses for date " << date << " have been deleted successfully.\n";
    } else {
        remove("temp.csv");
        cout << "No expenses found for the entered date.\n";
    }
}

void saveToCSV(const string &filename) {
    ofstream file(filename, ios::trunc); // Overwrites the file
    if (!file.is_open()) {
        cerr << "Error: Cannot open file for writing: " << filename << endl;
        return;
    }

    // ✅ Step 1: Get a list of all categories
    set<string> allCategories;
    for (const auto &dateEntry : expenseData) {
        for (const auto &categoryEntry : dateEntry.second) {
            allCategories.insert(categoryEntry.first);
        }
    }

    // ✅ Step 2: Write header row (Date, Food, Travel, Work, etc.)
    file << "Date";
    for (const auto &category : allCategories) {
        file << "," << category;
    }
    file << "\n";

    // ✅ Step 3: Write expense data in proper format
    for (const auto &dateEntry : expenseData) {
        file << dateEntry.first; // Write Date

        // Store category expenses for the current date
        map<string, double> categoryAmounts;
        for (const auto &categoryEntry : dateEntry.second) {
            double totalAmount = 0;
            for (const auto &exp : categoryEntry.second) {
                totalAmount += exp.amount;
            }
            categoryAmounts[categoryEntry.first] = totalAmount;
        }

        // Write category amounts in correct order
        for (const auto &category : allCategories) {
            if (categoryAmounts.find(category) != categoryAmounts.end()) {
                file << "," << categoryAmounts[category]; // Write amount
            } else {
                file << ","; // Write empty value if category not present
            }
        }
    }

    file.close();
    cout << "Expenses saved successfully to " << filename << "!\n";
}

// Main function
int main() {
    string filename = "filename.csv";

    if (!isValidFile(filename)) {
        cerr << "Error: File not found or inaccessible." << endl;
        return 1;
    }

    // Load existing expenses from CSV file
    parseCSV(filename);

    int choice;
    do {
        cout << "\nExpense Tracker Menu:";
        cout << "\n1. Display Expenses";
        cout << "\n2. Add Expense";
        cout << "\n3. Update Expense";
        cout << "\n4. Delete Expense";
        cout << "\n5. Save & Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {  // Display all expenses
                displayExpenses();
                break;
            }
            case 2:{
                addExpense();
                break;
            }
            case 3:{
                updateExpense();
                break;
            }
            case 4: {  // Delete an expense
                deleteExpenses();
                break;
            }
            case 5: {  // Save & Exit
                saveToCSV(filename);
                cout << "Expenses saved. Exiting program...\n";
                break;
            }
            default:
                cout << "Invalid choice! Please enter a valid option.\n";
        }
    } while (choice != 5);
    return 0;
}