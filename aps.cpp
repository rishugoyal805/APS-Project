#include "aps.h"
using namespace std;

bool isValidFile(const string &filename)
{
    ifstream file(filename);
    return file.good();
}

bool isValidCSV(const string &line)
{
    int commaCount = count(line.begin(), line.end(), ',');
    return commaCount == 3;
}

bool isValidDoubleInput(double &input)
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
        return day <= 29;
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

void parseCSV(const string &filename, vector<vector<pair<vector<int>, vector<int>>>> &vec, vector<int> &monthlyTotals)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        stringstream ss(line);
        string fullDate, amountStr;
        getline(ss, fullDate, ',');
        if (fullDate.length() < 10)
        {
            cerr << "Warning: Invalid date format in row: " << line << endl;
            continue;
        }
        int month = stoi(fullDate.substr(5, 2)) - 1;
        int day = stoi(fullDate.substr(8, 2)) - 1;
        if (month < 0 || month > 11 || day < 0 || day > 30)
        {
            cerr << "Warning: Invalid date in row: " << line << endl;
            continue;
        }
        for (size_t i = 0; i < essentialCategories.size(); i++)
        {
            if (getline(ss, amountStr, ','))
            {
                try
                {
                    int amount = stoi(amountStr);
                    vec[month][day].first[i] += amount;
                    if (filename == "OctExpenses.csv")
                    {
                        monthlyTotals[month] += amount;
                    }
                }
                catch (...)
                {
                    cerr << "Warning: Invalid amount in row: " << line << endl;
                }
            }
        }
        for (size_t i = 0; i < nonEssentialCategories.size(); i++)
        {
            if (getline(ss, amountStr, ','))
            {
                try
                {
                    int amount = stoi(amountStr);
                    vec[month][day].second[i] += amount;
                    if (filename == "OctExpenses.csv")
                    {
                        monthlyTotals[month] += amount;
                    }
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
        cout << "Parsing expenses details completed successfully!\n";
        detectFraudulentTransactions();
    }
    else
    {
        cout << "\nParsing card details completed successfully!\n";
    }
    Sleep(2000);
    file.close();
}

bool validateDateFormat(const string &date)
{
    regex datePattern(R"(\d{4}-\d{2}-\d{2})");
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
                continue;
            hasData = true;
            cout << "| 2024-"
                 << (month + 1 < 10 ? "0" : "") << month + 1 << "-"
                 << (day + 1 < 10 ? "0" : "") << day + 1 << " | ";
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

void addExpense()
{
    string date;
    cout << "Enter the date (YYYY-MM-DD): ";
    cin >> date;
    if (!validateDateFormat(date))
    {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }
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
    bool dateExists = false;
    for (int val : expenseData[month][day].first)
    {
        if (val != 0)
        {
            dateExists = true;
            break;
        }
    }
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
    map<string, double> expenseEntry;
    map<string, int> cards;
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
                    break;
                }
                else if (c_id == 4)
                {
                    c_id = 0;
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
            c_id = 0;
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
                    break;
                }
                else if (c_id == 4)
                {
                    c_id = 0;
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
            c_id = 0;
        }
        expenseEntry[category] = amount;
        cards[category] = c_id;
    }
    for (size_t i = 0; i < essentialCategories.size(); ++i)
    {
        expenseData[month][day].first[i] += expenseEntry[essentialCategories[i]];
        cardid[month][day].first[i] = ((cards[essentialCategories[i]]));
    }
    for (size_t i = 0; i < nonEssentialCategories.size(); ++i)
    {
        expenseData[month][day].second[i] += expenseEntry[nonEssentialCategories[i]];
        cardid[month][day].second[i] = ((cards[nonEssentialCategories[i]]));
    }
    ofstream file("OctExpenses.csv", ios::app);
    if (file.is_open())
    {
        file << "\n"
             << date;
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
    ofstream file1("carddetails.csv", ios::app);
    if (file1.is_open())
    {
        file1 << "\n"
              << date;
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
    cout << "Summary:\n";
    cout << "The function 'addExpense()' allows users to log daily essential and non-essential expenses by entering amounts and selecting the payment card used. "
         << "It validates the date format, prevents duplicate entries, stores data in structured maps, updates a 2D vector, and appends the data to CSV files. "
         << "This logic is comparable to LeetCode problems involving input validation, hashmap usage, and matrix data handling (e.g., Insert Delete GetRandom O(1)). "
         << "It uses concepts from date parsing, maps, vectors, file handling, and error checking. Time complexity is roughly O(n), where n is the number of categories.\n\n";
}

void updateExpense()
{
    string filename = "OctExpenses.csv";
    string date, category;
    double newAmount;
    displayExpenses();
    cout << "Enter the date (YYYY-MM-DD) to update the Expences: ";
    cin >> date;
    if (!validateDateFormat(date))
    {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return;
    }
    int month = stoi(date.substr(5, 2)) - 1;
    int day = stoi(date.substr(8, 2)) - 1;
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
        bool categoryFound = false;
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
        ifstream inFile(filename);
        if (!inFile.is_open())
        {
            cerr << "Error: Cannot open file " << filename << " for reading.\n";
            return;
        }
        ofstream outFile("temp.csv");
        if (!outFile.is_open())
        {
            cerr << "Error: Cannot open temporary file for writing.\n";
            inFile.close();
            return;
        }
        string line;
        bool found = false;
        if (getline(inFile, line))
        {
            outFile << line << "\n";
        }
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string recordDate, recordCategory;
            getline(ss, recordDate, ',');
            getline(ss, recordCategory, ',');
            if (recordDate == date)
            {
                found = true;
                stringstream newLineStream;
                newLineStream << recordDate;
                for (const auto &category : essentialCategories)
                {
                    newLineStream << "," << expenseData[month][day].first[&category - &essentialCategories[0]];
                }
                for (const auto &category : nonEssentialCategories)
                {
                    newLineStream << "," << expenseData[month][day].second[&category - &nonEssentialCategories[0]];
                }
                outFile << newLineStream.str();
            }
            else
            {
                outFile << line;
            }
            outFile << "\n";
        }
        inFile.close();
        outFile.close();
        if (found)
        {
            remove(filename.c_str());
            rename("temp.csv", filename.c_str());
            cout << "Expense for date " << date << " has been updated successfully.\n";
        }
        else
        {
            remove("temp.csv");
            cout << "Expense not found for the entered date.\n";
        }
    }
    else
    {
        cout << "Expense not found in the data structure!" << endl;
    }
    displayExpenses();
    cout << "Summary:\n";
    cout << "The function 'updateExpense' allows a user to update a specific category of daily expenses for a given date, validating inputs and modifying both in-memory and CSV-stored data accordingly. "
         << "It combines file I/O handling, data structure traversal, and user input validation. "
         << "This approach is similar to LeetCode problems like 'Edit Distance in a Matrix' where data updates must maintain consistency. "
         << "Key topics include file handling, 2D vector manipulation, string parsing, and input validation, with an overall time complexity of O(n) where n is the number of lines in the CSV file.\n\n";
}

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
    expenseData[month][day].first = {0, 0, 0};
    expenseData[month][day].second = {0, 0, 0};
    cardid[month][day].first = {0, 0, 0};
    cardid[month][day].second = {0, 0, 0};
    ifstream inFile(filename);
    if (!inFile.is_open())
    {
        cerr << "Error: Cannot open file " << filename << " for reading.\n";
        return false;
    }
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
    if (getline(inFile, line))
    {
        outFile << line << "\n";
    }
    while (getline(inFile, line))
    {
        stringstream ss(line);
        string recordDate;
        if (!getline(ss, recordDate, ','))
        {
            cerr << "Warning: Skipping malformed line.\n";
            continue;
        }
        if (recordDate == date)
        {
            found = true;
            continue;
        }
        if (!isFirstLine)
        {
            outFile << "\n";
        }
        outFile << line;
        isFirstLine = false;
    }
    inFile.close();
    outFile.close();
    if (found)
    {
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
        remove("temp.csv");
        cerr << "No expenses found for date: " << date << ". Nothing deleted.\n";
        return false;
    }
}

