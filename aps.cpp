#include "aps.h"

using namespace std;
// Function to validate if a file exists
bool isValidFile(const string &filename)
{
    ifstream file(filename);
    return file.good();
}

bool isValidCSV(const string &line)
{
    int commaCount = count(line.begin(), line.end(), ',');
    return commaCount == 3; // Expecting 4 fields (date, name, category, amount)
}

bool isValidDoubleInput(double &input)
{
    cin >> input;
    if (cin.fail() || input < 0)
    {
        cin.clear();                                         // Clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
        return false;
    }
    return true;
}

bool isValidIntInput(int &input)
{
    cin >> input;
    if (cin.fail() || input < 0)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

bool isValidDate(int year, int month, int day)
{
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12)
        return false;
    if (day < 1)
        return false;
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
        return day <= 29; // Leap year
    return day <= daysInMonth[month - 1];
}

void detectFraudulentTransactions()
{
    vector<double> essentialSum(3, 0), nonEssentialSum(3, 0);
    vector<int> essentialCount(3, 0), nonEssentialCount(3, 0);

    for (int month = 0; month < 12; month++)
    {
        for (int day = 0; day < 31; day++)
        {
            for (int i = 0; i < 3; i++)
            {
                if (expenseData[month][day].first[i] > 0)
                {
                    essentialSum[i] += expenseData[month][day].first[i];
                    essentialCount[i]++;
                }
                if (expenseData[month][day].second[i] > 0)
                {
                    nonEssentialSum[i] += expenseData[month][day].second[i];
                    nonEssentialCount[i]++;
                }
            }
        }
    }

    vector<double> essentialMean(3, 0), nonEssentialMean(3, 0);
    vector<double> essentialSD(3, 0), nonEssentialSD(3, 0);

    for (int i = 0; i < 3; i++)
    {
        if (essentialCount[i] > 0)
            essentialMean[i] = essentialSum[i] / essentialCount[i];
        if (nonEssentialCount[i] > 0)
            nonEssentialMean[i] = nonEssentialSum[i] / nonEssentialCount[i];
    }

    for (int month = 0; month < 12; month++)
    {
        for (int day = 0; day < 31; day++)
        {
            for (int i = 0; i < 3; i++)
            {
                if (expenseData[month][day].first[i] > 0)
                {
                    essentialSD[i] += pow(expenseData[month][day].first[i] - essentialMean[i], 2);
                }
                if (expenseData[month][day].second[i] > 0)
                {
                    nonEssentialSD[i] += pow(expenseData[month][day].second[i] - nonEssentialMean[i], 2);
                }
            }
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (essentialCount[i] > 1)
            essentialSD[i] = sqrt(essentialSD[i] / essentialCount[i]);
        if (nonEssentialCount[i] > 1)
            nonEssentialSD[i] = sqrt(nonEssentialSD[i] / nonEssentialCount[i]);
    }

    // Flag to check if any fraudulent transactions are found
    bool fraudDetected = false;
    stringstream output;

    for (int month = 0; month < 12; month++)
    {
        for (int day = 0; day < 31; day++)
        {
            for (int i = 0; i < 3; i++)
            {
                if (expenseData[month][day].first[i] > essentialMean[i] + 2 * essentialSD[i])
                {
                    if (!fraudDetected)
                    {
                        fraudDetected = true;
                        output << "\nDetecting Fraudulent Transactions...\n";
                        Sleep(500);
                        output << "\nFlagged Transactions:\n";
                        output << "-------------------------------------------------\n";
                        output << "|    Date    | Category |  Amount  |   Status   |\n";
                        output << "-------------------------------------------------\n";
                    }
                    output << "| 2024-"
                           << setfill('0') << setw(2) << month + 1 << "-"
                           << setw(2) << day + 1;

                    output << setfill(' ') << " | "
                           << left << setw(8) << essentialCategories[i] << " | "
                           << right << setw(8) << expenseData[month][day].first[i] << " | "
                           << left << setw(10) << "Fraudulent" << " |\n";
                }
            }
        }
    }

    if (fraudDetected)
    {
        output << "-------------------------------------------------\n";
        cout << output.str();
    }
}

void parseCSV(const string &filename, vector<vector<pair<vector<int>, vector<int>>>> &vec)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;

    // ✅ Skip the header line
    getline(file, line);

    // Read each row of data
    while (getline(file, line))
    {
        stringstream ss(line);
        string fullDate, amountStr;
        getline(ss, fullDate, ','); // Extract date

        // ✅ Validate Date Length Before Using substr()
        if (fullDate.length() < 10)
        {
            cerr << "Warning: Invalid date format in row: " << line << endl;
            continue;
        }

        int month = stoi(fullDate.substr(5, 2)) - 1; // Convert "MM" to 0-based index
        int day = stoi(fullDate.substr(8, 2)) - 1;   // Convert "DD" to 0-based index

        // ✅ Validate Month and Day Range
        if (month < 0 || month > 11 || day < 0 || day > 30)
        {
            cerr << "Warning: Invalid date in row: " << line << endl;
            continue;
        }

        // ✅ Read essential category amounts
        for (size_t i = 0; i < essentialCategories.size(); i++)
        {
            if (getline(ss, amountStr, ','))
            {
                try
                {
                    int amount = stoi(amountStr);
                    vec[month][day].first[i] += amount;
                }
                catch (...)
                {
                    cerr << "Warning: Invalid amount in row: " << line << endl;
                }
            }
        }

        // ✅ Read non-essential category amounts
        for (size_t i = 0; i < nonEssentialCategories.size(); i++)
        {
            if (getline(ss, amountStr, ','))
            {
                try
                {
                    int amount = stoi(amountStr);
                    vec[month][day].second[i] += amount;
                }
                catch (...)
                {
                    cerr << "Warning: Invalid amount in row: " << line << endl;
                }
            }
        }
    }

    Sleep(2000);
    if (filename != "carddetails.csv")
    {
        cout << "Parsing CSV file...\n";
        Sleep(2000);
        cout << "Parsing completed successfully!\n";
        detectFraudulentTransactions();
    }
    else
    {
        cout << "\n\nParsing card details completed successfully!\n";
    }
    Sleep(2000);

    file.close();
}

// Function to check if the date is in YYYY-MM-DD format
bool validateDateFormat(const string &date)
{
    regex datePattern(R"(\d{4}-\d{2}-\d{2})"); // Regular expression for YYYY-MM-DD
    return regex_match(date, datePattern);
}

