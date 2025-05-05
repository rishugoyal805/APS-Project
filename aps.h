#ifndef FILEIO_H
#define FILEIO_H

// ========================== Standard Library Includes ==========================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <regex>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <limits>
#include <windows.h> // For Sleep (Windows-specific)

using namespace std;

// ========================== Class Declarations ==========================
// -- Expense & Travel --
class City {
public:
    string name;
    int hotelCostPerNight;
};

class ERoute {
public:
    int destination;
    int flightCost;
};

class CNode {
public:
    int city;
    int cost;
    bool operator>(const CNode &other) const {
        return cost > other.cost;
    }
};

// -- Financial Planning --
class TravelOption {
public:
    string airline;
    int flightCostPerPerson;
    string hotel;
    int hotelCostPerDay;
};

class CreditCard {
public:
    string name;
    double interestRate;
    int minDue;
    int dueDate;
};

class PaymentResult {
public:
    string card;
    int totalDue;
    int amountPaid;
    int unpaidAmount;
    double interest;
};

class Expense {
public:
    string category;
    double amount;
};

class RentBuyInput {
public:
    double income;
    double rentCost;
    double emi;
    double propertyCost;
    int years;
};

class RentBuyResult {
public:
    double totalRentCost;
    double totalBuyCost;
    string recommendation;
    int yearsToOwnProperty;
};

class Product {
public:
    int size;
    int profit;
    int stock;
};

class InventoryResult {
public:
    int totalProfit;
    vector<int> selectedProductIndices;
};

class RecurringBill {
public:
    string name;
    int amount;
    int dueDate;
    int penalty;
};

class SchedulerResult {
public:
    vector<string> paymentSchedule;
    double totalPenaltyPaid=0;
};

// -- Loans --
class Loan {
public:
    int id;
    double amount;
    double interestRate;

    Loan(int id, double amount, double interestRate)
        : id(id), amount(amount), interestRate(interestRate) {}
};

class LoanRepaymentResult {
public:
    int id;
    double originalAmount;
    double amountPaid;
    double unpaidAmount;
    double interestIncurred;
};

// -- Investment --
class Investment {
public:
    int id;
    double riskPerUnit;
    double returnPerUnit;
    int maxUnits;

    Investment(int id, double risk, double ret, int maxUnits)
        : id(id), riskPerUnit(risk), returnPerUnit(ret), maxUnits(maxUnits) {}
};

class InvestmentSelection {
public:
    int id;
    int unitsSelected;
    int totalReturn;
};

// ========================== Global Variables ==========================
vector<vector<pair<vector<int>, vector<int>>>> expenseData(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<vector<pair<vector<int>, vector<int>>>> cardid(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<string> essentialCategories = {"food", "work", "travel"};
vector<string> nonEssentialCategories = {"snacks", "fun", "extra"};
const string filename = "OctExpenses.csv";

// ========================== Graph Constants ==========================
#define MAX_NODES 372
#define MAX_EDGES 400

struct Edge {
    int src;
    int dest;
    double weight;
};

int parent[MAX_NODES];
int rankArr[MAX_NODES];

// ========================== Huffman Encoding ==========================
struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode *l, HuffmanNode *r) {
        return l->freq > r->freq;
    }
};

// ========================== Function Declarations ==========================

// --- Validation ---
bool isValidFile(const string &filename);
bool isValidCSV(const string &line);
bool isValidDoubleInput(double &input);
bool isValidIntInput(int &input);
bool isValidDate(int year, int month, int day);
bool validateDateFormat(const string &date);

// --- File I/O ---
void parseCSV(const string &filename, vector<vector<pair<vector<int>, vector<int>>>> &vec, vector<int> &monthlyTotals);
void loadExpenseData(map<string, double> &expenses);
void saveExpenseData(const map<string, double> &expenses);
void updateExpenseData();
void restoreExpenseData();

// --- Expense Management ---
void displayExpenses();
void addExpense();
void updateExpense();
bool deleteExpenses(string &filename, string &date);
void listAllExpenses(const map<string, double> &expenses);
void menu(vector<int> &monthlyTotals);

// --- Fraud Detection ---
void detectFraudulentTransactions();

// --- Budget & Optimization ---
void optimizeSavingsPlan(vector<tuple<int, int, string>> &nonEssentialExpensesWithDates, int &goal);
double optimizeSavings(int &goal);
vector<PaymentResult> optimizeCreditCardPayments(
    const vector<vector<pair<vector<int>, vector<int>>>> &expenseData,
    const vector<vector<pair<vector<int>, vector<int>>>> &cardid,
    vector<CreditCard> &cardVec,
    int availableFunds);
void displayResults(const vector<PaymentResult> &results);

// --- Travel ---
int findMinTravelCost(
    const vector<City> &cities,
    const vector<vector<ERoute>> &graph,
    int source,
    int destination,
    int numPeople,
    int numDays,
    vector<int> &parent);
void printPath(const vector<int> &parent, int source, int destination, const vector<City> &cities);
void travelExpenseMinimizer();

// --- Emergency Fund ---
void allocateEmergencyFunds();

// --- Graph & MST ---
void initializeDisjointSet(int n);
int findParent(int x);
void unionSets(int x, int y);
void sortEdges(Edge edges[], int edgeCount);
void displayGraph(const Edge edges[], int count);

// --- Huffman Compression ---
void buildHuffmanTree(const string &data, unordered_map<char, string> &huffmanCode);
string compressData(const string &data, unordered_map<char, string> &huffmanCode);
string decompressData(const string &compressed, unordered_map<char, string> &huffmanCode);

// --- Encryption ---
void encrypt(const string &inputFilename, const string &outputFilename, int key);
void decrypt(const string &inputFilename, const string &outputFilename, int key);

// --- Loan Optimization ---
vector<LoanRepaymentResult> optimizeLoanRepayment(
    const vector<vector<pair<vector<int>, vector<int>>>> &expenseData,
    int income,
    int month,
    vector<Loan> &loans);
void displayLoanResults(const vector<LoanRepaymentResult> &results);

// --- Investment Portfolio ---
void optimizeInvestmentPortfolio(int totalRiskBudget);

// --- Budget Planning ---
void generateBudgetPlan(double monthlyIncome);

// --- Rent vs Buy ---
RentBuyResult rentVsBuyDecision(const RentBuyInput &input, double interestRate, double rentIncreaseRate);
void runRentVsBuySimulator(vector<int> &monthlyTotals);

// --- Inventory Optimization ---
InventoryResult optimizeInventory(const vector<Product> &products, int capacity);
void runInventoryOptimizer();

// --- Recurring Expenses Scheduler ---
SchedulerResult scheduleRecurringExpenses(const vector<RecurringBill> &bills, int income);
void runRecurringExpenseScheduler(vector<int> &monthlyTotals);

// --- Misc UI ---
void displayHeader();

#endif // FILEIO_H