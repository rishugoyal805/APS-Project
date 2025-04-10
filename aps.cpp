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

void detectFraudulentTransactions()
{
    cout << "\nDetecting Fraudulent Transactions...\n";

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

    cout << "\nFlagged Transactions:\n";
    cout << "------------------------------------------------------\n";
    cout << "|    Date    |  Category  |  Amount  |  Status  |\n";
    cout << "------------------------------------------------------\n";

    for (int month = 0; month < 12; month++)
    {
        for (int day = 0; day < 31; day++)
        {
            for (int i = 0; i < 3; i++)
            {
                if (expenseData[month][day].first[i] > essentialMean[i] + 2 * essentialSD[i])
                {
                    cout << "| " << "2024-"
                         << setfill('0') << setw(2) << month + 1 << "-"
                         << setfill('0') << setw(2) << day + 1 << " | "
                         << essentialCategories[i] << " | "
                         << expenseData[month][day].first[i] << " | Fraudulent |\n";
                }
                if (expenseData[month][day].second[i] > nonEssentialMean[i] + 2 * nonEssentialSD[i])
                {
                    cout << "| " << "2024-"
                         << setfill('0') << setw(2) << month + 1 << "-"
                         << setfill('0') << setw(2) << day + 1 << " | "
                         << nonEssentialCategories[i] << " | "
                         << expenseData[month][day].second[i] << " | Fraudulent |\n";
                }
            }
        }
    }
    cout << "------------------------------------------------------\n";
}

void parseCSV(const string &filename,vector<vector<pair<vector<int>, vector<int>>>>&vec)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;

    // Read each row of data
    while (getline(file, line))
    {
        stringstream ss(line);
        string fullDate, amountStr;

        getline(ss, fullDate, ','); // Extract date

        // ✅ Validate Date Length Before Using `substr()`
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
    detectFraudulentTransactions();
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
                continue; // Skip empty days

            hasData = true;

            // ✅ Format date correctly
            cout << "| " << "2024-" << setw(2) << month + 1
                 << "-" << setw(2) << day + 1 << " | ";

            // loops mai problem aari thi spacing ki
            cout << setw(6) << expenseData[month][day].first[0] << " | ";
            cout << setw(6) << expenseData[month][day].first[1] << " | ";
            cout << setw(8) << expenseData[month][day].first[2] << " | ";
            cout << setw(7) << expenseData[month][day].second[0] << "  | ";
            cout << setw(5) << expenseData[month][day].second[1] << " | ";
            cout << setw(7) << expenseData[month][day].second[2] << " | ";

            // // ✅ Print essential expenses
            // for (int i = 0; i < 3; i++) {
            //     cout << setw(6) << expenseData[month][day].first[i] << " | ";
            // }

            // cout << " "; // Separation for non-essential expenses

            // // ✅ Non-essential expenses: Travel, Fun, Extra (aligned properly)
            // for (int i = 0; i < 3; i++) {
            //     cout << setw(7) << expenseData[month][day].second[i] << " | ";
            // }

            cout << "\n";
        }
    }

    if (!hasData)
    {
        cout << "No expenses recorded." << endl;
    }

    cout << "------------------------------------------------------------------------\n";
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
    int month = stoi(date.substr(5, 2)) - 1; // 0-based index for month
    int day = stoi(date.substr(8, 2)) - 1;   // 0-based index for day

    if (month < 0 || month >= 12 || day < 0 || day >= 31)
    {
        cout << "Invalid date! Month or day out of range.\n";
        return;
    }

    // Map to hold expenses for each category
    map<string, double> expenseEntry;
    map<string,int> cards;

    // Take input for each category
    for (const auto &category : essentialCategories)
    {
        double amount;
        int c_id;
        cout << "Enter amount" << category << ": ";
        cin >> amount;
        bool flag=true;
        while(true){
        cout<<"choose the cardid you used for the payment"<<endl;
        cout<<"1-> A"<<endl;
        cout<<"2-> B"<<endl;
        cout<<"3-> C"<<endl;
        cout<<"4-> None"<<endl;
        cin>>c_id;
        if(c_id!=1&&c_id!=2&&c_id!=3 && c_id!=4){
            cout<<"wrong choice try again"<<endl;
 
        }
        else if(c_id==4){
            c_id=0;
            break;
        }
        else{
            break;
        }

       

    }
        
       
    
        

        expenseEntry[category] = amount;
        cards[category]=c_id;
    }

    for (const auto &category : nonEssentialCategories)
    {
        double amount;
        int c_id;
        cout << "Enter amount" << category << ": ";
        cin >> amount;
        bool flag=true;
        while(true){
        cout<<"choose the cardid you used for the payment"<<endl;
        cout<<"1-> A"<<endl;
        cout<<"2-> B"<<endl;
        cout<<"3-> C"<<endl;
        cout<<"4-> None"<<endl;
        cin>>c_id;
        if(c_id!=1&&c_id!=2&&c_id!=3 && c_id!=4){
            cout<<"wrong choice try again"<<endl;
 
        }
        else if(c_id==4){
            c_id=0;
            break;
        }
        else{
            break;
        }

       

    }
        
        

        expenseEntry[category] = amount;
        cards[category]=c_id;
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
        cardid[month][day].second[i] = ((cards[essentialCategories[i]]));
    }

    // Append to CSV file
    ofstream file("filename.csv", ios::app); // Append mode
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
    string filename = "filename.csv";
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

        cin >> choice;

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
        cin >> newAmount;

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
}

