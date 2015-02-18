#ifndef SHELL_INDEX_H
#define SHELL_INDEX_H

#include "IShellIndex.h"
#include <iostream>
#include <fstream>
#include <map>
#include <list>
namespace graphpp {

template<class Graph, class Vertex>
class ShellIndex: public IShellIndex<Graph, Vertex> {

	// Using int instead of unsigned int to avoid overflows when reducing the degrees to a negative value.

	typedef std::map<Vertex*, int> VertexCurrentDegreeMap;
	typedef typename std::map<Vertex*, int>::iterator VertexCurrDegreeMapIterator;

	typedef std::multimap<int, Vertex*> CurrentDegreeOrderedSet;
	typedef typename std::multimap<int, Vertex*>::iterator VertexCurrDegreeIterator;

public:

	typedef typename graphpp::IShellIndex<Graph, Vertex>::ShellIndexContainer ShellIndexContainer;
	typedef typename graphpp::IShellIndex<Graph, Vertex>::ShellIndexIterator ShellIndexIterator;

	ShellIndex(Graph& g) {
		initMap(g);
		initMultimapSet(g);
		calculateShellIndex();
	}

	virtual ShellIndexIterator iterator() {
		return ShellIndexIterator(shellIndex);
	}

private:

	void printCurrentDegreeSet() {
		std::ofstream outfile;
		outfile.open("log.txt", std::ios_base::app);
		outfile << "----------------------------\nPrinting the current degree ordered set:\n";
		VertexCurrDegreeIterator printIt = currentDegreesSet.begin();
		while (printIt != currentDegreesSet.end()) {
			outfile << "Parsing: " << printIt->second->getVertexId()
					<< "->" << printIt->first << "\n";
			printIt++;
		}
		outfile << "----------------------------\n";
		outfile.flush();
		outfile.close();
	}

	void printVertexCurrentDegreeMap(){
		std::ofstream outfile;
		outfile.open("log.txt", std::ios_base::app);
		outfile << "___________________________\nPrinting the map:\n";
		VertexCurrDegreeMapIterator setIt = currentDegreesMap.begin();
		while (setIt != currentDegreesMap.end()) {
			outfile << "Parsing: " << setIt->first->getVertexId()
					<< "->" << setIt->second << "\n";
			setIt++;
		}
		outfile << "___________________________\n";
		outfile.flush();
		outfile.close();
	}

	void deleteVertexFromGraph(int degree, Vertex* v) {
		std::ofstream outfile;
		if(debug){
			outfile.open("log.txt", std::ios_base::app);
			outfile << "Delete Iteration started\n";
			outfile.flush();
		}
		std::pair<VertexCurrDegreeIterator, VertexCurrDegreeIterator> iterpair = currentDegreesSet.equal_range(degree);
		VertexCurrDegreeIterator j;
		for (j = iterpair.first; j != iterpair.second; j++) {
			if ((j->second) == v) {
				if (debug) {
					outfile << "Removing from the current degree set: " << v->getVertexId() << "\n";
					outfile.flush();
				}
				currentDegreesSet.erase(j);
				break;
			}
		}
		if (debug) {
			outfile << "Delete Iteration finished\n";
			outfile.flush();
			outfile.close();
		}
	}

