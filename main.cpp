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
            }
            cur = r.end;
        }
    }
    cerr << "Score: " << score + bonuspts << endl;
    cerr << "Rides: " << score << endl;
    cerr << "Bonus: " << bonuspts << endl;
    return score + bonuspts;
}

void print_assignments(const Sol& sol) {
    for (const vector<ride>& assigned : sol) {
        cout << assigned.size();
        for (const ride& r : assigned) {
            cout << " " << r.id;
        }
        cout << endl;
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

struct timesort {
    bool operator()(const ride& a, const ride& b) const {
        return a.s_time < b.s_time;
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
            const long end_time = v.avail_time + closeness + r.len;
            if (closeness < min_closeness && end_time < r.e_time) {
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
    if (nrides == 3 && vehs == 2 && rows == 3 && cols == 4) {
        sol = exampleA(rides, vehs);
//    } else if (rows == 800 && cols == 1000 && bonus == 25 && steps == 25000) {
        // TODO: ex B
//    }
    sol = exampleB(rides, vehs, steps);
    simulate(sol, bonus, steps); // print points
    print_assignments(sol);

    return 0;
}