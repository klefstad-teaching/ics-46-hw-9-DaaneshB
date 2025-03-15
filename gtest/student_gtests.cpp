#include <gtest/gtest.h>
#include "ladder.h"
#include "dijkstras.h"


// Word Ladder Test Fixture
class WordLadderTest : public ::testing::Test {
protected:
    set<string> word_list;

    void SetUp() override {
        // Load words for testing
        load_words(word_list, "../src/words.txt");
    }
};

// Dijkstra's Algorithm Test Fixture
class DijkstrasTest : public ::testing::Test {
protected:
    Graph G;

    void SetUp() override {
        // Setup a sample graph for testing
        try {
            file_to_graph("../src/small.txt", G);
        } catch (const runtime_error& e) {
            FAIL() << "Could not load test graph";
        }
    }
};

// Word Ladder Tests
TEST_F(WordLadderTest, Adjacency) {
    EXPECT_TRUE(is_adjacent("cat", "bat")) << "cat and bat should be adjacent";
    EXPECT_TRUE(is_adjacent("cat", "cot")) << "cat and cot should be adjacent";
    EXPECT_FALSE(is_adjacent("cat", "dog")) << "cat and dog should not be adjacent";
}

// Test word ladder generation with simple cases
TEST_F(WordLadderTest, SimpleWordLadders) {
    // Test cases with known short ladders
    vector<string> ladder1 = generate_word_ladder("cat", "dog", word_list);
    ASSERT_FALSE(ladder1.empty()) << "Should find a ladder from cat to dog";
    EXPECT_EQ(ladder1.front(), "cat") << "Ladder should start with 'cat'";
    EXPECT_EQ(ladder1.back(), "dog") << "Ladder should end with 'dog'";
}




// Test minimum distance computation
TEST_F(DijkstrasTest, MinimumDistances) {
    vector<int> previous;
    vector<int> distances = dijkstra_shortest_path(G, 0, previous);

    // Verify all distances are finite
    for (size_t i = 0; i < distances.size(); ++i) {
        EXPECT_NE(distances[i], INF) << "Distance to vertex " << i << " should not be infinite";
    }
}

// Test single source vertex graph
TEST_F(DijkstrasTest, SingleVertexGraph) {
    Graph single_vertex_G;
    single_vertex_G.numVertices = 1;
    single_vertex_G.resize(1);

    vector<int> previous;
    vector<int> distances = dijkstra_shortest_path(single_vertex_G, 0, previous);

    EXPECT_EQ(distances[0], 0) << "Distance to single vertex should be 0";
}