	void calculateShellIndex() {
		std::ofstream outfile;
		if (debug) {
			outfile.open("log.txt", std::ios_base::app);
			outfile << "Init ShellIndex" << "\n";
			outfile.flush();
		}

		// FYI: Not using iterators for the big cylce, the elements are removed and reinserted to ensure
		// 		the correct order, so using iterators will create problems, since the collection is being modified.

		int k = 0;
		// While there are remaining vertices in the graph
		while (!currentDegreesSet.empty()) {
			if (debug) {
				printCurrentDegreeSet();
				printVertexCurrentDegreeMap();
			}

			// Select the vertex with the lowest current degree
			int degree = currentDegreesSet.begin()->first;
			Vertex* v = currentDegreesSet.begin()->second;

			if (debug) {
				outfile << "Parsing: " << v->getVertexId() << "->" << v->degree() << "\n";
				outfile.flush();
			}
			// If the degree is higher than k, then we have a new higher k-core
			// If lower, then it belongs to the actual k-core
			if (degree > k) {
				k = degree;
				if (debug) {
					outfile << "K changed to: " << k << "\n";
					outfile.flush();
				}
			}
			// Setting the coreness as the current k.
			shellIndex[v->getVertexId()] = k;

			// Will remove the vertex from the graph, since the coreness is already calculated
			deleteVertexFromGraph(degree, v);

			if (debug) {
				outfile << "SETTING K VALUE OF " << k << " TO V=" << v->getVertexId() << "\n";
				outfile.flush();
			}

			// Setting the vertex's degree to 0 in the map relation, to show it is no longer considered
			currentDegreesMap[v] = 0;

			NeighbourConstIterator neighborsIt = v->neighborsConstIterator();
			// Iterate through each of it's neighbors to reduce their degree by 1, since v was removed.
			while (!neighborsIt.end()) {

				Vertex * neigh = *neighborsIt;
				int neighDegree = currentDegreesMap[neigh];
				currentDegreesMap[neigh]--; // Reduce the degree on the map
				neighDegree--;

				//TODO: I probably could ignore it completely if the neighDegree < 0

				if (debug) {
					outfile << "	Parsing neighbor: " << neigh->getVertexId()
							<< " with current val: " << neighDegree << "\n";
					outfile.flush();
				}

				// Deleting the vertex from the set since its current degree changed
				// The ideal position of that element changed, so to avoid f*king up the order
				// we are gonna remove it and add it again if necessary.
				deleteVertexFromGraph(neighDegree+1, neigh); // It was present with the previous degree value
															 // the one it had before we reduced it.

				// If it still has a valid degree, we reinsert it to get it to the correct position
				// (remember this is an ordered set).
				if (neighDegree > 0) {
					if (debug) {
						outfile << "	Re-inserting\n";
						outfile.flush();
					}
					currentDegreesSet.insert(std::pair<int, Vertex*>(neighDegree, neigh));
				} else if (neighDegree == 0) {
					// If with this update we made the degree 0, then the vertex should be removed
					// and the shell index is the same as the current one.
					shellIndex[neigh->getVertexId()] = k;
					if (debug) {
						outfile << "SETTING K VALUE OF " << k << " TO V=" << neigh->getVertexId() << "\n";
						outfile.flush();
					}
					// Setting the vertex's degree to 0 in the map relation, to show it is no longer considered
					currentDegreesMap[neigh] = 0;
					// Not readding it to the set, it has current degree 0, no longer in the graph.
				}
				neighborsIt++;
			}
		}
		if (debug) {
			outfile.close();
		}
	}

	void initMap(Graph& g) {
		VerticesConstIterator it = g.verticesConstIterator();

		//initialize all elements in zero
		while (!it.end()) {
			Vertex* v = *it;
			shellIndex.insert(std::pair<typename Vertex::VertexId, unsigned int>(v->getVertexId(), 0));
			++it;
		}
	}

	void initMultimapSet(Graph& g) {
		VerticesConstIterator it = g.verticesConstIterator();

		std::ofstream outfile;
		if (debug) {
			outfile.open("log.txt", std::ios_base::app);
			outfile << "Init MultimapSet" << "\n";
		}
		//initialize all elements using the vertex id and the vertex degree
		while (!it.end()) {
			Vertex* v = *it;
			currentDegreesSet.insert(std::pair<int, Vertex*>(v->degree(), v));
			currentDegreesMap.insert(std::pair<Vertex*, int>(v, v->degree()));
			if (debug) {
				outfile << "Inserting: " << v->getVertexId() << "->" << v->degree() << "\n";
			}
			++it;
		}
		if (debug) {
			outfile.close();
		}
	}

	typedef typename Graph::VerticesConstIterator VerticesConstIterator;
	typedef typename Vertex::VerticesConstIterator NeighbourConstIterator;

	// This container will be set with the coreness of each vertex when found.
	ShellIndexContainer shellIndex;
	// This map relates vertexes with the current degree, to be able to fetch them from the
	// degreeOrderedSet and perform operations in ~O(1). When taken away from the set, they will
	// be set to 0.
	VertexCurrentDegreeMap currentDegreesMap;

	// This ordered set will contain the vertexes currently in the ideal graph, ordered by their
	// current degree. When a vertex is removed, all the vertexes connecting to itself will lower
	// their degree by 1.
	CurrentDegreeOrderedSet currentDegreesSet;

	// Wanna debug? set debug = true and enjoy.
	bool debug = false;
};
}

#endif
