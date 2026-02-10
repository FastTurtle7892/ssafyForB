#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;

struct Space {
    int idx = -1;
    int len = -1;
    bool operator<(const Space& other) const {
        return idx < other.idx;
    }
};

struct Word {
    int buck = -1;
    int row_in_buck = -1;
    int idx = -1;
    int len = -1;
};

int buckN; // 버킷 갯수
vector<set<Space>> space_list; // 각 행의 (시작idx, 길이len) 저장
vector<vector<int>> max_lens; // 모든 행의 max_len 저장, 버킷단위로 나뉘어 있음
vector<int> buck_max_lens; // 각 버킷(100개 단위)의 max_len 저장
vector<Word> words;


void init(int N, int M)
{
    space_list.clear();
    max_lens.clear();
    buck_max_lens.clear();
    words.clear();

    buckN = (N + 99) / 100;

    space_list.resize(N); // 한줄로 쭉 생긴 단어장, 빈공간 정보
    max_lens.resize(buckN, vector<int>(100, 0));
    // 고민할 부분: 단어장이 100보다 작을 경우
    buck_max_lens.resize(buckN, M); // bucketN
    words.resize(55'001); // 단어의 id별 정보 저장

    for (int i = 0; i < N; i++) {
        space_list[i].insert({0, M}); // 각 행의 시작은 (0, 가능한 길이 M)
        max_lens[i / 100][i % 100] = M;
    }
}

void update(int buck, int row_in_buck) {
    // 해당 row의 최댓값 업데이트
    int real_row = buck * 100 + row_in_buck;
    auto& row_set = space_list[real_row];

    int max_len_now = 0;
    for (auto iter = row_set.begin(); iter != row_set.end(); iter++) {
        max_len_now = max(max_len_now, iter->len);
    }
    max_lens[buck][row_in_buck] = max_len_now;

    // 해당 buck의 최댓값 업데이트
    int max_buck_len = 0;
    for (int i = 0; i < 100; i++) { // 해당 buck만 보기
        max_buck_len = max(max_buck_len, max_lens[buck][i]);
    }
    buck_max_lens[buck] = max_buck_len;
}

int writeWord(int mId, int mLen)
{
    Word nword;
    nword.len = mLen;

    for (int i = 0; i < buckN; i++) {
        if (buck_max_lens[i] >= mLen) {
            nword.buck = i;
            break;
        }
    }
    if (nword.buck == -1) {
        return -1; // 들어갈 자리 없음
    }

    for (int i = 0; i < 100; i++) { // 버킷 내에서 찾기
        if (max_lens[nword.buck][i] >= mLen) {
            nword.row_in_buck = i;
            break;
        }
    }
    int real_row = nword.buck * 100 + nword.row_in_buck;

    // 들어갈 수 있는 idx 찾기
    Space space;
    auto& row_set = space_list[real_row];
    for (auto iter = space_list[real_row].begin(); iter != space_list[real_row].end(); iter++) {
        if (iter->len >= mLen) {
            nword.idx = iter->idx;
            space = *iter;
            break;
        }
    }
    
    // 빈 공간 업데이트
    if (space.len - mLen > 0)
        space_list[real_row].insert({ space.idx + mLen, space.len - mLen });
    space_list[real_row].erase(space); // set에서 해당 값 지워버리기

    // idx 기준 단어장에 업로드 // erase 용
    words[mId] = nword;

    update(nword.buck, nword.row_in_buck);

    return real_row;
}

int eraseWord(int mId)
{
    // idx 단어장에서 word의 위치 값 가져오기
    Word word = words[mId];
    if (word.buck == -1) // 입력된 적 없는 단어, 혹은 이미 지워진 단어
        return -1;
    
    // space list에서 빈공간 만들기
    // 앞 뒤로 빈공간이 이어지면, 이어야함
    int real_row = word.buck * 100 + word.row_in_buck;
    auto& row_set = space_list[real_row];
    
    // 지금 있던 단어가 들어갈 수 있는 사이의 빈 공간 찾기
    auto iter = row_set.lower_bound(Space{ word.idx, 0 });

    int nstart = word.idx;
    int nlen = word.len;

    if (iter != row_set.begin()) { // 시작점이 아니면, 앞에꺼랑 비교
        auto prev_iter = prev(iter);
        if (prev_iter->idx + prev_iter->len == nstart) {
            nstart = prev_iter->idx;
            nlen += prev_iter->len;
            row_set.erase(prev_iter);
        }
    }
    if (iter != row_set.end()) { // 끝점이 아니면, 뒤에꺼랑 비교
        if (iter->idx == nstart + nlen) {
            nlen += iter->len;
            row_set.erase(iter);
        }
    }
    space_list[real_row].insert({ nstart, nlen });

    // len 리스트 업데이트하기
    update(word.buck, word.row_in_buck);

    // 단어 지우기
    words[mId] = { -1, -1, -1, -1 };

    return real_row;
}