void displayExpenses()
{
    bool hasData = false;

    cout << "\n------------------------------------------------------------------------\n";
    cout << "|    Date    |  Food  |  Work  |  Travel  |  Snacks  |  Fun  |  Extra  |\n";
    cout << "------------------------------------------------------------------------\n";

    for (int month = 0; month < 12; month++)
    {
        for (int day = 0; day < 31; day++)
        {
            // ✅ Check if all expenses for the day are zero
            bool hasExpense = false;
            for (int val : expenseData[month][day].first)
            {
                if (val > 0)
                {
                    hasExpense = true;
                    break;
                }
            }
            for (int val : expenseData[month][day].second)
            {
                if (val > 0)
                {
                    hasExpense = true;
                    break;
                }
            }

            if (!hasExpense)
                continue; // Skip if no expenses for the day

            hasData = true;

            cout << "| 2024-"
                 << (month + 1 < 10 ? "0" : "") << month + 1 << "-"
                 << (day + 1 < 10 ? "0" : "") << day + 1
                 << " | ";

            cout << right << setw(6) << expenseData[month][day].first[0] << " | ";
            cout << right << setw(6) << expenseData[month][day].first[1] << " | ";
            cout << right << setw(8) << expenseData[month][day].first[2] << " | ";
            cout << right << setw(7) << expenseData[month][day].second[0] << "  | ";
            cout << right << setw(5) << expenseData[month][day].second[1] << " | ";
            cout << right << setw(7) << expenseData[month][day].second[2] << " | ";
            cout << "\n";
        }
    }

    if (!hasData)
    {
        cout << "No expenses recorded." << endl;
    }

    cout << "------------------------------------------------------------------------\n";

    Sleep(500);
}

// Function to add a new expense with user input
void addExpense()
{
    string date;
    cout << "Enter the date (YYYY-MM-DD): ";
    cin >> date;

    // Validate Date Format
    if (!validateDateFormat(date))
    {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }

    // Extract month and day from the date
    int month, day;
    try
    {
        month = stoi(date.substr(5, 2)) - 1;
        day = stoi(date.substr(8, 2)) - 1;
    }
    catch (...)
    {
        cout << "Date parsing failed. Please ensure it's numeric and in YYYY-MM-DD format.\n";
        return;
    }

    int year = stoi(date.substr(0, 4));
    if (!isValidDate(year, month + 1, day + 1))
    {
        cout << "Invalid date! Please enter a real calendar date.\n";
        return;
    }

    // Check if the date already has data in expenseData
    bool dateExists = false;

    // Check for any non-zero value in essential categories
    for (int val : expenseData[month][day].first)
    {
        if (val != 0)
        {
            dateExists = true;
            break;
        }
    }

    // If not found yet, check non-essential categories
    if (!dateExists)
    {
        for (int val : expenseData[month][day].second)
        {
            if (val != 0)
            {
                dateExists = true;
                break;
            }
        }
    }

    if (dateExists)
    {
        cout << "Error: An expense entry already exists for this date!\n";
        return;
    }

    if (month < 0 || month >= 12 || day < 0 || day >= 31)
    {
        cout << "Invalid date! Month or day out of range.\n";
        return;
    }

    // Map to hold expenses for each category
    map<string, double> expenseEntry;
    map<string, int> cards;

    // Take input for each category
    for (const auto &category : essentialCategories)
    {
        double amount;
        int c_id;

        cout << "Enter amount " << category << ": ";
        if (!isValidDoubleInput(amount))
        {
            cout << "Invalid amount entered! Please enter a non-negative number.\n";
            return;
        }

        if (amount > 0)
        {
            while (true)
            {
                cout << "Choose the card ID you used for the payment:\n";
                cout << "1 -> A\n2 -> B\n3 -> C\n4 -> None\n";
                cout << "Enter choice: ";
                if (!isValidIntInput(c_id))
                {
                    cout << "Invalid input! Please enter a number between 1 and 4.\n";
                    return;
                }

                if (c_id >= 1 && c_id <= 3)
                {
                    break; // Valid card selected
                }
                else if (c_id == 4)
                {
                    c_id = 0; // No card used
                    break;
                }
                else
                {
                    cout << "Wrong choice! Valid options: 1-4.\n";
                }
            }
        }
        else
        {
            c_id = 0; // No card needed for 0 amount
        }

        expenseEntry[category] = amount;
        cards[category] = c_id;
    }

    for (const auto &category : nonEssentialCategories)
    {
        double amount;
        int c_id;
        cout << "Enter amount " << category << ": ";
        if (!isValidDoubleInput(amount))
        {
            cout << "Invalid amount entered! Please enter a non-negative number.\n";
            return;
        }
        // bool flag = true;
        if (amount > 0)
        {
            while (true)
            {
                cout << "Choose the card ID you used for the payment:\n";
                cout << "1 -> A\n2 -> B\n3 -> C\n4 -> None\n";
                cout << "Enter choice: ";
                if (!isValidIntInput(c_id))
                {
                    cout << "Invalid input! Please enter a number between 1 and 4.\n";
                    return;
                }

                if (c_id >= 1 && c_id <= 3)
                {
                    break; // Valid card selected
                }
                else if (c_id == 4)
                {
                    c_id = 0; // No card used
                    break;
                }
                else
                {
                    cout << "Wrong choice, try again.\n";
                }
            }
        }
        else
        {
            c_id = 0; // No card needed for 0 amount
        }

        expenseEntry[category] = amount;
        cards[category] = c_id;
    }

    // Store in expenseData
    // Update essential categories (first vector)
    for (size_t i = 0; i < essentialCategories.size(); ++i)
    {
        expenseData[month][day].first[i] += expenseEntry[essentialCategories[i]]; // Add to respective category
        cardid[month][day].first[i] = ((cards[essentialCategories[i]]));
    }

    // Update non-essential categories (second vector)
    for (size_t i = 0; i < nonEssentialCategories.size(); ++i)
    {
        expenseData[month][day].second[i] += expenseEntry[nonEssentialCategories[i]]; // Add to respective category
        cardid[month][day].second[i] = ((cards[nonEssentialCategories[i]]));
    }

    // Append to CSV file
    ofstream file("OctExpenses.csv", ios::app); // Append mode
    if (file.is_open())
    {
        file << "\n"
             << date;
        // Append expenses for each category
        for (const auto &category : essentialCategories)
        {
            file << "," << expenseEntry[category];
        }
        for (const auto &category : nonEssentialCategories)
        {
            file << "," << expenseEntry[category];
        }
        file.close();
        cout << "Expense added successfully!\n";
    }
    else
    {
        cout << "Error opening file.\n";
    }
    file.close();

    ofstream file1("carddetails.csv", ios::app); // Append mode
    if (file1.is_open())
    {
        file1 << "\n"
              << date;
        // Append expenses for each category
        for (const auto &category : essentialCategories)
        {
            file1 << "," << cards[category];
        }
        for (const auto &category : nonEssentialCategories)
        {
            file1 << "," << cards[category];
        }
        file1.close();
    }
    else
    {
        cout << "Error opening file.\n";
    }
    file1.close();
}

