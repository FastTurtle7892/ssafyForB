#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

using namespace std;


struct RESULT
{
    int cnt;
    int IDs[5];
};

struct Movie {
    int mID; // 영화 ID
    int mGenre; // 영화 장르
    int mTotal; // 영화 총점
    int mNum; // 영화 등록 순서
};

struct MovieCmp { // 총점이 큰 순
    bool operator()(const Movie* a, const Movie* b) const {
        if (a->mTotal != b->mTotal) {
            return a->mTotal > b->mTotal;
        }
        return a->mNum > b->mNum; // 등록이 늦은 순(최신 순)
    }
};


int N;
vector<Movie> moviePool; // 여기서 Movie 메모리를 한 번 만듦

unordered_map<int, Movie*> movies; // 영화 id -> 위치 mapping
vector<vector<int>> users; // 사용자들이 본 영화 ID 저장
vector<unordered_map<int, int>> user_watches; // 사용자별 본 영화 ID - 점수 mapping

vector<set<Movie*, MovieCmp>> genre_movies; // 장르별 영화 관리
set<Movie*, MovieCmp> all_movies; // 동시에 전체 영화 관리

unordered_set<int> deleted_movie;

int add_order;


void init(int inputN)
{
    N = inputN;

    moviePool.clear();
    movies.clear();
    users.clear();
    user_watches.clear();
    genre_movies.clear();
    all_movies.clear();
    deleted_movie.clear();
    add_order = 0;

    moviePool.resize(10005);
    users.resize(N + 1);
    user_watches.resize(N + 1);
    genre_movies.resize(6); // 장르 1 - 5

    return;
}

int add(int mID, int mGenre, int mTotal)
{
    if (movies.count(mID)) // 이미 사용중인 ID
        return 0;

    add_order++;

    Movie& now = moviePool[add_order];
    now.mID = mID;
    now.mGenre = mGenre;
    now.mNum = add_order;
    now.mTotal = mTotal;

    movies[mID] = &now;
    genre_movies[mGenre].insert(&now);
    all_movies.insert(&now);

    return 1;
}

int erase(int mID)
{
    if (!movies.count(mID)) // 등록된 적 없는 영화
        return 0;
    if (deleted_movie.count(mID)) // 이미 삭제된 영화
        return 0;

    deleted_movie.insert(mID);

    return 1;
}

int watch(int uID, int mID, int mRating)
{
    if (!movies.count(mID)) // 등록된 적 없는 영화 O(1)
        return 0;
    if (deleted_movie.count(mID)) // 이미 삭제된 영화 O(1)
        return 0;
    if (user_watches[uID].count(mID)) // 이미 시청한 영화 O(log N)
        return 0;

    Movie* now = movies[mID];

    // 삭제 먼저
    genre_movies[now->mGenre].erase(now);
    all_movies.erase(now);

    now->mTotal += mRating;

    // 추가 + 정렬
    genre_movies[now->mGenre].insert(now);
    all_movies.insert(now);

    // 시청목록 맨 뒤에 추가
    users[uID].push_back(mID);
    user_watches[uID][mID] = mRating;

    return 1;
}

RESULT suggest(int uID)
{
    RESULT res;

    res.cnt = 0;

    // 최근 시청한 영화 5개 중 사용자가 준 평점이 가장 높은 영화의 장르 뽑기
    int five = 0;
    int max_rating = -1;
    int max_rating_genre = -1;
    for (auto iter = users[uID].rbegin(); iter != users[uID].rend() && five < 5; iter++) {
        if (deleted_movie.count(*iter)) // 이미 삭제된 영화면 continue
            continue;

        five++;
        if (user_watches[uID][*iter] > max_rating) {
            max_rating = user_watches[uID][*iter];
            max_rating_genre = movies[*iter]->mGenre;
        }
    }

    // 총점이 높은 순으로 최대 5개 추천
    set<Movie*, MovieCmp>* chosen;

    if (max_rating_genre == -1) // 본 영화가 없다 -> all_movies에서
        chosen = &all_movies;
    else // 장르 선택
        chosen = &genre_movies[max_rating_genre];

    for (auto iter = chosen->begin(); iter != chosen->end() && res.cnt < 5; iter++) {
        if (user_watches[uID].count((*iter)->mID)) // 이미 시청한 영화면 continue
            continue;
        if (deleted_movie.count((*iter)->mID)) // 이미 삭제된 영화면 continue
            continue;
        
        res.IDs[res.cnt] = (*iter)->mID;
        res.cnt++;
    }

    return res;
}