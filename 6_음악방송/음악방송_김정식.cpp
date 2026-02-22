#include <map>
#include <vector>
#include <algorithm>
using namespace std;

static int gMusicTime;

struct EmpInfo {
    int start;
    int end;
};

static map<int, EmpInfo> empMap;

static const int MAXVAL = 1700001;
static const int MAXNODES = 400000;

struct Node {
    int left, right;
    vector<int> ends;
};

static Node nodes[MAXNODES];
static int nodeCnt;
static int root;

int newNode() {
    int idx = ++nodeCnt; // 1-indexed: 0은 null
    nodes[idx].left = 0;
    nodes[idx].right = 0;
    nodes[idx].ends.clear();
    return idx;
}

void seg_add(int node, int s, int e, int pos, int val) {
    vector<int>::iterator it = lower_bound(nodes[node].ends.begin(), nodes[node].ends.end(), val);
    nodes[node].ends.insert(it, val);
    if (s == e) return;
    int mid = (s + e) / 2;
    if (pos <= mid) {
        if (!nodes[node].left) nodes[node].left = newNode();
        seg_add(nodes[node].left, s, mid, pos, val);
    }
    else {
        if (!nodes[node].right) nodes[node].right = newNode();
        seg_add(nodes[node].right, mid + 1, e, pos, val);
    }
}

void seg_remove(int node, int s, int e, int pos, int val) {
    vector<int>::iterator it = lower_bound(nodes[node].ends.begin(), nodes[node].ends.end(), val);
    if (it != nodes[node].ends.end() && *it == val)
        nodes[node].ends.erase(it);
    if (s == e) return;
    int mid = (s + e) / 2;
    if (pos <= mid) {
        if (nodes[node].left)
            seg_remove(nodes[node].left, s, mid, pos, val);
    }
    else {
        if (nodes[node].right)
            seg_remove(nodes[node].right, mid + 1, e, pos, val);
    }
}

int seg_query(int node, int s, int e, int ql, int qr, int minEnd) {
    if (!node) return 0;
    if (ql <= s && e <= qr) {
        return (int)(nodes[node].ends.end() -
            lower_bound(nodes[node].ends.begin(), nodes[node].ends.end(), minEnd));
    }
    int mid = (s + e) / 2;
    int cnt = 0;
    if (ql <= mid) cnt += seg_query(nodes[node].left, s, mid, ql, qr, minEnd);
    if (qr > mid) cnt += seg_query(nodes[node].right, mid + 1, e, ql, qr, minEnd);
    return cnt;
}

void init(int musicTime)
{
    gMusicTime = musicTime;
    empMap.clear();
    for (int i = 1; i <= nodeCnt; i++) {
        nodes[i].left = 0;
        nodes[i].right = 0;
        nodes[i].ends.clear();
    }
    nodeCnt = 0;
    root = newNode();
}

void add(int mID, int mStart, int mEnd)
{
    map<int, EmpInfo>::iterator it = empMap.find(mID);
    if (it != empMap.end()) {
        seg_remove(root, 0, MAXVAL, it->second.start, it->second.end);
        it->second.start = mStart;
        it->second.end = mEnd;
    }
    else {
        EmpInfo info;
        info.start = mStart;
        info.end = mEnd;
        empMap[mID] = info;
    }
    empMap[mID].start = mStart;
    empMap[mID].end = mEnd;
    seg_add(root, 0, MAXVAL, mStart, mEnd);
}

void remove(int mID)
{
    map<int, EmpInfo>::iterator it = empMap.find(mID);
    if (it == empMap.end()) return;
    seg_remove(root, 0, MAXVAL, it->second.start, it->second.end);
    empMap.erase(it);
}

int getCnt(int mBSTime)
{
    int bsEnd = mBSTime + gMusicTime;
    return seg_query(root, 0, MAXVAL, 0, mBSTime, bsEnd);
}