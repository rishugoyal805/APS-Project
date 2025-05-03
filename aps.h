#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <algorithm>
#include <iomanip> // Include for formatting
#include <regex>   // For date validation
#include <set>     // For unique categories
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stdexcept>
#include <windows.h> // For Sleep function
#include <thread>    // For sleep_for function
#include <chrono>    // For sleep_for function
#include <limits>

using namespace std;
class City
{
public:
    string name;
    int hotelCostPerNight;
};

class ERoute
{
public:
    int destination;
    int flightCost;
};

class CNode
{
public:
    int city;
    int cost;
    bool operator>(const CNode &other) const
    {
        return cost > other.cost;
    }
};
// Struct to store expense data
class TravelOption
{
public:
    string airline;
    int flightCostPerPerson;
    string hotel;
    int hotelCostPerDay;
};
class CreditCard
{
public:
    string name;
    double interestRate; // percentage per billing cycle
    int minDue;          // minimum amount to be paid
    int dueDate;         // day of the month payment is due
};

class PaymentResult
{
public:
    string card;
    int totalDue;
    int amountPaid;
    int unpaidAmount;
    double interest;
};
class Expense
{
public:
    string category;
    double amount;
};

// For Rent vs Buy Simulator
class RentBuyInput
{
public:
    double income;
    double rentCost;
    double emi;
    double propertyCost;
    int years;
};

class RentBuyResult
{
public:
    double totalRentCost;
    double totalBuyCost;
    string recommendation; // "Rent" or "Buy"
};

// For Inventory Optimization
class Product
{
public:
    int size;
    int profit;
    int stock;
};

class InventoryResult
{
public:
    int totalProfit;
    vector<int> selectedProductIndices;
};

// For Recurring Expense Scheduler
class RecurringBill
{
public:
    string name;
    int amount;
    int dueDate; // 1 to 31
    int penalty; // penalty for late payment
};

class SchedulerResult
{
public:
    vector<string> paymentSchedule;
    double totalPenaltyPaid;
};

vector<vector<pair<vector<int>, vector<int>>>> expenseData(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<vector<pair<vector<int>, vector<int>>>> cardid(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<string> essentialCategories = {"food", "work", "travel"};
vector<string> nonEssentialCategories = {"snacks", "fun", "extra"};

// Store expense data: date → category → list of expenses
// map<string, map<string, vector<Expense>>> expenseData;
const string filename = "OctExpenses.csv";
// Function of operation to be performed on the file
bool isValidFile(const string &filename);
bool isValidCSV(const string &line);
void parseCSV(const string &filename);
void displayExpenses();
void addExpense();
void deleteExpense();
void updateExpense();
void saveToCSV(const string &filename);
bool validateDateFormat(const string &date);
// Function of algorithm
void detectFraudulentTransactions();

#define MAX_NODES 372 // 12 months × 31 days
#define MAX_EDGES 400 // approximate maximum number of edges

struct Edge
{
    int src;
    int dest;
    double weight;
};

int parent[MAX_NODES];
int rankArr[MAX_NODES];

struct HuffmanNode
{
    char data;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare
{
    bool operator()(HuffmanNode *l, HuffmanNode *r)
    {
        return l->freq > r->freq;
    }
};

// swayam did this
class Loan
{
public:
    int id;
    double amount;
    double interestRate;

    Loan(int id, double amount, double interestRate)
        : id(id), amount(amount), interestRate(interestRate) {}
};

class LoanRepaymentResult
{
public:
    int id;
    double originalAmount;
    double amountPaid;
    double unpaidAmount;
    double interestIncurred;
};
// swayam did this
class Investment
{
public:
    int id;
    double riskPerUnit;
    double returnPerUnit;
    int maxUnits;

    Investment(int id, double risk, double ret, int maxUnits)
        : id(id), riskPerUnit(risk), returnPerUnit(ret), maxUnits(maxUnits) {}
};

class InvestmentSelection
{
public:
    int id;
    int unitsSelected;
    int totalReturn;
};
void generateBudgetPlan(double monthlyIncome);
void optimizeInvestmentPortfolio(int totalRiskBudget);
void displayLoanResults(const vector<LoanRepaymentResult> &results);
vector<LoanRepaymentResult> optimizeLoanRepayment(
    const vector<vector<pair<vector<int>, vector<int>>>> &expenseData,
    int income,
    int month,
    vector<Loan> &loans);

// Caesar Cipher encryption and decryption
void encrypt(const string &inputFilename, const string &outputFilename, int key);
void decrypt(const string &inputFilename, const string &outputFilename, int key);

RentBuyResult rentVsBuyDecision(const RentBuyInput &input);
InventoryResult optimizeInventory(const vector<Product> &products, int capacity);
SchedulerResult scheduleRecurringExpenses(const vector<RecurringBill> &bills, int income);

#endif