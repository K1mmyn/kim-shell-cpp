#pragma once

#include <string>

class TrieNode {
    public: 
        bool isWord;
        TrieNode* children[26];
};

class Trie {
    private:
        TrieNode* root;
    public:
        void insert(std::string word);
        bool search(std::string word);
        bool startsWith(std::string word);
};