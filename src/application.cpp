#include "application.h"

#include <iostream>
#include <limits>
#include <map>
#include <queue> 
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <json.hpp>

#include "dist.h"
#include "graph.h"
using json = nlohmann::json;

using namespace std;

double INF = numeric_limits<double>::max();

void buildGraph(istream &input, graph<long long, double> &g,
                vector<BuildingInfo> &buildings,
                unordered_map<long long, Coordinates> &coords) {
  json j;
  input >> j;

  
  set<long long> buildingIDs;  

 //1,`
  for (const auto &wp : j["waypoints"]) {
    long long id = wp["id"];
    double lat = wp["lat"];
    double lon = wp["lon"];

    g.addVertex(id); 
    coords[id] = Coordinates(lat, lon);  
  }
  
  for (const auto &b : j["buildings"]) {
    long long id = b["id"];
    buildingIDs.insert(id);  

    string name = b["name"];
    string abbrev = b["abbr"];
    double lat = b["lat"];
    double lon = b["lon"];

    g.addVertex(id);
    Coordinates c(lat, lon);
    buildings.push_back(BuildingInfo{id, c, name, abbrev});
  }

  for (const auto &fw : j["footways"]) {
    const auto &nodes = fw.is_array() ? fw : fw["nodes"];
    for (size_t i = 0; i + 1 < nodes.size(); ++i) {
      long long from = nodes[i];
      long long to = nodes[i + 1];

      const auto &c1 = coords[from];
      const auto &c2 = coords[to];

      double dist = distBetween2Points(c1, c2);
      g.addEdge(from, to, dist);
      g.addEdge(to, from, dist);
    }
  }

  for (const auto &building : buildings) {
    long long bID = building.id;
    const auto &bCoord = building.location;

    for (const auto &[wpID, wpCoord] : coords) {
      if (wpID == bID || buildingIDs.count(wpID)) continue; 
      double dist = distBetween2Points(bCoord, wpCoord);
      if (dist <= 0.036) {
        g.addEdge(bID, wpID, dist);
        g.addEdge(wpID, bID, dist);
      }
    }
  }
}



BuildingInfo getBuildingInfo(const vector<BuildingInfo> &buildings,
                             const string &query) {
  for (const BuildingInfo &building : buildings) {
    if (building.abbr == query) {
      return building;
    } else if (building.name.find(query) != string::npos) {
      return building;
    }
  }
  BuildingInfo fail;
  fail.id = -1;
  return fail;
}

BuildingInfo getClosestBuilding(const vector<BuildingInfo> &buildings,
                                Coordinates c) {
  double minDestDist = INF;
  BuildingInfo ret = buildings.at(0);
  for (const BuildingInfo &building : buildings) {
    double dist = distBetween2Points(building.location, c);
    if (dist < minDestDist) {
      minDestDist = dist;
      ret = building;
    }
  }
  return ret;
}

vector<long long> dijkstra(const graph<long long, double> &G, long long start,
                           long long target,
                           const set<long long> &ignoreNodes) {
  unordered_map<long long, double> dist;
  unordered_map<long long, long long> prev;
  unordered_set<long long> visited;
  
  priority_queue<pair<double, long long>,
  vector<pair<double, long long>>,
  greater<pair<double, long long>>> pq;
  
  for (long long v : G.getVertices()) {
    dist[v] = INF;
  }

  dist[start] = 0.0;
  pq.push({0.0, start});
  
  while (!pq.empty()) {
    auto [currDist, curr] = pq.top();
    pq.pop();
    
    if ((curr != start && curr != target) && ignoreNodes.count(curr)) {
      continue;
    }
    
    visited.insert(curr);
    
    if (curr == target) {
      break;
    }
    
    for (long long neighbor : G.neighbors(curr)) {
      if ((neighbor != start && neighbor != target) && ignoreNodes.count(neighbor)) {
        continue;  
      }
    
      double weight;
      if (!G.getWeight(curr, neighbor, weight)) {
        continue;  
      }
      
      double alt = dist[curr] + weight;
      if (alt < dist[neighbor]) {
        dist[neighbor] = alt;
        prev[neighbor] = curr;
        pq.push({alt, neighbor});
      }
    }
  }
  
  
  vector<long long> path;
  if (!visited.count(target)) {
    return path;
  }
  
  for (long long at = target; at != start; at = prev[at]) {
    path.push_back(at);
  }
  path.push_back(start);
  reverse(path.begin(), path.end());
  
  return path;
                            
  return vector<long long>{};
}



