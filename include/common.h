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

struct Photo {
    enum Orient { VER, HOR };

    Orient orient;

    int ntags = 0;
    vector<int> tags;
};

struct Context {
    using TSolution = vector<pair<int, int>>;

    int n;
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

            Photo s{(c == 'V')? Photo::VER : Photo::HOR, ntags, tags};
            photos.emplace_back(std::move(s));
        }
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

    int ScorePhoto(const Photo& a, const Photo& b) {

    }

    int GetScore() {
        int ans = 0;
        for (int i = 0; i + 1 < solution.size(); ++i) {
            ans += ScorePhoto(solution[i], solution[i + 1]);
        }

        return ans;
    }

};
