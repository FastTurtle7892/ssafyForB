#include <iostream>
#include <unordered_map>
#include <cstring>
 
using namespace std;
 
// segment tree
 
const int MAX_TIME = 2'000'000;
 
int startTree[MAX_TIME * 4 + 1];
int endTree[MAX_TIME * 4 + 1];
 
struct Time {
    int start_time;
    int end_time;
};
 
int music_start_time;
unordered_map<int, Time> time_map;
 
 
void update(int *nowTree, int node, int start, int end, int idx, int val) {
 
    if (idx < start || idx > end) return;
 
    nowTree[node] += val;
    if (start != end) {
 
        int mid = (start + end) / 2;
        update(nowTree, node * 2, start, mid, idx, val);
        update(nowTree, node * 2 + 1, mid + 1, end, idx, val);
    }
}
 
int check(int* nowTree, int node, int start, int end, int left, int right) {
 
    if (left > end || right < start) return 0;
 
    if (left <= start && end <= right) return nowTree[node];
 
    else {
 
        int mid = (start + end) / 2;
        return check(nowTree, node * 2, start, mid, left, right) +
            check(nowTree, node * 2 + 1, mid + 1, end, left, right);
    }
}
 
void init(int musicTime) {
 
    time_map.clear();
    music_start_time  = musicTime;
    memset(startTree, 0, sizeof(startTree));
    memset(endTree, 0, sizeof(endTree));
}
 
void add(int mID, int mStart, int mEnd) {
 
    if (time_map.find(mID) != time_map.end()) {
 
        int prevStart = time_map[mID].start_time;
        int prevEnd = time_map[mID].end_time;
        update(startTree, 1, 1, MAX_TIME, prevStart, -1);
        update(endTree, 1, 1, MAX_TIME, prevEnd, -1);
    }
    time_map[mID] = { mStart, mEnd };
    update(startTree, 1, 1, MAX_TIME, mStart, 1);
    update(endTree, 1, 1, MAX_TIME, mEnd, 1);
}
 
void remove(int mID) {
 
    if (time_map.find(mID) != time_map.end()) {
 
        int prevStart = time_map[mID].start_time;
        int prevEnd = time_map[mID].end_time;
        update(startTree, 1, 1, MAX_TIME, prevStart, -1);
        update(endTree, 1, 1, MAX_TIME, prevEnd, -1);
        time_map.erase(mID); 
    }
}
 
int getCnt(int mBSTime) {
 
    int start_cnt = check(startTree, 1, 1, MAX_TIME, 1, mBSTime);
    int end_cnt = check(endTree, 1, 1, MAX_TIME, 1, mBSTime + music_start_time - 1);
    return start_cnt - end_cnt;
}