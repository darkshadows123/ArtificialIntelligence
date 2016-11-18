#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include <fstream>
#include <map>
#include <stack>
#include <queue>
#include <list>
using namespace std;

struct edge
{
	unsigned long long cost;
	string location;
};
struct pQ
{
	unsigned long long cost;
	string parent;
	string location;
};

struct aStarpQ
{
	unsigned long long heuristic_cost;
	unsigned long long cost;
	string location;
};

string start_state;
string goal_state;

void dfs(map<string, std::vector<edge> > adj, map<string, unsigned long long> sunday_heuristic_cost, vector<edge> sunday_traffic) {
	stack<string> s;
	s.push(start_state);
	std::map<string, string> parent;
	std::map<string, bool> visited;
	bool flag = true;
	while(!s.empty()) {
		string cur = s.top();
		s.pop();
		visited[cur] = true;
		if (cur == goal_state) {
					flag = false;
					break;
		}
		for (int i = (adj[cur].size() - 1); i >= 0; i--) {
			if (visited.find(adj[cur][i].location) == visited.end()) {
				s.push(adj[cur][i].location);
				parent[adj[cur][i].location] = cur;
				visited[adj[cur][i].location] = true;
			}
		}
	}
	string cur = goal_state;
	std::vector<string> path;
	path.push_back(cur);
	while (cur != start_state) {
			cur = parent[cur];
			path.push_back(cur);
	}
	for (int i = (path.size() - 1); i >= 0; --i)
	{
		cout << path[i] << " " << (path.size() - i - 1) << endl;
	}
}

void bfs(map<string, std::vector<edge> > adj, map<string, unsigned long long> sunday_heuristic_cost, vector<edge> sunday_traffic) {
	list<string> q;
	q.push_back(start_state);
	std::map<string, string> parent;
	std::map<string, bool> visited;
	visited[start_state] = true;
	while(!q.empty()) {
		string cur = q.front();
		q.pop_front();
		// cout << "cur = " << cur << endl;
		bool flag = true; 
		for (int i = 0; i < adj[cur].size(); i++) {
			if (visited.find(adj[cur][i].location) == visited.end()) {
				q.push_back(adj[cur][i].location);
				visited[adj[cur][i].location] = true;
				parent[adj[cur][i].location] = cur;
				if (adj[cur][i].location == goal_state) {
					flag = false;
					break;
				}
			}
		}
		if (flag == false) {
			break;
		}
	}
	string cur = goal_state;
	std::vector<string> path;
	path.push_back(cur);
	while (cur != start_state) {
			cur = parent[cur];
			path.push_back(cur);
	}
	for (int i = (path.size() - 1); i >= 0; --i)
	{
		cout << path[i] << " " << (path.size() - i - 1) << endl;
	}
}

// void displayUCFPath(struct pQ p)
// {
//   list<string>::iterator i;
//   // for (i = p.path.begin(); i != p.path.end(); ++i)
//   // {
//   //   cout<<"->"<<*i;
//   // cout<<" Pathlength: "<<p.cost;
//   // }
//   for (int i = 0; i < p.path.size(); ++i)
//   {
//   	cout << p.path[i] << " " << p.each_step_cost[i] << endl;
//   }
// }
bool compareUCFCost(const pQ &lhs, const pQ &rhs)
{
    return lhs.cost < rhs.cost;
}
struct find_location : std::unary_function<pQ, bool> {
    string location;
    find_location(string location):location(location) { }
    bool operator()(pQ const& pq) const {
        return pq.location == location;
    }
};


