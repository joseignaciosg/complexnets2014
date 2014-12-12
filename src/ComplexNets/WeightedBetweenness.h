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
	if ( v1->distance > v2->distance )
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
        if ( v1->distance < v2->distance )
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
		printPQ(Q);
                Vertex* v = Q.top();
		Q.pop();
                S.push(v);

                //iterate through v's neighbors
                NeighbourIterator neighbourIter = v->neighborsIterator();
		printf("source %i \n",v->getVertexId());

                while (!neighbourIter.end())
                {
                    //Vertex* w = *neighbourIter;
		    Vertex* w = static_cast<Vertex*>(*neighbourIter);
		    printf("neighbor %i \n",w->getVertexId());
		    double alt = v->distance + v->edgeWeight(w);
		    //printf("alt: %f\n",alt);
                    double wValue = w->distance;
		    //printf("wValue: %f\n",wValue);

                    //double vValue = d[v->getVertexId()];
		    printf("alt =  %f < wValue = %f \n", alt , wValue);
                    if ( alt < wValue )
                    {
			printf("INSIDE \n");
			w->distance = alt;
                        if ( wValue == -1  )
			{
			   Q.push(w);
			}
                        sigma[w->getVertexId()] =  0.0;
			//clearPredecessor2sOf(w);   
			p[w->getVertexId()].clear();;
	            }
                    //shortest path to w via v?
                    if ( w->distance == alt )
                    {
			sigma[w->getVertexId()] =  sigma[w->getVertexId()] + sigma[v->getVertexId()];
                        p[w->getVertexId()].push_back(v->getVertexId());
                        
                    }
		    
		    printf("distance: %f\n",w->distance);  
                    ++neighbourIter;
                }
                
	      }
	
	//debugging	
  	//while (!S.empty())
	//{
	//	std::cout << S.top()->getVertexId() << std::endl;
	//	S.pop();
	//}
                
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
 
    void printPQ(std::priority_queue<graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>*, std::vector<graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>*, std::allocator<graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>*> >, graphpp::BrandesNodeComparatorSmallerFirst>& S)
    {
	std::priority_queue<graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>*, std::vector<graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>*, std::allocator<graphpp::WeightedVertexAspect<graphpp::AdjacencyListVertex>*> >, graphpp::BrandesNodeComparatorSmallerFirst>  pq	= S;
	while (!pq.empty())
	{
		Vertex* v = pq.top();
		printf("[ ");
 		printf("%d , ", v->getVertexId() );
		printf("] \n "); 
		pq.pop();

	} 
    }

    BetweennessContainer betweenness;
};
}

#endif
