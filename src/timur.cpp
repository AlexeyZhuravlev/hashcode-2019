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

vector<int> Union(const std::vector<int>& f, const std::vector<int>& s) {
    int sz = (set_union(f.begin(), f.end(), s.begin(), s.end(), buffer) - buffer);
    return vector<int>(buffer, buffer + sz);
}

struct MySlide {
    vector<int> tags;
    pair<int, int> ids;
};

vector<pair<vector<int>, pair<int, int>>> GenerateVerticalPairs(const vector<pair<vector<int>, int>>& verticals, int n) {
    vector<pair<vector<int>, pair<int, int>>> vertical_pairs;
    vector<bool> used(n, false);
    for (int i = 0; i < verticals.size(); ++i) {
        if (!used[verticals[i].second]) {
            int mn = 99999;
            int mnj;
            for (int j = i + 1; j < verticals.size(); ++j) {
                if (!used[verticals[j].second]) {
                    int inter = Context::Intersect(verticals[i].first, verticals[j].first);
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
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(vertical_pairs.begin(), vertical_pairs.end(), rng);
    return vertical_pairs;
}

vector<pair<vector<int>, pair<int, int>>> GenerateVerticalPairsWithRepeating(const vector<pair<vector<int>, int>>& verticals, int n) {
    vector<pair<vector<int>, pair<int, int>>> vertical_pairs;
    for (int i = 0; i < verticals.size(); ++i) {
        int mn = 99999;
        vector<int> mnjs;
        for (int j = i + 1; j < verticals.size(); ++j) {
            int inter = Context::Intersect(verticals[i].first, verticals[j].first);
            if (inter < mn) {
                mn = inter;
                mnjs = {j};
            } else if (inter == mn && mnjs.size() < 10) {
                mnjs.push_back(j);
            }

        }
        for (auto mnj : mnjs) {
            vertical_pairs.emplace_back(Union(verticals[i].first, verticals[mnj].first),
                                        make_pair(verticals[i].second, verticals[mnj].second));
        }
    }
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    shuffle(vertical_pairs.begin(), vertical_pairs.end(), rng);
    cerr << "vertical_pairs size " <<vertical_pairs.size() << endl;
    return vertical_pairs;
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

        SolveWithSegments(horisontals, verticals);
        cerr << "solution size " << solution.size() << endl;
    }

    void SolveWithSegments(vector<pair<vector<int>, int>>& horisontals,
                           vector<pair<vector<int>, int>>& verticals) {
        auto vertical_pairs = GenerateVerticalPairs(verticals, n);
        mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        vector<MySlide> slides;
        slides.reserve(horisontals.size() + vertical_pairs.size());
        for (auto& hor : horisontals) {
            slides.push_back({hor.first, {hor.second, -1}});
        }
        for (auto& vp : vertical_pairs) {
            slides.push_back({vp.first, vp.second});
        }
        shuffle(slides.begin(), slides.end(), rng);

        vector<vector<MySlide*>> segments;
        for (auto& slide : slides) {
            segments.push_back({&slide});
        }

        cerr << "segments size " << segments.size() << endl;
        std::set<pair<int, pair<int, int>>> edges;
        vector<bool> used;
        while (segments.size() > 1) {
            int TOP = 100000000;
            vector<pair<int, pair<int, int>>> edges;
            edges.reserve(TOP*10);
            for (int i = 0; i < segments.size(); ++i) {
                for (int j = 0; j < segments.size(); ++j) {
                    if (i != j) {
                        edges.emplace_back(-ScoreTags(segments[i].back()->tags, segments[j].front()->tags), make_pair(i, j));
                        if (edges.size() > 10 * TOP) {
                            sort(edges.begin(), edges.end());
                            edges.resize(TOP);
                        }
                    }
                }
            }
            sort(edges.begin(), edges.end());
            vector<vector<MySlide*>> new_segments;
            used.assign(segments.size(), false);
            for (auto& e : edges) {
                if (!used[e.second.first] && !used[e.second.second]) {
                    used[e.second.first] = true;
                    used[e.second.second] = true;
                    new_segments.push_back(std::move(segments[e.second.first]));
                    new_segments.back().insert(new_segments.back().end(),
                                               segments[e.second.second].begin(), segments[e.second.second].end());
                }
            }
            for (int i = 0; i < segments.size(); ++i) {
                if (!used[i]) {
                    new_segments.push_back(std::move(segments[i]));
                }
            }
            segments.swap(new_segments);
            cerr << "segments size " << segments.size() << endl;
        }
        for (auto slide : segments[0]) {
            solution.push_back(slide->ids);
        }
    }

    void SolveSquare(vector<pair<vector<int>, int>>& horisontals,
                     vector<pair<vector<int>, int>>& verticals) {
        auto vertical_pairs = GenerateVerticalPairsWithRepeating(verticals, n);
        vector<bool> used(n, false);

        vector<int>* cur;
        if (horisontals.size()) {
            cur = &horisontals[0].first;
            used[horisontals[0].second] = true;
            solution.emplace_back(horisontals[0].second, -1);
        } else {
            cur = &vertical_pairs[0].first;
            used[vertical_pairs[0].second.first] = true;
            used[vertical_pairs[0].second.second] = true;
            solution.push_back(vertical_pairs[0].second);
        }

        while (true) {
            int mx = -1;
            vector<int>* mxcur;
            pair<int, int> res;
            for (auto& hor : horisontals) {
                if (!used[hor.second]) {
                    int inter = Intersect(*cur, hor.first);
                    int sc = min({inter, (int) (hor.first.size() - inter), (int) (cur->size() - inter)});
                    if (sc > mx) {
                        mxcur = &hor.first;
                        mx = sc;
                        res = {hor.second, -1};
                    }
                }
            }

            bool any_pair = 0;
            for (auto& vp : vertical_pairs) {
                if (!used[vp.second.first] && !used[vp.second.second]) {
                    any_pair = true;
                    int inter = Intersect(*cur, vp.first);
                    int sc = min({inter, (int)(vp.first.size() - inter), (int)(cur->size() - inter)});
                    if (sc > mx) {
                        mxcur = &vp.first;
                        mx = sc;
                        res = vp.second;
                    }
                }
            }
            bool generated = 0;
            if (verticals.size() && (solution.size() % 1000 == 0 || !any_pair)) {
                vector<pair<vector<int>, int>> new_verticals;
                for (int i = 0; i < verticals.size(); ++i) {
                    if (!used[verticals[i].second]) {
                        new_verticals.push_back(verticals[i]);
                    }
                }
                verticals = new_verticals;
                if (!new_verticals.empty()) {
                    vertical_pairs = GenerateVerticalPairsWithRepeating(new_verticals, n);
                }
                generated = 1;
            }

            if (mx != -1) {
                cur = mxcur;
                solution.push_back(res);
                used[res.first] = true;
                if (res.second != -1) {
                    used[res.second] = true;
                }
            } else if (!generated) {
                break;
            }
            if (solution.size() % 10000 == 0) {
                cerr << solution.size() << endl;
            }
        }
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
    solver.Verify();
    cerr << "Done!" << endl;
//    solver.Better();
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