void UCF(map<string, std::vector<edge> > adj, map<string, unsigned long long> sunday_heuristic_cost, vector<edge> sunday_traffic)
{
	list<pQ> open;
	struct pQ temp;
	temp.cost = 0;
	std::map<string, pQ> parent;
	temp.location = start_state;
	open.push_back(temp);
	list<pQ> closed;
	while (!open.empty()) {
		pQ NODE = open.front();
		string currnode = open.front().location;
		unsigned long long curr_cost = open.front().cost;
		// cout << "currnode = " << currnode << " cost = " << curr_cost << endl;
		open.pop_front();
		if (currnode == goal_state){
			// cout << "found path" << endl;
			break;
		}
		for (int i = 0; i < adj[currnode].size(); ++i) {
				list <pQ> ::iterator pq_it = std::find_if(open.begin(), open.end(), 
				         find_location(adj[currnode][i].location));
				list <pQ> :: iterator s_it = std::find_if(closed.begin(), closed.end(),
					find_location(adj[currnode][i].location));
				temp.cost = curr_cost + adj[currnode][i].cost;
				temp.location = adj[currnode][i].location;
				pQ pq;
				pq.cost = temp.cost;
				pq.location = currnode;
				if (pq_it == open.end() && s_it == closed.end()) {
					// cout << "children " << adj[currnode][i].location << endl;
					// cout << "parent " << currnode << endl;
					
					parent[adj[currnode][i].location] = pq;
					open.push_back(temp);
				} else if (s_it != closed.end()) {
					if (temp.cost < s_it->cost) {
						closed.erase(s_it);
						parent[adj[currnode][i].location] = pq;
						open.push_back(temp);
					}
				}
				else if (pq_it != open.end()) {
					if (temp.cost < pq_it->cost) {
						open.erase(pq_it);
						parent[adj[currnode][i].location] = pq;
						open.push_back(temp);
					}	
				}
		}
		closed.push_back(NODE);
		open.sort(&compareUCFCost);
	}
	string cur = goal_state;
	std::vector<string> path;
	path.push_back(cur);
	while (cur != start_state) {
			cur = parent[cur].location;
			path.push_back(cur);
	}
	for (int i = (path.size() - 1); i >= 0; --i)
	{
		cout << path[i] << " " << parent[path[i]].cost << endl;
	}
}

bool compareAStarCost(const aStarpQ &lhs, const aStarpQ &rhs)
{
    return lhs.heuristic_cost < rhs.heuristic_cost;
}
struct find_astar_location : std::unary_function<aStarpQ, bool> {
    string location;
    find_astar_location(string location):location(location) { }
    bool operator()(aStarpQ const& astarpq) const {
        return astarpq.location == location;
    }
};

