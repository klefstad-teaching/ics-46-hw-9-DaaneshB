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

        // Reset current ladder words
        current_ladder_words.clear();
        for (const string& word : current_ladder) {
            current_ladder_words.insert(word);
        }

        // Try every word in dictionary
        for (const string& candidate : word_list) {
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
    error(start, goal, "No ladder found");
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

void verify_word_ladder() {
    // Test cases to verify word ladder properties
    set<string> test_dictionary;
    
    // Load dictionary for verification
    try {
        load_words(test_dictionary, "words.txt");
    } catch (const exception& e) {
        cerr << "Failed to load dictionary: " << e.what() << endl;
        return;
    }

    // Test case 1: Basic valid ladder
    vector<string> valid_ladder1 = {"code", "cade", "cate", "date", "data"};
    vector<string> valid_ladder2 = {"cat", "cot", "dot", "dog"};
    vector<string> invalid_ladder1 = {"code", "cade", "cate", "date", "data", "extra"};
    vector<string> invalid_ladder2 = {"cat", "dog"};

    // Verification function to check a single ladder
    auto verify_single_ladder = [&](const vector<string>& ladder, bool expected_valid) {
        // Check if ladder is empty
        if (ladder.empty()) {
            if (expected_valid) {
                cerr << "Error: Expected valid ladder, but got empty ladder" << endl;
                return false;
            }
            return true;
        }

        // Check dictionary membership
        for (const string& word : ladder) {
            if (test_dictionary.find(to_lower(word)) == test_dictionary.end()) {
                cerr << "Error: Word '" << word << "' not found in dictionary" << endl;
                return false;
            }
        }

        // Check adjacency between consecutive words
        for (size_t i = 0; i < ladder.size() - 1; ++i) {
            if (!is_adjacent(ladder[i], ladder[i+1])) {
                cerr << "Error: Words '" << ladder[i] << "' and '" 
                     << ladder[i+1] << "' are not adjacent" << endl;
                return false;
            }
        }

        return true;
    };

    // Comprehensive ladder verification tests
    struct LadderTest {
        vector<string> ladder;
        bool expected_valid;
        string test_name;
    };

    vector<LadderTest> ladder_tests = {
        {valid_ladder1, true, "Valid 5-word ladder"},
        {valid_ladder2, true, "Valid 4-word ladder"},
        {invalid_ladder1, false, "Ladder with extra invalid word"},
        {invalid_ladder2, false, "Ladder with non-adjacent words"},
        {generate_word_ladder("code", "data", test_dictionary), true, "Generated ladder test"},
        {generate_word_ladder("cat", "dog", test_dictionary), true, "Another generated ladder test"}
    };

    // Run verification tests
    int passed_tests = 0;
    int total_tests = ladder_tests.size();

    for (const auto& test : ladder_tests) {
        bool result = verify_single_ladder(test.ladder, test.expected_valid);
        
        if (result == test.expected_valid) {
            passed_tests++;
            cout << "PASSED: " << test.test_name << endl;
        } else {
            cerr << "FAILED: " << test.test_name << endl;
        }
    }

    // Additional comprehensive checks
    // 1. Test some known problematic word pairs
    vector<pair<string, string>> test_word_pairs = {
        {"code", "data"},
        {"cat", "dog"},
        {"marty", "curls"}
    };

    cout << "\nGenerating ladders for test word pairs:" << endl;
    for (const auto& pair : test_word_pairs) {
        vector<string> ladder = generate_word_ladder(pair.first, pair.second, test_dictionary);
        
        if (!ladder.empty()) {
            cout << "Ladder from " << pair.first << " to " << pair.second << ":" << endl;
            for (const string& word : ladder) {
                cout << word << " ";
            }
            cout << endl;
            
            // Verify the generated ladder
            verify_single_ladder(ladder, true);
        } else {
            cerr << "No ladder found between " << pair.first << " and " << pair.second << endl;
        }
    }

    // Print overall test results
    cout << "\nLadder Verification Summary:" << endl;
    cout << "Passed " << passed_tests << " out of " << total_tests << " tests" << endl;

    // Optional: Stress test with random word generation
    cout << "\nPerforming stress test..." << endl;
    int stress_test_count = 10;
    int stress_test_passed = 0;

    // Random word selection for stress testing
    vector<string> dictionary_vector(test_dictionary.begin(), test_dictionary.end());
    
    for (int i = 0; i < stress_test_count; ++i) {
        // Randomly select start and end words
        string start_word = dictionary_vector[rand() % dictionary_vector.size()];
        string end_word = dictionary_vector[rand() % dictionary_vector.size()];

        vector<string> random_ladder = generate_word_ladder(start_word, end_word, test_dictionary);
        
        if (!random_ladder.empty()) {
            if (verify_single_ladder(random_ladder, true)) {
                stress_test_passed++;
            }
        }
    }

    cout << "Stress Test: Passed " << stress_test_passed 
         << " out of " << stress_test_count << " random ladder generations" << endl;
}