void updateExpense()
{
    string filename = "OctExpenses.csv";
    string date, category;
    double newAmount;
    displayExpenses();
    // Take user input
    cout << "Enter the date (YYYY-MM-DD) to update the Expences: ";
    cin >> date;

    // Validate Date Format
    if (!validateDateFormat(date))
    {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }

    // Extract month and day from the date
    int month = stoi(date.substr(5, 2)) - 1; // 0-based index for month
    int day = stoi(date.substr(8, 2)) - 1;   // 0-based index for day

    if (month < 0 || month >= 12 || day < 0 || day >= 31)
    {
        cout << "Invalid date! Month or day out of range.\n";
        return;
    }
    if (expenseData.empty())
    {
        cout << "Error: No expense data loaded in memory.\n";
        return;
    }

    // ✅ Check if the month and day exist in `expenseData`
    if (month < expenseData.size() && day < expenseData[month].size())
    {
        int choice;
        cout << "Select a category to update:\n";
        cout << "1 - Food\n";
        cout << "2 - Work\n";
        cout << "3 - Travel\n";
        cout << "4 - Snacks\n";
        cout << "5 - Fun\n";
        cout << "6 - Extra\n";
        cout << "Enter your choice (1-6): ";

        if (!isValidIntInput(choice))
        {
            cout << "Invalid input! Please enter a number between 1 and 4.\n";
            return;
        }
        if (choice < 1 || choice > 6)
        {
            cout << "Invalid choice! Please enter a number between 1 and 6.\n";
            return;
        }
        switch (choice)
        {
        case 1:
            category = "food";
            break;
        case 2:
            category = "work";
            break;
        case 3:
            category = "travel";
            break;
        case 4:
            category = "snacks";
            break;
        case 5:
            category = "fun";
            break;
        case 6:
            category = "extra";
            break;
        default:
            cout << "Invalid choice! Please enter a number between 1 and 6.\n";
        }
        cout << "Enter the new amount: ";
        if (!isValidDoubleInput(newAmount))
        {
            cout << "Invalid amount entered! Please enter a non-negative number.\n";
            return;
        }
        if (newAmount < 0)
        {
            cout << "Amount cannot be negative.\n";
            return;
        }
        // ✅ Check if the category exists for the given date
        bool categoryFound = false;

        // Update amount for the specified category in memory
        for (auto &exp : expenseData[month][day].first)
        {
            if (essentialCategories[&exp - &expenseData[month][day].first[0]] == category)
            {
                exp = newAmount;
                categoryFound = true;
                break;
            }
        }
        for (auto &exp : expenseData[month][day].second)
        {
            if (nonEssentialCategories[&exp - &expenseData[month][day].second[0]] == category)
            {
                exp = newAmount;
                categoryFound = true;
                break;
            }
        }

        if (!categoryFound)
        {
            cout << "Category not found for the specified date!" << endl;
            return;
        }

        cout << "Expense updated successfully in memory!" << endl;

        // Open the CSV file for reading
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cerr << "Error: Cannot open file " << filename << " for reading.\n";
            return;
        }

        // Create a temporary file to write the updated content
        ofstream outFile("temp.csv");
        if (!outFile.is_open())
        {
            cerr << "Error: Cannot open temporary file for writing.\n";
            inFile.close();
            return;
        }

        string line;
        bool found = false;

        // Read the first line (header) and keep it
        if (getline(inFile, line))
        {
            outFile << line << "\n"; // Keep header row
        }

        // Read the file line by line and update the specific row if found
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string recordDate, recordCategory;
            getline(ss, recordDate, ',');     // Get the date from the line
            getline(ss, recordCategory, ','); // Get the category from the line

            // Check if the current row matches the date
            if (recordDate == date)
            {
                found = true;

                // Replace the amount with the new value in the respective category
                stringstream newLineStream;
                newLineStream << recordDate;

                // Append the new updated expenses for each category
                for (const auto &category : essentialCategories)
                {
                    newLineStream << "," << expenseData[month][day].first[&category - &essentialCategories[0]];
                }
                for (const auto &category : nonEssentialCategories)
                {
                    newLineStream << "," << expenseData[month][day].second[&category - &nonEssentialCategories[0]];
                }

                outFile << newLineStream.str(); // Write the updated line
            }
            else
            {
                // If no update, just write the line as it is
                outFile << line;
            }
            outFile << "\n";
        }

        inFile.close();
        outFile.close();

        // Replace the old file with the new one
        if (found)
        {
            remove(filename.c_str());             // Delete the old file
            rename("temp.csv", filename.c_str()); // Rename the temporary file to the original filename
            cout << "Expense for date " << date << " has been updated successfully.\n";
        }
        else
        {
            remove("temp.csv"); // If no matching record is found, remove the temporary file
            cout << "Expense not found for the entered date.\n";
        }
    }
    else
    {
        cout << "Expense not found in the data structure!" << endl;
    }
    displayExpenses();
}

