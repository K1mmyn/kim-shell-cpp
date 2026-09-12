#include <string>
#include <iostream>
#include <cctype>
#include <algorithm> 

class TrieNode{
    public: 
        bool isWord;
        TrieNode* children[26];

        TrieNode() {
            isWord = false;
            for (int i = 0; i < 26; i++) {
                children[i] = nullptr;
            }
        }
};

class Trie {
private: 
    TrieNode* root;
public:
    Trie() { root = new TrieNode(); }
    
    char* lower_word(char* word) {
            char* word_start = word;
            while (*word != '\0') {
                char c{*word};
                c = std::tolower(static_cast<unsigned char>(c));
            }
            return word_start;
        }

        void insert(char* word) {

            word = lower_word(word);
            TrieNode* curr = root;
            while (*word != '\0') {
                char c{*word};
                int index = static_cast<int>(c) - 'a';
                
                if (!curr->children[index]) {
                    curr->children[index] = new TrieNode();
                }

                curr = curr->children[index];
            }

            curr->isWord = true;
        }

        bool search(char* word) {
            word = lower_word(word);

            TrieNode* curr = root;
            while (*word != '\0') {
                char c{*word};
                int index = static_cast<int>(c) - 'a';

                if (!curr->children[index]) {
                    return false;
                }

                curr = curr->children[index];
            }

            return curr->isWord;
        }

        bool startsWith(char* word) {

            word = lower_word(word);
            TrieNode* curr = root;
            while (*word != '\0') {
                char c{*word};

                int index = static_cast<int>(c) - 'a';
                
                if (!curr->children[index]) {
                    return false;
                }

                curr = curr->children[index];
            }

            return true;
        }

       
};
