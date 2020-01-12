#pragma once
#include<iostream>
#include<fstream>
#include<queue>
#include<vector>
#include<set>
#include<stack>
#include<list>
#include<map>
#include<unordered_set>
#include<algorithm>
using namespace std;
//All edges are objects, having as data-member source, destination and weight of an edge
class Edge
{
public:
	int source;
	int destination;
	int weight;
	Edge()
	{

	}
	Edge(int source, int destination, int weight)
	{
		this->source = source;
		this->destination = destination;
		this->weight = weight;
	}
};
class UnionFind {
private:
	int* leaders;
	int* ranks;
	int components;
public:
	UnionFind(int size)
	{
		leaders = new int[size];
		ranks = new int[size];
		components = size - 1;
		for (int i = 1; i < size; i++) {
			leaders[i] = i;
			ranks[i] = 0;
		}
	}
	//Return the number of components of the subgraph
	int count()
	{
		return components;
	}
	//Check if two vertices are connected or not
	bool connected(int site1, int site2)
	{
		return find(site1) == find(site2);
	}
	//Find the subset of an element
	int find(int site)
	{
		if (site == leaders[site])
		{
			return site;
		}
		return leaders[site] = find(leaders[site]);
	}
	//Union of two subsets
	void Union(int site1, int site2)
	{
		int leader1 = find(site1);
		int leader2 = find(site2);
		if (leader1 == leader2)
		{
			return;
		}
		if (ranks[leader1] < ranks[leader2])
		{
			leaders[leader1] = leader2;
		}
		else if (ranks[leader2] < ranks[leader1])
		{
			leaders[leader2] = leader1;
		}
		else
		{
			leaders[leader1] = leader2;
			ranks[leader2]++;
		}
		components--;
	}
};
//This allows us to order the edges by weight
struct LessThanByWeight
{
	bool operator()(const Edge* a, const Edge* b) const
	{
		return a->weight > b->weight;
	}
};
//Class to find bridges in a graph
class Bridge {
public:
	int bridges;
	int cnt;
	int* pre;
	int* low;
	//Constructor to initialize all data-members
	Bridge(vector<vector<pair<int, int>>> aux)
	{
		vector<Edge*> myBridges;
		int nr = aux.size();
		low = new int[aux.size()];
		pre = new int[aux.size()];
		for (int v = 1; v < nr; v++)
			low[v] = -1;
		for (int v = 1; v < nr; v++)
			pre[v] = -1;
	}
	//Function that has a vector passed by reference to store all bridges from the given graph
	void getBridges(vector<vector<pair<int, int>>> aux, vector<Edge*>& myBridges)
	{
		int nr = aux.size();
		for (int v = 1; v < nr; v++)
		{
			if (pre[v] == -1)
				dfs(aux, v, v, myBridges);
		}
	}
	//Return the number of bridges in the graph
	int components()
	{
		return bridges + 1;
	}
	//DFS to find all bridges in the graph
	void dfs(vector<vector<pair<int, int>>> aux, int u, int v, vector<Edge*>& myBridges)
	{
		pre[v] = cnt++;
		low[v] = pre[v];
		for (auto w : aux[v])
		{
			if (pre[w.first] == -1)
			{
				dfs(aux, v, w.first, myBridges);
				low[v] = min(low[v], low[w.first]);
				if (low[w.first] == pre[w.first])
				{
					bridges++;
					Edge* help = new Edge(v, w.first, w.second);
					myBridges.push_back(help);
				}
			}
			else if (w.first != u)
				low[v] = min(low[v], pre[w.first]);
		}

	}
};
