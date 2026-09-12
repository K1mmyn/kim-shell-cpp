#include <iostream>
#include <cctype>
#include <algorithm> 
#include "trie.h"

TrieNode::TrieNode() {
    isWord = false;
    for (int i = 0; i < 26; i++) {
        children[i] = nullptr;
    }
}

Trie::Trie() { root = new TrieNode(); }

void Trie::insert(char* word) {

    TrieNode* curr = root;
    while (*word != '\0') {
        int c = std::tolower(static_cast<unsigned char>(*word));
        int index = c - 'a';
        
        if (index < 0 || index > 25) {
            return;
        }
            
        if (!curr->children[index]) {
            curr->children[index] = new TrieNode();
        }

        curr = curr->children[index];
        word++;
    }

    curr->isWord = true;
}

bool Trie::search(char* word) {

    TrieNode* curr = root;
    while (*word != '\0') {
        int c = std::tolower(static_cast<unsigned char>(*word));
        int index = c - 'a';

        if (index < 0 || index > 25 || !curr->children[index]) {
            return false;
        }

        curr = curr->children[index];
        word++;
    }

    return curr->isWord;
}

bool Trie::startsWith(char* word) {

    TrieNode* curr = root;
    while (*word != '\0') {
        int c = std::tolower(static_cast<unsigned char>(*word));
        int index = c - 'a';

        if (index < 0 || index > 25 || !curr->children[index]) {
            return false;
        }

        curr = curr->children[index];
        word++;
    }

    return true;
}
