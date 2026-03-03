# Campus Pathfinding System

A C++ application that computes optimal walking routes across a university campus by modeling the map as a weighted graph and running Dijkstra's shortest-path algorithm. Built with an HTTP server and interactive map frontend.

## Features

- Parses OSM-format JSON map data to build a weighted adjacency-list graph of buildings, waypoints, and walkways
- Implements Dijkstra's algorithm with a priority queue for optimal route computation
- Connects buildings to nearby graph nodes and handles blocked or ignored vertices to simulate realistic navigation constraints
- Serves a web interface with an interactive Leaflet.js map for searching locations, finding nearby buildings, and visualizing paths
- REST API endpoints for pathfinding, nearby search, and nearest-node lookup

## Tech Stack

- **Language:** C++20
- **Build:** Make + Clang
- **Libraries:** [JSON for Modern C++](https://github.com/nlohmann/json), [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- **Testing:** Google Test / Google Mock
- 
## Build & Run

**Requirements:** Clang with C++20, Google Test (via Homebrew on macOS)

```bash
# Build and run tests
make test_all

# Run specific test suites
make test_graph
make test_dijkstra
make test_build_graph

# Run the server
make run_server

# Run the main program
make run_osm

# Clean build artifacts
make clean
```

## How It Works

1. **Graph Construction** — The app parses a JSON file containing OSM map data (nodes, ways, buildings) and builds an adjacency-list graph where edges are weighted by distance.
2. **Building Connection** — Buildings are connected to their nearest walkway nodes so they can be reached as start/end points.
3. **Pathfinding** — Dijkstra's algorithm finds the shortest path between two nodes, respecting any blocked or unreachable vertices.
4. **Server** — An HTTP server exposes endpoints that the frontend queries to render routes and search results on the map.
