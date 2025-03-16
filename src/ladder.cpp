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

// Bidirectional BFS for word ladder generation
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

    // Bidirectional BFS
    unordered_map<string, string> forward_parent;
    unordered_map<string, string> backward_parent;
    unordered_set<string> forward_visited;
    unordered_set<string> backward_visited;
    queue<string> forward_queue;
    queue<string> backward_queue;

    forward_queue.push(start);
    backward_queue.push(goal);
    forward_visited.insert(start);
    backward_visited.insert(goal);

    // Timeout mechanism
    auto start_time = chrono::steady_clock::now();
    const int MAX_SEARCH_TIME_SECONDS = 10;  // Reduced timeout
    const int MAX_LADDER_LENGTH = 50;

    string meeting_word;
    bool found = false;

    while (!forward_queue.empty() && !backward_queue.empty()) {
        // Check for timeout
        auto current_time = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(current_time - start_time);
        if (elapsed.count() > MAX_SEARCH_TIME_SECONDS) {
            error(start, goal, "Search timed out");
            return {};
        }

        // Expand forward search
        {
            int size = forward_queue.size();
            for (int i = 0; i < size; i++) {
                string current = forward_queue.front();
                forward_queue.pop();

                // Prevent excessively long ladders
                if (forward_parent.size() > MAX_LADDER_LENGTH) continue;

                // Try all possible transformations
                for (size_t j = 0; j < current.length(); j++) {
                    string next = current;
                    for (char c = 'a'; c <= 'z'; c++) {
                        next[j] = c;
                        
                        // Skip if not in dictionary or already visited
                        if (dictionary.count(next) == 0 || forward_visited.count(next)) 
                            continue;

                        // Check if this word connects the two searches
                        if (backward_visited.count(next)) {
                            meeting_word = next;
                            found = true;
                            break;
                        }

                        forward_queue.push(next);
                        forward_visited.insert(next);
                        forward_parent[next] = current;
                    }

                    if (found) break;
                }

                if (found) break;
            }

            if (found) break;
        }

        // Expand backward search
        {
            int size = backward_queue.size();
            for (int i = 0; i < size; i++) {
                string current = backward_queue.front();
                backward_queue.pop();

                // Prevent excessively long ladders
                if (backward_parent.size() > MAX_LADDER_LENGTH) continue;

                // Try all possible transformations
                for (size_t j = 0; j < current.length(); j++) {
                    string next = current;
                    for (char c = 'a'; c <= 'z'; c++) {
                        next[j] = c;
                        
                        // Skip if not in dictionary or already visited
                        if (dictionary.count(next) == 0 || backward_visited.count(next)) 
                            continue;

                        // Check if this word connects the two searches
                        if (forward_visited.count(next)) {
                            meeting_word = next;
                            found = true;
                            break;
                        }

                        backward_queue.push(next);
                        backward_visited.insert(next);
                        backward_parent[next] = current;
                    }

                    if (found) break;
                }

                if (found) break;
            }

            if (found) break;
        }
    }

    // If no path found
    if (!found) {
        error(start, goal, "No ladder found");
        return {};
    }

    // Reconstruct the path
    vector<string> ladder;
    string current = meeting_word;

    // Reconstruct forward path
    while (current != start) {
        ladder.push_back(current);
        current = forward_parent[current];
    }
    ladder.push_back(start);
    reverse(ladder.begin(), ladder.end());

    // Reconstruct backward path
    current = meeting_word;
    while (current != goal) {
        current = backward_parent[current];
        ladder.push_back(current);
    }

    return ladder;
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