double pathLength(const graph<long long, double> &G,
                  const vector<long long> &path) {
  double length = 0.0;
  double weight;
  for (size_t i = 0; i + 1 < path.size(); i++) {
    bool res = G.getWeight(path.at(i), path.at(i + 1), weight);
    if (!res) {
      return -1;
    }
    length += weight;
  }
  return length;
}

void outputPath(const vector<long long> &path) {
  for (size_t i = 0; i < path.size(); i++) {
    cout << path.at(i);
    if (i != path.size() - 1) {
      cout << "->";
    }
  }
  cout << endl;
}

void application(const vector<BuildingInfo> &buildings,
                 const graph<long long, double> &G) {
  string person1Building, person2Building;

  set<long long> buildingNodes;
  for (const auto &building : buildings) {
    buildingNodes.insert(building.id);
  }

  cout << endl;
  cout << "Enter person 1's building (partial name or abbreviation), or #> ";
  getline(cin, person1Building);

  while (person1Building != "#") {
    cout << "Enter person 2's building (partial name or abbreviation)> ";
    getline(cin, person2Building);

    BuildingInfo p1 = getBuildingInfo(buildings, person1Building);
    BuildingInfo p2 = getBuildingInfo(buildings, person2Building);
    Coordinates P1Coords, P2Coords;
    string P1Name, P2Name;

    if (p1.id == -1) {
      cout << "Person 1's building not found" << endl;
    } else if (p2.id == -1) {
      cout << "Person 2's building not found" << endl;
    } else {
      cout << endl;
      cout << "Person 1's point:" << endl;
      cout << " " << p1.name << endl;
      cout << " " << p1.id << endl;
      cout << " (" << p1.location.lat << ", " << p1.location.lon << ")" << endl;
      cout << "Person 2's point:" << endl;
      cout << " " << p2.name << endl;
      cout << " " << p2.id << endl;
      cout << " (" << p2.location.lon << ", " << p2.location.lon << ")" << endl;

      Coordinates centerCoords = centerBetween2Points(p1.location, p2.location);
      BuildingInfo dest = getClosestBuilding(buildings, centerCoords);

      cout << "Destination Building:" << endl;
      cout << " " << dest.name << endl;
      cout << " " << dest.id << endl;
      cout << " (" << dest.location.lat << ", " << dest.location.lon << ")"
           << endl;

      vector<long long> P1Path = dijkstra(G, p1.id, dest.id, buildingNodes);
      vector<long long> P2Path = dijkstra(G, p2.id, dest.id, buildingNodes);

      if (P1Path.empty() || P2Path.empty()) {
        cout << endl;
        cout << "At least one person was unable to reach the destination "
                "building. Is an edge missing?"
             << endl;
        cout << endl;
      } else {
        cout << endl;
        cout << "Person 1's distance to dest: " << pathLength(G, P1Path);
        cout << " miles" << endl;
        cout << "Path: ";
        outputPath(P1Path);
        cout << endl;
        cout << "Person 2's distance to dest: " << pathLength(G, P2Path);
        cout << " miles" << endl;
        cout << "Path: ";
        outputPath(P2Path);
      }
    }

    cout << endl;
    cout << "Enter person 1's building (partial name or abbreviation), or #> ";
    getline(cin, person1Building);
  }
}