#include "ladder.h"

int main() {
    // Load dictionary words
    set<string> word_list;
    load_words(word_list, "../src/words.txt");

    // Prompt user for start and end words
    string start_word, end_word;
    
    cout << "Enter start word: ";
    cin >> start_word;
    
    cout << "Enter end word: ";
    cin >> end_word;

    // Validate input
    if (start_word == end_word) {
        cerr << "Start and end words must be different." << endl;
        return 1;
    }

    // Generate word ladder
    vector<string> ladder = generate_word_ladder(start_word, end_word, word_list);

    // Print results
    if (ladder.empty()) {
        cout << "No word ladder found between " << start_word << " and " << end_word << endl;
    } else {
        cout << "Word Ladder:" << endl;
        print_word_ladder(ladder);
        cout << "Ladder length: " << ladder.size() << endl;
    }

    // Optional: run verification
    // verify_word_ladder();

    return 0;
}