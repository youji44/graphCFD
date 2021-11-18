#include <iostream>
#include <limits>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>
#include <fstream>
#include "vertex.cpp"
#include <string.h>
#include <limits.h>
#include <list>

using namespace std;

class Graph {
private:
    bool directed;
    vector<Vertex*> vertices;
    int** matrix;

    Vertex* getVertex(char label) {
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices[i]->getLabel() == label) {
                return vertices[i];
            }
        }
        return nullptr;
    }
    int getVertexIndex(char label) {
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices[i]->getLabel() == label) {
                return i;
            }
        }
        return -1;
    }
    void createMatrix(){
        int countVertexes = vertices.size();
        matrix = new int* [countVertexes];
        for (int i = 0; i < countVertexes; i++) {
            matrix[i] = new int[countVertexes];
            memset(matrix[i], 0, sizeof(int) * countVertexes);
        }
    }
    void clearMarks() {
        for (int i = 0; i < vertices.size(); i++) {
            vertices[i]->clearMark();
        }
    }

    void dfsHelper(Vertex* from, Vertex* to) {
        if (from) {
            cout << from->getLabel() << " - " << to->getLabel() << endl;
        }
        to->setMark();
        int index = getVertexIndex(to->getLabel());
        for (int i = 0; i < vertices.size(); i++) {
            if (matrix[index][i] > 0) {
                Vertex* neighbor = vertices[i];
                if (!neighbor->isMarked()) {
                    dfsHelper(to, neighbor);
                }
            }            
        }
    }

    void topSortHelper(Vertex* to) {
        to->setMark();
        int index = getVertexIndex(to->getLabel());
        for (int i = 0; i < vertices.size(); i++) {
            if (matrix[index][i] > 0) {
                Vertex* neighbor = vertices[i];
                if (!neighbor->isMarked()) {
                    topSortHelper(neighbor);
                }
            }            
        }
        cout << to->getLabel() << " ";
    }

