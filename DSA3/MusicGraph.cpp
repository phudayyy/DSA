#include "MusicGraph.h"

// =============================================================================
// PRIVATE HELPER METHODS
// =============================================================================

int MusicGraph::getSongIndex(const string& id) const {
    // TODO: Implement linear search to find the index of a song by its ID
    for (size_t i = 0; i < songsList.size(); ++i) {
        if (songsList[i].id == id) {
            return i;
        }
    }
    return -1;
}

bool MusicGraph::isVisited(const string& id, const vector<string>& visitedList) const {
    // TODO: Check if the given ID exists in the visitedList
    for (size_t i = 0; i < visitedList.size(); ++i) {
        if (visitedList[i] == id) {
            return true;
        }
    }
    return false;
}

// =============================================================================
// PUBLIC METHODS
// =============================================================================

void MusicGraph::addSong(const string& id, const string& title, const string& artist, const string& genre) {
    // TODO: Add a new song to the system and add its ID as a vertex in the graph
    Song newSong = {id, title, artist, genre};
    songsList.push_back({id, newSong});
    this->addVertex(id);
}

void MusicGraph::printSongInfo(const string& id) const {
    // TODO: Print the song information in the required format
    int idx = getSongIndex(id);
    if (idx != -1) {
        const Song& s = songsList[idx].data;
        cout << "[" << s.id << "] " << s.title << " - " << s.artist;
    }
}

// =============================================================================
// REQUIREMENT 1: Recommend related songs (BFS)
// =============================================================================
void MusicGraph::recommendRelatedSongs(const string& startId) const {
    if (!this->hasVertex(startId)) return;

    cout << "\n[1] SUGGEST RELATED SONGS FOR: ";
    printSongInfo(startId);
    cout << "\n-------------------------------------------------\n";

    // TODO: Implement Breadth-First Search (BFS) to recommend related songs
    vector<string> q;
    int head = 0;
    vector<string> visitedList;

    q.push_back(startId);
    visitedList.push_back(startId);

    // BFS Loop
    while (head < (int)q.size()) {
        string u = q[head++]; // Pop phần tử đầu Queue

        // Không in bài hát khởi đầu
        if (u != startId) {
            cout << "-> ";
            printSongInfo(u);
            cout << "\n";
        }

        vector<Edge> neighbors = getNeighbors(u);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            if (!isVisited(neighbors[i].target, visitedList)) {
                visitedList.push_back(neighbors[i].target);
                q.push_back(neighbors[i].target);
            }
        }
    }
}

// =============================================================================
// REQUIREMENT 2: Create playlist by clusters (Connected Components using BFS)
// =============================================================================
void MusicGraph::generatePlaylistsByClusters() const {
    cout << "\n[2] CREATE PLAYLIST BY CLUSTERS (CONNECTED COMPONENTS)\n";
    cout << "-------------------------------------------------\n";

    vector<string> globalVisited;
    int playlistCount = 1;

    // Duyệt qua tất cả các đỉnh trong đồ thị
    for (size_t i = 0; i < adjList.size(); ++i) {
        string startId = adjList[i].vertex;
        
        if (!isVisited(startId, globalVisited)) {
            cout << "=== Playlist " << playlistCount++ << " ===\n";
            
            // BFS cho cụm liên thông này
            vector<string> q;
            int head = 0;
            
            q.push_back(startId);
            globalVisited.push_back(startId);
            
            while (head < (int)q.size()) {
                string u = q[head++];
                
                cout << "  * ";
                printSongInfo(u);
                cout << "\n";
                
                // 1. Duyệt các cạnh ĐI RA (outgoing edges): u -> target
                vector<Edge> neighbors = getNeighbors(u);
                for (size_t j = 0; j < neighbors.size(); ++j) {
                    if (!isVisited(neighbors[j].target, globalVisited)) {
                        globalVisited.push_back(neighbors[j].target);
                        q.push_back(neighbors[j].target);
                    }
                }

                // 2. DUYỆT THÊM CÁC CẠNH ĐI VÀO (incoming edges): v -> u
                // Phục vụ cho yêu cầu đồ thị vô hướng
                for (size_t k = 0; k < adjList.size(); ++k) {
                    string v = adjList[k].vertex;
                    
                    // Nếu đỉnh v chưa được thăm, ta kiểm tra xem có cạnh từ v đến u không
                    if (!isVisited(v, globalVisited)) {
                        vector<Edge> vNeighbors = getNeighbors(v);
                        for (size_t m = 0; m < vNeighbors.size(); ++m) {
                            if (vNeighbors[m].target == u) {
                                // Nếu tồn tại cạnh v -> u, thêm v vào cụm
                                globalVisited.push_back(v);
                                q.push_back(v);
                                break; // Đã tìm thấy liên kết, ngắt vòng lặp kiểm tra hàng xóm của v
                            }
                        }
                    }
                }
            }
        }
    }
}