void AStar(map<string, std::vector<edge> > adj, map<string, unsigned long long> sunday_heuristic_cost, vector<edge> sunday_traffic)
{

	list<aStarpQ> open;
	struct aStarpQ temp;
	temp.cost = 0;
	temp.heuristic_cost = sunday_heuristic_cost[start_state];
	std::map<string, aStarpQ> parent;
	temp.location = start_state;
	open.push_back(temp);
	list<aStarpQ> closed;
	while (!open.empty()) {
		aStarpQ NODE = open.front();
		string currnode = open.front().location;
		unsigned long long curr_cost = open.front().cost;
		open.pop_front();
		if (currnode == goal_state){
			// cout << "found path" << endl;
			break;
		}
		for (int i = 0; i < adj[currnode].size(); ++i) {
				list <aStarpQ> ::iterator pq_it = std::find_if(open.begin(), open.end(), 
				         find_astar_location(adj[currnode][i].location));
				list <aStarpQ> :: iterator s_it = std::find_if(closed.begin(), closed.end(),
					find_astar_location(adj[currnode][i].location));
				temp.cost = curr_cost + adj[currnode][i].cost;
				temp.heuristic_cost = temp.cost + sunday_heuristic_cost[adj[currnode][i].location];
				temp.location = adj[currnode][i].location;
				aStarpQ pq;
				pq.cost = temp.cost;
				pq.location = currnode;
				if (pq_it == open.end() && s_it == closed.end()) {
					// cout << "children " << adj[currnode][i].location << endl;
					// cout << "parent " << currnode << endl;
					
					parent[adj[currnode][i].location] = pq;
					open.push_back(temp);
				} else if (s_it != closed.end()) {
					if (temp.heuristic_cost < s_it->heuristic_cost) {
						closed.erase(s_it);
						parent[adj[currnode][i].location] = pq;
						open.push_back(temp);
					}
				} else if (pq_it != open.end()) {
					if (temp.heuristic_cost < pq_it->heuristic_cost) {
						open.erase(pq_it);
						parent[adj[currnode][i].location] = pq;
						open.push_back(temp);
					}
				}
				
		}
		closed.push_back(NODE);
		open.sort(&compareAStarCost);
	}
	string cur = goal_state;
	std::vector<string> path;
	path.push_back(cur);
	while (cur != start_state) {
			cur = parent[cur].location;
			path.push_back(cur);
	}
	for (int i = (path.size() - 1); i >= 0; --i)
	{
		cout << path[i] << " " << parent[path[i]].cost << endl;
	}

}
template <typename T>
string NumberToString(T pNumber)
{
 ostringstream oOStrStream;
 oOStrStream << pNumber;
 return oOStrStream.str();
}
int main(int argc, char const *argv[])
{
//	freopen("input.txt","r",stdin);
//	freopen("output.txt","w",stdout);
	// for (int x = 0 ;x < 85;x++) {
	// string str1 = NumberToString(x);
	// string in = "input" + str1 + ".txt";
	// string out = "output" + str1 + ".txt";
	// const char *in1 = in.c_str();
	// const char *out1 = out.c_str();
	// freopen(in1,"r",stdin);
	//  freopen(out1,"w",stdout);
	string algo;
	unsigned long long live_traffic_lines;
	unsigned long long sunday_traffic_lines;
	std::map<string, std::vector<edge> > adj;
	std::map<string, unsigned long long> sunday_heuristic_cost;
	std::vector<edge> sunday_traffic;
	cin>>algo;
	cin>>start_state;	
	cin>>goal_state;
	cin>>live_traffic_lines;
	for (int i = 0; i < live_traffic_lines; ++i)
	{
		string source,destination;
		unsigned long long in_cost;
		cin >> source >> destination >> in_cost;
		edge e;
		e.location = destination;
		e.cost = in_cost;
		adj[source].push_back(e);
	}
	cin >> sunday_traffic_lines;
	for (int i = 0; i < sunday_traffic_lines; ++i)
	{
		string destination;
		unsigned long long in_cost;
		edge e;
		cin >> destination >> in_cost;
		// cout << "here" << endl;
		e.location = destination;
		e.cost = in_cost;
		sunday_traffic.push_back(e);
		sunday_heuristic_cost[destination] = in_cost;
	}
	// cout << "in the end" << endl;
	if (algo == "DFS") {
		dfs(adj,sunday_heuristic_cost,sunday_traffic);
	} else if (algo == "BFS") {
		bfs(adj,sunday_heuristic_cost,sunday_traffic);
	} else if (algo == "UCS") {
		UCF(adj,sunday_heuristic_cost,sunday_traffic);
	} else if (algo == "A*") {
		AStar(adj,sunday_heuristic_cost,sunday_traffic);
	}
	// map<string, vector<edge> > ::iterator it;
	// for (it = adj.begin(); it != adj.end() ; ++it)
	// {
	// 	cout << "edges of " << it->first << endl;
	// 	for (int i = 0; i < adj[it->first].size(); ++i)
	// 	{
	// 		cout << adj[it->first][i].location << " " << adj[it->first][i].cost << endl;
	// 	}
	// }
	// cout << "here" << endl;
	// for (int i = 0; i < sunday_traffic.size(); ++i)
	// {
	// 	cout << sunday_traffic[i].location << " " << sunday_traffic[i].cost << endl;
	// }
	// cout << goal_state << endl; 
	return 0;
}
	