// Function to delete expenses for a given date
bool deleteExpenses(string &filename, string &date)
{
    int month, day;
    try
    {
        month = stoi(date.substr(5, 2)) - 1;
        day = stoi(date.substr(8, 2)) - 1;

        if (month < 0 || month >= 12 || day < 0 || day >= 31)
        {
            cerr << "Invalid date! Month or day out of range.\n";
            return false;
        }
    }
    catch (const invalid_argument &e)
    {
        cerr << "Error: Invalid number in date. " << e.what() << "\n";
        return false;
    }
    catch (const out_of_range &e)
    {
        cerr << "Error: Date values out of range. " << e.what() << "\n";
        return false;
    }

    // Clear the expenses for the specified date
    expenseData[month][day].first = {0, 0, 0};  // Clear essential category expenses
    expenseData[month][day].second = {0, 0, 0}; // Clear non-essential category expenses
    cardid[month][day].first = {0, 0, 0};       // Clear essential category expenses
    cardid[month][day].second = {0, 0, 0};      // Clear non-essential category expenses

    // Open the CSV file for reading
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cerr << "Error: Cannot open file " << filename << " for reading.\n";
        return false;
    }

    // Create a temporary file to write the updated content
    ofstream outFile("temp.csv");
    if (!outFile.is_open())
    {
        cerr << "Error: Cannot open temporary file for writing.\n";
        inFile.close();
        return false;
    }

    string line;
    bool found = false;
    bool isFirstLine = true;

    // Read the first line (header) and keep it
    if (getline(inFile, line))
    {
        outFile << line << "\n"; // Keep the header row
    }

    // Read the file line by line and write all lines except the one with the given date
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string recordDate;
        if (!getline(ss, recordDate, ','))
        {
            cerr << "Warning: Skipping malformed line.\n";
            continue;
        }

        // If the date matches, skip writing this line to the new file
        if (recordDate == date)
        {
            found = true; // Mark that the date was found and will be deleted
            continue;     // Skip writing this line
        }

        // Ensure no extra blank lines in CSV (write the remaining lines)
        if (!isFirstLine)
        {
            outFile << "\n"; // Add newline only before new entries (not at the end)
        }

        outFile << line; // Write the remaining lines
        isFirstLine = false;
    }

    inFile.close();
    outFile.close();

    // Replace the old file with the new one
    if (found)
    {
        // 6. Replace old file with temp
        if (remove(filename.c_str()) != 0)
        {
            cerr << "Error: Could not delete original file.\n";
            return false;
        }

        if (rename("temp.csv", filename.c_str()) != 0)
        {
            cerr << "Error: Could not rename temp.csv to original file.\n";
            return false;
        }

        cout << "Expenses for " << date << " deleted successfully.\n";
        return true;
    }
    else
    {
        // If date not found
        remove("temp.csv");
        cerr << "No expenses found for date: " << date << ". Nothing deleted.\n";
        return false;
    }
    displayExpenses();
}
void menu();
void optimizeSavingsPlan(vector<tuple<int, int, string>> &nonEssentialExpensesWithDates, int &goal)
{
    int n = nonEssentialExpensesWithDates.size();
    vector<vector<int>> dp(n + 1, vector<int>(goal + 1, -1)); // -1 means unreachable
    vector<vector<int>> count(n + 1, vector<int>(goal + 1, 1e9));
    vector<vector<vector<tuple<int, int, string>>>> chosen(n + 1, vector<vector<tuple<int, int, string>>>(goal + 1)); // (expense, date, category)

    dp[0][0] = 0;
    count[0][0] = 0;

    for (int i = 1; i <= n; i++)
    {
        int expense = get<0>(nonEssentialExpensesWithDates[i - 1]);
        int date = get<1>(nonEssentialExpensesWithDates[i - 1]);
        string category = get<2>(nonEssentialExpensesWithDates[i - 1]);

        for (int j = 0; j <= goal; j++)
        {
            // Case 1: Don't take the current expense
            if (dp[i - 1][j] != -1)
            {
                dp[i][j] = dp[i - 1][j];
                count[i][j] = count[i - 1][j];
                chosen[i][j] = chosen[i - 1][j];
            }

            // Case 2: Take the current expense (if it fits)
            if (j >= expense && dp[i - 1][j - expense] != -1)
            {
                int newSum = dp[i - 1][j - expense] + expense;
                int newCount = count[i - 1][j - expense] + 1;

                if (dp[i][j] == -1 || (newSum > dp[i][j]) || (newSum == dp[i][j] && newCount < count[i][j]))
                {
                    dp[i][j] = newSum;
                    count[i][j] = newCount;
                    chosen[i][j] = chosen[i - 1][j - expense];
                    chosen[i][j].push_back({expense, date, category});
                }
            }
        }
    }

    // Find max achievable sum ≤ goal with fewest entries
    int bestSavings = 0, minCount = 1e9;
    vector<tuple<int, int, string>> bestSet;
    for (int j = 0; j <= goal; j++)
    {
        if (dp[n][j] != -1)
        {
            if (dp[n][j] > bestSavings || (dp[n][j] == bestSavings && count[n][j] < minCount))
            {
                bestSavings = dp[n][j];
                minCount = count[n][j];
                bestSet = chosen[n][j];
            }
        }
    }

    cout << "\nTotal Savings Achieved: " << bestSavings
         << " using " << minCount << " expense entries.\n";
    cout << "-------------------------------------------------\n";

    cout << "By reducing the following expenses, you can successfully meet your savings target upto " << bestSavings << "!\n";
    cout << "-------------------------------------------------\n";

    // Header border
    cout << "+-----------+---------------+-----------------+" << endl;

    // Header row
    cout << left
         << "| " << setw(10) << "Amount"
         << "| " << setw(14) << "Date (MM-DD)"
         << "| " << setw(16) << "Category"
         << "|" << endl;

    // Separator
    cout << "+-----------+---------------+-----------------+" << endl;

    for (const auto &entry : bestSet)
    {
        int e, d;
        string cat;
        tie(e, d, cat) = entry;
        int month = d / 100;
        int day = d % 100;
        stringstream dateStream;
        dateStream << setfill('0') << setw(2) << month << "-" << setw(2) << day;
        cout << left
             << "| Rs. " << setw(6) << e
             << "| " << setw(14) << dateStream.str()
             << "| " << setw(16) << cat
             << "|" << endl;
    }
    // Footer border
    cout << "+-----------+--------------+------------------+" << endl;
    cout << endl;
}

double optimizeSavings(int &goal)
{
    vector<tuple<int, int, string>> nonEssentialExpensesWithDates;

    for (int month = 0; month < 12; ++month)
    {
        for (int day = 0; day < 31; ++day)
        {
            const vector<int> &nonEssential = expenseData[month][day].second;
            int date = (month + 1) * 100 + (day + 1); // format: MMDD

            for (int i = 0; i < nonEssential.size(); ++i)
            {
                if (nonEssential[i] != 0)
                {
                    nonEssentialExpensesWithDates.push_back({nonEssential[i], date, nonEssentialCategories[i]});
                }
            }
        }
    }

    // cout << "\nAll Extracted Non-Essential Expenses with Dates and Categories:\n";
    // cout << left << setw(10) << "Amount" << setw(12) << "Date (MM-DD)" << "Category" << endl;
    // cout << "-----------------------------------------" << endl;
    // for (auto &entry : nonEssentialExpensesWithDates)
    // {
    //     int expense = get<0>(entry);
    //     int d = get<1>(entry);
    //     string category = get<2>(entry);
    //     int month = d / 100;
    //     int day = d % 100;
    //     cout << left << setw(10) << expense << setfill('0') << setw(2) << month << "-" << setw(2) << day << setfill(' ') << "   " << category << endl;
    // }

    optimizeSavingsPlan(nonEssentialExpensesWithDates, goal);
    menu();
    return 0;
}

vector<PaymentResult> optimizeCreditCardPayments(
    const vector<vector<pair<vector<int>, vector<int>>>> &expenseData,
    const vector<vector<pair<vector<int>, vector<int>>>> &cardid,
    vector<CreditCard> &cardVec,
    int availableFunds)
{
    vector<int> totalDue(4, 0); // index 1->A, 2->B, 3->C

    for (int month = 0; month < 12; ++month)
    {
        for (int day = 0; day < 31; ++day)
        {
            auto expenses = expenseData[month][day];
            auto cards = cardid[month][day];

            for (int i = 0; i < 3; ++i)
            {
                int cardIdx = cards.first[i];
                if (cardIdx >= 1 && cardIdx <= 3)
                {
                    totalDue[cardIdx] += expenses.first[i];
                }
            }
            for (int i = 0; i < 3; ++i)
            {
                int cardIdx = cards.second[i];
                if (cardIdx >= 1 && cardIdx <= 3)
                {
                    totalDue[cardIdx] += expenses.second[i];
                }
            }
        }
    }

    // Step 2: Pay minimum dues first
    vector<int> amountPaid(4, 0);
    for (int i = 1; i <= 3; ++i)
    {
        int minDue = cardVec[i].minDue;
        int pay = min(minDue, totalDue[i]);
        if (availableFunds >= pay)
        {
            amountPaid[i] += pay;
            availableFunds -= pay;
        }
    }

    // Step 3: Pay remaining dues by highest interest first
    vector<pair<int, double>> sortedCards;
    for (int i = 1; i <= 3; ++i)
    {
        int unpaid = totalDue[i] - amountPaid[i];
        if (unpaid > 0)
        {
            sortedCards.push_back(make_pair(i, cardVec[i].interestRate));
        }
    }

    sort(sortedCards.begin(), sortedCards.end(), [](const pair<int, double> &a, const pair<int, double> &b)
         { return a.second > b.second; });

    for (size_t i = 0; i < sortedCards.size(); ++i)
    {
        int idx = sortedCards[i].first;
        double rate = sortedCards[i].second;

        int unpaid = totalDue[idx] - amountPaid[idx];
        int pay = min(unpaid, availableFunds);
        amountPaid[idx] += pay;
        availableFunds -= pay;
        if (availableFunds == 0)
            break;
    }

    // Step 4: Prepare results
    vector<PaymentResult> results;
    for (int i = 1; i <= 3; ++i)
    {
        int unpaid = totalDue[i] - amountPaid[i];
        double interest = unpaid * cardVec[i].interestRate / 100.0;
        results.push_back({cardVec[i].name,
                           totalDue[i],
                           amountPaid[i],
                           unpaid,
                           interest});
    }

    // Sort output by interest
    sort(results.begin(), results.end(), [&](const PaymentResult &a, const PaymentResult &b)
         {
        int ai = a.card[0] - 'A' + 1;
        int bi = b.card[0] - 'A' + 1;
        return cardVec[ai].interestRate > cardVec[bi].interestRate; });

    return results;
}

