#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>

using namespace std;

// Caesar Cipher Encryption
string encryptCaesar(string text, int shift) {
    for (char &c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            c = (c - base + shift) % 26 + base;
        }
    }
    return text;
}

// Caesar Cipher Decryption
string decryptCaesar(string text, int shift) {
    return encryptCaesar(text, 26 - shift);
}

// Huffman Tree Node
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode *left, *right;
    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// Build Huffman Tree
HuffmanNode* buildHuffmanTree(unordered_map<char, int> freq) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto &p : freq) pq.push(new HuffmanNode(p.first, p.second));
    while (pq.size() > 1) {
        HuffmanNode *left = pq.top(); pq.pop();
        HuffmanNode *right = pq.top(); pq.pop();
        HuffmanNode *node = new HuffmanNode('\0', left->freq + right->freq);
        node->left = left;
        node->right = right;
        pq.push(node);
    }
    return pq.top();
}

// Generate Huffman Codes
void generateHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;
    if (root->ch != '\0') huffmanCode[root->ch] = code;
    generateHuffmanCodes(root->left, code + "0", huffmanCode);
    generateHuffmanCodes(root->right, code + "1", huffmanCode);
}

// Huffman Encoding
string encodeHuffman(string text, unordered_map<char, string>& huffmanCode) {
    string encoded = "";
    for (char c : text) encoded += huffmanCode[c];
    return encoded;
}

// Decompress Huffman
string decodeHuffman(string encoded, HuffmanNode* root) {
    string decoded = "";
    HuffmanNode* curr = root;
    for (char bit : encoded) {
        curr = (bit == '0') ? curr->left : curr->right;
        if (curr->ch != '\0') {
            decoded += curr->ch;
            curr = root;
        }
    }
    return decoded;
}

int main() {
    ifstream input("filename.csv");
    ofstream encryptedFile("encrypted.txt");
    ofstream compressedFile("compressed.txt");
    string data, line;
    
    // Read the CSV file
    while (getline(input, line)) data += line + "\n";
    
    // Encrypt using Caesar Cipher
    string encryptedData = encryptCaesar(data, 3);
    encryptedFile << encryptedData;
    encryptedFile.close();

    // Huffman Compression
    unordered_map<char, int> freq;
    for (char c : encryptedData) freq[c]++;
    HuffmanNode* root = buildHuffmanTree(freq);
    unordered_map<char, string> huffmanCode;
    generateHuffmanCodes(root, "", huffmanCode);
    string compressedData = encodeHuffman(encryptedData, huffmanCode);
    compressedFile << compressedData;
    compressedFile.close();
    
    // Decompress Huffman
    string decompressedData = decodeHuffman(compressedData, root);
    
    // Decrypt using Caesar Cipher
    string decryptedData = decryptCaesar(decompressedData, 3);
    
    // Output the final data
    cout << "Original Data:\n" << data;
    cout << "\nDecrypted Data (After Compression & Decryption):\n" << decryptedData;
    return 0;
}
