#include <gtest/gtest.h>
#include <NetworKit/graph/Graph.h>
#include "mapprocessestoprocessors.h"
#include <NetworKit/io/METISGraphReader.h>
#include "graphcreation.h"
#include "mappingtest.h"

#include <NetworKit/io/METISGraphReader.h>
using namespace NetworKit;

size_t abs(size_t lhs, size_t rhs) {
    return lhs < rhs ? rhs - lhs : lhs - rhs;
}
Graph createMeshGraph(size_t width) {
    Graph g(width * width, true, false);
    for(size_t i = 0; i< width * width - 1; ++i) {
        auto x = i % width;
        auto y = i / width;
        for(size_t j = i + 1; j < width * width; ++j) {
            auto x_2 = j % width;
            auto y_2 = j / width;
            auto x_diff = abs(x,x_2);
            auto y_diff = abs(y,y_2);
            edgeweight w = x_diff + y_diff;
            g.addEdge(i,j,w);
        }
    }
    return move(g);
}

TEST(nobullshiterrors, 1024) {
    METISGraphReader reader;
    auto processG = reader.read("/home/holger/Studium/AlgOpt/Programmierprojekt/edgeWeightedCommGraphs/extra_social_H/as-22july06_k=1024_seed=1_preconf=ecosocial.graph");
    auto processorG = createMeshGraph(32);

    MapProcessesToProcessors mapper(processG, processorG);
    mapper.run();
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();;;;
}
