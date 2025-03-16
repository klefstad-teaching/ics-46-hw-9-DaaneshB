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
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <limits>

#define my_assert(e) {cout << #e << ((e) ? " passed": " failed") << endl;}

using namespace std;

// Utility function to convert string to lowercase
string to_lower(string word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    return word;
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

// Optimized word ladder generation
vector<string> generate_word_ladder(
    const string& begin_word, 
    const string& end_word, 
    const set<string>& word_list
) {
    // Convert words to lowercase
    string start = to_lower(begin_word);
    string goal = to_lower(end_word);

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

    while (!ladder_queue.empty()) {
        // Get current ladder
        vector<string> current_ladder = ladder_queue.front();
        ladder_queue.pop();

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

    // No ladder found, return empty vector
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

// Verify word ladder
void verify_word_ladder() {
    set<string> word_list;
    load_words(word_list, "../src/words.txt");

    my_assert(generate_word_ladder("cat", "dog", word_list).size() == 4);
    my_assert(generate_word_ladder("marty", "curls", word_list).size() == 6);
    my_assert(generate_word_ladder("code", "data", word_list).size() == 6);
    my_assert(generate_word_ladder("work", "play", word_list).size() == 6);
    my_assert(generate_word_ladder("sleep", "awake", word_list).size() == 8);
    my_assert(generate_word_ladder("car", "cheat", word_list).size() == 4);
}