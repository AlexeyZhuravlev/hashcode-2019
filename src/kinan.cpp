#include <common.h>

#include <bitset>
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
#include <ctime>
#include <random>

using namespace std;

int buffer[200];

vector<int> Union(std::vector<int>& f, std::vector<int>& s) {
    int sz = (set_union(f.begin(), f.end(), s.begin(), s.end(), buffer) - buffer);
    return vector<int>(buffer, buffer + sz);
}

struct MySolver : public Context {
    vector<int> f;
    vector<vector<int>> gr;

    int find(int x) {
        if (x == f[x]) {
            return x;
        }
        return f[x] = find(f[x]);
    }

    void Solve() {
        // Solution goes here
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

        int m = horisontals.size() + vertical_pairs.size();

        vector<bitset<250>> bts;
        vector<vector<int>> tts;
        for (auto& p : horisontals) {
            bitset<250> cur;
            for (int i : p.first) {
                cur[i] = 1;
            }
            bts.push_back(cur);
            tts.push_back(p.first);
        }
        for (auto& p : vertical_pairs) {
            bitset<250> cur;
            for (int i : p.first) {
                cur[i] = 1;
            }
            bts.push_back(cur);
            tts.push_back(p.first);
        }

        auto get_id = [&](int i) {
            if (i < horisontals.size()) {
                return make_pair(horisontals[i].second, -1);
            } else {
                return vertical_pairs[i - horisontals.size()].second;
            }
        };
        vector<vector<int>> itag(ntags);
        for (int i = 0; i < m; i++) {
            auto& tt = tts[i];
            for (int v : tt) {
                itag[v].push_back(i);
            }
        }
        vector<std::tuple<int, int, int>> es;
        // for (int i = 0; i < ntags; i++) {
            // for (int j = 0; j < 2000; j++) {
                // int x = rand() % itag[i].size();
                // int y = rand() % itag[i].size();
                // int u = itag[i][x];
                // int v = itag[i][y];
            // // for (auto u : itag[i]) {
                // auto tu = get_tags(u);
                // // for (auto v : itag[i]) {
                // auto tv = get_tags(v);
                // if (u == v) {
                    // continue;
                // }
                // es.emplace_back(ScoreTags(tu, tv), u, v);
                // // }
            // }
        // }
        // for (int i = 0; i < ntags; i++) {
            // int sz = min<size_t>(itag[i].size(), 500);
            // for (auto u : itag[i]) {
                // auto tu = get_tags(u);
                // for (auto v : itag[i]) {
                // auto tv = get_tags(v);
                // if (u == v) {
                    // continue;
                // }
                // es.emplace_back(ScoreTags(tu, tv), u, v);
                // }
            // }
        // }

        auto score = [&](auto& a, auto &b) {
            int inter = (a & b).count();
            return min<int>(inter, min(a.count() - inter, b.count() - inter));
        };
        for (int i = 0; i < ntags; i++) {
            if (i % 10 == 0) {
                cerr << i << endl;
            }
            int sz = min<size_t>(itag[i].size(), 4000);
            for (int x = 0; x < sz; x++) {
                int u = itag[i][x];
                auto& tu = bts[u];
                for (int y = 0; y < sz; y++) {
                    int v = itag[i][y];
                    auto& tv = bts[v];
                    if (u == v) {
                        continue;
                    }
                    es.emplace_back(score(tu, tv), u, v);
                }
            }
        }
        cerr << "SORTING" << endl;
        sort(es.begin(), es.end());
        es.resize(unique(es.begin(), es.end()) - es.begin());
        reverse(es.begin(), es.end());
        cerr << "SORTED" << endl;

        f.resize(m);
        for (int i = 0; i < m; i++) {
            f[i] = i;
        }

        vector<int> deg(m);
        gr.resize(m);
        for (auto& t : es) {
            auto u = get<1>(t);
            auto v = get<2>(t);
            if (find(u) == find(v)) {
                continue;
            }
            if (deg[u] > 1 || deg[v] > 1) {
                continue;
            }
            f[u] = v;
            deg[u]++;
            deg[v]++;
            gr[u].push_back(v);
            gr[v].push_back(u);
        }

        vector<int> vis(m, 0);
        for (int i = 0; i < m; i++) {
            if (vis[i]) {
                continue;
            }
            for (int u = i; !vis[u]; ) {
                vis[u] = true;
                solution.push_back(get_id(u));
                assert(gr[u].size() <= 2);
                for (int v : gr[u]) {
                    if (!vis[v]) {
                        u = v;
                        break;
                    }
                }
            }
        }
    }
};

int main() {
    MySolver solver;

    solver.Input();

    auto start = std::chrono::system_clock::now();
    cerr << "Started solving..." << endl;
    solver.Solve();
    solver.Verify();
    cerr << "Done!" << endl;
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cerr << "Test time: " << elapsed_seconds.count() << endl;

    cerr << "Outputting" << endl;
    solver.Output();

    cerr << solver.GetScore() << endl;
    return 0;
}
