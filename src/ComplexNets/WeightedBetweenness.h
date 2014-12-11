#ifndef WEIGHTEDBETWEENNESS_H
#define WEIGHTEDBETWEENNESS_H
#include <stack>
#include <queue>
#include <vector>
#include <list>

#include "mili/mili.h"
#include "IBetweenness.h"

namespace graphpp
{
class BrandesNodeComparatorLargerFirst {
    public:
    //TODO try this out
    bool operator()(graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>* v1, graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>* v2)
    {
	if ( v1->distance > v1->distance )
	{
		return true;
	}
       return false;
    }
};

class BrandesNodeComparatorSmallerFirst {
    public:
    //TODO try this out
    bool operator()(graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>* v1, graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>* v2)
    {
        if ( v1->distance < v1->distance )
        {
                return true;
	}
       return false;
    }
};

template <class Graph, class Vertex>
class WeightedBetweenness : public IBetweenness<Graph, Vertex>
{
public:

    typedef typename IBetweenness<Graph, Vertex>::BetweennessContainer BetweennessContainer;
    typedef typename IBetweenness<Graph, Vertex>::BetweennessIterator BetweennessIterator;
    typedef typename Graph::VerticesIterator VerticesIterator;
    typedef typename Vertex::VerticesIterator NeighbourIterator;


    WeightedBetweenness(Graph& g)
    {
	printf("INSIDE WEIGHTEDBETWEENNESS\n");
        initMap(g, 1, betweenness, 0.0, 0.0);
        calculateBetweenness(g);
    }

    virtual BetweennessIterator iterator()
    {
        return BetweennessIterator(betweenness);
    }

private:

    void calculateBetweenness(Graph& g)
    {
	printf("CALCULATING BETWEENNESS INSIDE WEIGHTEDBETWEENNESS\n");
        VerticesIterator iter = g.verticesIterator();

        while (!iter.end())
        {
            Vertex* s = *iter;
	    std::priority_queue<Vertex*, std::vector<Vertex*>, BrandesNodeComparatorLargerFirst> S;
            std::priority_queue<Vertex*, std::vector<Vertex*>, BrandesNodeComparatorSmallerFirst> Q;
            std::map<typename Vertex::VertexId, std::list<typename Vertex::VertexId> > p;
            std::map<typename Vertex::VertexId, double> sigma;
            std::map<typename Vertex::VertexId, double> d;
            std::map<typename Vertex::VertexId, double> delta;

            initMap(g, s->getVertexId(), sigma, 0.0, 1.0);
            initMap(g, s->getVertexId(), delta, 0.0, 0.0);
	    initDistances(g, -1.0);
	    s->distance=0.0;

            Q.push(s);

            while (!Q.empty())
            {
                Vertex* v = Q.top();
		Q.pop();
                S.push(v);

                //iterate through v's neighbors
                NeighbourIterator neighbourIter = v->neighborsIterator();

                while (!neighbourIter.end())
                {
                    //Vertex* w = *neighbourIter;
		    Vertex* w = static_cast<Vertex*>(*neighbourIter);
		    printf("distance: %f\n",w->distance);
		    double aux = w->distance;
		    w->distance = 3.0;
		    printf("distance2: %f\n",w->distance);
	            w->distance = aux;
                    //w found for the first time?
		    double alt = w->distance + v->edgeWeight(w);
                    double wValue = w->distance;

                    //double vValue = d[v->getVertexId()];
                    if ( alt < wValue )
                    {
			w->distance = alt;
                        if ( wValue == -1  )
			{
			   Q.push(w);
			}
                        sigma[w->getVertexId()] =  0.0;
			//clearPredecessorsOf(w);   
			p[w->getVertexId()] = {};
	            }
                    //shortest path to w via v?
                    if ( w->distance == alt )
                    {
			sigma[w->getVertexId()] =  sigma[w->getVertexId()] + sigma[v->getVertexId()];
                        p[w->getVertexId()].push_back(v->getVertexId());
                        
                    }
		      
                    ++neighbourIter;
                }
                
	      }
	
	//debugging	
  	while (!S.empty())
	{
		std::cout << S.top()->getVertexId() << std::endl;
		S.pop();
	}
                
                //S returns vertices in order of non-increasing distance from s
             while (!S.empty())
                {

                    Vertex* w = S.top();
                    S.pop();
		    
                    std::list<typename Vertex::VertexId> vertices = p[w->getVertexId()];
                    typename std::list<typename Vertex::VertexId>::iterator it;
		     
                    for (it = vertices.begin(); it != vertices.end(); ++it)
                    {
			
                        typename Vertex::VertexId v = *it;
                        delta[v] = delta[v] + ( (1 + delta[w->getVertexId()]) * (sigma[v] / sigma[w->getVertexId()]) );
                         					 
                    }
		    
                    if (w->getVertexId() != s->getVertexId())
                    {
			 betweenness[w->getVertexId()] += delta[w->getVertexId()];
                    }
                }
            
            ++iter;
        }
    }

    void initMap(Graph& g, unsigned int vertexId, std::map<typename Vertex::VertexId, double>& m,
                 double commonValue, double distinguishedValue)
    {
        VerticesIterator it = g.verticesIterator();

        //initialize all elements in zero, except for the current vertex
        while (!it.end())
        {
            Vertex* v = *it;
            m.insert(std::pair<typename Vertex::VertexId, double>(v->getVertexId(), commonValue));
	    ++it;
        }
        //modify the value associated to key 'vertexId' 
        m[vertexId]=distinguishedValue;
    }

    void initDistances(Graph& g,double commonValue)
    {
        VerticesIterator it = g.verticesIterator();
        while (!it.end())
        {
            Vertex* v = *it;
	    v->distance = commonValue;
            ++it;
        }
    }

    BetweennessContainer betweenness;
};
}

#endif
