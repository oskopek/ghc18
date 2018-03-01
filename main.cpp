#include <vector>
//#include <boost/unordered_map.hpp>
#include <map>
#include <iostream>
#include <set>
#include <algorithm>
#include <climits>
#include <unordered_map>
#include <cassert>
#include <queue>
#include <random>
#include <fstream>

using namespace std;

struct pt {
    long x, y;
};

struct ride {
    long id, s_time, e_time, len;
    pt start, end;
};

typedef vector<vector<ride>> Sol;

inline long dist(const pt &a, const pt &b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

ulong simulate(const Sol& sol, const long BONUS, const long STEPS) {
    ulong score = 0;
    ulong bonuspts = 0;
    for (const vector<ride>& veh : sol) {
        long time = 0;
        pt cur{0, 0};
        for (const ride& r : veh) {
            const long to_start = dist(cur, r.start);
            time += to_start;
            time = max(time, r.s_time);

            const bool on_time = time == r.s_time;
            const long ride = dist(r.start, r.end);
            time += ride;
            if (time <= r.e_time) {
                score += ride;
                if (on_time) bonuspts += BONUS;
            } else {
//                assert(false);
            }
            cur = r.end;
        }
    }
    cerr << "Score: " << score + bonuspts << endl;
    cerr << "Rides: " << score << endl;
    cerr << "Bonus: " << bonuspts << endl;
    return score + bonuspts;
}

void print_assignments(const Sol& sol, std::ostream& str) {
    for (const vector<ride>& assigned : sol) {
        str << assigned.size();
        for (const ride& r : assigned) {
            str << " " << r.id;
        }
        str << endl;
    }
}

// explicit solution
Sol exampleA(const vector<ride>& rides, const long vehs) {
    Sol sol(vehs);

    // Example A:
    sol[0].push_back(rides[0]);
    sol[1].push_back(rides[2]);
    sol[1].push_back(rides[1]);

    return sol;
}

struct veh {
    long id, avail_time;
    pt loc;

    bool operator<(const veh& other) const {
        return avail_time > other.avail_time; // max queue
    }
};

struct lensort {
    bool operator()(const ride& a, const ride& b) const {
        return a.len > b.len;
    }
};

// greedy
Sol exampleB(const vector<ride>& rides, const long vehs, const long STEPS) {
    vector<bool> used(rides.size(), false);
    Sol sol(vehs);
    priority_queue<veh> vehicles;
    for (long i = 0; i < vehs; i++) {
        vehicles.push(veh{i, 0, pt{0, 0}});
    }

    for (long used_rides = 0; used_rides < rides.size(); used_rides++) {
        veh v = vehicles.top(); vehicles.pop();
        if (v.avail_time >= STEPS) break;

        // Find closest ride
        long min_closeness = LONG_MAX;
        long min_idx = -1;
        long min_end = 0;
        for (long i = 0; i < rides.size(); i++) {
            const ride& r = rides[i];
            if (used[i]) continue;
            if (r.e_time < v.avail_time) continue;
            const long closeness = dist(v.loc, r.start);
            const long end_time = max(v.avail_time + closeness + r.len, r.s_time + r.len); // max(no-wait, wait)
            if (closeness < min_closeness && end_time <= r.e_time) {
                min_closeness = closeness;
                min_idx = i;
                min_end = end_time;
            }
        }

        if (min_idx < 0) {
            continue; // leave out this car, cannot use it again
        }
        // Update sol
        const ride& chosen = rides[min_idx];
        sol[v.id].push_back(chosen);
        used[min_idx] = true;

        // Update vehicle
        v.avail_time = min_end;
        assert(min_end <= chosen.e_time);
        v.loc = chosen.end;
        vehicles.push(v);
    }

    return sol;
}

//bool is_left(const ride& r, const long ROW, const long COLS) {
//    return r.start.x < ROW/2 && r.start.y < COLS/2 && r.end.x < ROW/2 && r.end.y < COLS/2;
//}

// greedy
Sol earliestStart(const vector<ride>& rides, const long vehs, const long STEPS, const long THRESHOLD, const long rows, const long cols) {
    vector<bool> used(rides.size(), false);
    Sol sol(vehs);
    priority_queue<veh> vehicles;
    for (long i = 0; i < vehs; i++) {
        vehicles.push(veh{i, 0, pt{0, 0}});
    }

    for (long used_rides = 0; used_rides < rides.size(); used_rides++) {
        veh v = vehicles.top(); vehicles.pop();
        if (v.avail_time >= STEPS) break;

        // Find closest ride
        long min_start_time = LONG_MAX;
        for (long i = 0; i < rides.size(); i++) {
            const ride& r = rides[i];
            if (used[i]) continue;
            if (r.e_time < v.avail_time) continue;
            const long closeness = dist(v.loc, r.start);
            const long start_time = max(v.avail_time + closeness, r.s_time);
            const long end_time = start_time + r.len; // max(no-wait, wait)
            if (start_time < min_start_time && end_time <= r.e_time) {
                min_start_time = start_time;
            }
        }

        vector<ride> idxset;
        for (long i = 0; i < rides.size(); i++) {
            const ride& r = rides[i];
            if (used[i]) continue;
            if (r.e_time < v.avail_time) continue;
            const long closeness = dist(v.loc, r.start);
            const long start_time = max(v.avail_time + closeness, r.s_time);
            const long end_time = start_time + r.len; // max(no-wait, wait)
            if (min_start_time + THRESHOLD > start_time && end_time <= r.e_time) {
                idxset.push_back(r);
            }
        }

        // rand num between 0 and idxset.size()
        if (idxset.empty()) continue; // leave out this car
//        if (idxset.size() > 2) cerr << "IDXSET size: " << idxset.size() << endl;

        sort(idxset.begin(), idxset.end(), lensort());
        long min_idx = idxset[0].id;
        assert(min_idx >= 0 && min_idx < rides.size());

        // Update sol
        const ride& chosen = rides[min_idx];
        sol[v.id].push_back(chosen);
        used[min_idx] = true;

        // Update vehicle
        long closeness = dist(v.loc, chosen.start);
        long start_time = max(v.avail_time + closeness, chosen.s_time);
        v.avail_time = start_time + chosen.len;
        assert(v.avail_time <= chosen.e_time);
        v.loc = chosen.end;
        vehicles.push(v);
    }

    return sol;
}

int main() {
    long rows, cols, vehs, nrides, bonus, steps;
    cin >> rows >> cols >> vehs >> nrides >> bonus >> steps;

    vector<ride> rides(nrides);
    for (long i = 0; i < nrides; i++) {
        long xstart, ystart, xend, yend, s_time, e_time;
        cin >> xstart >> ystart >> xend >> yend >> s_time >> e_time;
        pt start{xstart, ystart};
        pt end{xend, yend};
        long len = dist(start, end);
        rides[i] = ride {i, s_time, e_time, len, start, end};
    }



    Sol sol;
//    if (nrides == 3 && vehs == 2 && rows == 3 && cols == 4) {
//        sol = exampleA(rides, vehs);
//    } else if (rows == 800 && cols == 1000 && bonus == 25 && steps == 25000) {
        // TODO: ex B
//    }

    Sol best_sol;
    ulong best_pts = 0;
    for (long i = 0; i < 30; i++) {
//        long THRESHOLD = 9 + (rand() % static_cast<int>(16 - 9 + 1));
        long THRESHOLD = i;
        cerr << "iteration: " << i << ", threshold: " << THRESHOLD << endl;
        sol = earliestStart(rides, vehs, steps, THRESHOLD, rows, cols);
        ulong pts = simulate(sol, bonus, steps); // print points
        if (pts > best_pts) {
            cerr << "Improved: " << best_pts << "->" << pts << endl;
            best_pts = pts;
            best_sol = sol;

            ofstream f;
            cerr << "Writing to file" << endl;
            f.open("/tmp/res.txt");
            print_assignments(best_sol, f);
            f.close();
        }
    }

    print_assignments(best_sol, cout);

    return 0;
}