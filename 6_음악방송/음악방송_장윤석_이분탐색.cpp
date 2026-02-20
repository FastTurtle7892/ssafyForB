#include <iostream>
#include <unordered_map>
#include <cstring>
#include <vector>
#include <algorithm>
 
using namespace std;
 
vector<int> startTime;
vector<int> endTime;
 
int music_start_time;
 
struct Time {
 
    int start_time;
    int end_time;
};
unordered_map<int, Time> time_map;
 
 
void init(int musicTime) {
 
    time_map.clear();
    startTime.clear();
    endTime.clear();
    music_start_time = musicTime;
}
 
void add(int mID, int mStart, int mEnd){
 
    int start_idx, end_idx;
    if (time_map.find(mID) != time_map.end()) {
 
        int prev_start = time_map[mID].start_time;
        int prev_end = time_map[mID].end_time;
 
        start_idx = lower_bound(startTime.begin(), startTime.end(), prev_start) - startTime.begin();
        startTime.erase(startTime.begin() + start_idx);
 
        end_idx = lower_bound(endTime.begin(), endTime.end(), prev_end) - endTime.begin();
        endTime.erase(endTime.begin() + end_idx);
    }
 
    start_idx = lower_bound(startTime.begin(), startTime.end(), mStart) - startTime.begin();
    startTime.insert(startTime.begin() + start_idx, mStart);
 
    end_idx = lower_bound(endTime.begin(), endTime.end(), mEnd) - endTime.begin();
    endTime.insert(endTime.begin() + end_idx, mEnd);
    time_map[mID] = { mStart, mEnd };
      
}
 
void remove(int mID) {
 
    if (time_map.find(mID) != time_map.end()) {
 
        int prev_start = time_map[mID].start_time;
        int prev_end = time_map[mID].end_time;
 
        int start_idx = lower_bound(startTime.begin(), startTime.end(), prev_start) - startTime.begin();
        startTime.erase(startTime.begin() + start_idx);
 
        int end_idx = lower_bound(endTime.begin(), endTime.end(), prev_end) - endTime.begin();
        endTime.erase(endTime.begin() + end_idx);
 
        time_map.erase(mID);
    }
}
 
int getCnt(int mBSTime)
{
    int start_cnt = lower_bound(startTime.begin(), startTime.end(), mBSTime + 1) - startTime.begin();
    int end_cnt = lower_bound(endTime.begin(), endTime.end(), mBSTime + music_start_time) - endTime.begin();
 
    return start_cnt - end_cnt;
}