// Function to delete expenses for a given date
bool deleteExpenses(string &filename,string & date)
{
    
    
    displayExpenses();
    

    // Validate Date Format
    if (!validateDateFormat(date))
    {
        cout << "Invalid date format! Please enter in YYYY-MM-DD format.\n";
        return false;
    }

    // Extract month and day from the date
    int month = stoi(date.substr(5, 2)) - 1; // 0-based index for month
    int day = stoi(date.substr(8, 2)) - 1;   // 0-based index for day

    if (month < 0 || month >= 12 || day < 0 || day >= 31)
    {
        cout << "Invalid date! Month or day out of range.\n";
        return false;
    }

    // Clear the expenses for the specified date
    expenseData[month][day].first = {0, 0, 0};  // Clear essential category expenses
    expenseData[month][day].second = {0, 0, 0}; // Clear non-essential category expenses
    cardid[month][day].first = {0, 0, 0};  // Clear essential category expenses
    cardid[month][day].second = {0, 0, 0}; // Clear non-essential category expenses
    

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
        getline(ss, recordDate, ',');

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
        remove(filename.c_str());             // Delete the old file
        rename("temp.csv", filename.c_str()); // Rename the temporary file to the original filename
        return true;
    }
    else
    {
        remove("temp.csv"); // If no matching date is found, remove the temporary file
        return false;
       
    }
}

void menu();


void optimizeSavingsPlan(vector<int> &nonEssentialExpenses, int &goal) {
    int n = nonEssentialExpenses.size();
    vector<vector<int>> dp(n + 1, vector<int>(goal + 1, -1)); // -1 means unreachable
    vector<vector<int>> count(n + 1, vector<int>(goal + 1, 1e9));

    dp[0][0] = 0;
    count[0][0] = 0;

    for (int i = 1; i <= n; i++) {
        int expense = nonEssentialExpenses[i - 1];
        for (int j = 0; j <= goal; j++) {
            // Case 1: Don't take the current expense
            if (dp[i - 1][j] != -1) {
                dp[i][j] = dp[i - 1][j];
                count[i][j] = count[i - 1][j];
            }

            // Case 2: Take the current expense (if it fits)
            if (j >= expense && dp[i - 1][j - expense] != -1) {
                int newSum = dp[i - 1][j - expense] + expense;
                int newCount = count[i - 1][j - expense] + 1;

                if (dp[i][j] == -1 || (newSum > dp[i][j]) || (newSum == dp[i][j] && newCount < count[i][j])) {
                    dp[i][j] = newSum;
                    count[i][j] = newCount;
                }
            }
        }
    }

    // Find max achievable sum ≤ goal with fewest entries
    int bestSavings = 0, minCount = 1e9;
    for (int j = 0; j <= goal; j++) {
        if (dp[n][j] != -1) {
            if (dp[n][j] > bestSavings || (dp[n][j] == bestSavings && count[n][j] < minCount)) {
                bestSavings = dp[n][j];
                minCount = count[n][j];
            }
        }
    }

    cout << "\nTotal Savings Achieved: " << bestSavings
         << " using " << minCount << " expense entries.\n";
}

