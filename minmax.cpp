#include <iostream>
#include <vector>
#include <string>
#include <climits>
using namespace std;
int n;
char youplay;
char opponentplay;
int cell_value[26 + 1][26 + 1];
int depth;
struct State
{
	vector<string> curr_state;
	int curr_value;
	string move;
	int p;
	int q;
};
int m_x[4] = {1,0,0,-1};
int m_y[4] = {0,1,-1,0};


bool isValidPoint(int x, int y){
	if (x >= n || x < 0 || y >= n || y < 0) {
		return false;
	}

	return true;
}


bool isRaidValid(State st, int i, int j,char play) {
	bool flag = false;
	for (int k = 0; k < 4; ++k)
	{
			int x = i + m_x[k];
			int y = j + m_y[k];
			if (isValidPoint(x,y) && st.curr_state[x][y] == play) {
				flag = true;
				break;
			}
	}

	return flag;
}

void printState(State st) {
	for (int i = 0; i < n; ++i)
	{
		cout << st.curr_state[i] << endl;
	}
}

struct State minmax(State intermediate_st, int curr_d) {
	bool terminate = true;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if (intermediate_st.curr_state[i][j] == '.') {
				terminate = false;
			}
		}
	}
	if (terminate == true) {
		return intermediate_st;
	}
	if (curr_d == depth) {
		return intermediate_st;
	}
	if (curr_d % 2 == 0) {
		State max_st;
		max_st.curr_value = INT_MIN;
		// need to check the condition when all nodes are filled
		//do stake
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				if (intermediate_st.curr_state[i][j] == '.')
				{
					State temp_st = intermediate_st;
					temp_st.curr_state[i][j] = youplay;
					temp_st.p = i;
					temp_st.q = j;
					temp_st.curr_value = temp_st.curr_value + cell_value[i][j];
					temp_st.move = "stake";
					State ret_st = minmax(temp_st, curr_d + 1);
					if (max_st.curr_value < ret_st.curr_value) {
						max_st.curr_state = temp_st.curr_state;
						max_st.curr_value = ret_st.curr_value;
						max_st.move = "stake";
						max_st.p = temp_st.p;
						max_st.q = temp_st.q;
					}
				}
			}
		}
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				if (intermediate_st.curr_state[i][j] == '.')
				{
					State temp_st = intermediate_st;
					temp_st.p = i;
					temp_st.q = j;
					temp_st.curr_state[i][j] = youplay;
					temp_st.curr_value = temp_st.curr_value + cell_value[i][j];
					if (isRaidValid(temp_st, i, j, youplay)) {
						for (int k = 0; k < 4; ++k)
						{
							int x = i + m_x[k];
							int y = j + m_y[k];
							if (isValidPoint(x,y) && (temp_st.curr_state[x][y] == opponentplay)) {
								temp_st.curr_value = temp_st.curr_value + 2 * cell_value[x][y];
								temp_st.curr_state[x][y] = youplay;
							}
						}
						temp_st.move = "raid";
						State ret_st = minmax(temp_st, curr_d + 1);
						if (max_st.curr_value < ret_st.curr_value) {
							max_st.curr_state = temp_st.curr_state;
							max_st.curr_value = ret_st.curr_value;
							max_st.move = "raid";
							max_st.p = temp_st.p;
							max_st.q = temp_st.q;
						}
					}
				}
			}
		}
		return max_st;
	}
	else {
		State min_st;
		min_st.curr_value = INT_MAX;
		// need to check the condition when all nodes are filled
		//do stake
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				if (intermediate_st.curr_state[i][j] == '.')
				{
					State temp_st = intermediate_st;
					temp_st.p = i;
					temp_st.q = j;
					temp_st.curr_state[i][j] = opponentplay;
					temp_st.curr_value = temp_st.curr_value - cell_value[i][j];
					temp_st.move = "stake";
					State ret_st = minmax(temp_st, curr_d + 1);
					if (ret_st.curr_value < min_st.curr_value) {
						min_st.curr_state = temp_st.curr_state;
						min_st.curr_value = ret_st.curr_value;
						min_st.move = "stake";
						min_st.p = temp_st.p;
						min_st.q = temp_st.q;
					}
				}
			}
		}
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				if (intermediate_st.curr_state[i][j] == '.')
				{
					State temp_st = intermediate_st;
					temp_st.p = i;
					temp_st.q = j;
					temp_st.curr_state[i][j] = opponentplay;
					temp_st.curr_value = temp_st.curr_value - cell_value[i][j];
					if (isRaidValid(temp_st, i, j, opponentplay)) {
						for (int k = 0; k < 4; ++k)
						{
							int x = i + m_x[k];
							int y = j + m_y[k];
							if (isValidPoint(x,y) && (temp_st.curr_state[x][y] == youplay)) {
								temp_st.curr_value = temp_st.curr_value - 2 * cell_value[x][y];
								temp_st.curr_state[x][y] = opponentplay;
							}
						}
						temp_st.move = "raid";
						State ret_st = minmax(temp_st, curr_d + 1);
						if (ret_st.curr_value < min_st.curr_value) {
							min_st.curr_state = temp_st.curr_state;
							min_st.curr_value = ret_st.curr_value;
							min_st.move = "raid";
							min_st.p = temp_st.p;
							min_st.q = temp_st.q;
						}
					}
				}
			}
		}
		return min_st;
	}
}
int main(int argc, char const *argv[])
{
	
	cin >> n;
	string mode;
	cin >> mode;
	
	cin >> youplay;

	cin >> depth;
	opponentplay = ((youplay == 'O') ? 'X' : 'O');
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			cin >> cell_value[i][j];
		}
	}
	vector <string>v;
	string s;
	int initial_value = 0;
	for (int i = 0; i < n; ++i)
	{
		cin >> s;
		for (int j = 0; j < s.length(); ++j)
		{
			if (youplay == s[j])
			{
				initial_value = initial_value + cell_value[i][j];
			} else if (s[j] == opponentplay)
			{
				initial_value = initial_value - cell_value[i][j];
			}
		}
		v.push_back(s);
	}
	State initial_st,final_st;
	initial_st.curr_state = v;
	initial_st.curr_value = initial_value;
	initial_st.move = "";
	final_st = minmax(initial_st, 0);


	cout << char('A' + final_st.q) << final_st.p + 1 << " "; 
	cout << final_st.move << endl;
	printState(final_st);
	// cout << n << endl;
	// cout << mode << endl;
	// cout << youplay << endl;
	// cout << depth << endl;
	// for (int i = 0; i < n; ++i)
	// {
	// 	for (int j = 0; j < n; ++j)
	// 	{
	// 		cout << cell_value[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// for (int i = 0; i < n; ++i)
	// {
	// 	cout << v[i] << endl;
	// }
	return 0;
}