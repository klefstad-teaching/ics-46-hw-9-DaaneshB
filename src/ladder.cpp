#include "ladder.h"
#include <algorithm>
#include <cctype>
#include <queue>
#include <set>
#include <string>
#include <vector>

using namespace std;

// Convert string to lowercase
string to_lower(const string& word) {
    string lower_word = word;
    transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);
    return lower_word;
}

// Error handling function
void error(string word1, string word2, string msg) {
    cerr << "Error: " << msg << " (" << word1 << ", " << word2 << ")" << endl;
}

// Check edit distance between two words
bool edit_distance_within(const string& str1, const string& str2, int d) {
    string lower_str1 = to_lower(str1);
    string lower_str2 = to_lower(str2);
    
    int len1 = lower_str1.length();
    int len2 = lower_str2.length();
    
    // If lengths differ by more than allowed edits, return false
    if (abs(len1 - len2) > d) return false;

    // If lengths are the same, count character differences
    if (len1 == len2) {
        int diff = 0;
        for (int i = 0; i < len1; ++i) {
            if (lower_str1[i] != lower_str2[i]) diff++;
        }
        return diff <= d;
    }

    // If lengths differ by 1, check for insertion/deletion
    const string& shorter = (len1 < len2) ? lower_str1 : lower_str2;
    const string& longer = (len1 < len2) ? lower_str2 : lower_str1;
    
    int i = 0, j = 0, diff = 0;
    while (i < shorter.length() && j < longer.length()) {
        if (shorter[i] != longer[j]) {
            diff++;
            j++;
        } else {
            i++;
            j++;
        }
        
        if (diff > d) return false;
    }
    
    return true;
}

// Check if two words are adjacent
bool is_adjacent(const string& word1, const string& word2) {
    return edit_distance_within(word1, word2, 1);
}

// Load words from file into a set
void load_words(set<string>& word_list, const string& file_name) {
    ifstream file(file_name);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << file_name << endl;
        return;
    }
    
    string word;
    while (file >> word) {
        word_list.insert(to_lower(word));
    }

    if (word_list.empty()) {
        cerr << "Warning: No words loaded from " << file_name << endl;
    }
}

// Generate word ladder using Breadth-First Search
vector<string> generate_word_ladder(const string& begin_word, const string& end_word, const set<string>& word_list) {
    // Handle same word case
    if (begin_word == end_word) {
        return {};
    }

    // Convert to lowercase
    string start = to_lower(begin_word);
    string goal = to_lower(end_word);
    
    // Validate goal is in dictionary
    if (word_list.find(goal) == word_list.end()) {
        return {};
    }
    
    // Precompute words of similar length for faster searching
    vector<string> candidate_words;
    for (const auto& word : word_list) {
        // Only consider words within ±1 length of start word
        if (abs((int)word.length() - (int)start.length()) <= 1) {
            candidate_words.push_back(word);
        }
    }
    
    // Queue for BFS
    queue<vector<string>> ladder_queue;
    ladder_queue.push({start});
    
    // Track visited words to prevent cycles
    set<string> visited;
    visited.insert(start);
    
    // Limit ladder length to prevent infinite loops
    const int MAX_LADDER_LENGTH = 10;
    
    while (!ladder_queue.empty()) {
        auto current_ladder = ladder_queue.front();
        ladder_queue.pop();
        
        // Stop if ladder gets too long
        if (current_ladder.size() > MAX_LADDER_LENGTH) {
            continue;
        }
        
        string last_word = current_ladder.back();
        
        // Try candidate words
        for (const auto& word : candidate_words) {
            // Skip visited words and check adjacency
            if (visited.find(word) == visited.end() && is_adjacent(last_word, word)) {
                vector<string> new_ladder = current_ladder;
                new_ladder.push_back(word);
                
                // Goal reached
                if (word == goal) {
                    return new_ladder;
                }
                
                // Mark as visited and continue search
                visited.insert(word);
                ladder_queue.push(new_ladder);
            }
        }
    }
    
    // No ladder found
    return {};
}

// Print word ladder
void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No word ladder found." << endl;
        return;
    }

    cout << "Word ladder found: ";
    for (const auto& word : ladder) {
        cout << word << " ";
    }
    cout << endl;
}

// Verify word ladder functionality
void verify_word_ladder() {
    set<string> word_list;
    load_words(word_list, "words.txt");
    
    // Ensure words are loaded
    if (word_list.empty()) {
        cerr << "No words loaded for verification" << endl;
        return;
    }

    // Test cases
    vector<string> test_cases[] = {
        generate_word_ladder("cat", "dog", word_list),
        generate_word_ladder("code", "data", word_list),
        generate_word_ladder("work", "play", word_list)
    };

    // Verify each test case
    for (const auto& ladder : test_cases) {
        if (ladder.empty()) {
            cerr << "Failed to generate word ladder" << endl;
        } else {
            cout << "Word ladder generated successfully:" << endl;
            print_word_ladder(ladder);
        }
    }
}