#include <vector>
//#include <boost/unordered_map.hpp>
#include <map>
#include <iostream>
#include <set>
#include <algorithm>
#include <climits>
#include <unordered_map>

using namespace std;

struct ride {
    long id, xstart, ystart, xend, yend, s_time, e_time;
};

void print_assignments(vector<vector<ride>> &vehicle_assignments) {
    for (const vector<ride>& assigned : vehicle_assignments) {
        cout << assigned.size();
        for (const ride& r : assigned) {
            cout << " " << r.id;
        }
        cout << endl;
    }
}

int main() {
    long rows, cols, vehs, nrides, bonus, steps;
    cin >> rows >> cols >> vehs >> nrides >> bonus >> steps;

    vector<ride> rides(nrides);
    for (long i = 0; i < nrides; i++) {
        long xstart, ystart, xend, yend, s_time, e_time;
        cin >> xstart >> ystart >> xend >> yend >> s_time >> e_time;
        rides[i] = ride {i, xstart, ystart, xend, yend, s_time, e_time};
    }

    vector<vector<ride>> vehicle_assignments(vehs);

    print_assignments(vehicle_assignments);
    return 0;
}