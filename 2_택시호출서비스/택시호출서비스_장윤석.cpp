#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <climits>
#include <set> 

using namespace std;

const int MAX_TAXI = 2005;
const int MAP_SIZE = 10005;
const int BLOCK_SIZE = 200; 
const int GRID_DIM = MAP_SIZE / BLOCK_SIZE + 5;

struct Result
{
    int mX, mY;
    int mMoveDistance; // 총 운행 거리
    int mRideDistance; // 승객 탑승 거리
};

Result ti[MAX_TAXI]; // 택시 정보 (1-indexed)
int NN, MM, LL;


vector<int> sectors[GRID_DIM][GRID_DIM];


struct TaxiComp {
    bool operator()(int a, int b) const {
        
        if (ti[a].mRideDistance == ti[b].mRideDistance) {
            return a < b;
        }
        return ti[a].mRideDistance > ti[b].mRideDistance;
    }
};

set<int, TaxiComp> ranked_taxis;


void add_to_sector(int id) {
    int r = ti[id].mY / BLOCK_SIZE;
    int c = ti[id].mX / BLOCK_SIZE;
    sectors[r][c].push_back(id);
}

void remove_from_sector(int id) {
    int r = ti[id].mY / BLOCK_SIZE;
    int c = ti[id].mX / BLOCK_SIZE;

    vector<int>& vec = sectors[r][c];
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == id) {
            vec[i] = vec.back();
            vec.pop_back();
            return;
        }
    }
}



void init(int N, int M, int L, int mXs[], int mYs[])
{
    NN = N;
    MM = M;
    LL = L;

    // 초기화
    ranked_taxis.clear();
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            sectors[i][j].clear();
        }
    }

    for (int i = 1; i <= MM; i++) {
        ti[i].mX = mXs[i - 1];
        ti[i].mY = mYs[i - 1];
        ti[i].mMoveDistance = 0;
        ti[i].mRideDistance = 0;

        add_to_sector(i);
        ranked_taxis.insert(i);
    }
}

int find_nearest_taxi(int mSX, int mSY, int& found_dist) {
    int best_id = 0;
    found_dist = INT_MAX;

    // 탐색 범위 계산
    int min_r = max(0, (mSY - LL) / BLOCK_SIZE);
    int max_r = min(GRID_DIM - 1, (mSY + LL) / BLOCK_SIZE);
    int min_c = max(0, (mSX - LL) / BLOCK_SIZE);
    int max_c = min(GRID_DIM - 1, (mSX + LL) / BLOCK_SIZE);

    for (int r = min_r; r <= max_r; r++) {
        for (int c = min_c; c <= max_c; c++) {


            int dist_to_sector = 0;

            int s_min_x = c * BLOCK_SIZE;
            int s_max_x = s_min_x + BLOCK_SIZE - 1;
            if (mSX < s_min_x) dist_to_sector += (s_min_x - mSX);
            else if (mSX > s_max_x) dist_to_sector += (mSX - s_max_x);

            int s_min_y = r * BLOCK_SIZE;
            int s_max_y = s_min_y + BLOCK_SIZE - 1;
            if (mSY < s_min_y) dist_to_sector += (s_min_y - mSY);
            else if (mSY > s_max_y) dist_to_sector += (mSY - s_max_y);


            if (dist_to_sector > found_dist) continue;


            for (int id : sectors[r][c]) {
                int dist = abs(ti[id].mX - mSX) + abs(ti[id].mY - mSY);

                if (dist <= LL) {
                  
                    if (dist < found_dist) {
                        found_dist = dist;
                        best_id = id;

                    }
                    else if (dist == found_dist) {
                        if (best_id == 0 || id < best_id) {
                            best_id = id;
                        }
                    }
                }
            }
        }
    }
    return best_id;
}

int pickup(int mSX, int mSY, int mEX, int mEY)
{
    int min_first_dist = 0;

    int taxi_n = find_nearest_taxi(mSX, mSY, min_first_dist);

    if (taxi_n == 0) {
        return -1;
    }
    else {
  
        remove_from_sector(taxi_n);


        ranked_taxis.erase(taxi_n);

        int drive_dist = abs(mSX - mEX) + abs(mSY - mEY);

        ti[taxi_n].mX = mEX;
        ti[taxi_n].mY = mEY;
        ti[taxi_n].mMoveDistance += (min_first_dist + drive_dist);
        ti[taxi_n].mRideDistance += drive_dist;

        ranked_taxis.insert(taxi_n); 

        add_to_sector(taxi_n);

        return taxi_n;
    }
}

Result reset(int mNo)
{
    Result res = ti[mNo];

    ranked_taxis.erase(mNo);

    ti[mNo].mMoveDistance = 0;
    ti[mNo].mRideDistance = 0;

    ranked_taxis.insert(mNo);

    return res;
}

void getBest(int mNos[])
{
    int count = 0;
    for (const auto& id : ranked_taxis) {
        mNos[count++] = id;
        if (count == 5) break;
    }
}