// =============================================================================
// REQUIREMENT 3: Smooth song transition (Dijkstra's Algorithm)
// =============================================================================
void MusicGraph::findSmoothTransition(const string& startId, const string& endId) const {
    // ĐÃ XÓA ĐOẠN: if (!this->hasVertex(startId) || !this->hasVertex(endId)) return;

    // 1. In Header trước
    cout << "\n[3] SMOOTHEST TRANSITION (DIJKSTRA)\n";
    cout << "From: "; printSongInfo(startId); cout << "\n";
    cout << "To: "; printSongInfo(endId); cout << "\n";
    cout << "-------------------------------------------------\n";

    // 2. Lấy Index
    int n = this->adjList.size();
    int startIdx = this->getVertexIndex(startId);
    int endIdx = this->getVertexIndex(endId);

    // 3. Nếu không tồn tại thì in lỗi theo đúng format của testcase
    if (startIdx == -1 || endIdx == -1) {
        cout << "Error: Song not found!\n";
        return;
    }

    // 4. Bắt đầu thuật toán Dijkstra
    vector<double> dist(n, 1e9); // 1e9 đóng vai trò là Vô Cực
    vector<int> prev(n, -1);
    vector<bool> visited(n, false);

    dist[startIdx] = 0;

    for (int count = 0; count < n; ++count) {
        // Tìm đỉnh có dist nhỏ nhất chưa visited
        int u = -1;
        double minD = 1e9;
        for (int i = 0; i < n; ++i) {
            if (!visited[i] && dist[i] < minD) {
                minD = dist[i];
                u = i;
            }
        }

        // Nếu không tìm được đỉnh nào hoặc đã đến đích thì ngắt
        if (u == -1 || u == endIdx) break;

        visited[u] = true;

        // Cập nhật khoảng cách cho các hàng xóm
        vector<Edge> neighbors = getNeighbors(adjList[u].vertex);
        for (size_t i = 0; i < neighbors.size(); ++i) {
            int v = getVertexIndex(neighbors[i].target);
            if (v != -1 && !visited[v]) {
                double alt = dist[u] + neighbors[i].weight;
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                }
            }
        }
    }

    // 5. In kết quả nếu không có đường đi
    if (dist[endIdx] == 1e9) {
        cout << "No transition path between these two songs.\n";
        return;
    }

    // 6. Truy vết ngược để in đường đi
    vector<int> path;
    for (int at = endIdx; at != -1; at = prev[at]) {
        path.push_back(at);
    }

    cout << "-> Total Deviation (Cost): " << dist[endIdx] << "\n";
    cout << "-> Playback Order:\n";
    
    int step = 1;
    // In từ start -> end (duyệt ngược mảng truy vết)
    for (int i = (int)path.size() - 1; i >= 0; --i) {
        cout << "   " << step++ << ". ";
        printSongInfo(adjList[path[i]].vertex);
        cout << "\n";
    }
}

