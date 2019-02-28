#include <common.h>

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <cstdlib>
#include <chrono>
#include <random>

using namespace std;

int buffer[200];

//int Intersect(std::vector<int>& f, std::vector<int>& s) {
//    return (set_intersection(f.begin(), f.end(), s.begin(), s.end(), buffer) - buffer);
//}

vector<int> Union(std::vector<int>& f, std::vector<int>& s) {
    int sz = (set_union(f.begin(), f.end(), s.begin(), s.end(), buffer) - buffer);
    return vector<int>(buffer, buffer + sz);
}

struct MySolver : public Context {
    void Solve() {
        vector<pair<vector<int>, int>> verticals;
        vector<pair<vector<int>, int>> horisontals;
        int id = 0;
        for (auto ph : photos) {
            if (ph.orient == Photo::Orient::HOR) {
                horisontals.emplace_back(ph.tags, id++);
            } else {
                verticals.emplace_back(ph.tags, id++);
            }
        }

        mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        shuffle(verticals.begin(), verticals.end(), rng);
        shuffle(horisontals.begin(), horisontals.end(), rng);

        vector<pair<vector<int>, pair<int, int>>> vertical_pairs;
        vector<bool> used(n, false);
        for (int i = 0; i < verticals.size(); ++i) {
            if (!used[verticals[i].second]) {
                int mn = 99999;
                int mnj;
                for (int j = i + 1; j < verticals.size(); ++j) {
                    if (!used[verticals[j].second]) {
                        int inter = Intersect(verticals[i].first, verticals[j].first);
                        if (inter < mn) {
                            mn = inter;
                            mnj = j;
                            if (mn == 0) {
                                break;
                            }
                        }
                    }
                }
                if (mn != 99999) {
                    vertical_pairs.emplace_back(Union(verticals[i].first, verticals[mnj].first),
                            make_pair(verticals[i].second, verticals[mnj].second));
                    used[verticals[mnj].second] = true;
                }
            }
        }
        cerr << "vertical_pairs " << vertical_pairs.size() << endl;
        used.assign(n, false);
        shuffle(vertical_pairs.begin(), vertical_pairs.end(), rng);

        vector<int>* cur;
        if (horisontals.size()) {
            cur = &horisontals[0].first;
            used[horisontals[0].second] = true;
            solution.emplace_back(horisontals[0].second, -1);
        } else {
            cur = &vertical_pairs[0].first;
            used[vertical_pairs[0].second.first] = true;
            solution.push_back(vertical_pairs[0].second);
        }

        while (true) {
            int mx = -1;
            vector<int>* mxcur;
            pair<int, int> res;
            for (auto& hor : horisontals) {
                if (!used[hor.second]) {
                    int inter = Intersect(*cur, hor.first);
                    int sc = min({inter, (int)(hor.first.size() - inter), (int)(cur->size() - inter)});
                    if (sc > mx) {
                        mxcur = &hor.first;
                        mx = sc;
                        res = {hor.second, -1};
                    }
                }
            }

            for (auto& vp : vertical_pairs) {
                if (!used[vp.second.first]) {
                    int inter = Intersect(*cur, vp.first);
                    int sc = min({inter, (int)(vp.first.size() - inter), (int)(cur->size() - inter)});
                    if (sc > mx) {
                        mxcur = &vp.first;
                        mx = sc;
                        res = vp.second;
                    }
                }
            }

            if (mx != -1) {
                cur = mxcur;
                solution.push_back(res);
                used[res.first] = true;
            } else {
                break;
            }
            if (solution.size() % 10000 == 0) {
                cerr << solution.size() << endl;
            }
        }
        cerr << "solution size " << solution.size() << endl;
    }
};

void countTags();

int main() {
//    countTags();
    MySolver solver;

    solver.Input();

    auto start = std::chrono::system_clock::now();
    cerr << "Started solving..." << endl;
    solver.Solve();
    cerr << "Done!" << endl;
    solver.Verify();
    solver.Better();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cerr << "Test time: " << elapsed_seconds.count() << endl;
    cerr << "Outputting" << endl;
    solver.Output();

    cerr << solver.GetScore() << endl;
    return 0;
}

void countTags()  {
    int n;
    cin >> n;
    map<std::string, int> mp;
    int h = 0, v = 0;
    for (int i =0 ; i < n; ++i) {
        char ch;
        cin >> ch;
        if (ch == 'H') {
            ++h;
        } else {
            ++v;
        }

        int m;
        cin >> m;
        for (int j = 0; j < m; ++j) {
            std::string tag;
            cin >> tag;
            mp[tag]++;
        }
    }

    vector<pair<std::string, int>> a(mp.begin(), mp.end());
    std::sort(a.begin(), a.end(), [](auto a, auto b) {
        return a.second > b.second;
    });
    cerr << mp.size() << endl;
    cerr << "H: " << h << " V:" << v << endl;
    for (int i = 0; i < min((int)a.size(), 100); ++i) {
        cerr << a[i].first << " " << a[i].second << endl;
    }
}
