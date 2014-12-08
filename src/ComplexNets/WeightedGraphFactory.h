#ifndef WEIGHTEDGRAPHFACTORY_H
#define WEIGHTEDGRAPHFACTORY_H

#include "IGraphFactory.h"
#include "WeightedBetweenness.h"
#include "IWeightedBetweenness.h"
#include "WeightedClusteringCoefficient.h"
#include "WeightedNearestNeighborsDegree.h"
//#include "WeightedShellIndex.h"
#include "WeightedGraphReader.h"
#include "DegreeDistribution.h"

namespace graphpp
{
template<class Graph, class Vertex>
class WeightedGraphFactory: public IGraphFactory<Graph, Vertex>
{
public:
    virtual IGraphReader<Graph, Vertex>* createGraphReader()
    {
        return new WeightedGraphReader<Graph, Vertex>();
    }
    virtual IWeightedBetweenness<WeightedGraph, WeightedVertex>* createBetweenness(WeightedGraph& g)
    {
	//return new Betweenness<Graph, Vertex>(g);
	return new WeightedBetweenness<WeightedGraph, WeightedVertex>(g);
    }
    virtual IClusteringCoefficient<Graph, Vertex>* createClusteringCoefficient()
    {
        return new WeightedClusteringCoefficient<Graph, Vertex>();
    }
    virtual INearestNeighborsDegree<Graph, Vertex>* createNearestNeighborsDegree()
    {
        return new WeightedNearestNeighborsDegree<Graph, Vertex>();
    }
    virtual IShellIndex<Graph, Vertex>* createShellIndex(Graph&)
    {
        //return new WeightedShellIndex<Graph,Vertex>();
        return NULL;
    }
    virtual IDegreeDistribution<Graph, Vertex>* createDegreeDistribution(Graph& g)
    {
        return new DegreeDistribution<Graph, Vertex>(g);
    }
    
    virtual MaxClique<Graph, Vertex>* createMaxClique(Graph&)
    {
        return NULL;
    }
        
    virtual MaxCliqueExact<Graph, Vertex>* createExactMaxClique(Graph&, int)
    {
        return NULL;
    }
};
}

#endif

