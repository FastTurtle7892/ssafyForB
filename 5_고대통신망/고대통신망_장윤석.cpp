#include <iostream>
#include <queue>
#include <vector>
#include <climits>
#include <unordered_map>
#include <algorithm>

#define MAX_NODE 5001

using namespace std;

struct Info {
	int mId;
	int now_node;
	int cost;
};

struct Info2 {
	int now_node;
	int cost;
	int max_e;

	bool operator < (const Info2& next) const {
		if (cost != next.cost) return cost > next.cost;
		return max_e > next.max_e;
	}
};

int NN;
int start_node;
vector<Info> graph[MAX_NODE];
unordered_map<int, pair<int, int>> mId_table;

int dist[MAX_NODE];
int max_edge[MAX_NODE];
int parent_node[MAX_NODE];

priority_queue<Info2> pq;

bool is_dirty = false;

void run_dijkstra() {
	while (!pq.empty()) {
		Info2 now = pq.top();
		pq.pop();

		if (dist[now.now_node] < now.cost) continue;
		if (dist[now.now_node] == now.cost && max_edge[now.now_node] < now.max_e) continue;

		for (const Info& next : graph[now.now_node]) {
			int nextcost = now.cost + next.cost;
			int nextmaxedge = max(now.max_e, next.cost);

			if (nextcost < dist[next.now_node]) {
				dist[next.now_node] = nextcost;
				max_edge[next.now_node] = nextmaxedge;
				parent_node[next.now_node] = now.now_node;
				pq.push({ next.now_node, nextcost, nextmaxedge });
			}
			else if (nextcost == dist[next.now_node]) {
				if (nextmaxedge < max_edge[next.now_node]) {
					max_edge[next.now_node] = nextmaxedge;
					parent_node[next.now_node] = now.now_node;
					pq.push({ next.now_node, nextcost, nextmaxedge });
				}
			}
		}
	}
}

void init(int N, int mCapital, int K, int mId[], int sCity[], int eCity[], int mDistance[]) {
	NN = N;
	start_node = mCapital;
	mId_table.clear();
	mId_table.reserve(K);

	while (!pq.empty()) pq.pop();

	for (int i = 0; i < MAX_NODE; i++) {
		graph[i].clear();
		dist[i] = INT_MAX;
		max_edge[i] = INT_MAX;
		parent_node[i] = -1;
	}

	for (int i = 0; i < K; i++) {
		int u = sCity[i];
		int idx = (int)graph[u].size();
		graph[u].push_back({ mId[i], eCity[i], mDistance[i] });
		mId_table[mId[i]] = { u, idx };
	}

	dist[start_node] = 0;
	max_edge[start_node] = 0;
	parent_node[start_node] = -1;
	pq.push({ start_node, 0, 0 });

	run_dijkstra();
	is_dirty = false;
}

void add(int mId, int sCity, int eCity, int mDistance) {
	int idx = (int)graph[sCity].size();
	graph[sCity].push_back({ mId, eCity, mDistance });
	mId_table[mId] = { sCity, idx };

	if (dist[sCity] != INT_MAX) {
		pq.push({ sCity, dist[sCity], max_edge[sCity] });
	}
}

void remove(int mId) {
	auto it = mId_table.find(mId);
	if (it == mId_table.end()) return;

	int u = it->second.first;
	int idx = it->second.second;

	int v = graph[u][idx].now_node;

	vector<Info>& edges = graph[u];
	int last_idx = (int)edges.size() - 1;
	if (idx != last_idx) {
		Info last_element = edges.back();
		edges[idx] = last_element;
		mId_table[last_element.mId] = { u, idx };
	}
	edges.pop_back();
	mId_table.erase(it);

	if (parent_node[v] == u) {
		is_dirty = true;
	}
}

int calculate(int mCity) {
	if (is_dirty) {
		while (!pq.empty()) pq.pop();

		for (int i = 0; i <= NN; i++) {
			dist[i] = INT_MAX;
			max_edge[i] = INT_MAX;
			parent_node[i] = -1;
		}

		dist[start_node] = 0;
		max_edge[start_node] = 0;
		pq.push({ start_node, 0, 0 });

		run_dijkstra();
		is_dirty = false;
	}
	else {
		run_dijkstra();
	}

	if (dist[mCity] == INT_MAX) return -1;
	return max_edge[mCity];
}