void menu(vector<int> &monthlyTotals);

void optimizeSavingsPlan(vector<tuple<int, int, string>> &nonEssentialExpensesWithDates, int &goal)
{
    int n = nonEssentialExpensesWithDates.size();
    vector<vector<int>> dp(n + 1, vector<int>(goal + 1, -1));
    vector<vector<int>> count(n + 1, vector<int>(goal + 1, 1e9));
    vector<vector<vector<tuple<int, int, string>>>> chosen(n + 1, vector<vector<tuple<int, int, string>>>(goal + 1));
    dp[0][0] = 0;
    count[0][0] = 0;
    for (int i = 1; i <= n; i++)
    {
        int expense = get<0>(nonEssentialExpensesWithDates[i - 1]);
        int date = get<1>(nonEssentialExpensesWithDates[i - 1]);
        string category = get<2>(nonEssentialExpensesWithDates[i - 1]);
        for (int j = 0; j <= goal; j++)
        {
            if (dp[i - 1][j] != -1)
            {
                dp[i][j] = dp[i - 1][j];
                count[i][j] = count[i - 1][j];
                chosen[i][j] = chosen[i - 1][j];
            }
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
    cout << "+-----------+---------------+-----------------+" << endl;
    cout << left
         << "| " << setw(10) << "Amount"
         << "| " << setw(14) << "Date (MM-DD)"
         << "| " << setw(16) << "Category"
         << "|" << endl;
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
            int date = (month + 1) * 100 + (day + 1);
            for (int i = 0; i < nonEssential.size(); ++i)
            {
                if (nonEssential[i] != 0)
                {
                    nonEssentialExpensesWithDates.push_back({nonEssential[i], date, nonEssentialCategories[i]});
                }
            }
        }
    }
    optimizeSavingsPlan(nonEssentialExpensesWithDates, goal);
    return 0;
}

