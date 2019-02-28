#pragma once

#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <cstdlib>
#include <unordered_map>

using namespace std;

int __buffer[200];

struct Photo {
    enum Orient { VER, HOR };

    Orient orient;

    int ntags = 0;
    vector<int> tags;
};

struct Context {
    using TSolution = vector<pair<int, int>>;

    int n;
    int ntags;
    vector<Photo> photos;
    TSolution solution;

    void Input() {
        cin >> n;

        unordered_map<string, int> tag_to_id;
        for (int i = 0; i < n; i++) {
            char c;
            cin >> c;
            assert(c == 'V' || c == 'H');

            int ntags;
            cin >> ntags;

            vector<int> tags;
            for (int j = 0; j < ntags; j++) {
                string s;
                cin >> s;
                auto it = tag_to_id.find(s);
                int id = -1;
                if (it == tag_to_id.end()) {
                    id = tag_to_id.size();
                    tag_to_id.emplace(s, id);
                } else {
                    id = it->second;
                }
                assert(id >= 0);
                tags.push_back(id);
            }

            sort(tags.begin(), tags.end());
            tags.resize(unique(tags.begin(), tags.end()) - tags.begin());

            Photo s{(c == 'V')? Photo::VER : Photo::HOR, ntags, tags};
            photos.emplace_back(std::move(s));
        }
        ntags = tag_to_id.size();
    }

    void Stats() {
    }


    void Output() {
        cout << solution.size() << endl;
        for (auto& p : solution) {
            if (p.second < 0) {
                cout << p.first << endl;
            } else {
                cout << p.first << " " << p.second << endl;
            }
        }
    }

    void Verify() {
        vector<int> used(n, 0);
        for (auto& p : solution) {
            assert(p.first >= 0 && p.second >= -1);
            assert(p.first < n && p.second < n);
            assert(!used[p.first] && !used[p.second]);
            used[p.first] = used[p.second] = 1;
        }
    }

    static int Intersect(const std::vector<int>& f, const std::vector<int>& s) {
        return (set_intersection(f.begin(), f.end(), s.begin(), s.end(), __buffer) - __buffer);
    }

    int ScoreTags(const vector<int>& a, const vector<int>& b) {
        /*
        auto is_in = [=](auto& v, auto x) {
            return lower_bound(v.begin(), v.end(), x) != v.end();
        };
        int s1 = 0, s2 = 0, s3 = 0;
        for (auto v : a) {
            s1 += !is_in(b, v);
            s2 += is_in(b, v);
        }
        for (auto v : b) {
            s3 += !is_in(a, v);
        }

        return min(s1, min(s2, s3));
        */
        int inter = Intersect(a, b);
        int sc = min({inter, (int)(a.size() - inter), (int)(b.size() - inter)});

        return sc;
    }

    int GetScore() {
        int ans = 0;

        auto unq = [=](auto& v) {
            sort(v.begin(), v.end());
            v.resize(unique(v.begin(), v.end()) - v.begin());
        };

        auto unn = [=](auto& a, auto& b) {
            a.insert(a.end(), b.begin(), b.end());
            unq(a);
        };

        auto get_tags = [=](auto p) {
            int i = p.first;
            int j = p.second;
            auto a = photos[i].tags;
            auto b = (j < 0)? vector<int>() : photos[j].tags;

            unn(a, b);

            return a;
        };
        for (size_t i = 0; i + 1 < solution.size(); ++i) {
            auto t1 = get_tags(solution[i]);
            auto t2 = get_tags(solution[i + 1]);

            ans += ScoreTags(t1, t2);
        }

        return ans;
    }

    void Better() {
        const int iterations = 20;
        const int generation = 10;

        vector<pair<int, TSolution>> solutions;
        int score = GetScore();
        solutions.emplace_back(score, solution);
        for (int i = 0; i < iterations; i++) {
            const int M = solutions.size();
            for (int j = 0; j < M; j++) {
                TSolution mutated = solutions[j].second;
                int i1 = std::rand() % solution.size();
                int i2 = std::rand() % solution.size();
                swap(mutated[i1], mutated[i2]);
                solution = mutated;
                int score = GetScore();
                solutions.emplace_back(score, mutated);
            }

            sort(solutions.rbegin(), solutions.rend());
            if (solutions.size() > generation) {
                solutions.resize(generation);
            }

            cerr << i << " " << " best score " << solutions[0].first << endl;
        }

        solution = solutions[0].second;
    }

};