// =============================================================================
// REQUIREMENT 4: Find the network hub song (In-degree Calculation)
// =============================================================================
void MusicGraph::findMostPopularSong() const {
    if (songsList.empty()) { 
        return; 
    }

    cout << "\n[4] FIND NETWORK HUB SONG (IN-DEGREE)\n";
    cout << "-------------------------------------------------\n";

    // TODO: Calculate the in-degree of all vertices and find the one with the maximum value
    int n = adjList.size();
    if (n == 0) return;

    // Mảng lưu bậc vào (In-degree) của mỗi đỉnh
    vector<int> inDegree(n, 0);

    for (int i = 0; i < n; ++i) {
        vector<Edge> neighbors = getNeighbors(adjList[i].vertex);
        for (size_t j = 0; j < neighbors.size(); ++j) {
            int v = getVertexIndex(neighbors[j].target);
            if (v != -1) {
                inDegree[v]++;
            }
        }
    }

    // Tìm đỉnh có inDegree lớn nhất
    int maxIdx = 0;
    for (int i = 1; i < n; ++i) {
        if (inDegree[i] > inDegree[maxIdx]) {
            maxIdx = i;
        }
    }

    cout << "-> Network Hub Song:\n   ";
    printSongInfo(adjList[maxIdx].vertex);
    cout << "\n   (In-degree: " << inDegree[maxIdx] << ")\n";
}

// =============================================================================
// REQUIREMENT 5: Detect music loop (DFS Cycle Detection)
// =============================================================================

// DFS recursive helper function
bool MusicGraph::dfsCycleHelper(int idx, vector<bool>& visited, vector<bool>& recursionStack,
                                 vector<int>& parent, bool& found) const {
    // TODO: Implement the recursive DFS logic to detect cycles
    visited[idx] = true;
    recursionStack[idx] = true;

    vector<Edge> neighbors = getNeighbors(adjList[idx].vertex);
    for (size_t i = 0; i < neighbors.size(); ++i) {
        int v = getVertexIndex(neighbors[i].target);
        if (v == -1) continue;

        if (!visited[v]) {
            parent[v] = idx;
            if (dfsCycleHelper(v, visited, recursionStack, parent, found)) {
                return true;
            }
        } 
        // Nếu đỉnh v đã thăm và đang nằm trong stack đệ quy => Có chu trình
        else if (recursionStack[v]) {
            cout << "-> Music loop detected!\n";
            cout << "-> Loop:\n";
            
            vector<int> cyclePath;
            int curr = idx;
            
            // Truy vết ngược về v
            while (curr != v) {
                cyclePath.push_back(curr);
                curr = parent[curr];
            }
            cyclePath.push_back(v);
            
            // In chu trình theo chiều thuận (từ v -> idx -> v)
            for (int j = (int)cyclePath.size() - 1; j >= 0; --j) {
                cout << "   ";
                printSongInfo(adjList[cyclePath[j]].vertex);
                cout << "\n";
            }
            // In đỉnh kết thúc chu kỳ
            cout << "   ";
            printSongInfo(adjList[v].vertex);
            cout << "\n";
            
            found = true;
            return true;
        }
    }

    recursionStack[idx] = false;
    return false;
}

void MusicGraph::detectMusicLoop() const {
    cout << "\n[5] DETECT MUSIC LOOP (DFS CYCLE DETECTION)\n";
    cout << "-------------------------------------------------\n";

    // TODO: Initialize required arrays and start DFS to detect a music loop
    int n = adjList.size();
    vector<bool> visited(n, false);
    vector<bool> recursionStack(n, false);
    vector<int> parent(n, -1);
    bool found = false;

    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            if (dfsCycleHelper(i, visited, recursionStack, parent, found)) {
                return; // Nếu tìm thấy một chu trình thì dừng luôn
            }
        }
    }

    if (!found) {
        cout << "No music loop detected.\n";
    }
}