vector<PaymentResult> optimizeCreditCardPayments(const vector<vector<pair<vector<int>, vector<int>>>> &expenseData, const vector<vector<pair<vector<int>, vector<int>>>> &cardid,
                                                 vector<CreditCard> &cardVec, int availableFunds)
{
    vector<int> totalDue(4, 0);
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
    vector<PaymentResult> results;
    for (int i = 1; i <= 3; ++i)
    {
        int unpaid = totalDue[i] - amountPaid[i];
        double interest = unpaid * cardVec[i].interestRate / 100.0;
        results.push_back({cardVec[i].name, totalDue[i], amountPaid[i], unpaid, interest});
    }
    sort(results.begin(), results.end(), [&](const PaymentResult &a, const PaymentResult &b)
         {
        int ai = a.card[0] - 'A' + 1;int bi = b.card[0] - 'A' + 1;
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
    cout << "+-----------+------------+-----------+-----------+-------------------+" << endl;
}

int findMinTravelCost(
    const vector<City> &cities, const vector<vector<ERoute>> &graph, int source, int destination, int numPeople, int numDays, vector<int> &parent)
{
    int n = cities.size();
    vector<int> minCost(n, numeric_limits<int>::max());
    priority_queue<CNode, vector<CNode>, greater<CNode>> pq;
    parent.assign(n, -1);
    minCost[source] = cities[source].hotelCostPerNight * numDays * numPeople;
    pq.push({source, minCost[source]});
    while (!pq.empty())
    {
        CNode current = pq.top();
        pq.pop();
        for (const ERoute &edge : graph[current.city])
        {
            int newCost = current.cost + edge.flightCost * numPeople + cities[edge.destination].hotelCostPerNight * numDays * numPeople;
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
    int numCities;
    while (true)
    {
        cout << "Enter number of cities: ";
        if (isValidIntInput(numCities) && numCities > 1)
            break;
        cout << "Invalid input. Number of cities must be greater than 1.\n";
    }
    vector<City> cities(numCities);
    unordered_map<string, int> cityIndex;
    cout << "\nEnter city names and hotel costs per night:\n";
    for (int i = 0; i < numCities; ++i)
    {
        string cityName;
        int hotelCost;
        cout << "City " << i + 1 << " name: ";
        cin >> ws;
        getline(cin, cityName);
        while (true)
        {
            cout << "Hotel cost per night in " << cityName << ": ";
            if (isValidIntInput(hotelCost))
                break;
            cout << "Please enter a valid non-negative number for hotel cost.\n";
        }
        cities[i] = {cityName, hotelCost};
        cityIndex[cityName] = i;
    }
    int numFlights;
    while (true)
    {
        cout << "\nEnter number of flight routes: ";
        if (isValidIntInput(numFlights) && numFlights >= 1)
            break;
        cout << "Enter at least 1 valid flight route.\n";
    }
    vector<vector<ERoute>> graph(numCities);
    for (int i = 0; i < numFlights; ++i)
    {
        string from, to;
        int cost;
        cout << "\nFlight Route " << i + 1 << ":\n";
        cout << "From city: ";
        cin >> ws;
        getline(cin, from);
        cout << "To city: ";
        getline(cin, to);
        if (cityIndex.find(from) == cityIndex.end() || cityIndex.find(to) == cityIndex.end())
        {
            cout << " Invalid city names. Please re-enter this flight route.\n";
            --i;
            continue;
        }
        while (true)
        {
            cout << "Flight cost from " << from << " to " << to << ": ";
            if (isValidIntInput(cost))
                break;
            cout << "Invalid cost. Please enter a non-negative number.\n";
        }
        graph[cityIndex[from]].push_back({cityIndex[to], cost});
    }
    string src, dest;
    cout << "\nEnter starting city: ";
    cin >> ws;
    getline(cin, src);
    cout << "Enter destination city: ";
    getline(cin, dest);
    if (cityIndex.find(src) == cityIndex.end() || cityIndex.find(dest) == cityIndex.end())
    {
        cout << "One or both cities are invalid.\n";
        return;
    }
    int people;
    while (true)
    {
        cout << "Enter number of travelers: ";
        if (isValidIntInput(people) && people > 0)
            break;
        cout << "Invalid number. Please enter a positive value.\n";
    }
    int days;
    while (true)
    {
        cout << "Enter number of days to stay in each city: ";
        if (isValidIntInput(days) && days > 0)
            break;
        cout << "Please enter a positive number of days.\n";
    }
    vector<int> parent;
    int totalCost = findMinTravelCost(cities, graph, cityIndex[src], cityIndex[dest], people, days, parent);
    cout << "\nMinimum total cost from " << src << " to " << dest
         << " for " << people << " traveler(s) over " << days << " day(s) is: Rs " << totalCost << endl;
    printPath(parent, cityIndex[src], cityIndex[dest], cities);
    cout << endl;
    cout << endl;
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
    const char *monthNames[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
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
    for (auto &e : mst)
    {
        if (e.weight > 0)
        {
            int day1 = e.src % 31;
            int month1 = e.src / 31;
            int day2 = e.dest % 31;
            int month2 = e.dest / 31;
            double total1 = 0, total2 = 0;
            for (int i = 0; i < 3; ++i)
            {
                total1 += expenseData[month1][day1].first[i] + expenseData[month1][day1].second[i];
                total2 += expenseData[month2][day2].first[i] + expenseData[month2][day2].second[i];
            }
            cout << monthNames[month1] << " " << (day1 + 1) << " ---> "
                 << monthNames[month2] << " " << (day2 + 1) << "\n"
                 << "   - " << monthNames[month1] << " " << (day1 + 1) << " Expenses: Rs. " << total1 << "\n"
                 << "   - " << monthNames[month2] << " " << (day2 + 1) << " Expenses: Rs. " << total2 << "\n"
                 << "   - Difference (Transfer Need): Rs. " << e.weight << "\n\n";
        }
    }
    cout << "\nTotal Minimum Transfer Cost: Rs. " << totalCost << "\n";
    cout << "Summary:\n";
    cout << "The function 'allocateEmergencyFunds' models daily expenses as a graph and applies Kruskal's algorithm (greedy algorithm) to minimize the cost of transferring emergency funds over time. "
         << "It treats each day as a node and connects consecutive days with edges weighted by the absolute difference in expense totals. "
         << "This resembles classic MST problems on LeetCode like 1135 (Connecting Cities With Minimum Cost) and applies greedy and union-find techniques. "
         << "The time complexity is approximately O(E log E + N), where E is the number of edges and N is the number of nodes.\n\n";
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
        encode(node->right, str + "1"); };
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
    cout << "Summary:\n";
    cout << "The function 'updateExpenseData' compresses CSV data using Huffman Encoding by building a frequency-based binary tree and replacing characters with shorter binary codes. "
         << "It stores both the Huffman dictionary and the compressed bitstring to a new file. "
         << "This is similar to LeetCode problems involving compression and encoding like 451 (Sort Characters by Frequency) and 271 (Encode and Decode Strings). "
         << "The time complexity is O(n log n) due to the priority queue operations over n characters in the Huffman tree construction.\n\n";
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
    cout << "Summary:\n";
    cout << "The function 'restoreExpenseData' reads a Huffman-encoded CSV file, reconstructs the encoding map, and decodes the binary string back into original text. "
         << "It uses a reverse lookup on the binary codes and writes the decompressed result into a new file. "
         << "This process is conceptually related to decoding problems like LeetCode 271 (Encode and Decode Strings) or 5 (Longest Palindromic Substring, in terms of parsing). "
         << "The time complexity is O(n), where n is the number of bits in the compressed data.\n\n";
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

vector<LoanRepaymentResult> optimizeLoanRepayment(const vector<vector<pair<vector<int>, vector<int>>>> &expenseData, int income, int month, vector<Loan> &loans)
{
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
    vector<pair<int, double>> loanPriority;
    for (int i = 0; i < loans.size(); ++i)
    {
        double density = loans[i].interestRate / loans[i].amount;
        loanPriority.push_back({i, density});
    }
    sort(loanPriority.begin(), loanPriority.end(), [](auto &a, auto &b)
         { return a.second > b.second; });
    vector<LoanRepaymentResult> results(loans.size());
    for (int i = 0; i < loans.size(); ++i)
    {
        results[i] = {loans[i].id, loans[i].amount, 0.0, loans[i].amount, loans[i].amount * loans[i].interestRate / 100.0};
    }
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
    vector<Investment> investments = {{101, 2000, 5000, 20}, {102, 100, 500, 10}, {103, 10000, 50000, 50}, {104, 100, 500, 10}, {105, 6000, 10000, 50}, {106, 2000, 10000, 20}, {107, 5000, 20000, 30}, {108, 1000, 5000, 15}, {109, 3000, 15000, 25}, {110, 4000, 20000, 40}};
    sort(investments.begin(), investments.end(), [](const Investment &a, const Investment &b)
         { return (a.returnPerUnit / a.riskPerUnit) > (b.returnPerUnit / b.riskPerUnit); });
    double totalReturn = 0;
    vector<pair<int, int>> selected;
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
    cout << left << "| " << setw(15) << "Investment ID" << "| " << setw(18) << "Units Selected"
         << "| " << setw(19) << "Return (Rs.)" << "|" << endl;
    cout << "+----------------+-------------------+--------------------+" << endl;
    for (const auto &entry : selected)
    {
        int id = entry.first;
        int units = entry.second;
        double ret = units * investments[id % 100 - 1].returnPerUnit;
        cout << left << "| " << setw(15) << id << "| " << setw(18) << units
             << "| Rs. " << setw(15) << fixed << setprecision(2) << ret << "|" << endl;
    }
    cout << "+----------------+-------------------+--------------------+" << endl;
    cout << "\nTotal Expected Return: Rs." << fixed << setprecision(2) << totalReturn << endl;
}

void generateBudgetPlan(double monthlyIncome)
{
    cout << fixed << setprecision(2);
    cout << "\n Monthly Budget for Income: " << monthlyIncome << "\n"
         << endl;
    cout << " NEEDS (50%) - " << 0.50 * monthlyIncome << endl;
    cout << "   Rent (30%): " << 0.30 * monthlyIncome << endl;
    cout << "   Groceries (10%): " << 0.10 * monthlyIncome << endl;
    cout << "   Utilities (5%): " << 0.05 * monthlyIncome << endl;
    cout << "   Transportation (5%): " << 0.05 * monthlyIncome << endl;
    cout << "\n WANTS (15%) - " << 0.15 * monthlyIncome << endl;
    cout << "   Shopping + EMIs (10%): " << 0.10 * monthlyIncome << endl;
    cout << "   Entertainment + Travel (5%): " << 0.05 * monthlyIncome << endl;
    cout << "\n SAVINGS + INVESTMENTS (20%) - " << 0.20 * monthlyIncome << endl;
    cout << "   Health + Term Insurance (5%): " << 0.05 * monthlyIncome << endl;
    cout << "   SIP (in Mutual Funds) (10%): " << 0.10 * monthlyIncome << endl;
    cout << "   Emergency Fund (5%): " << 0.05 * monthlyIncome << endl;
    cout << "\n OTHERS (15%) - " << 0.15 * monthlyIncome << endl;
    cout << "   Upskilling (5%): " << 0.05 * monthlyIncome << endl;
    cout << "   Family Support (10%): " << 0.10 * monthlyIncome << endl;
    cout << "\n TOTAL: " << monthlyIncome << " distributed across needs, wants, savings, and others.\n"
         << endl;
    cout << "Summary:\n";
    cout << "The function 'generateBudgetPlan' categorizes a user's monthly income into Needs, Wants, Savings + Investments, and Others based on fixed percentages. "
         << "It calculates and displays each category and sub-category with exact monetary allocations using standard financial budgeting principles. \n\n";
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

RentBuyResult rentVsBuyDecision(const RentBuyInput &input, double interestRate, double rentIncreaseRate)
{
    RentBuyResult result;
    result.totalRentCost = 0;
    double currentRent = input.rentCost;
    for (int i = 0; i < input.yearsToOwnProperty; ++i)
    {
        result.totalRentCost += currentRent * 12;
        currentRent *= (1 + rentIncreaseRate);
    }
    double remainingPrincipal = input.propertyCost;
    double monthlyRate = interestRate / 12;
    int months = 0;
    double totalEmiPaid = 0;
    while (remainingPrincipal > 0 && months < 1000 * 12)
    {
        double monthlyInterest = remainingPrincipal * monthlyRate;
        double principalPaid = input.emi - monthlyInterest;
        if (principalPaid <= 0)
        {
            result.totalBuyCost = -1;
            result.yearsToOwnProperty = -1;
            result.recommendation = "EMI too low to ever repay the loan.";
            return result;
        }
        remainingPrincipal -= principalPaid;
        totalEmiPaid += input.emi;
        months++;
    }
    result.totalBuyCost = totalEmiPaid;
    result.yearsToOwnProperty = (months + 11) / 12;
    result.recommendation = (result.totalBuyCost < result.totalRentCost) ? "Buy" : "Rent";
    return result;
}

InventoryResult optimizeInventory(const vector<Product> &products, int capacity)
{
    int n = products.size();
    vector<vector<int>> dp(n + 1, vector<int>(capacity + 1, 0));
    vector<vector<bool>> taken(n + 1, vector<bool>(capacity + 1, false));
    for (int i = 1; i <= n; ++i)
    {
        const Product &p = products[i - 1];
        for (int c = 0; c <= capacity; ++c)
        {
            dp[i][c] = dp[i - 1][c];
            if (p.size <= c)
            {
                int newProfit = dp[i - 1][c - p.size] + p.profit;
                if (newProfit > dp[i][c])
                {
                    dp[i][c] = newProfit;
                    taken[i][c] = true;
                }
            }
        }
    }
    InventoryResult result;
    result.totalProfit = dp[n][capacity];
    int c = capacity;
    for (int i = n; i >= 1; --i)
    {
        if (taken[i][c])
        {
            result.selectedProductIndices.push_back(i - 1);
            c -= products[i - 1].size;
        }
    }
    return result;
}

SchedulerResult scheduleRecurringExpenses(const vector<RecurringBill> &bills, int income)
{
    vector<RecurringBill> sorted = bills;
    sort(sorted.begin(), sorted.end(), [](const RecurringBill &a, const RecurringBill &b)
         { return a.penalty > b.penalty; });
    int remaining = income;
    SchedulerResult result;
    for (const auto &bill : sorted)
    {
        if (bill.amount <= remaining)
        {
            result.paymentSchedule.push_back("Paid: " + bill.name);
            remaining -= bill.amount;
        }
        else
        {
            result.paymentSchedule.push_back("Unpaid: " + bill.name + " (Penalty: " + to_string(bill.penalty) + ")");
            result.totalPenaltyPaid += bill.penalty;
        }
    }
    return result;
}

void runRentVsBuySimulator(vector<int> &monthlyTotals)
{
    RentBuyInput input;
    double totalMonthlyExpenses = monthlyTotals[9];
    cout << "Enter monthly income: ";
    if (!isValidDoubleInput(input.income))
    {
        cout << "Invalid amount entered! Please enter a correct income value.\n";
        return;
    }
    if (input.income < totalMonthlyExpenses)
    {
        char proceed;
        cout << "Your income is less than monthly expenses (" << totalMonthlyExpenses << "). Do you want to proceed further? (y/n): ";
        cin >> proceed;
        if (proceed != 'y' && proceed != 'Y')
        {
            cout << "Exiting as per your choice.\n";
            return;
        }
    }
    cout << "Enter monthly rent cost: ";
    if (!isValidDoubleInput(input.rentCost))
    {
        cout << "Invalid amount entered! Please enter a valid rent cost.\n";
        return;
    }
    double savings = input.income - totalMonthlyExpenses;
    if (input.rentCost < savings)
    {
        cout << " The rent is affordable, as it is less than your monthly savings (Rs. " << fixed << setprecision(2) << savings << ").\n";
    }
    else
    {
        cout << " The rent might stretch your budget, as it exceeds your monthly savings.\n";
    }
    cout << "Enter home loan EMI: ";
    if (!isValidDoubleInput(input.emi))
    {
        cout << "Invalid amount entered! Please enter a valid EMI value.\n";
        return;
    }
    cout << "Enter total property cost: ";
    if (!isValidDoubleInput(input.propertyCost))
    {
        cout << "Invalid amount entered! Please enter a valid property cost.\n";
        return;
    }
    cout << "Enter years of stay: ";
    if (!isValidIntInput(input.yearsToOwnProperty))
    {
        cout << "Invalid input! Please enter a valid year.\n";
        return;
    }
    double emiInterestRate;
    cout << "Enter home loan interest rate (e.g., 0.08 for 8%): ";
    if (!isValidDoubleInput(emiInterestRate))
    {
        cout << "Invalid amount entered! Please enter a valid home loan interest rate.\n";
        return;
    }
    double rentIncreaseRate;
    cout << "Enter expected annual rent increase rate (e.g., 0.05 for 5%): ";
    if (!isValidDoubleInput(rentIncreaseRate))
    {
        cout << "Invalid amount entered! Please enter a valid rent increase rate.\n";
        return;
    }
    RentBuyResult result = rentVsBuyDecision(input, emiInterestRate, rentIncreaseRate);
    cout << "\n--- Rent vs Buy Analysis ---\n";
    cout << "Total cost if Renting (with yearly rent increase): Rs. " << fixed << setprecision(2) << result.totalRentCost << endl;
    cout << "Total cost if Buying (with compound EMI): Rs. " << fixed << setprecision(2) << result.totalBuyCost << endl;
    cout << "Years to own the property: " << fixed << setprecision(2) << result.yearsToOwnProperty << endl;
    cout << "Recommendation: " << result.recommendation << endl;
    Sleep(2000);
    cout << endl;
}

void runInventoryOptimizer()
{
    int capacity;
    cout << "Enter warehouse capacity: ";
    if (!isValidIntInput(capacity))
    {
        cout << "Invalid input! Please enter a valid capacity value.\n";
        return;
    }
    vector<Product> products;
    products.push_back({4, 400});
    products.push_back({3, 300});
    products.push_back({2, 200});
    products.push_back({1, 100});
    products.push_back({5, 500});
    products.push_back({6, 600});
    products.push_back({7, 700});
    products.push_back({8, 800});
    products.push_back({9, 900});
    products.push_back({10, 1000});
    products.push_back({11, 1100});
    products.push_back({12, 1200});
    InventoryResult result = optimizeInventory(products, capacity);
    cout << "\n--- Inventory Optimization Result ---\n";
    cout << "Max Profit: " << result.totalProfit << "\n";
    cout << "Selected Products:\n";
    cout << "ID\tSize\tProfit\n";
    for (int idx : result.selectedProductIndices)
    {
        const Product &p = products[idx];
        cout << "P" << idx + 1 << "\t" << p.size << "\t" << p.profit << "\n";
    }
    int usedCapacity = 0;
    for (int idx : result.selectedProductIndices)
        usedCapacity += products[idx].size;
    cout << "Used Capacity: " << usedCapacity << " / " << capacity << endl;
    Sleep(2000);
    cout << endl;
}

void runRecurringExpenseScheduler(vector<int> &monthlyTotals)
{
    int income;
    cout << "Enter your monthly income: ";
    if (!isValidIntInput(income) || income <= 0)
    {
        cout << "Invalid input! Please enter a positive integer for income.\n";
        return;
    }
    int month;
    cout << "Enter month number (1-12): ";
    if (!isValidIntInput(month) || month < 1 || month > 12)
    {
        cout << "Invalid month! Please enter a number between 1 and 12.\n";
        return;
    }
    double totalMonthExpenses = 0;
    totalMonthExpenses = monthlyTotals[month - 1];
    double availableIncome = income - totalMonthExpenses;
    cout << "Total expenses for month " << month << ": " << totalMonthExpenses << endl;
    cout << "Available income after expenses: " << availableIncome << endl;
    if (availableIncome <= 0)
    {
        cout << "Warning: You have no disposable income for scheduling bills.\n";
        return;
    }
    vector<RecurringBill> bills = {
        {"Electricity", 1500, 10, 100},
        {"Credit Card", 4000, 5, 200},
        {"Mobile Bill", 1000, 15, 50},
        {"Internet", 1200, 20, 80}};
    SchedulerResult result = scheduleRecurringExpenses(bills, static_cast<int>(availableIncome));
    cout << "\n--- Recurring Expense Schedule ---\n";
    for (const string &entry : result.paymentSchedule)
    {
        cout << entry << endl;
    }
    cout << "Total Penalty Paid: Rs." << result.totalPenaltyPaid << endl;
    Sleep(2000);
}

void menu(vector<int> &monthlyTotals)
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
            cout << "\n16. Rent vs Buy";
            cout << "\n17. Inventory Optimizer";
            cout << "\n18. Recurring Expense Scheduler";
            cout << "\n19. Exit";
            cout << "\nEnter your choice: ";
            cin >> choice;
            if (cin.fail())
            {
                throw invalid_argument("Input must be an integer.");
            }
            if (choice < 1 || choice > 19)
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
            vector<Loan> loans = {Loan(1, 15000, 10.5), Loan(2, 20000, 8.2),
                                  Loan(3, 12000, 12.0), Loan(4, 5000, 5.0), Loan(5, 8000, 7.5),
                                  Loan(6, 10000, 9.0), Loan(7, 7000, 6.5)};
            vector<LoanRepaymentResult> payVecLoan;
            vector<Investment> investments;
            vector<InvestmentSelection> result;
            switch (choice)
            {
            case 1:
                displayExpenses();
                Sleep(2000);
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
                if (!validateDateFormat(date))
                {
                    cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
                    break;
                }
                if (deleteExpenses(filename, date) || deleteExpenses(filename2, date))
                {
                    cout << "Expenses for date " << date << " have been deleted successfully.\n";
                    displayExpenses();
                    cout << "Summary:\n";
                    cout << "The function `deleteExpenses` removes all expense data for a specified date from both memory and a CSV file. "
                         << "It first parses the date, validates it, clears the corresponding in-memory entries, then rewrites the CSV without that date's entry. "
                         << "This resembles LeetCode file-handling or string-parsing problems like 'Delete Operation for Two Strings'. "
                         << "It uses topics like file I/O, error handling, date parsing, and string manipulation. Time complexity is O(n), where n is the number of lines in the file.\n\n";
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
                if (cin.fail() || income <= 0)
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid income amount. Please enter a positive numeric value.\n";
                    break;
                }
                cout << "\nGenerating your personalized budget plan...\n\n";
                generateBudgetPlan(income);
                Sleep(5000);
                break;
            }
            case 6:
                allocateEmergencyFunds();
                Sleep(200);
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
                while (true)
                {
                    cout << "Enter your target savings goal (in Rs): ";
                    if (isValidIntInput(goal) && goal > 0)
                        break;
                    cout << "Invalid input. Please enter a **positive** numeric value for your goal.\n";
                }
                int excessAmount;
                while (true)
                {
                    cout << "\nWould you like to allow a small flexibility margin above your goal? (optional)\n";
                    cout << "Enter the extra amount you are willing to accept (Enter 0 if you want an exact match): ";
                    if (isValidIntInput(excessAmount))
                        break;
                    cout << "Invalid amount. Please enter a **non-negative** numeric value.\n";
                }
                cout << "\nSummary of your savings configuration:\n";
                cout << "  - Base Target Goal     : Rs. " << goal << endl;
                cout << "  - Flexibility Allowed  : Rs. " << excessAmount << endl;
                cout << "\nAttempting optimization for *Exact Goal (Rs. " << goal << ")*...\n";
                optimizeSavings(goal);
                if (excessAmount > 0)
                    cout << "  - Extended Target Goal : Rs. " << goal + excessAmount << endl;
                else
                    cout << "  - No flexibility allowed (exact goal only)\n";
                if (excessAmount > 0)
                {
                    int finalGoal = goal + excessAmount;
                    cout << "\nAttempting optimization for *Flexible Goal (Up to Rs. " << finalGoal << ")*...\n";
                    optimizeSavings(finalGoal);
                }
                cout << "Summary:\n";
                cout << "The function identifies low-priority expenses that can be trimmed to meet savings goals with minimal disruption, \n"
                     << "using a variation of the 0/1 Knapsack algorithm to maximize savings while minimizing the number of eliminated expenses.\n"
                     << "This is similar to LeetCode Problem 416 ('Partition Equal Subset Sum') with the added constraint of minimizing the number of items used.\n"
                     << "The time complexity is O(N * G), where N is the number of expenses and G is the savings goal (treated as capacity).\n\n";
                break;
            }
            case 10:
            {
                int funds;
                while (true)
                {
                    cout << "Enter your available funds for credit card payments (in Rs): ";
                    if (isValidIntInput(funds))
                        break;
                    cout << "Invalid input. Please enter a non-negative numeric value.\n";
                }
                vector<PaymentResult> payVec = optimizeCreditCardPayments(expenseData, cardid, cardVec, funds);
                displayResults(payVec);
                double maxInterest = 0;
                string maxCard;
                for (const auto &r : payVec)
                {
                    if (r.interest > maxInterest)
                    {
                        maxInterest = r.interest;
                        maxCard = r.card;
                    }
                }
                if (maxInterest > 0)
                {
                    cout << "\nStrategy Suggestion: Prioritize clearing dues on card '" << maxCard
                         << "' to reduce the highest incurred interest of Rs. " << fixed << setprecision(2) << maxInterest << ".\n";
                }
                else
                {
                    cout << "\nGreat job! All dues are covered  no interest will be incurred.\n";
                }
                cout << endl
                     << endl;
                cout << "Summary:\n";
                cout << "The function optimizes credit card payments using a greedy approach by prioritizing low dues and high-interest cards,\n"
                     << "allocating funds to minimize interest accumulation.\n"
                     << "This is similar to LeetCode Problem 134 ('Gas Station') and strategies for debt repayment.\n"
                     << "The time complexity is O(M * N + 3 log 3 + 3 log 3), where M is the number of months, and N is the number of days.\n\n";
                break;
            }
            case 11:
                travelExpenseMinimizer();
                cout << "Summary:\n";
                cout << "The function calculates the minimum travel cost using Dijkstra's algorithm, factoring in flight and hotel costs for a group. \n"
                     << "It uses a greedy approach, prioritizing flights and hotel stays while ensuring optimal travel cost calculation.\n"
                     << "This is similar to problems like LeetCode Problem 787, but extended to include hotel costs.\n"
                     << "The time complexity is O(E x log V), where V is the number of cities and E is the number of flight routes.\n\n";
                break;
            case 12:
                cout << "Enter your income: ";
                int income1;
                cin >> income1;
                if (cin.fail())
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a numeric value.\n";
                    break;
                }
                if (income1 <= 0)
                {
                    cout << "Income must be a positive value.\n";
                    break;
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
                cout << "Summary:\n";
                cout << "The function optimizes loan repayments using a greedy strategy by distributing leftover income after expenses to loans with the highest interest-to-amount ratio. "
                     << "It ensures minimal interest accumulation by prioritizing high-density loans first. "
                     << "This is similar to greedy problems like those on LeetCode (e.g., Candy or Queue Reconstruction). "
                     << "The time complexity is O(m log m + 31n), where m is the number of loans.\n\n";
                break;
            case 13:
                int riskBudget;
                cout << "Enter your total risk budget: ";
                cin >> riskBudget;
                optimizeInvestmentPortfolio(riskBudget);
                cout << "Summary:\n";
                cout << "The function optimizes investment selections by using a greedy strategy to choose investments with the highest return-to-risk ratio within a given total risk budget. "
                     << "It allocates the budget across available options to maximize total return, selecting as many units as allowed by the risk constraint. "
                     << "This is conceptually similar to the Fractional Knapsack problem on LeetCode, where items with the highest value-to-weight ratio are prioritized. "
                     << "The topics used include Greedy Algorithms and Sorting, and the time complexity is O(n log n), where n is the number of investments.\n\n";
                break;
            case 14:
            {
                int key;
                cout << "Enter encryption key (positive integer): ";
                cin >> key;
                encrypt(filename, "encrypted_Expenses.csv", key);
                encrypt(filename2, "encrypted_carddetails.csv", key);
                cout << "Summary:\n";
                cout << "The function 'encrypt' reads a plaintext file and writes an encrypted version to another file using a Caesar cipher by shifting alphabetic and numeric characters forward by a key. "
                     << "It maintains case sensitivity and handles wrap-around with modular arithmetic for both letters and digits. "
                     << "This resembles LeetCode string manipulation problems like 1844 (Replace All Digits with Characters) or 709 (To Lower Case). "
                     << "The time complexity is O(n), where n is the number of characters in the input file.\n\n";
                Sleep(200);
                break;
            }
            case 15:
            {
                int key;
                cout << "Enter decryption key (must match encryption key): ";
                cin >> key;
                decrypt("encrypted_Expenses.csv", "decrypted_Expenses.csv", key);
                decrypt("encrypted_carddetails.csv", "decrypted_carddetails.csv", key);
                cout << "Summary:\n";
                cout << "The function 'decrypt' reads an encrypted file and writes the decrypted content to another file using a Caesar cipher reversal on letters and digits. "
                     << "It preserves case and handles wrap-around with modular arithmetic for both alphabets and numbers. "
                     << "This is similar to LeetCode problems involving string transformations, such as 2325 (Decode the Message) or 1528 (Shuffle String). "
                     << "The time complexity is O(n), where n is the number of characters in the input file.\n\n";
                Sleep(200);
                break;
            }
            case 16:
                runRentVsBuySimulator(monthlyTotals);
                cout << "Summary:\n";
                cout << "The function evaluates whether renting or buying a property is financially smarter by comparing cumulative rent costs (with yearly hikes) against compounded EMI payments and maintenance using basic financial modeling. "
                     << "It uses decision-making strategies using simulation and Financial Math modeling. "
                     << "The time complexity is O(y) for y years of simulation, with additional O(1) compound interest math operations.\n\n";

                Sleep(3000);
                break;
            case 17:
                runInventoryOptimizer();
                cout << "Summary:\n";
                cout << "The function optimizes inventory storage using a dynamic programming strategy, selecting products that maximize profit without exceeding warehouse capacity. "
                     << "It reconstructs the selection path to determine which products were included in the optimal solution. "
                     << "This mirrors classic 0/1 Knapsack problems on LeetCode (e.g., '01 Knapsack' or 'Partition Equal Subset Sum'). "
                     << "The time complexity is O(n * capacity), where n is the number of products.\n\n";
                Sleep(3000);
                break;
            case 18:
                runRecurringExpenseScheduler(monthlyTotals);
                cout << "Summary:\n";
                cout << "The function schedules recurring expenses by first calculating disposable income after monthly expenses, then uses a greedy strategy to prioritize bill payments based on penalty severity. "
                     << "It ensures that the most financially damaging bills are paid first when income is limited, reducing total penalties. "
                     << "This problem resembles greedy selection problems like LeetCode's 'Task Scheduler' or 'IPO', where maximum gain or minimum loss is prioritized under constraints. "
                     << "The key topics include Greedy Algorithms, Sorting, and Budgeting Simulation. Time complexity is O(n log n), where n is the number of bills.\n\n";
                Sleep(3000);
                break;
            case 19:
                cout << "Exiting program...\n";
                return;
            }
        }
        catch (const exception &e)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: " << e.what() << "\nPlease try again.\n";
        }
    }
}

void displayHeader()
{
    cout << "****************************************************************" << endl;
    cout << "*                      WELCOME TO OUR                          *" << endl;
    cout << "*         Wealthwise: Algorithmic Financial Supervisor         *" << endl;
    cout << "*                                                              *" << endl;
    cout << "* Submitted to:                                 Programmed by: *" << endl;
    cout << "* Dr. Suma Dawn                                          Rishu *" << endl;
    cout << "* Mr. Tarun Agrawal                               Swayam Gupta *" << endl;
    cout << "* Mr. Rohit Kumar Sony                            Maanya Gupta *" << endl;
    cout << "*                                              Priyanshu Rawat *" << endl;
    cout << "*                                                              *" << endl;
    cout << "****************************************************************" << "\n"
         << endl;
}

int main()
{
    string filename1 = "OctExpenses.csv";
    string filename2 = "carddetails.csv";
    vector<int> monthlyTotals(12, 0.0);
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
    displayHeader();
    parseCSV(filename1, expenseData, monthlyTotals);
    parseCSV(filename2, cardid, monthlyTotals);
    menu(monthlyTotals);
    return 0;
}
