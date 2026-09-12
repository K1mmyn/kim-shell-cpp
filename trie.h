#pragma once

class TrieNode {
    public: 
        bool isWord;
        TrieNode* children[26];

        TrieNode();
};

class Trie {
    private:
        TrieNode* root;
    public:
        Trie();
        void insert(char* word);
        bool search(char* word);
        bool startsWith(char* word);
};