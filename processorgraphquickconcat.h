#ifndef PROCESSORGRAPHQUICKCONCAT_H
#define PROCESSORGRAPHQUICKCONCAT_H
#include <iostream>
#include <NetworKit/graph/Graph.h>
#include <vector>

#include "sortedvectorwrapper.h"
#include "weightededgewrapper.h"

namespace NetworKit {
    class ProcessorGraphQuickConcat
    {
    private:
        SortedVectorWrapper<node> nodes;
        std::vector<std::vector<edgeweight>> weights;

        size_t numberOfNodes;

        node concatNode;
        size_t concatNodeIndex;
        edgeweight minEdgeWeight = std::numeric_limits<edgeweight>::max();

        node lightestNeighbour;
        size_t lightestNeighbourIndex;

        std::vector<bool> concated;
        size_t amountConcated = 0;

        void concat(size_t pos) {
            concated[pos] = true;
            amountConcated++;
        }

        bool isConcated(node n) { return concated[nodes.getIndexOf(n)]; }

    public:
        ProcessorGraphQuickConcat(const Graph& g) {
            assert(g.isWeighted());
            assert(!g.isDirected());

            numberOfNodes = g.numberOfNodes();

            nodes.vector_.reserve(numberOfNodes);
            concated.reserve(numberOfNodes);
            for(int i = 0; i < numberOfNodes; ++i) concated.push_back(false);

            auto nodeFunc = [&](node n) {
                nodes.push_back(n);
            };

            g.forNodes(nodeFunc);
            nodes.sort();

            weights.reserve(numberOfNodes);
            for(size_t i = 0; i < numberOfNodes; ++i) {
                weights.push_back(std::vector<edgeweight>());
                weights[i].reserve(numberOfNodes);
                for(int j = 0; j < numberOfNodes; ++j) {
                    weights[i].push_back(0);
                }
            }


            auto edgeFunc = [&](node u, node v, edgeweight w) {
                auto uIndex = nodes.getIndexOf(u);
                auto vIndex = nodes.getIndexOf(v);

                weights[uIndex][vIndex] = w;
                weights[vIndex][uIndex] = w;

                if(minEdgeWeight > w) {
                    minEdgeWeight = w;
                    concatNode = u;
                    concatNodeIndex = uIndex;
                    lightestNeighbour = v;
                    lightestNeighbourIndex = vIndex;
                }
            };

            g.forEdges(edgeFunc);
            concated[concatNodeIndex] = true;
        }

        void concatWithConcatNode(node n) {
            auto otherIndex = nodes.getIndexOf(n);
            concated[otherIndex] = true;
            #pragma omp parallel for
            for(int i = 0; i < numberOfNodes; ++i) {
                weights[concatNodeIndex][i] += weights[otherIndex][i];
            }
        }

        node getConcatNode() {return concatNode;}
        node getBeginningLightestNode(){return lightestNeighbour;}

        node getNodeWhichMinimizesSum(const std::vector<edgeweight>& vcWeights, const std::vector<node>& vcMapped) {
            assert(vcWeights.size() == vcMapped.size());

            edgeweight minSum = std::numeric_limits<edgeweight>::max();
            node cheapestNode = 0;

            std::vector<size_t> mappedIndices;
            mappedIndices.reserve(vcMapped.size());
            for(auto n : vcMapped) mappedIndices.push_back(nodes.getIndexOf(n));

            for(int nodeIndex = 0; nodeIndex < numberOfNodes; ++nodeIndex) {
                if(concated[nodeIndex]) continue;

                edgeweight sum = 0;
                for(int i = 0; i < vcWeights.size(); ++i) {
                    edgeweight vcWeight = vcWeights[i];
                    auto mappedWeight = weights[nodeIndex][mappedIndices[i]];

                    sum = sum > vcWeight * mappedWeight ? sum : vcWeight * mappedWeight;
                }

                if(minSum > sum) {
                    minSum = sum;
                    cheapestNode = nodes[nodeIndex];
                }
            }
            return cheapestNode;
        }

        node getLeftOverNode() {
            for(int i = 0; i < concated.size(); ++i) if(!concated[i]) return nodes[i];
        }
#ifdef DEBUG
        void printEdges() {
            std::cout << "concatnode: " << concatNode << " amt nodes: " << numberOfNodes - amountConcated << '\n';
            for(int i = 0; i < numberOfNodes; ++i) {
                std::cout << "node " << nodes[i] << ": ";
                for(int j = 0; j < numberOfNodes; ++j) {
                    std::cout << weights[i][j] << " ";
                }
                std::cout << '\n';
            }
        }
    };
#endif // DEBUG
}
#endif // PROCESSORGRAPHQUICKCONCAT_H
