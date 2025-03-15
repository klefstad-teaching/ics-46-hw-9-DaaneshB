#include "dijkstras.h"

int main() {
    // Prompt for input file
    string filename = "../src/largest.txt";
    // Create graph
    Graph G;
    
    try {
        // Load graph from file
        file_to_graph(filename, G);
        
        // Source vertex (typically 0)
        int source = 0;
        
        // Vector to store previous vertices for path reconstruction
        vector<int> previous;
        
        // Run Dijkstra's algorithm
        vector<int> distances = dijkstra_shortest_path(G, source, previous);
        
        // Print shortest paths from source to all vertices
        cout << "Shortest paths from vertex " << source << ":" << endl;
        for (int destination = 0; destination < G.numVertices; ++destination) {
            if (destination == source) continue; // Skip source to source
            
            // Extract and print path
            vector<int> path = extract_shortest_path(distances, previous, destination);
            
            if (!path.empty()) {
                cout << "\nPath to vertex " << destination << ":" << endl;
                print_path(path, distances[destination]);
            } else {
                cout << "No path to vertex " << destination << endl;
            }
        }
    }
    catch (const runtime_error& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}