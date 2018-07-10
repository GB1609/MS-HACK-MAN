#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <queue>

using namespace std;

class Point {
public:
	int x;
	int y;
	Point() {}
	Point(int x, int y): x(x), y(y) {}
	bool operator<(const Point &a) const {
		return x < a.x || (x == a.y && y < a.y);
	}
	bool operator==(const Point &a) const {
		return x == a.x && y == a.y;
	}
};

class Player : public Point {
public:
	string name;
	int id;
	int snippets;
	int bombs;
};

int width;
int height;
vector<vector<bool> > is_wall;
int timebank;
int time_per_move;
int time_remaining;
int max_rounds;
int current_round;
Player me;
Player enemy;
vector< Point > snippets;
vector< pair<Point, int> > weapons;
vector< pair<Point, int> > bugs;
vector< pair<Point, int> > spawn_points;
vector< pair<Point, int> > gates;
vector< vector<string> > cells;

void choose_character();
void do_move();

void process_next_command() {
	string command;
	cin >> command;
	if (command == "settings") {
		string type;
		cin >> type;
		if (type == "timebank") {
			cin >> timebank;
		} else if (type == "time_per_move") {
			cin >> time_per_move;
		} else if (type == "player_names") {
			string names;
			cin >> names;
			// player names aren't very useful
		} else if (type == "your_bot") {
			cin >> me.name;
		} else if (type == "your_botid") {
			cin >> me.id;
		} else if (type == "field_width") {
			cin >> width;
		} else if (type == "field_height") {
			cin >> height;
		} else if (type == "max_rounds") {
			cin >> max_rounds;
		}
	} else if (command == "update") {
		string player_name, type;
		cin >> player_name >> type;
		if (type == "round") {
			cin >> current_round;
		} else if (type == "field") {
			snippets.clear();
			weapons.clear();
			bugs.clear();
			gates.clear();
			spawn_points.clear();
			is_wall.clear();
			for (int x = 0; x < height; x++) {
				is_wall.push_back(vector<bool>(width));
			}
			string s;
			cin >> s;
			cells.clear();
			while (true) {
				size_t found = s.find(",");
				string t;
				if (found == string::npos) {
					t = s;
				} else {
					t = s.substr(0, found);
					s.erase(0, found + 1);
				}

				vector<string> a; a.clear();
				while (true) {
					size_t found = t.find(";");
					if (found == string::npos) {
						a.push_back(t);
						break;
					}
					a.push_back(t.substr(0, found));
					t.erase(0, found + 1);
				}
				cells.push_back(a);

				if (found == string::npos) {
					break;
				}
			}

			for (int x = 0; x < height; x++) {
				for (int y = 0; y < width; y++) {
					Point pt;
					pt.x = x;
					pt.y = y;
					int l = x * width + y;
					int size = cells[l].size();
					for (int i = 0; i < size; i++) {
						string c = cells[l][i];
						if (c[0] == 'x') {
							is_wall[x][y] = true;
						} else if (c[0] == '.') {
							//do nothing, is_wall[x][y] == 0 by default
						} else if (c[0] == 'P') {
							// player id
							int id = c[1] - '0';
							if (id == me.id) {
								me.x = x;
								me.y = y;
							}
							else {
								enemy.x = x;
								enemy.y = y;
							}
						} else if (c[0] == 'S') {
							if (c != "S") {
								spawn_points.push_back(make_pair(pt, stoi(c.erase(0, 1))));
							} else {
								spawn_points.push_back(make_pair(pt, -1));
							}
						} else if (c[0] == 'G') {
							int d;
							switch (c[1]) {
							case 'u': d = 0; break;
							case 'd': d = 1; break;
							case 'l': d = 2; break;
							case 'r': d = 3; break;
							};
							gates.push_back(make_pair(pt, d));
						} else if (c[0] == 'E') {
							bugs.push_back(make_pair(pt, c[1] - '0'));
						} else if (c[0] == 'B') {
							if (c != "B") {
								weapons.push_back(make_pair(pt, stoi(c.erase(0, 1))));
							} else {
								weapons.push_back(make_pair(pt, -1));
							}
						} else if (c[0] == 'C') {
							snippets.push_back(pt);
						}
					}
				}
			}
		} else if (type == "snippets") {
			if (player_name == me.name) {
				cin >> me.snippets;
			} else {
				cin >> enemy.snippets;
			}
		} else if (type == "bombs") {
			if (player_name == me.name) {
				cin >> me.bombs;
			} else {
				cin >> enemy.bombs;
			}
		}
	} else if (command == "action") {
		string action;
		cin >> action;
		if (action == "character") {
			cin >> time_remaining;
			choose_character();
		} else if (action == "move") {
			cin >> time_remaining;
			do_move();
		}
	}
}

int main() {
	while (true) {
		process_next_command();
	}
	return 0;
}

//-----------------------------------------//
//  Improve the code below to win 'em all  //
//-----------------------------------------//

int dx[4] = { -1,0,1,0 };
int dy[4] = { 0,-1,0,1 };
string moves[4] = { "up", "left", "down", "right" };

void choose_character() {
	cout << "bixie" << endl;
	// cout << "bixiette" << endl;
}

void do_move() {
	srand (time(NULL));
	vector<string> valid_moves;
	valid_moves.clear();
	for (int dir = 0; dir < 4; dir++) {
		int nextx = me.x + dx[dir];
		int nexty = me.y + dy[dir];
		if (nextx >= 0 && nextx < height && nexty >= 0 && nexty < width) {
			if (!is_wall[nextx][nexty]) {
				valid_moves.push_back(moves[dir]);
			}
		}
	}
	if (valid_moves.size() == 0) {
		valid_moves.push_back("pass");
	}
	cout << valid_moves[rand() % valid_moves.size()] << endl;
}