void displayResults(const vector<PaymentResult> &results)
{
    cout << "+-----------+------------+-----------+-----------+-------------------+" << endl;
    cout << left
         << "| " << setw(10) << "Card"
         << "| " << setw(11) << "Total Due"
         << "| " << setw(10) << "Paid"
         << "| " << setw(10) << "Unpaid"
         << "| " << setw(18) << "Interest Incurred"
         << "|" << endl;

    cout << "+-----------+------------+-----------+-----------+-------------------+" << endl;
    for (const auto &r : results)
    {
        cout << left
             << "| " << setw(10) << r.card
             << "| " << setw(11) << fixed << setprecision(2) << r.totalDue
             << "| " << setw(10) << r.amountPaid
             << "| " << setw(10) << r.unpaidAmount
             << "| " << setw(18) << ("Rs. " + to_string(r.interest))
             << "|" << endl;
    }

    // Footer border
    cout << "+-----------+------------+-----------+-----------+-------------------+" << endl;
}

int findMinTravelCost(
    const vector<City> &cities,
    const vector<vector<ERoute>> &graph,
    int source,
    int destination,
    int numPeople,
    int numDays,
    vector<int> &parent)
{
    int n = cities.size();
    vector<int> minCost(n, numeric_limits<int>::max());
    priority_queue<CNode, vector<CNode>, greater<CNode>> pq;
    parent.assign(n, -1);

    // Initial cost is hotel at source
    minCost[source] = cities[source].hotelCostPerNight * numDays * numPeople;
    pq.push({source, minCost[source]});

    while (!pq.empty())
    {
        CNode current = pq.top();
        pq.pop();

        for (const ERoute &edge : graph[current.city])
        {
            int newCost = current.cost +
                          edge.flightCost * numPeople + // flight for all
                          cities[edge.destination].hotelCostPerNight * numDays * numPeople;

            if (newCost < minCost[edge.destination])
            {
                minCost[edge.destination] = newCost;
                parent[edge.destination] = current.city;
                pq.push({edge.destination, newCost});
            }
        }
    }

    return minCost[destination];
}

void printPath(const vector<int> &parent, int source, int destination, const vector<City> &cities)
{
    vector<string> path;
    int current = destination;

    while (current != -1)
    {
        path.push_back(cities[current].name);
        if (current == source)
            break;
        current = parent[current];
    }

    if (path.back() != cities[source].name)
    {
        cout << "No path exists from " << cities[source].name << " to " << cities[destination].name << ".\n";
        return;
    }

    reverse(path.begin(), path.end());

    cout << "Cheapest path: ";
    for (size_t i = 0; i < path.size(); ++i)
    {
        cout << path[i];
        if (i != path.size() - 1)
            cout << " -> ";
    }
    cout << endl;
}

void travelExpenseMinimizer()
{
    int numCities, numFlights;
    cout << "Enter number of cities: ";
    cin >> numCities;

    vector<City> cities(numCities);
    unordered_map<string, int> cityIndex;
    string cityName;
    int hotelCost;

    cout << "Enter city names and hotel costs per night:\n";
    for (int i = 0; i < numCities; ++i)
    {
        cout << "City " << i + 1 << " name: ";
        cin >> cityName;
        cout << "Hotel cost per night in " << cityName << ": ";
        cin >> hotelCost;
        cities[i] = {cityName, hotelCost};
        cityIndex[cityName] = i;
    }

    vector<vector<ERoute>> graph(numCities);
    cout << "Enter number of flight routes: ";
    cin >> numFlights;

    for (int i = 0; i < numFlights; ++i)
    {
        string from, to;
        int cost;
        cout << "From city: ";
        cin >> from;
        cout << "To city: ";
        cin >> to;
        cout << "Flight cost from " << from << " to " << to << ": ";
        cin >> cost;

        graph[cityIndex[from]].push_back({cityIndex[to], cost});
    }

    string src, dest;
    int people, days;
    cout << "Enter starting city: ";
    cin >> src;
    cout << "Enter destination city: ";
    cin >> dest;
    cout << "Enter number of travelers: ";
    cin >> people;
    cout << "Enter number of days to stay in each city: ";
    cin >> days;

    vector<int> parent;
    int totalCost = findMinTravelCost(cities, graph, cityIndex[src], cityIndex[dest], people, days, parent);

    cout << "\nMinimum total cost from " << src << " to " << dest << " for "
         << people << " people over " << days << " days is: Rs " << totalCost << endl;

    printPath(parent, cityIndex[src], cityIndex[dest], cities);
}

void initializeDisjointSet(int n)
{
    for (int i = 0; i < n; ++i)
    {
        parent[i] = i;
        rankArr[i] = 0;
    }
}

int findParent(int x)
{
    if (parent[x] != x)
        parent[x] = findParent(parent[x]);
    return parent[x];
}