public:
    Graph(bool directed) { 
        this->directed = directed;
        matrix = NULL;
    }
    Graph(bool directed,string filename) {
        this->directed = directed;
        matrix = NULL;

        ifstream infile(filename);
        if (!infile.is_open()) {
            cout << "Can't open this " << filename << " file" << endl;
            return;
        }
        
        string header;
        infile >> header;
        if (header == "Vertices") {
            while (true) {
                infile >> header;
                if (header != "Edges")
                    addVertex(header.at(0));
                else
                    break;
            }
            while (!infile.eof()) {
                char label1, label2;
                int weight;
                infile >> label1 >> label2 >> weight;
                addEdge(label1, label2, weight);
            }
        }       

    }
    bool addVertex(char label) {
        if (getVertex(label)) return false;  // already exists
        Vertex* newV = new Vertex(label);
        vertices.push_back(newV);
        return true;
    }

    bool addEdge(char label1, char label2) {
        if (matrix == NULL)
            createMatrix();
        int index1 = getVertexIndex(label1);
        int index2 = getVertexIndex(label2);
        if (index1 == -1 || index2 == -1)
            return false;
        matrix[index1][index2] = 1;
        if (!directed) {
            matrix[index2][index1] = 1;
        }
        return true;
    }

    bool addEdge(char label1, char label2, int edgeWeight) {
        if (matrix == NULL)
            createMatrix();
        int index1 = getVertexIndex(label1);
        int index2 = getVertexIndex(label2);
        if (index1 == -1 || index2 == -1)
            return false;
        matrix[index1][index2] = edgeWeight;
        if (!directed) {
            matrix[index2][index1] = edgeWeight;
        }
        return true;
    }

    void dfs() {
        clearMarks();
        for (int i = 0; i < vertices.size(); i++) {
            if (!vertices[i]->isMarked()) {
                dfsHelper(nullptr, vertices[i]);
            }
        }
        clearMarks();
    }

    void topologicalSort(char label) {
        clearMarks();
        topSortHelper(getVertex(label));
        cout << endl;
        clearMarks();
    }

    void bfs() {
        clearMarks();
        queue<Vertex*> q;
        q.push(vertices[0]);
        vertices[0]->setMark();
        while (!q.empty()) {
            Vertex* v = q.front();
            q.pop();
            cout << v->getLabel() << " " << endl;
            int index = getVertexIndex(v->getLabel());
            for (int i = 0; i < vertices.size(); i++) {
                if (matrix[index][i] > 0) {
                    Vertex* neighbor = vertices[i];
                    if (!neighbor->isMarked()) {
                        neighbor->setMark();
                        q.push(neighbor);
                    }
                }                
            }
        }
        clearMarks();
    }

    void dijkstrasAlg(char sourceLabel) {
        clearMarks();

        // create an unordered_map where the first thing in each pair
        // is the Vertex* and the second thing is the int that
        // represents the length of the shortest known path from
        // the source vertex to this one
        unordered_map<Vertex*, int> distances;
        int count = 0;

        // in this map, initialize all of the values to infinity
        for (int i = 0; i < vertices.size(); i++) {
            distances.insert(make_pair(vertices[i], INT_MAX));
        }

        // then set the one for the source vertex to zero
        Vertex* current = getVertex(sourceLabel);
        if (!current) return;
        distances[current] = 0;

        while (count < vertices.size()) {
            // find the unmarked vertex that goes with the smallest
            // value in the map
            int smallest = INT_MAX;
            Vertex* current = nullptr;

            for (auto& it : distances) {
                if (it.second < smallest && !it.first->isMarked()) {
                    smallest = it.second;
                    current = it.first;
                }
            }

            // loop over all of the neighbors of this current vertex
            if (current != nullptr) {
                int index = getVertexIndex(current->getLabel());
                for (int i = 0; i < vertices.size(); i++) {
                    // if the distance from the source vertex to the current one
                    // PLUS the distance from the current one to the neighbor that
                    // we're looking at right now is less than the value of that
                    // neighbor in the map, update that neighbor's map value to this
                    // sum
                    if (matrix[index][i] <= 0)
                        continue;
                    Vertex* neighbor = vertices[i];
                    int sourceToCurrent = distances[current];
                    int currentToNeighbor = matrix[index][i];
                    if (sourceToCurrent + currentToNeighbor < distances[neighbor]) {
                        distances[neighbor] = sourceToCurrent + currentToNeighbor;
                    }
                }
                current->setMark();
            }
            
            count++;
        } // while there are still unmarked vertices

        // display results
        for (auto& it : distances) {
            cout << it.first->getLabel() << ": " << it.second << endl;
        }

        clearMarks();
    }


    void prim() {

        clearMarks();

        // empty graph
        if (!vertices.size()) return;

        unordered_map<Vertex*, pair<int, Vertex*> > distances;
        int count = 0;

        // in this map, initialize all of the values to infinity
        for (int i = 0; i < vertices.size(); i++) {
            distances.insert(make_pair(vertices[i], make_pair(INT_MAX, nullptr)));
        }

        // then pick any vertex to current from 
        Vertex* current = vertices[0];

        while (count < vertices.size() - 1) {
            if (current != nullptr) {
                distances[current] = make_pair(0, nullptr);
                current->setMark();
            }
            
            count++;
            int index = 0;
            if (current != nullptr) {
                index = getVertexIndex(current->getLabel());
            }
            
            
            for (int i = 0; i < vertices.size(); i++) {
                if (matrix[index][i] <= 0)
                    continue;
                if (matrix[index][i] < distances[vertices[i]].first) {
                    distances[vertices[i]] =
                        make_pair(matrix[index][i], current);
                }
            }

            int smallest = INT_MAX;
            Vertex* next = nullptr;
            Vertex* from = nullptr;

            for (auto& it : distances) {
                if (it.second.first < smallest && !it.first->isMarked()) {
                    smallest = it.second.first;
                    next = it.first;
                    from = it.second.second;
                }
            }
            if (from != nullptr && next != nullptr) {
                cout << from->getLabel() << " - " << next->getLabel() << endl;
                current = next;
            }
        }

        clearMarks();

    }
    int getInDegree(char vertexLabel) {
        int index = getVertexIndex(vertexLabel);
        int count = 0;
        for (int i = 0; i < vertices.size(); i++) {
            if (matrix[i][index] > 0)
                count++;
        }
        return count;
    }
    int getOutDegree(char vertexLabel) {
        int index = getVertexIndex(vertexLabel);
        int count = 0;
        for (int i = 0; i < vertices.size(); i++) {
            if (matrix[index][i] > 0)
                count++;
        }
        return count;
    }
    vector<Vertex*> getMaxInDegree(int& maxDegree) {
        vector<Vertex*> vets;
        for (int i = 0; i < vertices.size(); i++)
            if (getInDegree(vertices[i]->getLabel()) == maxDegree)
                vets.push_back(vertices[i]);
        return vets;
    }
    vector<Vertex*> getMaxOutDegree(int& maxDegree) {
        vector<Vertex*> vets;
        for (int i = 0; i < vertices.size(); i++)
            if (getOutDegree(vertices[i]->getLabel()) == maxDegree)
                vets.push_back(vertices[i]);
        return vets;
    }
    vector<Vertex*> getNeighborhood(char vertexLabel, int neighborhoodSize) {
        int index = getVertexIndex(vertexLabel);
        vector<Vertex*> vets;
        
        for (int i = 0; i < vertices.size(); i++)
        {
            if (matrix[index][i] > 0)
                vets.push_back(vertices[i]);
        }
        return vets;
    }
    vector<Vertex*> connected;
    vector<Vertex*> getLargestConnectedComponent() {
        bool* visited = new bool[vertices.size()];
        for (int v = 0; v < vertices.size(); v++)
            visited[v] = false;
        connected.clear();
        
        for (int v = 0; v < vertices.size(); v++) {
            if (visited[v] == false) {
                
                DFSUtil(v, visited);

                cout << "\n";
            }
        }
        delete[] visited;

        return connected;
    }
    void DFSUtil(int v, bool visited[])
    {
        visited[v] = true;
        connected.push_back(vertices[v]);

        list<int>::iterator i;
        for (int i = 0; i < vertices.size(); i++) {
            if (matrix[v][i] > 0)
            if (!visited[i])
                DFSUtil(i, visited);
        }
            
    }
};
