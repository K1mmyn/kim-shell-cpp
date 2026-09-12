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

char* lower_word(char* word) {
        char* word_start = word;
        while (*word != '\0') {
            char c{*word};
            if (c < 'a') {
                c += 26;
            }
            *word = c;
            word++;
        }
        return word_start;
    }

void Trie::insert(char* word) {

    word = lower_word(word);
    TrieNode* curr = root;
    while (*word != '\0') {
        char c{*word};
        int index = static_cast<int>(c) - 'a';
        
        if (index < 0 || index > 25 || !curr->children[index]) {
            curr->children[index] = new TrieNode();
        }

        curr = curr->children[index];
        word++;
    }

    curr->isWord = true;
}

bool Trie::search(char* word) {
    word = lower_word(word);

    TrieNode* curr = root;
    while (*word != '\0') {
        char c{*word};
        int index = static_cast<int>(c) - 'a';

        if (index < 0 || index > 25 || !curr->children[index]) {
            return false;
        }

        curr = curr->children[index];
        word++;
    }

    return curr->isWord;
}

bool Trie::startsWith(char* word) {

    word = lower_word(word);
    TrieNode* curr = root;
    while (*word != '\0') {
        char c{*word};

        int index = static_cast<int>(c) - 'a';

        if (index < 0 || index > 25 || !curr->children[index]) {
            return false;
        }

        curr = curr->children[index];
        word++;
    }

    return true;
}
