#pragma once
#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <map>
#include <set>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type{};
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    if (!q.bus.empty()) {
        q.bus.clear();
    }
    if (!q.stop.empty()) {
        q.stop.clear();
    }
    if (!q.stops.empty()) {
        q.stops.clear();
    }

    string s;
    is >> s;

    if (s == "NEW_BUS"s) {
        q.type = QueryType::NewBus;
        is >> q.bus;
        int count_stops;
        is >> count_stops;
        string new_stop;
        for (int i = 0; i < count_stops; i++) {
            is >> new_stop;
            q.stops.push_back(new_stop);
        }
    }
    else if (s == "BUSES_FOR_STOP"s) {
        q.type = QueryType::BusesForStop;
        is >> q.stop;
    }
    else if (s == "STOPS_FOR_BUS"s) {
        q.type = QueryType::StopsForBus;
        is >> q.bus;
    }
    else if (s == "ALL_BUSES"s) {
        q.type = QueryType::AllBuses;
    }

    return is;
}

struct BusesForStopResponse {
    vector<string> buses_;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
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
    return os;
}

struct StopsForBusResponse {
    vector<pair<string, vector<string>>> stops_inter_buses;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
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

    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> buses_stops;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
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

    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        for (const auto& stop : stops) {
            buses_stops[bus].push_back(stop);
            stops_buses[stop].push_back(bus);
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        BusesForStopResponse response{};
        if (stops_buses.count(stop)) {
            response.buses_ = stops_buses.at(stop);
        }
        return response;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        StopsForBusResponse response{};
        if (buses_stops.count(bus)) {
            for (const string& stop : buses_stops.at(bus)) {
                response.stops_inter_buses.push_back({ stop, vector<string>{} });
                for (const auto& bus_ : stops_buses.at(stop)) {
                    if (bus_ != bus) {
                        response.stops_inter_buses.back().second.push_back(bus_);
                    }
                }
            }
        }

        return response;
    }

    AllBusesResponse GetAllBuses() const {
        AllBusesResponse response{};
        if (!buses_stops.empty()) {
            for (const auto& [bus, stops] : buses_stops) {
                response.buses_stops[bus] = vector<string>{};
                for (const auto& stop : stops) {
                    response.buses_stops[bus].push_back(stop);
                }
            }
        }

        return response;
    }

private:
    map<string, vector<string>> buses_stops;
    map<string, vector<string>> stops_buses;
};