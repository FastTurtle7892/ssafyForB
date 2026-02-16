#include <cstring>

const int MAX_N = 5005;
const int MAX_E = 30005;
const int MAX_HEAP = 500000;
const int HASH_SIZE = 65537;
const int INF = 0x7FFFFFFF;

struct FastHashMap {
    int keys[HASH_SIZE];
    int values[HASH_SIZE];

    inline void clear() {
        memset(keys, -1, sizeof(keys));
    }

    inline unsigned int hash(int k) {
        return ((unsigned int)k * 2654435761u) % HASH_SIZE;
    }

    inline void insert(int k, int v) {
        unsigned int h = hash(k);
        while (keys[h] != -1 && keys[h] != k) {
            h = (h + 1) % HASH_SIZE;
        }
        keys[h] = k;
        values[h] = v;
    }

    inline int find(int k) {
        unsigned int h = hash(k);
        while (keys[h] != -1) {
            if (keys[h] == k) return values[h];
            h = (h + 1) % HASH_SIZE;
        }
        return -1;
    }
} edgeMap;

struct Edge {
    int to, dist, id, next;
} edges[MAX_E];

int head[MAX_N];
int edge_cnt;

static int N, capital;
static int heap[MAX_HEAP << 1];
static int heapSize;

static int bestKey[MAX_N];   
static bool is_dirty;       

inline void heapPush(int key, int node) {
    int pos = ++heapSize;
    while (pos > 1) {
        int p = pos >> 1;
        if (heap[p << 1] <= key) break;
        heap[pos << 1] = heap[p << 1];
        heap[(pos << 1) | 1] = heap[(p << 1) | 1];
        pos = p;
    }
    heap[pos << 1] = key;
    heap[(pos << 1) | 1] = node;
}

inline bool heapPop(int& key, int& node) {
    if (heapSize == 0) return false;
    key = heap[2]; node = heap[3];
    int lastKey = heap[heapSize << 1];
    int lastNode = heap[(heapSize << 1) | 1];
    heapSize--;

    int pos = 1, child;
    while ((child = pos << 1) <= heapSize) {
        if (child < heapSize && heap[(child + 1) << 1] < heap[child << 1])
            child++;
        if (lastKey <= heap[child << 1]) break;
        heap[pos << 1] = heap[child << 1];
        heap[(pos << 1) | 1] = heap[(child << 1) | 1];
        pos = child;
    }
    heap[pos << 1] = lastKey;
    heap[(pos << 1) | 1] = lastNode;
    return true;
}

void runDijkstra() {
    while (heapSize > 0) {
        int curKey, u;
        heapPop(curKey, u);

        if (curKey > bestKey[u]) continue;

        int d = curKey >> 7;
        int m = curKey & 127;

        for (int i = head[u]; i != -1; i = edges[i].next) {
            if (edges[i].id == -1) continue;

            int v = edges[i].to;
            int w = edges[i].dist;

            int nDist = d + w;
            int nMax = (m > w) ? m : w;

            int nKey = (nDist << 7) | nMax;

            if (nKey < bestKey[v]) {
                bestKey[v] = nKey;
                heapPush(nKey, v);
            }
        }
    }
}

void init(int n, int mCapital, int K, int mId[], int sCity[], int eCity[], int mDistance[]) {
    N = n;
    capital = mCapital;
    edge_cnt = 0;

    memset(head, -1, sizeof(int) * N);
    edgeMap.clear();

    for (int i = 0; i < K; ++i) {
        edges[edge_cnt] = { eCity[i], mDistance[i], mId[i], head[sCity[i]] };
        edgeMap.insert(mId[i], edge_cnt);
        head[sCity[i]] = edge_cnt++;
    }

    is_dirty = true;
}

void add(int mId, int sCity, int eCity, int mDistance) {
    edges[edge_cnt] = { eCity, mDistance, mId, head[sCity] };
    edgeMap.insert(mId, edge_cnt);
    head[sCity] = edge_cnt++;

    if (is_dirty) return;

    if (bestKey[sCity] == INF) return;

    int uKey = bestKey[sCity];
    int uDist = uKey >> 7;
    int uMax = uKey & 127;

    int nDist = uDist + mDistance;
    int nMax = (uMax > mDistance) ? uMax : mDistance;
    int nKey = (nDist << 7) | nMax;

    if (nKey < bestKey[eCity]) {
        bestKey[eCity] = nKey;
        heapSize = 0;    
        heapPush(nKey, eCity);
        runDijkstra();       
    }
}

void remove(int mId) {
    int idx = edgeMap.find(mId);
    if (idx != -1) {
        edges[idx].id = -1; 
        is_dirty = true;
    }
}

int calculate(int mCity) {
    if (mCity == capital) return 0;

    if (is_dirty) {
        for (int i = 0; i < N; ++i) bestKey[i] = INF;
        bestKey[capital] = 0;
        heapSize = 0;
        heapPush(0, capital);
        runDijkstra();
        is_dirty = false;
    }

    if (bestKey[mCity] == INF) return -1;

    return bestKey[mCity] & 127;
}