#ifndef WEIGHTED_BETWEENNESS_H
#define WEIGHTED_BETWEENNESS_H
#include <stack>
#include <queue>
#include <vector>
#include <list>

#include "mili/mili.h"
#include "IWeightedBetweenness.h"
#include "WeightedVertexAspect.h"
#include "WeightedGraphAspect.h"

namespace graphpp
{
template <class WeightedGraph, class WeightedVertex>
class WeightedBetweenness : public IWeightedBetweenness<WeightedGraph, WeightedVertex>
{

public:

    typedef typename IWeightedBetweenness<WeightedGraph, WeightedVertex>::BetweennessContainer BetweennessContainer;
    typedef typename IWeightedBetweenness<WeightedGraph, WeightedVertex>::BetweennessIterator BetweennessIterator;
    typedef typename WeightedGraph::VerticesIterator VerticesIterator;
    typedef typename WeightedVertex::VerticesIterator NeighbourIterator;


    WeightedBetweenness(WeightedGraph& g)
    {
        initMap(g, 1, betweenness, 0.0, 0.0);
        calculateBetweenness(g);
    }

    virtual BetweennessIterator iterator()
    {
        return BetweennessIterator(betweenness);
    }

private:

    std::map<typename WeightedVertex::VertexId, double> sigma;
    std::map<typename WeightedVertex::VertexId, double> d;
    std::map<typename WeightedVertex::VertexId, double> delta;
    std::map<typename WeightedVertex::VertexId, std::list<typename WeightedVertex::VertexId> > p;

    void calculateBetweenness(WeightedGraph& g)
    {
        VerticesIterator iter = g.verticesIterator();

        while (!iter.end())
        {
            WeightedVertex* s = *iter;
            std::stack<WeightedVertex*> stack;
            std::queue<WeightedVertex*> queue;
	    std::priority_queue<WeightedVertex*> pqueue;
            //std::map<typename Vertex::VertexId, std::list<typename Vertex::VertexId> > p;
            //std::map<typename Vertex::VertexId, double> sigma;
            //std::map<typename Vertex::VertexId, double> d;
            //std::map<typename Vertex::VertexId, double> delta;

            initMap(g, s->getVertexId(), sigma, 0.0, 1.0);
            initMap(g, s->getVertexId(), d, -1.0, 0.0);
            initMap(g, s->getVertexId(), delta, 0.0, 0.0);

	    
	    pqueue =  dijkstraExplore(s, g);

             //S returns vertices in order of non-increasing distance from s
             while (!pqueue.empty())
             {

                    WeightedVertex* w = pqueue.pop();
		    
		    //predecesors
                    std::list<typename WeightedVertex::VertexId> vertices = p[w->getVertexId()];
                    typename std::list<typename WeightedVertex::VertexId>::iterator it;
		     
                    for (it = vertices.begin(); it != vertices.end(); ++it)
                    {
			
                        typename WeightedVertex::VertexId v = *it;
			double c = ( (1.0 + delta[w->getVertexId()]) * (sigma[v] / sigma[w->getVertexId()]) ); 
                        delta[v] = delta[v] + c;
                         					 
                    }
		    
                    if (w->getVertexId() != s->getVertexId())
                    {
			 betweenness[w->getVertexId()] += delta[w->getVertexId()];
                    }
              }
            
            ++iter;
        }
    }

    std::priority_queue<WeightedVertex*> dijkstraExplore(WeightedVertex& s ,WeightedGraph& g)
    {
	std::priority_queue<WeightedVertex*> S;
	std::priority_queue<WeightedVertex*> Q;
	initMap(g, s->getVertexId(), sigma, 0.0, 1.0);
	initMap(g, s->getVertexId(), d, 0.0, 0.0);
	initMap(g, s->getVertexId(), delta, 0.0, 0.0);
	Q.push(s);
	
	while(!Q.empty())
	{
		WeightedVertex* v = Q.pop();
		
		S.push(v);
                
		NeighbourIterator neighbourIter = v->neighborsIterator();

                while (!neighbourIter.end())
                {
                    WeightedVertex* w = *neighbourIter;

                    //w found for the first time?
		    double alt = d[v->getVertexId()] + v->edgeWeight(w);
                    double wValue = d[w->getVertexId()];
			
                    //double vValue = d[v->getVertexId()];
                    if (alt < wValue)
                    {
                        d[w->getVertexId()] = alt;
			//update priority w in S -> falta el contains
			S.pop(w);
			S.push(w);
			//update priority w in Q -> falta el contains
			Q.pop(w);
			Q.push(w);
			if (wValue > 0)
			{
				Q.push(w);
			}
                        sigma[w->getVertexId()] =  0.0;
			//clear predecesors of w
                    }
                    //shortest path to w via v?
                    if (d[w->getVertexId()] == alt )
                    {
                        sigma[w->getVertexId()] =  sigma[w->getVertexId()] + sigma[v->getVertexId()];
                        p[w->getVertexId()].push_back(v->getVertexId());
                    }

                    ++neighbourIter;
                }
		
	}
	return S;
	
    }

    void initMap(WeightedGraph& g, unsigned int vertexId, std::map<typename WeightedVertex::VertexId, double>& m,
                 double commonValue, double distinguishedValue)
    {
        VerticesIterator it = g.verticesIterator();

        //initialize all elements in zero, except for the current vertex
        while (!it.end())
        {
            WeightedVertex* v = *it;
            m.insert(std::pair<typename WeightedVertex::VertexId, double>(v->getVertexId(), commonValue));
	    ++it;
        }
        //modify the value associated to key 'vertexId' 
        m[vertexId]=distinguishedValue;
    }

    BetweennessContainer betweenness;
};
}

#endif
