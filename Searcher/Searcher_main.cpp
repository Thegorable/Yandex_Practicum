#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <map>
#include <set>

#include "BusManager_tests.h"
#include "BusManager.h"

using namespace std;

void WriteOS(ostream& os, const BusesForStopResponse& r) {
    if (r.buses_.empty()) {
        os << "No stop";
    }
    bool is_first = true;
    for (const auto& bus : r.buses_) {
        if (!is_first) {
            os << ' ';
        }
        else {
            is_first = false;
        }
        os << bus;
    }

    os << '\n';
}

void WriteOS(ostream& os, const StopsForBusResponse& r) {
    if (r.stops_inter_buses.empty()) {
        os << "No bus";
    }

    for (auto it = r.stops_inter_buses.begin(); it != r.stops_inter_buses.end(); it++) {
        const auto& [stop, buses] = *it;

        os << "Stop "s << stop << ':';

        for (const auto& bus : buses) {
            os << ' ' << bus;
        }
        if (buses.empty()) {
            os << " no interchange"s;
        }

        if (next(it) != r.stops_inter_buses.end()) {
            os << '\n';
        }
    }

    os << '\n';
}

void WriteOS(ostream& os, const AllBusesResponse& r) {
    if (r.buses_stops.empty()) {
        os << "No buses";
    }

    for (auto it = r.buses_stops.begin(); it != r.buses_stops.end(); it++) {
        const auto& [bus, stops] = *it;

        os << "Bus "s << bus << ':';
        for (const auto& stop : stops) {
            os << ' ' << stop;
        }

        if (next(it) != r.buses_stops.end()) {
            os << '\n';
        }
    }

    os << '\n';
}

void CompareInputOutput(stringstream& o, stringstream& i_test) {
    bool was_bug = false;
    while (i_test.tellg() != -1) {
        string str_1;
        getline(o, str_1);
        string str_test;
        getline(i_test, str_test);
        
        if (str_1 != str_test) {
            if (!was_bug) {
                was_bug = true;
                cout << "\n\n";
            }
            cout << str_1 << "||\n";
            cout << str_test << "||\n";

            cout << str_test << "\t - the line is not correct \n\n";
        }
    }
}

int main() {
    int query_count;
    Query q;

    test_1_in >> query_count;
    stringstream output;
    output << query_count << '\n';

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        test_1_in >> q;
        switch (q.type) {
        case QueryType::NewBus:
            bm.AddBus(q.bus, q.stops);
            break;
        case QueryType::BusesForStop:
            WriteOS(output, bm.GetBusesForStop(q.stop));
            cout << bm.GetBusesForStop(q.stop) << endl;
            break;
        case QueryType::StopsForBus:
            WriteOS(output, bm.GetStopsForBus(q.bus));
            cout << bm.GetStopsForBus(q.bus) << endl;
            break;
        case QueryType::AllBuses:
            WriteOS(output, bm.GetAllBuses() );
            cout << bm.GetAllBuses() << endl;
            break;
        }
    }

    CompareInputOutput(output, test_1_out);
}