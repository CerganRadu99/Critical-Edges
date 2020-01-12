#include<iostream>
#include<fstream>
#include"header.h"
using namespace std;
ifstream fin("myGraph.txt");
int no_vertices;
int no_edges;
int x;
int y;
int z;
vector<Edge*> findCriticalEdges(priority_queue<Edge*, vector<Edge*>, LessThanByWeight> pq, vector<vector<pair<int, int>>> adj_list)
{
	//Modified Kruskal's algorithm
	vector<Edge*> criticalEdges;
	vector<Edge*> minimumSpanningTree;
	int nr = adj_list.size();
	UnionFind* unionfind = new UnionFind(nr);
	//Subgraph with components
	vector<vector<pair<int, int>>> componentsSubgraph(nr);
	while (!pq.empty() && minimumSpanningTree.size() < adj_list.size() - 2)
	{
		Edge* edge = pq.top();
		pq.pop();
		//the source of the edge
		int vertex1 = edge->source;
		//the destination of the edge
		int vertex2 = edge->destination;
		//avoid cycle in MST
		if (unionfind->connected(vertex1, vertex2))
		{
			continue;
		}
		int currentWeight = edge->weight;
		//create a set with all edges of weight currentWeight
		unordered_set<Edge*> equalWeightEdges;
		equalWeightEdges.insert(edge);
		//add all edges of weight currentWeight to the set
		while (!pq.empty())
		{
			if (pq.top()->weight == currentWeight)
			{
				equalWeightEdges.insert(pq.top());
				pq.pop();
			}
			else
			{
				break;
			}
		}
		//there is only one edge of weight currentWeight
		if (equalWeightEdges.size() == 1)
		{
			//There is no cycle,so this is a critical edge
			criticalEdges.push_back(edge);
			unionfind->Union(vertex1, vertex2);
			minimumSpanningTree.push_back(edge);
			continue;
		}
		//Multimap to make the mapping between edges in the components subgraph and the original graph
		multimap<Edge*, Edge*> subGraphToGraphEdgeMap;
		list<Edge*> edgesToAddToComponentsSubGraph;
		unordered_set<int> verticesInSubGraph;
		//Generate subgraph with the current components
		for (Edge* edgeInCurrentBlock : equalWeightEdges)
		{
			vertex1 = edgeInCurrentBlock->source;
			vertex2 = edgeInCurrentBlock->destination;
			int component1 = unionfind->find(vertex1);
			int component2 = unionfind->find(vertex2);
			Edge* subGraphEdge = new Edge(component1, component2, currentWeight);
			edgesToAddToComponentsSubGraph.push_back(subGraphEdge);
			subGraphToGraphEdgeMap.insert(pair<Edge*, Edge*>(subGraphEdge, edgeInCurrentBlock));
			verticesInSubGraph.insert(component1);
			verticesInSubGraph.insert(component2);
		}
		//Create a set with all non-critical edges of the subgraph
		unordered_set<Edge*> noncriticalEdges;
		//Every multiple edge and every self-loop is non-critical.
		for (Edge* edge : edgesToAddToComponentsSubGraph)
		{
			//self-loop
			if (edge->source == edge->destination)
			{
				for (auto jt : subGraphToGraphEdgeMap)
				{
					if ((jt.first->source == edge->source && jt.first->destination == edge->destination && jt.first->weight == edge->weight) || (jt.first->source == edge->destination && jt.first->destination == edge->source && jt.first->weight == edge->weight))
					{
						noncriticalEdges.insert(jt.second);
					}
				}
				continue;
			}
			pair<int, int> help = make_pair(edge->destination, edge->weight);
			pair<int, int> help2 = make_pair(edge->source, edge->weight);
			//multiple edge
			if (find(componentsSubgraph[edge->source].begin(), componentsSubgraph[edge->source].end(), help) != componentsSubgraph[edge->source].end())
			{
				for (auto jt : subGraphToGraphEdgeMap)
				{
					if ((jt.first->source == edge->source && jt.first->destination == edge->destination && jt.first->weight == edge->weight) || (jt.first->source == edge->destination && jt.first->destination == edge->source && jt.first->weight == edge->weight))
					{
						noncriticalEdges.insert(jt.second);
					}
				}

			}
			//multiple edge
			else if (find(componentsSubgraph[edge->destination].begin(), componentsSubgraph[edge->destination].end(), help2) != componentsSubgraph[edge->destination].end())
			{
				for (auto jt : subGraphToGraphEdgeMap)
				{
					if ((jt.first->source == edge->source && jt.first->destination == edge->destination && jt.first->weight == edge->weight) || (jt.first->source == edge->destination && jt.first->destination == edge->source && jt.first->weight == edge->weight))
					{
						noncriticalEdges.insert(jt.second);
					}
				}
				continue;

			}
			//normal edge
			else
			{
				componentsSubgraph[edge->source].push_back(make_pair(edge->destination, edge->weight));
			}
		}
		int no_elements = componentsSubgraph.size();
		//Find all bridges in the subgraph
		Bridge* sal = new Bridge(componentsSubgraph);
		vector<Edge*> myBridges;
		//Put all bridges of the subgraph in vector myBridges
		sal->getBridges(componentsSubgraph, myBridges);
		//Mark all edges that are no bridges in the subgraph as non-critical edges.
		for (auto it : edgesToAddToComponentsSubGraph)
		{
			if (find(myBridges.begin(), myBridges.end(), it) != myBridges.end())
			{
				for (auto jt : subGraphToGraphEdgeMap)
				{
					if ((jt.first->source == it->source && jt.first->destination == it->destination && jt.first->weight == it->weight) || (jt.first->source == it->destination && jt.first->destination == it->source && jt.first->weight == it->weight))
					{
						noncriticalEdges.insert(jt.second);
					}
				}
			}
			
		}
		//clear components subgraph edges
		for (int i = 1; i < no_elements; i++)
		{
			componentsSubgraph[i].clear();
		}
		//Mark all edges of weight w that are non-critical as critical edges
		for (auto edgeInCurrentBlock : equalWeightEdges)
		{
			if (find(noncriticalEdges.begin(), noncriticalEdges.end(), edgeInCurrentBlock) == noncriticalEdges.end())
			{
				criticalEdges.push_back(edgeInCurrentBlock);

			}
			vertex1 = edgeInCurrentBlock->source;
			vertex2 = edgeInCurrentBlock->destination;

			if (!unionfind->connected(vertex1, vertex2))
			{
				//make the union between vertices of edges(weight currentWeight)
				unionfind->Union(vertex1, vertex2);
				//add edges to MST
				minimumSpanningTree.push_back(edgeInCurrentBlock);
			}
		}
	}
	//return the vector with critical edges
	return criticalEdges;

}

int main()
{
	//Create priority_queue with all edges of the graph ordered increasingly by weight
	priority_queue<Edge*, vector<Edge*>, LessThanByWeight> pq;
	fin >> no_vertices >> no_edges;
	//Adjacency list of the graph
	vector<vector<pair<int, int>>> adj_list(no_vertices + 1);
	//Read all edges of the given graph and create adjacency list of the graph
	for (int edges = 1; edges <= no_edges; ++edges)
	{
		fin >> x >> y >> z;
		adj_list[x].push_back(make_pair(y, z));
		adj_list[y].push_back(make_pair(x, z));
		Edge* edge = new Edge(x, y, z);
		//Add edges into priority_queue
		pq.push(edge);
	}
	//Put all critical edges into a vector named "critical"
	vector<Edge*> critical = findCriticalEdges(pq, adj_list);
	//If the function marked all edges as critical that means the graph has no critical edges
	if (critical.size() == no_edges)
		cout << "This graph has no critical edges";
	else
	{
		cout << "The critical edges of this graph are" << endl;
		//Print critical edges of the given graph
		for (auto it : critical)
		{
			cout << it->source << " " << it->destination << " " << it->weight << endl;
		}
	}

	return 0;
}