void unionSets(int x, int y)
{
    int rootX = findParent(x);
    int rootY = findParent(y);
    if (rootX != rootY)
    {
        if (rankArr[rootX] > rankArr[rootY])
        {
            parent[rootY] = rootX;
        }
        else if (rankArr[rootX] < rankArr[rootY])
        {
            parent[rootX] = rootY;
        }
        else
        {
            parent[rootY] = rootX;
            rankArr[rootX]++;
        }
    }
}
// Bubble sort for edges by weight
void sortEdges(Edge edges[], int edgeCount)
{
    for (int i = 0; i < edgeCount - 1; ++i)
    {
        for (int j = 0; j < edgeCount - i - 1; ++j)
        {
            if (edges[j].weight > edges[j + 1].weight)
            {
                Edge temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
}

// Overload displayGraph to show all edges
void displayGraph(const Edge edges[], int count)
{
    cout << "\nGraph Edges:\n";
    for (int i = 0; i < count; ++i)
        cout << "Edge from " << edges[i].src << " to " << edges[i].dest << " with cost Rs. " << edges[i].weight << "\n";
}

void allocateEmergencyFunds()
{
    Edge edges[MAX_EDGES];
    int edgeCount = 0;
    int nodeId = 0;

    // Construct Graph First
    for (int month = 0; month < 12; ++month)
    {
        for (int day = 0; day < 31; ++day)
        {
            int currentNode = nodeId++;
            int nextNode = currentNode + 1;
            if (nextNode >= MAX_NODES)
                continue;

            int nextMonth = nextNode / 31;
            int nextDay = nextNode % 31;

            if (nextMonth >= 12 || nextDay >= 31)
                continue;

            double total1 = 0, total2 = 0;
            for (int i = 0; i < 3; ++i)
            {
                total1 += expenseData[month][day].first[i] + expenseData[month][day].second[i];
                total2 += expenseData[nextMonth][nextDay].first[i] + expenseData[nextMonth][nextDay].second[i];
            }

            double diff = abs(total1 - total2);
            edges[edgeCount++] = {currentNode, nextNode, diff};
        }
    }

    sortEdges(edges, edgeCount);
    initializeDisjointSet(MAX_NODES);

    double totalCost = 0;
    vector<Edge> mst;
    for (int i = 0; i < edgeCount; ++i)
    {
        int u = edges[i].src, v = edges[i].dest;
        if (findParent(u) != findParent(v))
        {
            unionSets(u, v);
            totalCost += edges[i].weight;
            mst.push_back(edges[i]);
        }
    }

    cout << "\nEmergency Fund Transfer Graph Constructed.\n";
    /*displayGraph(edges, edgeCount); */

    cout << "\nMinimum Spanning Transfers (MST):\n";
    for (auto &e : mst)
    {
        if (e.weight > 0)
            cout << "Transfer from " << e.src << " to " << e.dest << " with cost Rs. " << e.weight << "\n";
    }

    cout << "Total Minimum Transfer Cost: Rs. " << totalCost << "\n";
}

void buildHuffmanTree(const string &data, unordered_map<char, string> &huffmanCode)
{
    unordered_map<char, int> freq;
    for (char ch : data)
        freq[ch]++;
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, Compare> pq;
    for (auto &pair : freq)
        pq.push(new HuffmanNode(pair.first, pair.second));
    while (pq.size() > 1)
    {
        HuffmanNode *left = pq.top();
        pq.pop();
        HuffmanNode *right = pq.top();
        pq.pop();
        HuffmanNode *node = new HuffmanNode('\0', left->freq + right->freq);
        node->left = left;
        node->right = right;
        pq.push(node);
    }
    function<void(HuffmanNode *, string)> encode = [&](HuffmanNode *node, string str)
    {
        if (!node)
            return;
        if (node->data != '\0')
            huffmanCode[node->data] = str;
        encode(node->left, str + "0");
        encode(node->right, str + "1");
    };
    encode(pq.top(), "");
}

string compressData(const string &data, unordered_map<char, string> &huffmanCode)
{
    string compressed = "";
    for (char ch : data)
        compressed += huffmanCode[ch];
    return compressed;
}

string decompressData(const string &compressed, unordered_map<char, string> &huffmanCode)
{
    unordered_map<string, char> reverseCode;
    for (auto &pair : huffmanCode)
        reverseCode[pair.second] = pair.first;
    string temp = "", decompressed = "";
    for (char bit : compressed)
    {
        temp += bit;
        if (reverseCode.count(temp))
        {
            decompressed += reverseCode[temp];
            temp = "";
        }
    }
    return decompressed;
}

void updateExpenseData()
{
    string compressFile = "compress.csv";
    string workingFile = (isValidFile(compressFile)) ? compressFile : filename;

    ifstream file(workingFile);
    stringstream buffer;
    buffer << file.rdbuf();
    string data = buffer.str();
    file.close();

    unordered_map<char, string> huffmanCode;
    buildHuffmanTree(data, huffmanCode);
    string compressed = compressData(data, huffmanCode);

    ofstream outFile(compressFile);
    outFile << huffmanCode.size() << '\n';
    for (auto &pair : huffmanCode)
        outFile << (int)(unsigned char)pair.first << ' ' << pair.second << '\n';
    outFile << compressed;
    outFile.close();

    cout << "CSV data compressed to compress.csv!\n";
}
void restoreExpenseData()
{
    string compressFile = "compress.csv";
    string decompressFile = "decompress.csv";

    if (!isValidFile(compressFile))
    {
        cout << "No compressed file found to decompress.\n";
        return;
    }

    ifstream file(compressFile);
    int mapSize;
    file >> mapSize;

    unordered_map<char, string> huffmanCode;
    for (int i = 0; i < mapSize; ++i)
    {
        int chInt;
        string code;
        file >> chInt >> code;
        huffmanCode[(char)chInt] = code;
    }

    file.ignore(); // skip the newline after header
    string compressed;
    getline(file, compressed, '\0');
    file.close();

    string decompressed = decompressData(compressed, huffmanCode);

    ofstream outFile(decompressFile);
    outFile << decompressed;
    outFile.close();

    cout << "Data restored from compress.csv to decompress.csv\n";
}

void loadExpenseData(map<string, double> &expenses)
{
    ifstream file(filename);
    if (!file)
    {
        cout << "No previous expense data found." << endl;
        return;
    }
    string category;
    double amount;
    while (file >> category >> amount)
    {
        expenses[category] += amount;
    }
    file.close();
}

void saveExpenseData(const map<string, double> &expenses)
{
    ofstream file(filename);
    for (const auto &entry : expenses)
    {
        file << entry.first << " " << entry.second << endl;
    }
    file.close();
}

void listAllExpenses(const map<string, double> &expenses)
{
    cout << "All Expenses:\n";
    for (const auto &entry : expenses)
    {
        cout << entry.first << ": " << entry.second << "\n";
    }
}
vector<LoanRepaymentResult> optimizeLoanRepayment(
    const vector<vector<pair<vector<int>, vector<int>>>> &expenseData,
    int income,
    int month,
    vector<Loan> &loans)
{
    // Calculate total spending from expense data
    int totalSpent = 0;
    for (int day = 0; day < 31; ++day)
    {
        auto ess = expenseData[month][day].first;
        auto nonEss = expenseData[month][day].second;
        for (int x : ess)
            totalSpent += x;
        for (int x : nonEss)
            totalSpent += x;
    }

    cout << "\n Total Spent: Rs." << totalSpent << endl;
    cout << " Total Income: Rs." << income << endl;

    int availableFunds = max(0, income - totalSpent);
    cout << " Available for Loan Repayment: Rs." << availableFunds << endl
         << endl;

    // Sort loans by interest density (high interest per unit amount first)
    vector<pair<int, double>> loanPriority; // {index, interest density}
    for (int i = 0; i < loans.size(); ++i)
    {
        double density = loans[i].interestRate / loans[i].amount;
        loanPriority.push_back({i, density});
    }

    sort(loanPriority.begin(), loanPriority.end(), [](auto &a, auto &b)
         { return a.second > b.second; });

    vector<LoanRepaymentResult> results(loans.size());

    // Initialize all loans with 0 paid
    for (int i = 0; i < loans.size(); ++i)
    {
        results[i] = {
            loans[i].id,
            loans[i].amount,
            0.0,
            loans[i].amount,
            loans[i].amount * loans[i].interestRate / 100.0};
    }

    // Distribute available funds
    for (const auto &entry : loanPriority)
    {
        int idx = entry.first;

        if (availableFunds <= 0)
            break;

        double pay = min(loans[idx].amount, (double)availableFunds);
        results[idx].amountPaid = pay;
        results[idx].unpaidAmount = loans[idx].amount - pay;
        results[idx].interestIncurred = results[idx].unpaidAmount * loans[idx].interestRate / 100.0;

        availableFunds -= pay;
    }

    return results;
}

void displayLoanResults(const vector<LoanRepaymentResult> &results)
{
    cout << "\nLoan Repayment Summary:\n";
    cout << "+-----------+----------------+-----------+-----------+-------------------+" << endl;
    cout << left
         << "| " << setw(10) << "Loan ID"
         << "| " << setw(15) << "Original (Rs.)"
         << "| " << setw(10) << "Paid"
         << "| " << setw(10) << "Unpaid"
         << "| " << setw(18) << "Interest Incurred"
         << "|" << endl;
    cout << "+-----------+----------------+-----------+-----------+-------------------+" << endl;
    for (const auto &r : results)
    {
        cout << left
             << "| " << setw(10) << r.id
             << "| " << setw(15) << fixed << setprecision(2) << r.originalAmount
             << "| " << setw(10) << r.amountPaid
             << "| " << setw(10) << r.unpaidAmount
             << "| " << setw(18) << ("Rs. " + to_string(r.interestIncurred))
             << "|" << endl;
    }
    cout << "+-----------+----------------+-----------+-----------+-------------------+" << endl;
}

void optimizeInvestmentPortfolio(int totalRiskBudget)
{
    // Predefined investments
    vector<Investment> investments = {
        {101, 2000, 5000, 20},
        {102, 100, 500, 10},
        {103, 10000, 50000, 50},
        {104, 100, 500, 10},
        {105, 6000, 10000, 50}};

    // Sort by return-to-risk ratio
    sort(investments.begin(), investments.end(), [](const Investment &a, const Investment &b)
         { return (a.returnPerUnit / a.riskPerUnit) > (b.returnPerUnit / b.riskPerUnit); });

    double totalReturn = 0;
    vector<pair<int, int>> selected; // {id, unitsTaken}

    for (const auto &inv : investments)
    {
        int maxUnitsWeCanTake = min(inv.maxUnits, totalRiskBudget / (int)inv.riskPerUnit);

        if (maxUnitsWeCanTake >= 1)
        {
            int usedRisk = maxUnitsWeCanTake * inv.riskPerUnit;
            totalRiskBudget -= usedRisk;
            totalReturn += maxUnitsWeCanTake * inv.returnPerUnit;

            selected.push_back({inv.id, maxUnitsWeCanTake});
        }

        if (totalRiskBudget <= 0)
            break;
    }

    cout << "\nInvestment Portfolio Optimization:\n"
         << endl;
    cout << "+----------------+-------------------+--------------------+" << endl;
    cout << left
         << "| " << setw(15) << "Investment ID"
         << "| " << setw(18) << "Units Selected"
         << "| " << setw(19) << "Return (Rs.)"
         << "|" << endl;
    cout << "+----------------+-------------------+--------------------+" << endl;
    for (const auto &entry : selected)
    {
        int id = entry.first;
        int units = entry.second;
        double ret = units * investments[id % 100 - 1].returnPerUnit;

        cout << left
             << "| " << setw(15) << id
             << "| " << setw(18) << units
             << "| Rs. " << setw(15) << fixed << setprecision(2) << ret
             << "|" << endl;
    }
    cout << "+----------------+-------------------+--------------------+" << endl;
    cout << "\nTotal Expected Return: Rs." << fixed << setprecision(2) << totalReturn << endl;
}

void generateBudgetPlan(double monthlyIncome)
{
    cout << fixed << setprecision(2);
    cout << "\n Monthly Budget for Income: " << monthlyIncome << "\n"
         << endl;

    // NEEDS (50%)
    cout << " NEEDS (50%) - " << 0.50 * monthlyIncome << endl;
    cout << "   Rent (30%): " << 0.30 * monthlyIncome << endl;
    cout << "   Groceries (10%): " << 0.10 * monthlyIncome << endl;
    cout << "   Utilities (5%): " << 0.05 * monthlyIncome << endl;
    cout << "   Transportation (5%): " << 0.05 * monthlyIncome << endl;

    // WANTS (15%)
    cout << "\n WANTS (15%) - " << 0.15 * monthlyIncome << endl;
    cout << "   Shopping + EMIs (10%): " << 0.10 * monthlyIncome << endl;
    cout << "   Entertainment + Travel (5%): " << 0.05 * monthlyIncome << endl;

    // SAVINGS + INVESTMENTS (20%)
    cout << "\n SAVINGS + INVESTMENTS (20%) - " << 0.20 * monthlyIncome << endl;
    cout << "   Health + Term Insurance (5%): " << 0.05 * monthlyIncome << endl;
    cout << "   SIP (in Mutual Funds) (10%): " << 0.10 * monthlyIncome << endl;
    cout << "   Emergency Fund (5%): " << 0.05 * monthlyIncome << endl;

    // OTHERS (15%)
    cout << "\n OTHERS (15%) - " << 0.15 * monthlyIncome << endl;
    cout << "   Upskilling (5%): " << 0.05 * monthlyIncome << endl;
    cout << "   Family Support (10%): " << 0.10 * monthlyIncome << endl;

    cout << "\n TOTAL: " << monthlyIncome << " distributed across needs, wants, savings, and others.\n"
         << endl;
}
void encrypt(const string &inputFilename, const string &outputFilename, int key)
{
    ifstream input(inputFilename);
    ofstream output(outputFilename);
    if (!input.is_open() || !output.is_open())
    {
        cout << "Error opening files.\n";
        return;
    }

    char ch;
    while (input.get(ch))
    {
        if (isalpha(ch))
        {
            char base = islower(ch) ? 'a' : 'A';
            ch = (ch - base + key) % 26 + base;
        }
        else if (isdigit(ch))
        {
            ch = (ch - '0' + key) % 10 + '0';
        }
        output.put(ch);
    }

    input.close();
    output.close();
    cout << "Encrypted " << inputFilename << "into " << outputFilename << endl;
}

void decrypt(const string &inputFilename, const string &outputFilename, int key)
{
    ifstream input(inputFilename);
    ofstream output(outputFilename);
    if (!input.is_open() || !output.is_open())
    {
        cout << "Error opening files.\n";
        return;
    }

    char ch;
    while (input.get(ch))
    {
        if (isalpha(ch))
        {
            char base = islower(ch) ? 'a' : 'A';
            ch = ((ch - base - key) % 26 + 26) % 26 + base;
        }
        else if (isdigit(ch))
        {
            ch = ((ch - '0' - key) % 10 + 10) % 10 + '0';
        }
        output.put(ch);
    }

    input.close();
    output.close();
    cout << "Decrypted " << inputFilename << " into " << outputFilename << endl;
}

void menu()
{
    int choice;

    while (true)
    {
        try
        {
            cout << "\nExpense Tracker Menu:";
            cout << "\n1. Display Expenses";
            cout << "\n2. Add Expense";
            cout << "\n3. Update Expense";
            cout << "\n4. Delete Expense";
            cout << "\n5. Design A budget plan";
            cout << "\n6. Allocate Emergency Funds";
            cout << "\n7. Compress Data";
            cout << "\n8. Decompress Data";
            cout << "\n9. Optimize Savings";
            cout << "\n10. Credit Card Payment Strategy";
            cout << "\n11. Best Flight optimization";
            cout << "\n12. Loan Repayment Strategy";
            cout << "\n13. Investment Portfolio Optimization";
            cout << "\n14. Encrypt CSV";
            cout << "\n15. Decrypt CSV";
            cout << "\n16. Exit";
            cout << "\nEnter your choice: ";

            cin >> choice;

            // Validate input
            if (cin.fail())
            {
                throw invalid_argument("Input must be an integer.");
            }

            if (choice < 1 || choice > 16)
            {
                throw out_of_range("Choice must be between 1 and 16.");
            }

            string filename = "OctExpenses.csv";
            string filename2 = "carddetails.csv";
            string date;
            vector<CreditCard> cardVec(4);
            cardVec[1] = {"A", 3.5, 500, 15};
            cardVec[2] = {"B", 2.0, 300, 12};
            cardVec[3] = {"C", 1.5, 200, 18};
            vector<PaymentResult> payVec;
            vector<Loan> loans = {
                Loan(1, 15000, 10.5),
                Loan(2, 20000, 8.2),
                Loan(3, 12000, 12.0)};
            vector<LoanRepaymentResult> payVecLoan;
            vector<Investment> investments;
            vector<InvestmentSelection> result;
            switch (choice)
            {
            case 1:
                displayExpenses();
                Sleep(2000); // Sleep for 2 seconds
                break;
            case 2:
                addExpense();
                break;
            case 3:
                updateExpense();
                break;
            case 4:
                displayExpenses();
                cout << "Enter the date (YYYY-MM-DD) to delete all expenses of that day: ";
                cin >> date;
                // Validate Date Format
                if (!validateDateFormat(date))
                {
                    cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
                    break;
                }
                if (deleteExpenses(filename, date) || deleteExpenses(filename2, date))
                {
                    cout << "Expenses for date " << date << " have been deleted successfully.\n";
                    Sleep(2000);
                }
                else
                {
                    cout << "No expenses found for the entered date.\n";
                }

                break;
            case 5:
            {
                double income;
                cout << "Enter your monthly income in Rupees: ";
                cin >> income;

                // Input validation
                if (cin.fail() || income <= 0)
                {
                    cin.clear();                                         // Clear input flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                    cout << "Invalid income amount. Please enter a positive numeric value.\n";
                    break;
                }

                cout << "\nGenerating your personalized budget plan...\n\n";
                generateBudgetPlan(income);
                Sleep(5000); // Delay to simulate processing
                break;
            }

            case 6:
                allocateEmergencyFunds();
                break;
            case 7:
                updateExpenseData();
                Sleep(200);
                break;
            case 8:
                restoreExpenseData();
                Sleep(200);
                break;
            case 9:
            {
                int goal;
                cout << "Enter your target savings goal (in Rs): ";
                if (!(cin >> goal) || goal <= 0)
                {
                    cout << " Invalid input. Please enter a positive numeric value for your goal.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                int excessAmount;
                cout << "\nWould you like to allow a small flexibility margin above your goal? (optional)\n";
                cout << "Enter the extra amount you are willing to accept (Enter 0 if you want an exact match): ";
                if (!(cin >> excessAmount) || excessAmount < 0)
                {
                    cout << " Invalid amount. Please enter a non-negative numeric value.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                cout << "\n Summary of your savings configuration:\n";
                cout << "  - Base Target Goal     : Rs. " << goal << endl;
                cout << "  - Flexibility Allowed  : Rs. " << excessAmount << endl;
                if (excessAmount > 0)
                    cout << "  - Extended Target Goal : Rs. " << goal + excessAmount << endl;
                else
                    cout << "  - No flexibility allowed (exact goal only)\n";

                cout << "\n Attempting optimization for *Exact Goal (Rs. " << goal << ")*...\n";
                optimizeSavings(goal); // Call for exact match

                if (excessAmount > 0)
                {
                    int finalGoal = goal + excessAmount;
                    cout << "\n Attempting optimization for *Flexible Goal (Up to Rs. " << finalGoal << ")*...\n";
                    optimizeSavings(finalGoal); // Call for flexible version
                }

                break;
            }
            case 10:

                cout << "Enter your available funds: " << endl;
                int funds;
                cin >> funds;
                payVec = optimizeCreditCardPayments(expenseData, cardid, cardVec, funds);
                displayResults(payVec);

                break;
            case 11:
                travelExpenseMinimizer(); // Call our feature here
                break;

            case 12:
                cout << "Enter your income: ";
                int income1;
                cin >> income1;
                // Input validation loop
                if (cin.fail())
                {
                    cin.clear();                                         // Clear the error flag
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                    cout << "Invalid input. Please enter a numeric value.\n";
                    break; // or break if inside switch-case
                }

                if (income1 <= 0)
                {
                    cout << "Income must be a positive value.\n";
                    break; // or break if inside switch-case
                }
                cout << "Enter the month number (1 for January, 12 for December): ";
                int month;
                cin >> month;

                if (month < 1 || month > 12)
                {
                    cout << "Invalid month entered. Please enter a value between 1 and 12." << endl;
                    break;
                }

                payVecLoan = optimizeLoanRepayment(expenseData, income1, (month - 1), loans);
                displayLoanResults(payVecLoan);
                break;
            case 13:
                int riskBudget;
                cout << "Enter your total risk budget: ";
                cin >> riskBudget;
                optimizeInvestmentPortfolio(riskBudget);
                break;
            case 14:
            {
                int key;
                cout << "Enter encryption key (positive integer): ";
                cin >> key;
                encrypt(filename, "encrypted_Expenses.csv", key);
                encrypt(filename2, "encrypted_carddetails.csv", key);
                break;
            }
            case 15:
            {
                int key;
                cout << "Enter decryption key (must match encryption key): ";
                cin >> key;
                decrypt("encrypted_Expenses.csv", "decrypted_Expenses.csv", key);
                decrypt("encrypted_carddetails.csv", "decrypted_carddetails.csv", key);
                break;
            }
            case 16:
                cout << "Exiting program...\n";
                return;
            }
        }
        catch (const exception &e)
        {
            cin.clear();                                         // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard input
            cout << "Error: " << e.what() << "\nPlease try again.\n";
        }
    }
}

// Main function
int main()
{
    string filename1 = "OctExpenses.csv";
    string filename2 = "carddetails.csv";
    if (!isValidFile(filename1))
    {
        cerr << "Error: File1 not found or inaccessible." << endl;
        return 1;
    }
    if (!isValidFile(filename2))
    {
        cerr << "Error: File2 not found or inaccessible." << endl;
        return 1;
    }
    // Load existing expenses from CSV file
    parseCSV(filename1, expenseData);
    parseCSV(filename2, cardid);
    menu();
    return 0;
}
// 2024-10-09,200,250,600,300,250,500   #Fraudulent: Sudden high spending
// 2024-10-10,300,400,1000,500,450,900  #Fraudulent: Extreme anomaly
// 2024-10-11,250,300,800,400,350,700   #Fraudulent: Unusual high expense