double optimizeSavings(int &goal) {
    vector<int> nonEssentialExpenses;

    for (const auto &month : expenseData) {
        for (const auto &day : month) {
            const vector<int> &nonEssential = day.second;
            for (int expense : nonEssential) {
                if (expense != 0) {
                    nonEssentialExpenses.push_back(expense);
                }
            }
        }
    }

    cout << "\nExtracted Non-Essential Expenses:\n";
    for (int e : nonEssentialExpenses) {
        cout << e << " ";
    }
    cout << "\n";

    optimizeSavingsPlan(nonEssentialExpenses, goal);
    menu();
    return 0;
}



vector<PaymentResult> optimizeCreditCardPayments(
    const vector<vector<pair<vector<int>, vector<int>>>>& expenseData,
    const vector<vector<pair<vector<int>, vector<int>>>>& cardid,
    vector<CreditCard>& cardVec,
    int availableFunds
) {
    vector<int> totalDue(4, 0); // index 1->A, 2->B, 3->C

    for (int month = 0; month < 12; ++month) {
        for (int day = 0; day < 31; ++day) {
            auto expenses = expenseData[month][day];
            auto cards = cardid[month][day];

            for (int i = 0; i < 3; ++i) {
                int cardIdx = cards.first[i];
                if (cardIdx >= 1 && cardIdx <= 3) {
                    totalDue[cardIdx] += expenses.first[i];
                }
            }
            for (int i = 0; i < 3; ++i) {
                int cardIdx = cards.second[i];
                if (cardIdx >= 1 && cardIdx <= 3) {
                    totalDue[cardIdx] += expenses.second[i];
                }
            }
        }
    }

    // Step 2: Pay minimum dues first
    vector<int> amountPaid(4, 0);
    for (int i = 1; i <= 3; ++i) {
        int minDue = cardVec[i].minDue;
        int pay = min(minDue, totalDue[i]);
        if (availableFunds >= pay) {
            amountPaid[i] += pay;
            availableFunds -= pay;
        }
    }

    // Step 3: Pay remaining dues by highest interest first
    vector<pair<int, double>> sortedCards;
    for (int i = 1; i <= 3; ++i) {
        int unpaid = totalDue[i] - amountPaid[i];
        if (unpaid > 0) {
            sortedCards.push_back(make_pair(i, cardVec[i].interestRate));
        }
    }

    sort(sortedCards.begin(), sortedCards.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
        return a.second > b.second;
    });

    for (size_t i = 0; i < sortedCards.size(); ++i) {
        int idx = sortedCards[i].first;
        double rate = sortedCards[i].second;

        int unpaid = totalDue[idx] - amountPaid[idx];
        int pay = min(unpaid, availableFunds);
        amountPaid[idx] += pay;
        availableFunds -= pay;
        if (availableFunds == 0) break;
    }

    // Step 4: Prepare results
    vector<PaymentResult> results;
    for (int i = 1; i <= 3; ++i) {
        int unpaid = totalDue[i] - amountPaid[i];
        double interest = unpaid * cardVec[i].interestRate / 100.0;
        results.push_back({
            cardVec[i].name,
            totalDue[i],
            amountPaid[i],
            unpaid,
            interest
        });
    }

    // Sort output by interest
    sort(results.begin(), results.end(), [&](const PaymentResult& a, const PaymentResult& b) {
        int ai = a.card[0] - 'A' + 1;
        int bi = b.card[0] - 'A' + 1;
        return cardVec[ai].interestRate > cardVec[bi].interestRate;
    });

    return results;
}

