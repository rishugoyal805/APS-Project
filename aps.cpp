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

    // Read each row of data
    while (getline(file, line)) {
        stringstream ss(line);
        string fullDate, amountStr;
        
        getline(ss, fullDate, ','); // Extract date

        // ✅ Validate Date Length Before Using `substr()`
        if (fullDate.length() < 10) {
            cerr << "Warning: Invalid date format in row: " << line << endl;
            continue;
        }

        int month = stoi(fullDate.substr(5, 2)) - 1;  // Convert "MM" to 0-based index
        int day = stoi(fullDate.substr(8, 2)) - 1;    // Convert "DD" to 0-based index

        // ✅ Validate Month and Day Range
        if (month < 0 || month > 11 || day < 0 || day > 30) {
            cerr << "Warning: Invalid date in row: " << line << endl;
            continue;
        }

        // ✅ Read essential category amounts
        for (size_t i = 0; i < essentialCategories.size(); i++) {
            if (getline(ss, amountStr, ',')) {
                try {
                    int amount = stoi(amountStr);
                    expenseData[month][day].first[i] += amount;
                } catch (...) {
                    cerr << "Warning: Invalid amount in row: " << line << endl;
                }
            }
        }

        // ✅ Read non-essential category amounts
        for (size_t i = 0; i < nonEssentialCategories.size(); i++) {
            if (getline(ss, amountStr, ',')) {
                try {
                    int amount = stoi(amountStr);
                    expenseData[month][day].second[i] += amount;
                } catch (...) {
                    cerr << "Warning: Invalid amount in row: " << line << endl;
                }
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
    bool hasData = false;

    cout << "\n------------------------------------------------------\n";
    cout << "|    Date    |  Food  |  Regular  |  Work  |  Snaks  |  Travel  |  Fun  |  Extra  |\n";
    cout << "------------------------------------------------------\n";

    for (int month = 0; month < 12; month++) {
        for (int day = 0; day < 31; day++) {
            // ✅ Check if all expenses for the day are zero
            bool hasExpense = false;
            for (int val : expenseData[month][day].first) {
                if (val > 0) { hasExpense = true; break; }
            }
            for (int val : expenseData[month][day].second) {
                if (val > 0) { hasExpense = true; break; }
            }

            if (!hasExpense) continue; // Skip empty days

            hasData = true;

            // ✅ Format date correctly
            cout << "| " << "2024-" << setw(2) << month + 1
                 << "-" << setw(2) << day + 1 << " | ";

            // ✅ Print essential expenses
            for (int amount : expenseData[month][day].first) {
                cout << setw(5) << amount << " ";
            }
            cout << "| ";

            // ✅ Print non-essential expenses
            for (int amount : expenseData[month][day].second) {
                cout << setw(5) << amount << " ";
            }
            cout << "|\n";
        }
    }

    if (!hasData) {
        cout << "No expenses recorded." << endl;
    }

    cout << "------------------------------------------------------\n";
}

//Function to add a new expense with user input
void addExpense() {
    string date;
    cout << "Enter the date (YYYY-MM-DD): ";
    cin >> date;

    // Validate Date Format
    if (!validateDateFormat(date)) {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }

    // Extract month and day from the date
    int month = stoi(date.substr(5, 2)) - 1; // 0-based index for month
    int day = stoi(date.substr(8, 2)) - 1; // 0-based index for day

    if (month < 0 || month >= 12 || day < 0 || day >= 31) {
        cout << "Invalid date! Month or day out of range.\n";
        return;
    }

    // Map to hold expenses for each category
    map<string, double> expenseEntry;

    // Take input for each category
    for (const auto &category : essentialCategories) {
        double amount;
        cout << "Enter amount for " << category << ": ";
        cin >> amount;
        expenseEntry[category] = amount;
    }

    for (const auto &category : nonEssentialCategories) {
        double amount;
        cout << "Enter amount for " << category << ": ";
        cin >> amount;
        expenseEntry[category] = amount;
    }

    // Store in expenseData
    // Update essential categories (first vector)
    for (size_t i = 0; i < essentialCategories.size(); ++i) {
        expenseData[month][day].first[i] += expenseEntry[essentialCategories[i]];  // Add to respective category
    }

    // Update non-essential categories (second vector)
    for (size_t i = 0; i < nonEssentialCategories.size(); ++i) {
        expenseData[month][day].second[i] += expenseEntry[nonEssentialCategories[i]];  // Add to respective category
    }

    // Append to CSV file
    ofstream file("filename.csv", ios::app);  // Append mode
    if (file.is_open()) {
        file << "\n" << date;
        // Append expenses for each category
        for (const auto &category : essentialCategories) {
            file << "," << expenseEntry[category];
        }
        for (const auto &category : nonEssentialCategories) {
            file << "," << expenseEntry[category];
        }
        file.close();
        cout << "Expense added successfully!\n";
    } else {
        cout << "Error opening file.\n";
    }
}

void updateExpense() {
    string filename = "filename.csv";
    string date, category;
    double newAmount;

    // Take user input
    cout << "Enter the date (YYYY-MM-DD): ";
    cin >> date;

    // Validate Date Format
    if (!validateDateFormat(date)) {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }

    // Extract month and day from the date
    int month = stoi(date.substr(5, 2)) - 1; // 0-based index for month
    int day = stoi(date.substr(8, 2)) - 1; // 0-based index for day

    if (month < 0 || month >= 12 || day < 0 || day >= 31) {
        cout << "Invalid date! Month or day out of range.\n";
        return;
    }

    // ✅ Check if the month and day exist in `expenseData`
    if (month < expenseData.size() && day < expenseData[month].size()) {

        // Take input for category and new amount
        cout << "Enter the category to update: ";
        cin >> category;
        cout << "Enter the new amount: ";
        cin >> newAmount;

        // ✅ Check if the category exists for the given date
        bool categoryFound = false;

        // Update amount for the specified category in memory
        for (auto &exp : expenseData[month][day].first) {
            if (essentialCategories[&exp - &expenseData[month][day].first[0]] == category) {
                exp = newAmount;
                categoryFound = true;
                break;
            }
        }
        for (auto &exp : expenseData[month][day].second) {
            if (nonEssentialCategories[&exp - &expenseData[month][day].second[0]] == category) {
                exp = newAmount;
                categoryFound = true;
                break;
            }
        }

        if (!categoryFound) {
            cout << "Category not found for the specified date!" << endl;
            return;
        }

        cout << "Expense updated successfully in memory!" << endl;

        // Open the CSV file for reading
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "Error: Cannot open file " << filename << " for reading.\n";
            return;
        }

        // Create a temporary file to write the updated content
        ofstream outFile("temp.csv");
        if (!outFile.is_open()) {
            cerr << "Error: Cannot open temporary file for writing.\n";
            inFile.close();
            return;
        }

        string line;
        bool found = false;

        // Read the first line (header) and keep it
        if (getline(inFile, line)) {
            outFile << line << "\n"; // Keep header row
        }

        // Read the file line by line and update the specific row if found
        while (getline(inFile, line)) {
            stringstream ss(line);
            string recordDate, recordCategory;
            getline(ss, recordDate, ','); // Get the date from the line
            getline(ss, recordCategory, ','); // Get the category from the line

            // Check if the current row matches the date
            if (recordDate == date) {
                found = true;

                // Replace the amount with the new value in the respective category
                stringstream newLineStream;
                newLineStream << recordDate;

                // Append the new updated expenses for each category
                for (const auto &category : essentialCategories) {
                    newLineStream << "," << expenseData[month][day].first[&category - &essentialCategories[0]];
                }
                for (const auto &category : nonEssentialCategories) {
                    newLineStream << "," << expenseData[month][day].second[&category - &nonEssentialCategories[0]];
                }

                outFile << newLineStream.str(); // Write the updated line
            } else {
                // If no update, just write the line as it is
                outFile << line;
            }
            outFile << "\n";
        }

        inFile.close();
        outFile.close();

        // Replace the old file with the new one
        if (found) {
            remove(filename.c_str()); // Delete the old file
            rename("temp.csv", filename.c_str()); // Rename the temporary file to the original filename
            cout << "Expense for date " << date << " has been updated successfully.\n";
        } else {
            remove("temp.csv"); // If no matching record is found, remove the temporary file
            cout << "Expense not found for the entered date.\n";
        }

    } else {
        cout << "Expense not found in the data structure!" << endl;
    }
}

// Function to delete expenses for a given date
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

    // Extract month and day from the date
    int month = stoi(date.substr(5, 2)) - 1; // 0-based index for month
    int day = stoi(date.substr(8, 2)) - 1; // 0-based index for day

    if (month < 0 || month >= 12 || day < 0 || day >= 31) {
        cout << "Invalid date! Month or day out of range.\n";
        return;
    }

    // Clear the expenses for the specified date
    expenseData[month][day].first = {0, 0, 0}; // Clear essential category expenses
    expenseData[month][day].second = {0, 0, 0}; // Clear non-essential category expenses

    // Open the CSV file for reading
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error: Cannot open file " << filename << " for reading.\n";
        return;
    }

    // Create a temporary file to write the updated content
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
        outFile << line << "\n"; // Keep the header row
    }

    // Read the file line by line and write all lines except the one with the given date
    while (getline(inFile, line)) {
        stringstream ss(line);
        string recordDate;
        getline(ss, recordDate, ',');

        // If the date matches, skip writing this line to the new file
        if (recordDate == date) {
            found = true; // Mark that the date was found and will be deleted
            continue; // Skip writing this line
        }

        // Ensure no extra blank lines in CSV (write the remaining lines)
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
        remove(filename.c_str()); // Delete the old file
        rename("temp.csv", filename.c_str()); // Rename the temporary file to the original filename
        cout << "Expenses for date " << date << " have been deleted successfully.\n";
    } else {
        remove("temp.csv"); // If no matching date is found, remove the temporary file
        cout << "No expenses found for the entered date.\n";
    }
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
        cout << "\n5. Exit";
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
            case 5: {  // Exit
                cout << "Exiting program...\n";
                break;
            }
            default:
                cout << "Invalid choice! Please enter a valid option.\n";
        }
    } while (choice != 5);
    return 0;
}