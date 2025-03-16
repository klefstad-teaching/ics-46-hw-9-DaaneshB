#include "ladder.h"
#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <chrono>

using namespace std;

// Utility function to convert string to lowercase
string to_lower(string word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    return word;
}

// Load words from a file into a set
void load_words(set<string>& dictionary, const string& filename) {
    ifstream file(filename);
    if (!file) {
        throw runtime_error("Cannot open dictionary file: " + filename);
    }

    dictionary.clear();
    string word;
    while (file >> word) {
        dictionary.insert(to_lower(word));
    }
}

// Check if two words are adjacent (can be transformed with one edit)
bool is_adjacent(const string& w1, const string& w2) {
    // If lengths differ by more than 1, they can't be adjacent
    if (abs((int)w1.length() - (int)w2.length()) > 1) 
        return false;

    // Ensure w1 is the shorter or equal length word
    const string& shorter = (w1.length() <= w2.length()) ? w1 : w2;
    const string& longer = (w1.length() <= w2.length()) ? w2 : w1;

    int edits = 0;
    size_t i = 0, j = 0;

    while (i < shorter.length() && j < longer.length()) {
        if (shorter[i] == longer[j]) {
            // Characters match, move both indices
            i++;
            j++;
        } else {
            // Mismatch found
            edits++;
            
            if (edits > 1) 
                return false;

            // If lengths are same, move both indices (substitution)
            if (shorter.length() == longer.length()) {
                i++;
                j++;
            } else {
                // Lengths differ, move only longer word index (insertion/deletion)
                j++;
            }
        }
    }

    // Account for any remaining characters
    if (i < shorter.length() || j < longer.length()) 
        edits++;

    return (edits == 1);
}

// Generate word ladder using BFS
vector<string> generate_word_ladder(
    const string& begin_word, 
    const string& end_word, 
    const set<string>& dictionary
) {
    // Convert words to lowercase
    string start = to_lower(begin_word);
    string goal = to_lower(end_word);

    // Validate end word is in dictionary
    if (dictionary.find(goal) == dictionary.end()) {
        cout << "End word not in dictionary." << endl;
        return {};
    }

    // Queue to store partial ladders
    queue<vector<string>> ladder_queue;
    
    // Set to track visited words GLOBALLY
    unordered_set<string> global_visited;

    // Set to track words used in current ladder to prevent reuse
    unordered_set<string> current_ladder_words;

    // Initialize with start word
    ladder_queue.push({start});
    global_visited.insert(start);

    // Limit search to prevent infinite loops
    const int MAX_LADDER_LENGTH = 20;
    const int MAX_SEARCH_TIME_SECONDS = 30;
    auto start_time = chrono::steady_clock::now();

    while (!ladder_queue.empty()) {
        // Check for timeout
        auto current_time = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(current_time - start_time);
        if (elapsed.count() > MAX_SEARCH_TIME_SECONDS) {
            cout << "Search timed out after " << MAX_SEARCH_TIME_SECONDS << " seconds." << endl;
            return {};
        }

        // Get current ladder
        vector<string> current_ladder = ladder_queue.front();
        ladder_queue.pop();

        // Prevent excessively long ladders
        if (current_ladder.size() > MAX_LADDER_LENGTH) {
            continue;
        }

        // Get last word in current ladder
        string last_word = current_ladder.back();

        // Reset current ladder words
        current_ladder_words.clear();
        for (const string& word : current_ladder) {
            current_ladder_words.insert(word);
        }

        // Try every word in dictionary
        for (const string& candidate : dictionary) {
            // Skip if globally visited or in current ladder
            if (global_visited.count(candidate) || 
                current_ladder_words.count(candidate)) continue;

            // Check if candidate is adjacent to last word
            if (is_adjacent(last_word, candidate)) {
                // Create new ladder
                vector<string> new_ladder = current_ladder;
                new_ladder.push_back(candidate);

                // Mark as globally visited
                global_visited.insert(candidate);

                // Check if goal reached
                if (candidate == goal) {
                    return new_ladder;
                }

                // Enqueue new ladder
                ladder_queue.push(new_ladder);
            }
        }
    }

    // No ladder found
    cout << "No ladder found between " << begin_word << " and " << end_word << endl;
    return {};
}

// Print word ladder
void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No ladder found." << endl;
        return;
    }

    cout << "Word Ladder:" << endl;
    for (const string& word : ladder) {
        cout << word << " ";
    }
    cout << endl;
}