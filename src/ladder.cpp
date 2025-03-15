#include "ladder.h"

void error(string word1, string word2, string msg) {
    cerr << "Error: " << msg << " (" << word1 << ", " << word2 << ")" << endl;
}

bool edit_distance_within(const std::string& str1, const std::string& str2, int d) {
    int len1 = str1.length();
    int len2 = str2.length();
    
    // If lengths differ by more than allowed edits, return false
    if (abs(len1 - len2) > d) return false;

    // If lengths are the same, count character differences
    if (len1 == len2) {
        int diff = 0;
        for (int i = 0; i < len1; ++i) {
            if (str1[i] != str2[i]) diff++;
        }
        return diff <= d;
    }

    // If lengths differ by 1, check for insertion/deletion
    const string& shorter = (len1 < len2) ? str1 : str2;
    const string& longer = (len1 < len2) ? str2 : str1;
    
    int i = 0, j = 0, diff = 0;
    while (i < shorter.length() && j < longer.length()) {
        if (shorter[i] != longer[j]) {
            diff++;
            j++;
        } else {
            i++;
            j++;
        }
    }
    
    return diff <= d;
}

bool is_adjacent(const string& word1, const string& word2) {
    return edit_distance_within(word1, word2, 1);
}

vector<string> generate_word_ladder(const string& begin_word, const string& end_word, const set<string>& word_list) {
    if (begin_word == end_word) return {begin_word};

    queue<vector<string>> ladder_queue;
    ladder_queue.push({begin_word});
    
    set<string> visited;
    visited.insert(begin_word);
    
    while (!ladder_queue.empty()) {
        auto ladder = ladder_queue.front();
        ladder_queue.pop();
        
        string last_word = ladder.back();
        
        for (const auto& word : word_list) {
            if (is_adjacent(last_word, word) && visited.find(word) == visited.end()) {
                visited.insert(word);
                vector<string> new_ladder = ladder;
                new_ladder.push_back(word);
                
                if (word == end_word) {
                    return new_ladder;
                }
                
                ladder_queue.push(new_ladder);
            }
        }
    }
    
    return {}; // No ladder found
}

void load_words(set<string>& word_list, const string& file_name) {
    ifstream file(file_name);
    if (!file) {
        cerr << "Error: Unable to open file " << file_name << endl;
        return;
    }
    
    string word;
    while (file >> word) {
        // Optional: convert to lowercase if needed
        // transform(word.begin(), word.end(), word.begin(), ::tolower);
        word_list.insert(word);
    }
}

void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No word ladder found." << endl;
        return;
    }
    
    for (const auto& word : ladder) {
        cout << word << " ";
    }
    cout << endl;
}

void verify_word_ladder() {
    set<string> word_list;
    load_words(word_list, "words.txt");
    
    // Example test cases
    vector<string> ladder;
    
    ladder = generate_word_ladder("cat", "dog", word_list);
    cout << "cat -> dog: " << (ladder.empty() ? "Failed" : "Passed") << endl;
    
    ladder = generate_word_ladder("code", "data", word_list);
    cout << "code -> data: " << (ladder.empty() ? "Failed" : "Passed") << endl;
    
    ladder = generate_word_ladder("work", "play", word_list);
    cout << "work -> play: " << (ladder.empty() ? "Failed" : "Passed") << endl;
}