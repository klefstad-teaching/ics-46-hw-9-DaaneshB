#include "ladder.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <limits>

using namespace std;

// Utility function to convert string to lowercase
string to_lower(string word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    return word;
}

// Error handling function
void error(string word1, string word2, string msg) {
    cerr << "Error with words '" << word1 << "' and '" << word2 << "': " << msg << endl;
    exit(1);
}

// Edit distance calculation with more flexible matching
bool edit_distance_within(const string& str1, const string& str2, int d) {
    // If lengths differ too much, they can't be within the edit distance
    if (abs((int)str1.length() - (int)str2.length()) > d) 
        return false;

    // Ensure str1 is the shorter string
    const string& shorter = (str1.length() <= str2.length()) ? str1 : str2;
    const string& longer = (str1.length() <= str2.length()) ? str2 : str1;

    int edits = 0;
    size_t i = 0, j = 0;

    while (i < shorter.length() && j < longer.length()) {
        if (shorter[i] == longer[j]) {
            i++;
            j++;
        } else {
            edits++;
            
            if (edits > d) 
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

    return edits <= d;
}

// Check if two words are adjacent
bool is_adjacent(const string& word1, const string& word2) {
    return edit_distance_within(word1, word2, 1);
}

// Load words from a file into a set
void load_words(set<string>& word_list, const string& file_name) {
    ifstream file(file_name);
    if (!file) {
        throw runtime_error("Cannot open dictionary file: " + file_name);
    }

    word_list.clear();
    string word;
    while (file >> word) {
        word_list.insert(to_lower(word));
    }
}

// Word ladder generation using advanced BFS
vector<string> generate_word_ladder(
    const string& begin_word, 
    const string& end_word, 
    const set<string>& word_list
) {
    // Convert words to lowercase
    string start = to_lower(begin_word);
    string goal = to_lower(end_word);

    // Validate end word is in dictionary
    if (word_list.find(goal) == word_list.end()) {
        error(start, goal, "End word not in dictionary");
        return {};
    }

    // If start and goal are the same
    if (start == goal) {
        return {start};
    }

    // Convert word_list to unordered_set for faster lookups
    unordered_set<string> dictionary(word_list.begin(), word_list.end());

    // Queue for BFS
    queue<vector<string>> ladder_queue;
    unordered_set<string> visited;

    // Initialize with start word
    ladder_queue.push({start});
    visited.insert(start);

    // Timeout and length constraints
    auto start_time = chrono::steady_clock::now();
    const int MAX_SEARCH_TIME_SECONDS = 30;
    const int MAX_LADDER_LENGTH = 100;

    while (!ladder_queue.empty()) {
        // Check for timeout
        auto current_time = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(current_time - start_time);
        if (elapsed.count() > MAX_SEARCH_TIME_SECONDS) {
            error(start, goal, "Search timed out");
            return {};
        }

        // Get current ladder
        vector<string> current_ladder = ladder_queue.front();
        ladder_queue.pop();

        // Prevent excessively long ladders
        if (current_ladder.size() > MAX_LADDER_LENGTH) continue;

        // Get last word in current ladder
        string last_word = current_ladder.back();

        // Try every word in dictionary
        for (const string& candidate : dictionary) {
            // Skip if already visited
            if (visited.count(candidate)) continue;

            // Check if candidate is adjacent to last word
            if (is_adjacent(last_word, candidate)) {
                // Create new ladder
                vector<string> new_ladder = current_ladder;
                new_ladder.push_back(candidate);

                // Check if goal reached
                if (candidate == goal) {
                    return new_ladder;
                }

                // Mark as visited and enqueue
                visited.insert(candidate);
                ladder_queue.push(new_ladder);
            }
        }
    }

    // No ladder found
    error(start, goal, "No ladder found");
    return {};
}

// Print word ladder
void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No ladder found." << endl;
        return;
    }

    // Specific format for the test case
    cout << "Word ladder found: ";
    for (const string& word : ladder) {
        cout << word << " ";
    }
    cout << endl;
}

// Verify word ladder
void verify_word_ladder() {
    // Load dictionary
    set<string> test_dictionary;
    load_words(test_dictionary, "words.txt");

    // Test cases with known challenging ladders
    vector<pair<string, string>> test_pairs = {
        {"awake", "sleep"},
        {"code", "data"},
        {"cat", "dog"}
    };

    for (const auto& pair : test_pairs) {
        try {
            vector<string> ladder = generate_word_ladder(pair.first, pair.second, test_dictionary);
            
            // Verify ladder
            if (!ladder.empty()) {
                cout << "Ladder from " << pair.first << " to " << pair.second << ":" << endl;
                print_word_ladder(ladder);
            } else {
                cout << "No ladder found between " << pair.first << " and " << pair.second << endl;
            }
        } catch (const exception& e) {
            cerr << "Error generating ladder: " << e.what() << endl;
        }
    }
}