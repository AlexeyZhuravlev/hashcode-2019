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

using namespace std;

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
        vector<vector<int>> itag(ntags);
        for (int i = 0; i < n; i++) {
            for (int v : photos[i].tags) {
                itag[v].push_back(i);
            }
        }
        vector<std::tuple<int, int, int>> es;
        for (int i = 0; i < ntags; i++) {
            if (itag[i].size() == 2) {
                int u = itag[i][0];
                int v = itag[i][1];
                es.emplace_back(ScoreTags(photos[u].tags, photos[v].tags), u, v);
            }
        }
        sort(es.begin(), es.end());
        es.resize(unique(es.begin(), es.end()) - es.begin());
        reverse(es.begin(), es.end());

        f.resize(n);
        for (int i = 0; i < n; i++) {
            f[i] = i;
        }

        vector<int> deg(n);
        gr.resize(n);
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

        vector<int> vis(n, 0);
        for (int i = 0; i < n; i++) {
            if (vis[i]) {
                continue;
            }
            for (int u = i; !vis[u]; ) {
                vis[u] = true;
                solution.emplace_back(u, -1);
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
    cerr << "Done!" << endl;
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    cerr << "Test time: " << elapsed_seconds.count() << endl;

    cerr << "Outputting" << endl;
    solver.Output();

    cerr << solver.GetScore() << endl;
    return 0;
}
