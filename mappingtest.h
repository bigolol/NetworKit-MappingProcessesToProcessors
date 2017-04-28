#ifndef MAPPINGTEST_H
#define MAPPINGTEST_H
#include <gtest/gtest.h>
#include <NetworKit/graph/Graph.h>
#include "mapprocessestoprocessors.h"

class MappingTest : public testing::Test {
public:
    NetworKit::Graph g3;
    NetworKit::Graph g3cpu;

    NetworKit::Graph g4;
    NetworKit::Graph g4cpu;

    NetworKit::Graph g5;
    NetworKit::Graph g5cpu;

    MappingTest() {
        WeightedEdge e12(0,1,2);
        WeightedEdge e13(0,2,5);

        g3 = NetworKit::Graph{e12,e13};

        WeightedEdge e12cpu(0,1,1);
        WeightedEdge e13cpu(0,2,3);
        WeightedEdge e23cpu(1,2,6);

        g3cpu = NetworKit::Graph{e12cpu, e13cpu, e23cpu};

        WeightedEdge ef01(0,1,2);
        WeightedEdge ef02(0,2,5);
        WeightedEdge ef12(1,2,4);
        WeightedEdge ef13(1,3,1);

        g4 = NetworKit::Graph{ef01, ef02, ef12, ef13};

        WeightedEdge ef01cpu(0,1,1);
        WeightedEdge ef02cpu(0,2,3);
        WeightedEdge ef03cpu(0,3,7);
        WeightedEdge ef12cpu(1,2,6);
        WeightedEdge ef13cpu(1,3,4);
        WeightedEdge ef23cpu(2,3,3);

        g4cpu = NetworKit::Graph{ef01cpu, ef02cpu, ef03cpu, ef12cpu, ef13cpu, ef23cpu};

        WeightedEdge efi04(0,4,7);
        WeightedEdge efi12(1,2,2);
        WeightedEdge efi13(1,3,3);
        WeightedEdge efi14(1,4,3);
        WeightedEdge efi24(2,4,4);
        g5 = NetworKit::Graph{efi04, efi12, efi13, efi14, efi24};

        WeightedEdge efi01cpu(0,1,8);
        WeightedEdge efi02cpu(0,2,4);
        WeightedEdge efi03cpu(0,3,5);
        WeightedEdge efi04cpu(0,4,6);
        WeightedEdge efi12cpu(1,2,1);
        WeightedEdge efi13cpu(1,3,6);
        WeightedEdge efi14cpu(1,4,3);
        WeightedEdge efi23cpu(2,3,4);
        WeightedEdge efi24cpu(2,4,2);
        WeightedEdge efi34cpu(3,4,2);
        g5cpu = NetworKit::Graph{efi01cpu,efi02cpu, efi03cpu, efi04cpu,efi12cpu,efi13cpu,efi14cpu, efi23cpu, efi24cpu, efi34cpu};


    }

};

TEST_F(MappingTest, mapsThreeNodeGraph) {
    NetworKit::MapProcessesToProcessors mapper{g3, g3cpu};
    mapper.run();
    ASSERT_EQ(0, mapper.getMappedNode(0));
    ASSERT_EQ(1, mapper.getMappedNode(2));
    ASSERT_EQ(2, mapper.getMappedNode(1));
}

TEST_F(MappingTest, mapsFourNodes) {
    NetworKit::MapProcessesToProcessors mapper{g4, g4cpu};
    mapper.run();
    ASSERT_EQ(0, mapper.getMappedNode(0));
    ASSERT_EQ(1, mapper.getMappedNode(2));
    ASSERT_EQ(2, mapper.getMappedNode(1));
    ASSERT_EQ(3, mapper.getMappedNode(3));
}

TEST_F(MappingTest, mapsFiveNodes) {
    NetworKit::MapProcessesToProcessors mapper{g5, g5cpu};
    mapper.run();
    ASSERT_EQ(1, mapper.getMappedNode(0));
    ASSERT_EQ(3, mapper.getMappedNode(1));
    ASSERT_EQ(4, mapper.getMappedNode(2));
    ASSERT_EQ(0, mapper.getMappedNode(3));
    ASSERT_EQ(2, mapper.getMappedNode(4));
}

#endif // MAPPINGTEST_H