void displayResults(const vector<PaymentResult>& results) {
    for (const auto& r : results) {
        cout << "Card: " << r.card
             << " | Total Due: " << r.totalDue
             << " | Paid: " << r.amountPaid
             << " | Unpaid: " << r.unpaidAmount
             << " | Interest Incurred: " << r.interest << endl;
    }
}


int findMinTravelCost(
    const vector<City>& cities,
    const vector<vector<ERoute>>& graph,
    int source,
    int destination,
    int numPeople,
    int numDays,
    vector<int>& parent
) {
    int n = cities.size();
    vector<int> minCost(n, numeric_limits<int>::max());
    priority_queue<CNode, vector<CNode>, greater<CNode>> pq;
    parent.assign(n, -1);

    // Initial cost is hotel at source
    minCost[source] = cities[source].hotelCostPerNight * numDays * numPeople;
    pq.push({source, minCost[source]});

    while (!pq.empty()) {
        CNode current = pq.top();
        pq.pop();

        for (const ERoute& edge : graph[current.city]) {
            int newCost = current.cost +
                edge.flightCost * numPeople + // flight for all
                cities[edge.destination].hotelCostPerNight * numDays * numPeople;

            if (newCost < minCost[edge.destination]) {
                minCost[edge.destination] = newCost;
                parent[edge.destination] = current.city;
                pq.push({edge.destination, newCost});
            }
        }
    }

    return minCost[destination];
}

void printPath(const vector<int>& parent, int source, int destination, const vector<City>& cities) {
    vector<string> path;
    int current = destination;

    while (current != -1) {
        path.push_back(cities[current].name);
        if (current == source) break;
        current = parent[current];
    }

    if (path.back() != cities[source].name) {
        cout << "No path exists from " << cities[source].name << " to " << cities[destination].name << ".\n";
        return;
    }

    reverse(path.begin(), path.end());

    cout << "Cheapest path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        cout << path[i];
        if (i != path.size() - 1) cout << " -> ";
    }
    cout << endl;
}

