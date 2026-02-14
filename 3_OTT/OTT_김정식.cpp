#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

struct Movie {
    int id;
    int genre;
    int total;
    int addOrder;
};

// 영화 ID -> 인덱스 매핑
unordered_map<int, int> movieIdToIdx;
Movie movies[10001];
int movieCount;

bool isDeleted[10001];

// 사용자별 시청 정보
vector<pair<int, int>> userWatchList[1001];  // {movieIdx, rating}
bool userWatched[1001][10001];

// 장르별 영화 인덱스
vector<int> moviesByGenre[6];
int addCounter;

struct RESULT
{
    int cnt;
    int IDs[5];
};

void init(int N)
{
    movieIdToIdx.clear();
    movieCount = 0;
    addCounter = 0;

    for (int i = 0; i <= 10000; i++) {
        isDeleted[i] = false;
    }

    for (int i = 0; i <= N; i++) {
        userWatchList[i].clear();
        for (int j = 0; j <= 10000; j++) {
            userWatched[i][j] = false;
        }
    }

    for (int g = 0; g <= 5; g++) {
        moviesByGenre[g].clear();
    }

    return;
}

int add(int mID, int mGenre, int mTotal)
{
    if (movieIdToIdx.count(mID)) {
        return 0;
    }

    int idx = movieCount++;
    movieIdToIdx[mID] = idx;

    movies[idx].id = mID;
    movies[idx].genre = mGenre;
    movies[idx].total = mTotal;
    movies[idx].addOrder = addCounter++;

    moviesByGenre[mGenre].push_back(idx);

    return 1;
}

int erase(int mID)
{
    if (!movieIdToIdx.count(mID)) {
        return 0;
    }

    int idx = movieIdToIdx[mID];
    if (isDeleted[idx]) {
        return 0;
    }

    isDeleted[idx] = true;
    return 1;
}

int watch(int uID, int mID, int mRating)
{
    if (!movieIdToIdx.count(mID)) {
        return 0;
    }

    int idx = movieIdToIdx[mID];
    if (isDeleted[idx] || userWatched[uID][idx]) {
        return 0;
    }

    movies[idx].total += mRating;
    userWatchList[uID].push_back({ idx, mRating });
    userWatched[uID][idx] = true;

    return 1;
}

RESULT suggest(int uID)
{
    RESULT res;
    res.cnt = 0;

    vector<pair<int, int>>& watchList = userWatchList[uID];
    int targetGenre = -1;

    // 최근 5개 영화 중 최고 평점 찾기
    int size = watchList.size();
    if (size > 0) {
        int maxRating = -1;
        int maxRatingMovieIdx = -1;
        int validCount = 0;

        for (int i = size - 1; i >= 0 && validCount < 5; i--) {
            int movieIdx = watchList[i].first;

            if (isDeleted[movieIdx]) continue;

            validCount++;
            int rating = watchList[i].second;

            if (rating > maxRating) {
                maxRating = rating;
                maxRatingMovieIdx = movieIdx;
            }
        }

        if (maxRatingMovieIdx != -1) {
            targetGenre = movies[maxRatingMovieIdx].genre;
        }
    }

    // 후보 수집
    struct Candidate {
        int id;
        int total;
        int addOrder;
    };

    Candidate candidates[10000];
    int candidateCount = 0;

    if (targetGenre != -1) {
        // 특정 장르만
        for (int idx : moviesByGenre[targetGenre]) {
            if (isDeleted[idx]) continue;
            if (userWatched[uID][idx]) continue;

            candidates[candidateCount++] = { movies[idx].id, movies[idx].total, movies[idx].addOrder };
        }
    }
    else {
        // 모든 장르
        for (int g = 1; g <= 5; g++) {
            for (int idx : moviesByGenre[g]) {
                if (isDeleted[idx]) continue;
                if (userWatched[uID][idx]) continue;

                candidates[candidateCount++] = { movies[idx].id, movies[idx].total, movies[idx].addOrder };
            }
        }
    }

    int k = candidateCount < 5 ? candidateCount : 5;

    for (int i = 0; i < k; i++) {
        int bestIdx = i;
        for (int j = i + 1; j < candidateCount; j++) {
            if (candidates[j].total > candidates[bestIdx].total ||
                (candidates[j].total == candidates[bestIdx].total &&
                    candidates[j].addOrder > candidates[bestIdx].addOrder)) {
                bestIdx = j;
            }
        }

        if (bestIdx != i) {
            Candidate temp = candidates[i];
            candidates[i] = candidates[bestIdx];
            candidates[bestIdx] = temp;
        }

        res.IDs[res.cnt++] = candidates[i].id;
    }

    return res;
}