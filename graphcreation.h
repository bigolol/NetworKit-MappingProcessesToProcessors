#ifndef GRAPHCREATION_H
#define GRAPHCREATION_H
#include <NetworKit/graph/Graph.h>
#include <memory>
#include <algorithm>
#include <cmath>

using namespace NetworKit;
using namespace std;

unsigned int randint(unsigned int min, unsigned int max) {
    return rand()%(max-min + 1) + min;
}

unique_ptr<Graph> createConnectedGraph(unsigned int nodes, unsigned int degreeConnection) {
    srand(time(0));
    unique_ptr<Graph> g = make_unique<Graph>(nodes, true, false);
    for(unsigned int i = 1; i < nodes; ++i) {
        auto connectTo = randint(0u, i - 1);
        g->addEdge(connectTo, i, static_cast<edgeweight>(rand() % 100 + 1));
        for(unsigned int j; j < randint(0u, degreeConnection); ++j) {
            auto connectTo = randint(0u, i - 1);
            g->addEdge(connectTo, i, static_cast<edgeweight>(rand() % 100 + 1));
        }
    }
    return g;
}

unique_ptr<Graph> createCompleteGraph(size_t nodes) {
    srand(time(0));
    unique_ptr<Graph> g =  make_unique<Graph>(nodes, true, false);

    for(size_t i = 0; i < nodes - 1; ++i) {
        for(size_t j = i + 1; j < nodes; ++j) {
            g->addEdge(i,j, static_cast<edgeweight>(rand() % 100 + 1));
        }
    }
    return g;
}




unique_ptr<Graph> createSquareMeshGraph(size_t width) {
    unique_ptr<Graph> g = make_unique<Graph>(width * width, true, false);
    for(size_t i = 0; i < (width * width)- 1; ++i) {
        auto i_x = i % width;
        auto i_y = i / width;
        for(size_t j = i + 1; j < width * width; ++j) {
            auto j_x = j % width;
            auto j_y = j / width;
        }
    }
    return g;
}

#endif // GRAPHCREATION_H
