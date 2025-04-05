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

// Struct to store expense data
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


struct Edge {
    int src, dest;
    double weight;
};

class DisjointSet {
public:
    unordered_map<int, int> parent, rank;
    void makeSet(int n) {
        for (int i = 0; i < n; i++) {
            parent[i] = i;
            rank[i] = 0;
        }
    }
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            } else if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }
};

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



#endif


