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
#include <regex>  // For date validation
#include <set>  // For unique categories
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stdexcept>

using namespace std;
struct City {
    string name;
    int hotelCostPerNight;
};

struct ERoute {
    int destination;
    int flightCost;
};

struct CNode {
    int city;
    int cost;
    bool operator>(const CNode& other) const {
        return cost > other.cost;
    }
};
// Struct to store expense data
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
        double interestRate; // percentage per billing cycle
        int minDue;          // minimum amount to be paid
        int dueDate;         // day of the month payment is due
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

vector<vector<pair<vector<int>, vector<int>>>> expenseData(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<vector<pair<vector<int>, vector<int>>>> cardid(12, vector<pair<vector<int>, vector<int>>>(31, {{0, 0, 0}, {0, 0, 0}}));
vector<string> essentialCategories = {"food", "work", "travel"};
vector<string> nonEssentialCategories = {"snacks", "fun", "extra"};

// Store expense data: date → category → list of expenses
// map<string, map<string, vector<Expense>>> expenseData;
const string filename = "filename.csv";
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


#define MAX_NODES 372     // 12 months × 31 days
#define MAX_EDGES 400     // approximate maximum number of edges

struct Edge {
    int src;
    int dest;
    double weight;
};

int parent[MAX_NODES];
int rankArr[MAX_NODES];

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

// Caesar Cipher encryption and decryption
void encrypt(const string& inputFilename, const string& outputFilename, int key);
void decrypt(const string& inputFilename, const string& outputFilename, int key);


#endif



