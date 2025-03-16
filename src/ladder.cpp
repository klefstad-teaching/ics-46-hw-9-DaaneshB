#include "ladder.h"
// ladder.cpp
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <limits>

using namespace std;

const int MAX_LADDER_LENGTH = 50;  
const int MAX_SEARCH_TIME_SECONDS = 60;
// Error handling function
void error(string word1, string word2, string msg) {
    cerr << "Error with words '" << word1 << "' and '" << word2 << "': " << msg << endl;
    exit(1);
}

// Utility function to convert string to lowercase
string to_lower(string word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    return word;
}

// Edit distance calculation
bool edit_distance_within(const string& str1, const string& str2, int d) {
    // If lengths differ by more than d, they can't be within the edit distance
    if (abs((int)str1.length() - (int)str2.length()) > d) 
        return false;

    // Create a matrix to store edit distances
    vector<vector<int>> dp(str1.length() + 1, vector<int>(str2.length() + 1, 0));

    // Initialize first row and column
    for (size_t i = 0; i <= str1.length(); ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= str2.length(); ++j)
        dp[0][j] = j;

    // Compute edit distance
    for (size_t i = 1; i <= str1.length(); ++i) {
        for (size_t j = 1; j <= str2.length(); ++j) {
            // If characters are the same, no edit needed
            if (str1[i-1] == str2[j-1]) {
                dp[i][j] = dp[i-1][j-1];
            }
            else {
                // Minimum of insert, delete, or replace
                dp[i][j] = 1 + min({
                    dp[i-1][j],     // deletion
                    dp[i][j-1],     // insertion
                    dp[i-1][j-1]    // replacement
                });
            }
        }
    }

    // Return true if edit distance is within d
    return dp[str1.length()][str2.length()] <= d;
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

// Generate word ladder using BFS
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

    // Queue to store partial ladders
    queue<vector<string>> ladder_queue;
    
    // Set to track visited words GLOBALLY
    unordered_set<string> global_visited;

    // Initialize with start word
    ladder_queue.push({start});
    global_visited.insert(start);

    // Limit search to prevent infinite loops
    auto start_time = chrono::steady_clock::now();

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
        if (current_ladder.size() > MAX_LADDER_LENGTH) {
            continue;
        }

        // Get last word in current ladder
        string last_word = current_ladder.back();

        // Try every word in dictionary
        for (const string& candidate : word_list) {
            // Skip if globally visited
            if (global_visited.count(candidate)) continue;

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
    error(start, goal, "No ladder found");
    return {};
}

// Print word ladder
void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No ladder found." << endl;
        return;
    }

    cout << "Word ladder found: ";
    for (const string& word : ladder) {
        cout << word << " ";
    }
    cout << endl;
}

void verify_word_ladder() {
    // Load dictionary
    set<string> test_dictionary;
    load_words(test_dictionary, "words.txt");

    // Test cases with known good ladders
    vector<pair<string, string>> test_pairs = {
        {"code", "data"},
        {"awake", "sleep"},
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