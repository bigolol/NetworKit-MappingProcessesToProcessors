#ifndef WEIGHTEDEDGEWRAPPER_H
#define WEIGHTEDEDGEWRAPPER_H
#include <NetworKit/graph/Graph.h>

namespace NetworKit {
    struct WeightedEdgeWrapper {
        node n_;
        edgeweight w_;

        WeightedEdgeWrapper(node n, edgeweight w) : n_(n), w_(w) {

        }

        node getNode() const {
            return n_;
        }
        edgeweight getWeight() const {
            return w_;
        }

        friend bool operator==(const WeightedEdgeWrapper& lhs, const WeightedEdgeWrapper& rhs);
        friend bool operator<(const WeightedEdgeWrapper& lhs, const WeightedEdgeWrapper& rhs);
    };

    bool operator==(const WeightedEdgeWrapper& lhs, const WeightedEdgeWrapper& rhs) {
        return lhs.n_ == rhs.n_;
    }

    bool operator<(const WeightedEdgeWrapper& lhs, const WeightedEdgeWrapper& rhs) {
        return lhs.n_ < rhs.n_;
    }

}
#endif // WEIGHTEDEDGEWRAPPER_H
