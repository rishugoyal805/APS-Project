#include "aps.h"

void allocateEmergencyFunds(vector<Edge> &edges, int n) {
    DisjointSet ds;
    ds.makeSet(n);
    sort(edges.begin(), edges.end(), [](Edge a, Edge b) { return a.weight < b.weight; });
    double totalCost = 0;
    cout << "Allocating Emergency Funds Using Kruskal's Algorithm:\n";
    for (auto &edge : edges) {
        if (ds.find(edge.src) != ds.find(edge.dest)) {
            ds.unite(edge.src, edge.dest);
            totalCost += edge.weight;
            cout << "Transfer from " << edge.src << " to " << edge.dest << " with cost " << edge.weight << "\n";
        }
    }
    cout << "Total Transfer Cost: " << totalCost << "\n";
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




