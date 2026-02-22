#include <iostream>
#include <unordered_map>
#include <cstring>
#define debug 
//#define debug printf
using namespace std;
 
const int MAX_TIME = 2000000;
const int MAX_EMP = 5000;
int time_slice;
 
struct Emp {
    int start, end;
};
 
unordered_map<int, Emp> emp_map;
 
int start_tree[MAX_TIME * 4 + 5];
int end_tree[MAX_TIME * 4 + 5];
 
int update(int* tree, const int& node, 
    const int& start, const int& end,
    const int& idx, const int& val) {
    if (idx < start || idx > end) return tree[node];
 
    if (start == end) return tree[node] += val;
 
    int mid = (start + end) / 2;
    int left_ = update(tree, node*2, start, mid, idx, val);
    int right_ = update(tree, node*2+1, mid + 1, end, idx, val);
 
    return tree[node] = (left_ + right_);
}
 
int query(int* tree, const int& node,
    const int& start, const int& end,
    const int& left, const int& right) {
    if (left > end || right < start) return 0;
    if (left <= start && right >= end) return tree[node];
 
    int mid = (start + end) / 2;
    int left_ = query(tree, node * 2, start, mid, left, right);
    int right_ = query(tree, node * 2 + 1, mid + 1, end, left, right);
     
    return left_ + right_;
}
 
 
void init(int musicTime)
{
    time_slice = musicTime;
    memset(start_tree, 0, sizeof(start_tree));
    memset(end_tree, 0, sizeof(end_tree));
 
    emp_map.clear();
    emp_map.reserve(MAX_EMP);
}
 
// 타임슬라이스 보다 End-start가 큰건 입증됨
void add(int mID, int mStart, int mEnd)
{
    auto it = emp_map.find(mID);
    if (it != emp_map.end()) {
        auto& emp = it->second;
        update(start_tree, 1, 1, MAX_TIME ,emp.start, -1);
        update(end_tree, 1, 1, MAX_TIME, emp.end + 1, -1);
        emp = { mStart,mEnd };
    }
    update(start_tree, 1, 1, MAX_TIME, mStart, 1);
    update(end_tree, 1, 1, MAX_TIME, mEnd + 1, 1);
    emp_map[mID] = {mStart,mEnd};
 
}
 
void remove(int mID)
{
    auto it = emp_map.find(mID);
    if (it == emp_map.end()) return;
    auto& emp = it->second;
    update(start_tree, 1, 1, MAX_TIME, emp.start, -1);
    update(end_tree, 1, 1, MAX_TIME, emp.end + 1, -1);
    emp_map.erase(it);
}
 
int getCnt(int mBSTime)
{
    int ans = 0;
    int start_emp = query(start_tree, 1, 1, MAX_TIME, 1, mBSTime);
    int end_emp = query(end_tree, 1, 1, MAX_TIME, 1, mBSTime + time_slice );
 
    ans = start_emp - end_emp;
 
    debug("ans %d\n", ans);
    return ans;
}
