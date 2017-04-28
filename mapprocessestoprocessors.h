#ifndef MAPPROCESSESTOPROCESSORS_H
#define MAPPROCESSESTOPROCESSORS_H
#include <NetworKit/base/Algorithm.h>
#include <NetworKit/graph/Graph.h>
#include <tuple>
#include <iostream>

#include "processgraphquickconcat.h"
#include "processorgraphquickconcat.h"
#include "sortedvectorwrapper.h"



namespace NetworKit  {

    struct MappedWrapper {
        node u_;
        node v_;

        MappedWrapper(node u, node v) : u_(u), v_(v) {

        }

        friend bool operator==(const MappedWrapper& lhs, const MappedWrapper& rhs);
        friend bool operator==(const node lhs, const MappedWrapper& rhs);
        friend bool operator< (const MappedWrapper& lhs, const MappedWrapper& rhs);
        friend bool operator<(const node lhs, const MappedWrapper& rhs);
    };

    bool operator==(const MappedWrapper& lhs, const MappedWrapper& rhs) {
        return lhs.u_ == rhs.u_;
    }

    bool operator==(const node lhs, const MappedWrapper& rhs) {
        return lhs == rhs.u_;
    }

    bool operator==(const MappedWrapper& lhs, const node rhs) {
        return rhs < lhs;
    }

    bool operator< (const MappedWrapper& lhs, const MappedWrapper& rhs) {
        return lhs.u_ < rhs.u_;
    }

    bool operator<(const node lhs, const MappedWrapper& rhs) {
        return lhs < rhs.u_;
    }

    bool operator<(const MappedWrapper& lhs, const node rhs) {
        return lhs.u_ < rhs;
    }

    class MapProcessesToProcessors : public Algorithm
    {
    private:
        ProcessGraphQuickConcat processG;
        ProcessorGraphQuickConcat processorG;

        SortedVectorWrapper<MappedWrapper> mapping;
        std::vector<node> currentMappedNodes;

        void createCurrentMappedNodes(std::vector<node>& nodes) {
            currentMappedNodes.clear();
            for(auto n : nodes) {
                auto ind = mapping.getIndexOf(n);
                currentMappedNodes.push_back(mapping[ind].v_);
            }
        }

        void map(node u, node v) {
            mapping.push_back(MappedWrapper{u,v});
            mapping.sort();
        }


    public:
        MapProcessesToProcessors(const Graph& processGraph, const Graph& processorGraph) : processG{processGraph}, processorG{processorGraph} {

        }

        void printMapping() {
            for(auto t : mapping.vector_) {
                std::cout << t.u_ << " " << t.v_ << " | ";
            }
            std::cout << '\n';
        }

        node getMappedNode(node n) {
            for(auto currentMapping : mapping) {
                if(currentMapping.u_ == n) return currentMapping.v_;
            }
            return 0;
        }

        void run() override {

            map(processG.getConcatNode(), processorG.getConcatNode());
            map(processG.getCurrentHeaviestNeighbour(), processorG.getBeginningLightestNode());

            processG.concatNodeWithHeaviestNeighbourIndexVersion();
            processorG.concatWithConcatNode(processorG.getBeginningLightestNode());

            while(processG.getAmountOfNodes() > 2) {
                auto vcStar = processG.getCurrentHeaviestNeighbour();

                auto weightsOfConcatedNodesWithVcStar = processG.getWeightWithConcatedNodes();

                assert(std::get<0>(weightsOfConcatedNodesWithVcStar).size() ==
                       std::get<1>(weightsOfConcatedNodesWithVcStar).size());

                createCurrentMappedNodes(std::get<0>(weightsOfConcatedNodesWithVcStar));

                assert(std::get<1>(weightsOfConcatedNodesWithVcStar).size() == currentMappedNodes.size());

                auto vpStar = processorG.getNodeWhichMinimizesSum(
                            std::get<1>(weightsOfConcatedNodesWithVcStar),
                            currentMappedNodes);

                map(vcStar, vpStar);

                processG.concatNodeWithHeaviestNeighbourIndexVersion();
                processorG.concatWithConcatNode(vpStar);
            }

            map(processG.getCurrentHeaviestNeighbour(), processorG.getLeftOverNode());
        }
    };

}
#endif // MAPPROCESSESTOPROCESSORS_H
