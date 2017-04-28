#ifndef PROCESSGRAPHQUICKCONCAT_H
#define PROCESSGRAPHQUICKCONCAT_H
#include <tuple>
#include <NetworKit/graph/Graph.h>
#include <functional>

#include "sortedvectorwrapper.h"
#include "weightededgewrapper.h"

namespace NetworKit {

template <class _First, class _Secound>
struct TupleWithFirstComparator {
    _First f_;
    _Secound s_;
};


template <class _First, class _Secound>
bool operator==(const TupleWithFirstComparator<_First, _Secound>& lhs, const TupleWithFirstComparator<_First, _Secound>& rhs) {
    return lhs.f_ == rhs.f_;
}

template <class _First, class _Secound>
bool operator<(const TupleWithFirstComparator<_First, _Secound>& lhs, const TupleWithFirstComparator<_First, _Secound>& rhs) {
    return lhs.f_ < rhs.f_;
}

template <class _First, class _Secound>
bool operator==(const _First& lhs, const TupleWithFirstComparator<_First, _Secound>& rhs) {
    return lhs == rhs.f_;
}

template <class _First, class _Secound>
bool operator<(const _First& lhs, const TupleWithFirstComparator<_First, _Secound>& rhs) {
    return lhs < rhs.f_;
}

template <class _First, class _Secound>
bool operator==(const TupleWithFirstComparator<_First, _Secound>& lhs, const _First& rhs) {
    return rhs == lhs.f_;
}

template <class _First, class _Secound>
bool operator<(const TupleWithFirstComparator<_First, _Secound>& lhs, const _First& rhs) {
    return lhs.f_ < rhs;
}

class ProcessGraphQuickConcat
{
private:
    size_t numberOfNodes;
    SortedVectorWrapper<node> nodes;
    std::tuple<std::vector<node>, std::vector<edgeweight>> concatedNodeWeightsToHeaviestNeighbour;

    typedef TupleWithFirstComparator<size_t, edgeweight> IndexAndWeight;

    std::vector<SortedVectorWrapper<IndexAndWeight>> indexEdges;
    std::vector<edgeweight> concatNodeWeightVector;

    std::vector<bool> concated;
    size_t amountConcated = 0;

    node concatNode;
    size_t concatNodeIndex;
    edgeweight maxEdgeWeight = 0;

    size_t currentHeaviestNeighbourIndex;

    void concatHeaviestNeighbour() { concated[currentHeaviestNeighbourIndex] = true; ++amountConcated; }

    void buildConcatedWeightsIndexVersion() {
        std::get<0>(concatedNodeWeightsToHeaviestNeighbour).clear();
        std::get<1>(concatedNodeWeightsToHeaviestNeighbour).clear();

        for(int i =0; i < concated.size(); ++i) {
            if(concated[i]) {
                std::get<0>(concatedNodeWeightsToHeaviestNeighbour).push_back(nodes[i]);
                std::get<1>(concatedNodeWeightsToHeaviestNeighbour).push_back(
                            indexEdges[i][indexEdges[i].getIndexOf(currentHeaviestNeighbourIndex)].s_);
            }
        }
    }


public:
    ProcessGraphQuickConcat(const Graph& g) {
        assert(g.isWeighted());
        assert(!g.isDirected());

        numberOfNodes = g.numberOfNodes();
        nodes.vector_.reserve(numberOfNodes);

        indexEdges.reserve(numberOfNodes);
        for(int i = 0; i < numberOfNodes; ++i) indexEdges.push_back(SortedVectorWrapper<IndexAndWeight>());

        concated.reserve(numberOfNodes);

        for(int i = 0; i < numberOfNodes; ++i) concated.push_back(false);

        auto nodeFunc = [&](node n) {
            nodes.push_back(n);
        };

        g.forNodes(nodeFunc);
        nodes.sort();


        auto edgeFunc = [&](node u, node v, edgeweight w) {
            auto uIndex = nodes.getIndexOf(u);
            auto vIndex = nodes.getIndexOf(v);
            indexEdges[uIndex].push_back(IndexAndWeight{vIndex, w});
            indexEdges[vIndex].push_back(IndexAndWeight{uIndex, w});

            if(maxEdgeWeight < w) {
                maxEdgeWeight = w;
                concatNode = u;
                concatNodeIndex = uIndex;
                currentHeaviestNeighbourIndex = vIndex;
            }
        };

        g.forEdges(edgeFunc);

        concatNodeWeightVector.reserve(numberOfNodes);

        for(int i = 0; i < numberOfNodes; ++i) {
            concatNodeWeightVector.push_back(0);
        }

        for(auto w : indexEdges[concatNodeIndex]) {
            concatNodeWeightVector[w.f_] = w.s_;
        }

        #pragma omp parallel for
        for(int i = 0; i < indexEdges.size(); ++i) {
            indexEdges[i].sort();
        }

        concated[concatNodeIndex] = true;
    }

    node getConcatNode() {return concatNode;}
    node getCurrentHeaviestNeighbour(){return nodes[currentHeaviestNeighbourIndex];}

    size_t getAmountOfNodes() { return numberOfNodes - amountConcated; }

    std::tuple<std::vector<node>, std::vector<edgeweight>>& getWeightWithConcatedNodes(){ return concatedNodeWeightsToHeaviestNeighbour;}

    void concatNodeWithHeaviestNeighbourIndexVersion() {
        concated[currentHeaviestNeighbourIndex] = true;
         ++amountConcated;

        auto prevHeaviestIndex = currentHeaviestNeighbourIndex;


        auto raw_otherdata = indexEdges[currentHeaviestNeighbourIndex].vector_.data();
        auto raw_concdata = concatNodeWeightVector.data();

        const size_t size = indexEdges[currentHeaviestNeighbourIndex].size();

        #pragma omp parallel for
        for(int i = 0; i < size; ++i) {
            auto w = raw_otherdata[i];
            raw_concdata[w.f_] += w.s_;
            raw_otherdata[i].s_ += raw_concdata[w.f_] - w.s_;
        }

        maxEdgeWeight = 0;
        for(int i = 0; i < concatNodeWeightVector.size(); ++i) {
            if(concated[i]) continue;
            if(concatNodeWeightVector[i] > maxEdgeWeight) {
                maxEdgeWeight = concatNodeWeightVector[i];
                currentHeaviestNeighbourIndex = i;
            }
        }
        assert(currentHeaviestNeighbourIndex != prevHeaviestIndex);

        buildConcatedWeightsIndexVersion();
    }

    void printEdges() {
        for(auto n : nodes) printEdge(n);
     }

    void printEdge(node n) {
        auto index = nodes.getIndexOf(n);
        std::cout << "node " << n << ": ";
        for(auto e : indexEdges[index]) std::cout << e.f_ << " " << e.s_ << " | ";
        std::cout << '\n';
    }


};

}
#endif // PROCESSGRAPHQUICKCONCAT_H
