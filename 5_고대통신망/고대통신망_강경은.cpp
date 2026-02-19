#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <unordered_set>
#include <algorithm>

using namespace std;

struct Node {
	int node;
	int weight;
	int id;
};

struct Elem {
	int node;
	int weight;

	bool operator>(const Elem& other) const {
		return weight > other.weight;
	}
};

int N, capital;
vector<vector<Node>> graph;
vector<vector<Node>> way;
unordered_set<int> deleted;

vector<int> max_ways;
vector<int> route;


void init(int inputN, int mCapital, int K, int mId[], int sCity[], int eCity[], int mDistance[]) {
	graph.clear();
	way.clear();
	deleted.clear();

	N = inputN;
	capital = mCapital;

	graph.resize(N);

	for (int i = 0; i < K; i++) {
		graph[sCity[i]].push_back({ eCity[i], mDistance[i], mId[i] });
	}
	
	return;
}

void add(int mId, int sCity, int eCity, int mDistance) {
	graph[sCity].push_back({ eCity, mDistance, mId });
	return;
}

void remove(int mId) {
	// lazy로 할까
	deleted.insert(mId);
	return;
}

void get_max_way(int dest) {
	if (dest == capital) {
		int max_way = *max_element(route.begin(), route.end());
		max_ways.push_back(max_way);
		return;
	}

	for (int i = 0; i < way[dest].size(); i++) {
		//graph[way[dest][i]]에서 dest값을 가진 것의 weight
		int now_way = way[dest][i].weight;
		route.push_back(now_way);
		get_max_way(way[dest][i].node);
		route.pop_back();
	}
}

int calculate(int mCity) {
	way.clear();
	way.resize(N);

	max_ways.clear();
	route.clear();

	priority_queue<Elem, vector<Elem>, greater<>> pque;
	vector<int> dist(N, INT_MAX);

	pque.push({ capital, 0 });
	dist[capital] = 0;

	while (!pque.empty()) {
		Elem now = pque.top();
		pque.pop();

		if (now.weight > dist[now.node])
			continue;

		for (int i = 0; i < graph[now.node].size(); i++) {
			Node next = graph[now.node][i];
			if (deleted.count(next.id))
				continue;

			int ndist = now.weight + next.weight;
			if (ndist == dist[next.node]) {
				// 일로 온 경로 저장
				way[next.node].push_back({ now.node, next.weight });
			}
			else if (ndist < dist[next.node]) {
				// 저장되어있던 경로 리셋 후 새로운 것만 저장
				way[next.node].clear();
				way[next.node].push_back({ now.node, next.weight });
				pque.push({ next.node, ndist });
				dist[next.node] = ndist;
			}
		}
	}

	if (dist[mCity] == INT_MAX)
		return -1;
	
	get_max_way(mCity);
	int min_way = *min_element(max_ways.begin(), max_ways.end());

	return min_way;
}