void travelExpenseMinimizer() {
    int numCities, numFlights;
    cout << "Enter number of cities: ";
    cin >> numCities;

    vector<City> cities(numCities);
    unordered_map<string, int> cityIndex;
    string cityName;
    int hotelCost;

    cout << "Enter city names and hotel costs per night:\n";
    for (int i = 0; i < numCities; ++i) {
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

    for (int i = 0; i < numFlights; ++i) {
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

void initializeDisjointSet(int n) {
    for (int i = 0; i < n; ++i) {
        parent[i] = i;
        rankArr[i] = 0;
    }
}

int findParent(int x) {
    if (parent[x] != x)
        parent[x] = findParent(parent[x]);
    return parent[x];
}

void unionSets(int x, int y) {
    int rootX = findParent(x);
    int rootY = findParent(y);
    if (rootX != rootY) {
        if (rankArr[rootX] > rankArr[rootY]) {
            parent[rootY] = rootX;
        } else if (rankArr[rootX] < rankArr[rootY]) {
            parent[rootX] = rootY;
        } else {
            parent[rootY] = rootX;
            rankArr[rootX]++;
        }
    }
}
// Bubble sort for edges by weight
void sortEdges(Edge edges[], int edgeCount) {
    for (int i = 0; i < edgeCount - 1; ++i) {
        for (int j = 0; j < edgeCount - i - 1; ++j) {
            if (edges[j].weight > edges[j + 1].weight) {
                Edge temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
}

void allocateEmergencyFunds() {
    Edge edges[MAX_EDGES];
    int edgeCount = 0;
    int nodeId = 0;

    for (int month = 0; month < 12; ++month) {
        for (int day = 0; day < 31; ++day) {
            int currentNode = nodeId++;
            int nextNode = currentNode + 1;
            if (nextNode >= MAX_NODES) continue;

            int nextMonth = nextNode / 31;
            int nextDay = nextNode % 31;

            if (nextMonth >= 12 || nextDay >= 31) continue;

            double total1 = 0, total2 = 0;
            for (int i = 0; i < 3; ++i) {
                total1 += expenseData[month][day].first[i] + expenseData[month][day].second[i];
                total2 += expenseData[nextMonth][nextDay].first[i] + expenseData[nextMonth][nextDay].second[i];
            }

            double diff = (total1 > total2) ? (total1 - total2) : (total2 - total1);
            edges[edgeCount].src = currentNode;
            edges[edgeCount].dest = nextNode;
            edges[edgeCount].weight = diff;
            edgeCount++;
        }
    }

    sortEdges(edges, edgeCount);
    initializeDisjointSet(MAX_NODES);

    double totalCost = 0;
    cout << "\nEmergency Fund Transfer:\n";
    for (int i = 0; i < edgeCount; ++i) {
        int u = edges[i].src;
        int v = edges[i].dest;
        if (findParent(u) != findParent(v)) {
            unionSets(u, v);
            totalCost += edges[i].weight;
            cout << "Transfer from " << u << " to " << v << " with cost " << edges[i].weight << "\n";
        }
    }

    cout << "Total Minimum Transfer Cost: " << totalCost << "\n";
}

void buildHuffmanTree(const string &data, unordered_map<char, string> &huffmanCode) {
    unordered_map<char, int> freq;
    for (char ch : data) freq[ch]++;
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto &pair : freq) pq.push(new HuffmanNode(pair.first, pair.second));
    while (pq.size() > 1) {
        HuffmanNode *left = pq.top(); pq.pop();
        HuffmanNode *right = pq.top(); pq.pop();
        HuffmanNode *node = new HuffmanNode('\0', left->freq + right->freq);
        node->left = left; node->right = right;
        pq.push(node);
    }
    function<void(HuffmanNode*, string)> encode = [&](HuffmanNode* node, string str) {
        if (!node) return;
        if (node->data != '\0') huffmanCode[node->data] = str;
        encode(node->left, str + "0");
        encode(node->right, str + "1");
    };
    encode(pq.top(), "");
}

string compressData(const string &data, unordered_map<char, string> &huffmanCode) {
    string compressed = "";
    for (char ch : data) compressed += huffmanCode[ch];
    return compressed;
}

string decompressData(const string &compressed, unordered_map<char, string> &huffmanCode) {
    unordered_map<string, char> reverseCode;
    for (auto &pair : huffmanCode) reverseCode[pair.second] = pair.first;
    string temp = "", decompressed = "";
    for (char bit : compressed) {
        temp += bit;
        if (reverseCode.count(temp)) {
            decompressed += reverseCode[temp];
            temp = "";
        }
    }
    return decompressed;
}

void updateExpenseData() {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    string data = buffer.str();
    file.close();
    unordered_map<char, string> huffmanCode;
    buildHuffmanTree(data, huffmanCode);
    string compressed = compressData(data, huffmanCode);
    ofstream outFile(filename, ios::trunc);
    outFile << compressed;
    outFile.close();
    cout << "Data compressed and updated successfully!\n";
}

void restoreExpenseData() {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    string data = buffer.str();
    file.close();
    unordered_map<char, string> huffmanCode;
    buildHuffmanTree(data, huffmanCode);
    string decompressed = decompressData(data, huffmanCode);
    ofstream outFile(filename, ios::trunc);
    outFile << decompressed;
    outFile.close();
    cout << "Data decompressed and restored successfully!\n";
}

void displayGraph(const vector<Edge>& edges) {
    cout << "Graph Representation:\n";
    for (const auto& edge : edges) {
        cout << "Account " << edge.src << " -(" << edge.weight << ")-> Account " << edge.dest << "\n";
    }
}

void loadExpenseData(map<string, double>& expenses) {
    ifstream file(filename);
    if (!file) {
        cout << "No previous expense data found." << endl;
        return;
    }
    string category;
    double amount;
    while (file >> category >> amount) {
        expenses[category] += amount;
    }
    file.close();
}

void saveExpenseData(const map<string, double>& expenses) {
    ofstream file(filename);
    for (const auto& entry : expenses) {
        file << entry.first << " " << entry.second << endl;
    }
    file.close();
}

void listAllExpenses(const map<string, double>& expenses) {
    cout << "All Expenses:\n";
    for (const auto& entry : expenses) {
        cout << entry.first << ": " << entry.second << "\n";
    }
}
void menu() {
    int choice;
    do {
        cout << "\nExpense Tracker Menu:";
        cout << "\n1. Display Expenses";
        cout << "\n2. Add Expense";
        cout << "\n3. Update Expense";
        cout << "\n4. Delete Expense";
        cout << "\n5. Allocate Emergency Funds";
        cout << "\n6. Compress Data";
        cout << "\n7. Decompress Data";
        cout << "\n8. Optimize Savings";
        cout << "\n9. Credit Card Payment Strategy";
        cout << "\n10. Best Flight optimization";
        cout << "\n11. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;
        string filename1 = "filename.csv";
        string filename2 ="carddetails.csv";
        string date;
        vector<CreditCard> cardVec(4);
        cardVec[1] = {"A", 3.5, 500, 15};
        cardVec[2] = {"B", 2.0, 300, 12};
        cardVec[3] = {"C", 1.5, 200, 18};
        vector<PaymentResult> payVec;

        switch (choice) {
        case 1:
            displayExpenses();
            break;
        case 2:
            addExpense();
            break;
        case 3:
            updateExpense();
            break;
        case 4:
            cout << "Enter the date (YYYY-MM-DD) to delete all expenses of that day: ";
            cin >> date;
       
            if(deleteExpenses(filename1,date) || deleteExpenses(filename2,date)){
                cout << "Expenses for date " << date << " have been deleted successfully.\n";
                
            }
            else{
                cout << "No expenses found for the entered date.\n";
              
            }
           
            break;
        case 5:
            allocateEmergencyFunds();
            break;
        case 6:
            updateExpenseData();
            break;
        case 7:
            restoreExpenseData();
            break;
        case 8:

            int goal;
            cout<<" Enter your goal that is to be achived by reducing minimum  number of expenses";
            cin>>goal;
           
           
            optimizeSavings(goal);
            
            break;
        case 9:
           
            cout<<"Enter your available funds"<<endl;
            int funds;
            cin>>funds;
            payVec= optimizeCreditCardPayments(expenseData,cardid,cardVec,funds);
            displayResults(payVec);

            break;
        case 10:
            travelExpenseMinimizer();  // Call our feature here
            break;
            
        case 11:
            cout << "Exiting program...\n";
            break;
            
        default:
            cout << "Invalid choice! Please enter a valid option.\n";
        }
    } while (choice != 11);
}

// Main function
int main()

{   
    string filename1 = "filename.csv";
    string filename2 ="carddetails.csv";
    if (!isValidFile(filename1))
    {
        cerr << "Error: File not found or inaccessible." << endl;
        return 1;
    }
    if (!isValidFile(filename2))
    {
        cerr << "Error: File not found or inaccessible." << endl;
        return 1;
    }
    // Load existing expenses from CSV file
    parseCSV(filename1,expenseData);
    parseCSV(filename2,cardid);
    menu();
    return 0;
}