#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <unordered_set>

using namespace std;

struct Node {
	int next;
	int dist;
};

struct Elem {
	int node;
	int weight;

	bool operator>(const Elem& other) const {
		return weight > other.weight;
	}
};


int N, K;
vector<vector<Node>> graph;

void init(int inputN, int inputK, int sBuilding[], int eBuilding[], int mDistance[]) {
	graph.clear();
	
	N = inputN;
	K = inputK;

	graph.resize(N);
	for (int i = 0; i < K; i++) {
		int s = sBuilding[i];
		int e = eBuilding[i];
		int d = mDistance[i];
		graph[s].push_back({ e, d });
		graph[e].push_back({ s, d });
	}	
	
	return;
}

void add(int sBuilding, int eBuilding, int mDistance) {
	graph[sBuilding].push_back({ eBuilding, mDistance });
	graph[eBuilding].push_back({ sBuilding, mDistance });

	return;
}

void dijkstra(priority_queue<Elem, vector<Elem>, greater<Elem>>& pque, vector<int>& dist, int R) {
	while (!pque.empty()) {
		auto now = pque.top();
		pque.pop();

		if (now.weight > dist[now.node])
			continue;

		for (int i = 0; i < graph[now.node].size(); i++) {
			int next = graph[now.node][i].next;
			int ndist = now.weight + graph[now.node][i].dist;
			if (ndist > R) {
				continue;
			}
			if (ndist < dist[next]) {
				dist[next] = ndist;
				pque.push({ next, ndist });
			}
		}
	}
}

int calculate(int M, int mCoffee[], int P, int mBakery[], int R) {
	vector<int> not_home(N, 0);

	priority_queue<Elem, vector<Elem>, greater<Elem>> coffee_pque;
	priority_queue<Elem, vector<Elem>, greater<Elem>> bakery_pque;
	vector<int> coffee_dist(N, INT_MAX);
	vector<int> bakery_dist(N, INT_MAX);

	for (int i = 0; i < M; i++) {
		int coffee_id = mCoffee[i];
		coffee_pque.push({ coffee_id, 0 });
		coffee_dist[coffee_id] = 0;
		
		not_home[coffee_id] = 1;
	}
	for (int i = 0; i < P; i++) {
		int bakery_id = mBakery[i];
		bakery_pque.push({ bakery_id, 0 });
		bakery_dist[bakery_id] = 0;

		not_home[bakery_id] = 1;
	}

	dijkstra(coffee_pque, coffee_dist, R);
	dijkstra(bakery_pque, bakery_dist, R);

	int min_dist = INT_MAX;
	for (int i = 0; i < N; i++) {
		if (not_home[i] == 1)
			continue;
		if (coffee_dist[i] == INT_MAX || bakery_dist[i] == INT_MAX)
			continue;

		int now_dist = coffee_dist[i] + bakery_dist[i];
		if (now_dist < min_dist) {
			min_dist = now_dist;
		}
	}

	if (min_dist == INT_MAX)
		return -1;
	else
		return min_dist;
}