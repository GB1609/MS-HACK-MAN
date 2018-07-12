#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <queue>
#include <sstream>
#include <iterator>
#include <cmath>
using namespace std;
int width = -1;
int height = -1;
int numNodes = -1;

////////// let a pair of coordinates returns the corresponding node
int pairToNode(const int & r, const int & c) {
	return r * width + c;
}

float euclidianDistanceNode(const int & node1, const int & node2);
float euclidianDistanceCells(const int & x1, const int & y1, const int & x2, const int & y2);
int getClosestPlayer(const int & posBegin);
int getClosestBug(const int & posBegin);
int getFartherPlayer(const int & posBegin);
int getObjectiveRedBugsTL(const int & posBegin);
int getObjectiveGreenBugsTR(const int & posBegin);
int getObjectiveYellowBugsBR(const int& posBegin);
int getObjectiveBlueBugsBL(const int& posBegin);

class Point {
public:
	int x;
	int y;
	int node;
	Point() // @suppress("Class members should be properly initialized")
	{
	}
	Point(int x, int y) :
			x(x), y(y), node(pairToNode(y, x)) {
	}
	Point(int node) :
			x(node - ((node / width) * width)), y(node / width), node(node) {
	}
	Point(const Point & p) :
			x(p.x), y(p.y), node(p.node) {
	}
	Point(const Point && p) :
			x(p.x), y(p.y), node(p.node) {
	}
	Point(Point&& p) :
			x(std::move(p.x)), y(std::move(p.y)), node(std::move(p.node)) {
	}
	bool operator<(const Point &a) const {
		return x < a.x || (x == a.y && y < a.y);
	}
	bool operator==(const Point &a) const {
		return x == a.x && y == a.y;
	}
	Point& operator=(const Point &a) {
		this->x = a.x;
		this->y = a.y;
		this->node = a.node;
		return *this;
	}
};
///////// let a node returns its coordinates
Point nodeToPair(const int & node) {
	int r = node / width;
	int c = node - (r * width);
	return std::move(Point(c, r));
}
class Player: public Point {
public:
	string name;
	string direction;
	int id;
	int snippets;
	int bombs;
	void setPosition(int r, int c) {
		x = c;
		y = r;
		node = pairToNode(r, c);
	}
	void setPosition(int node) {
		Point tmp(nodeToPair(node));
		setPosition(tmp);
	}
	void setPosition(Point & p) {
		x = p.x;
		y = p.y;
		node = p.node;
	}
};
Player darkMind;
Player enemy;
class SimpleObject: public Point {
public:
	SimpleObject(int x, int y) :
			Point(x, y) {
	}

	SimpleObject(int node) :
			Point(node) {
	}
	bool operator<(const SimpleObject &so) const { //used to sort the snippets vector from the closest to the farthest
		return euclidianDistanceCells(darkMind.x, darkMind.y, this->x, this->y) < euclidianDistanceCells(darkMind.x, darkMind.y, so.x, so.y);
	}
};
class ObjectWithRounds: public Point {
public:
	int rounds;
	ObjectWithRounds(int x, int y, int rounds) :
			Point(x, y), rounds(rounds) {
	}
	ObjectWithRounds(int node, int rounds) :
			Point(node), rounds(rounds) {
	}
	ObjectWithRounds(int node) :
			Point(node), rounds(-1) {
	}
};
class Bug: public Point {
public:
	int type;
	string direction;
	Bug(int x, int y, int type) :
			Point(x, y), type(type) {
	}
	Bug(int node, int type) :
			Point(node), type(type) {
	}
	Bug(int node) :
			Point(node), type(-1) {

	}
};
class Cell: public Point {
public:
	float weight = 0.0;
	Cell(int x, int y, float weight) :
			Point(x, y), weight(weight) {
	}
	Cell(int node, float weight) :
			Point(node), weight(weight) {
	}
	Cell(int node) :
			Point(node), weight(0) {

	}
	Cell() :
			Point(), weight(0) {
	}
};
//class Cell: public Point{
template<char delimiter>
class StringDelimitedBy: public string {

	friend std::istream& operator>>(std::istream& is, StringDelimitedBy& output) {
		std::getline(is, output, delimiter);
		return is;
	}
};
//vector<vector<bool> > is_wall;
int timebank;
int time_per_move;
int time_remaining;
int max_rounds;
int current_round;
vector<SimpleObject> snippets;
vector<ObjectWithRounds> weapons;
vector<Bug> bugs;
vector<ObjectWithRounds> spawn_points;
vector<ObjectWithRounds> gates;
//vector<ObjectWithRounds> cells;

void choose_character();
void do_move();

bool** matrixAdiacents;
Cell** matrixWeight;
bool isSetWalls = false;
int dx[4] = { 0, -1, 0, 1 };
int dy[4] = { -1, 0, 1, 0 };
string moves[4] = { "up", "left", "down", "right" };
void initAdiacents() {
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {

			int currentNode = pairToNode(r, c);
			int adiacentNode;
			for (int i = 0; i < 4; i++) {
				int adiacentR = r + dy[i];
				int adiacentC = c + dx[i];
				if (adiacentR < height && adiacentR >= 0 && adiacentC < width && adiacentC >= 0) {
					adiacentNode = pairToNode(adiacentR, adiacentC);
					matrixAdiacents[currentNode][adiacentNode] = 1;
					matrixAdiacents[adiacentNode][currentNode] = 1;
				}
			}
			Cell & cell = matrixWeight[r][c];
			cell.x = c;
			cell.y = r;
			c = currentNode;
		}
	}
}
void removeAdiacents(int cell) {
	Point cellCoordinate(cell);
	for (int i = 0; i < 4; i++) {
		int adiacentR = cellCoordinate.y + dy[i];
		int adiacentC = cellCoordinate.x + dx[i];

		if (adiacentR < height && adiacentR >= 0 && adiacentC < width && adiacentC >= 0) {
			int adiacentNode = pairToNode(adiacentR, adiacentC);
			matrixAdiacents[cell][adiacentNode] = 0;
			matrixAdiacents[adiacentNode][cell] = 0;
		}
	}
}
void parseSingleCharacter(const char& c, const int & cell) {
	switch (c) {
	case 'S':
		spawn_points.push_back(ObjectWithRounds(cell));
		break;
	case 'C':
		snippets.push_back(SimpleObject(cell));
		break;
	case 'B':
		weapons.push_back(ObjectWithRounds(cell));
		break;
	default:
		break;
	}
}
void parseObjects(const string & objList, const int &cell) {
	Point currentCoordinates(nodeToPair(cell));
	if (objList.size() == 1) parseSingleCharacter(objList[0], cell);
	else {
		std::istringstream issl(objList);
		std::vector<string> objs((std::istream_iterator<StringDelimitedBy<';'>>(issl)), std::istream_iterator<StringDelimitedBy<';'>>());
		for (unsigned int i = 0; i < objs.size(); i++) {
			if (objs[i].size() == 1) parseSingleCharacter(objs[i][0], cell);
			else {
				switch (objs[i][0]) {
				case 'P': {
					int id = objs[i][1] - '0';
					if (id == darkMind.id) {
						darkMind.setPosition(currentCoordinates);

					} else {
						enemy.setPosition(currentCoordinates);
					}
				}
					break;
				case 'S': {
					spawn_points.push_back(ObjectWithRounds(cell, stoi(objs[i].erase(0, 1))));
				}
					break;
				case 'G':
					int d;
					switch (objs[i][1]) {
					case 'u':
						d = 0;
						break;
					case 'd':
						d = 1;
						break;
					case 'l':
						d = 2;
						break;
					case 'r':
						d = 3;
						break;
					}
					gates.push_back(ObjectWithRounds(cell, d));
					break;
				case 'E':
					bugs.push_back(Bug(cell, objs[i][1] - '0'));
					break;
				case 'B':
					weapons.push_back(ObjectWithRounds(cell, stoi(objs[i].erase(0, 1))));
					break;
				default:
					break;
				}
			}
		}
	}
}
void addAdiacentsGates() {
	for (unsigned int i = 0; i < gates.size(); i++)
		for (unsigned int j = 0; j < gates.size(); j++)
			matrixAdiacents[gates[i].node][gates[j].node] = 1;

}
void weighs_cells() {
	cerr << "quiquiquiquiquiquiquiqiuqiu" << endl;
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {

			matrixWeight[r][c].weight = 0;
			for (unsigned int bug = 0; bug < bugs.size(); bug++) {
				int bugCol = bugs[bug].x;
				int bugRow = bugs[bug].y;
				if (r < bugRow) {
					float rPeso = 1 - ((bugRow - (1 + r)) / 10);

					if (c <= bugCol) {
					float cPeso = rPeso - bugCol - c;
						matrixWeight[r][c].weight += cPeso > 0 ? cPeso : 0;
					} else {
						float cPeso = rPeso + ((bugCol - c) / 10);
						matrixWeight[r][c].weight += cPeso < 1 ? cPeso : 1;

					}
				}
				if (r == bugRow) {
					if (c < bugCol) {
						float p = 1 - ((bugCol - c - 1) / 10);
						matrixWeight[r][c].weight += p > 0 ? p : 0;
					} else {
						if (c == bugCol) {
						matrixWeight[r][c].weight = 1.5;
					}
					else {
							float p = 1 - ((c - bugCol - 1) / 10);
							matrixWeight[r][c].weight += p > 0 ? p : 0;
						}
					}
			}
				if (r > bugRow) {
					float rPeso = 1 - ((bugRow - r + 1) / 10);
					if (c <= bugCol) {
						float cPeso = 1 - ((bugCol - c) / 10);
						matrixWeight[r][c].weight += cPeso > 0 ? cPeso : 0;
					} else {
						float cPeso = rPeso - ((c - bugCol) / 10);
						matrixWeight[r][c].weight += cPeso > 0 ? cPeso : 0;
					}

				}
			}
		}
	}
	for (int r = 0; r < height; r++)
		{
		for (int c = 0; c < width; c++)
			cerr << matrixWeight[r][c].weight << "-";
		cerr << endl;
	}
}
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
			cin >> darkMind.name;
		} else if (type == "your_botid") {
			cin >> darkMind.id;
		} else if (type == "field_width") {
			cin >> width;
		} else if (type == "field_height") {
			cin >> height;
		} else if (type == "max_rounds") {
			cin >> max_rounds;
		}
		if (width != -1 && height != -1) {
			//int tmp=width;
			//width=height;
			//height=tmp;
			numNodes = width * height;
			matrixAdiacents = new bool*[numNodes];
			for (int r = 0; r < numNodes; r++) {
				matrixAdiacents[r] = new bool[numNodes] { 0 };
			}
			matrixWeight = new Cell*[height];
				for (int i = 0; i < height; i++)
					matrixWeight[i] = new Cell[width];
			initAdiacents();
		}

	}

	else if (command == "update") {
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
			string s;
			cin >> s;

			std::istringstream iss(s);
			std::vector<std::string> fields((std::istream_iterator<StringDelimitedBy<','>>(iss)), std::istream_iterator<StringDelimitedBy<','>>());

			for (unsigned int ce = 0; ce < fields.size(); ce++) {
				if (fields[ce][0] == 'x' && !isSetWalls) removeAdiacents(ce);
				else if (fields[ce][0] == 'x') continue;
				parseObjects(fields[ce], ce);
			}
			if (!isSetWalls) //todo delete if the gates modify their direction
				addAdiacentsGates();
			isSetWalls = true;
			sort(snippets.begin(), snippets.end());
			weighs_cells();

		} else if (type == "snippets") {
			if (player_name == darkMind.name) {
				cin >> darkMind.snippets;
			} else {
				cin >> enemy.snippets;
			}
		} else if (type == "bombs") {
			if (player_name == darkMind.name) {
				cin >> darkMind.bombs;
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
	for (int i = 0; i < numNodes; i++) {
		delete[] matrixAdiacents[i];
	}
		delete[] matrixAdiacents;
		for (int i = 0; i < height; i++) {
			delete[] matrixWeight[i];
		}
		delete[] matrixWeight;
	return 0;
}

//-----------------------------------------//
//  Improve the code below to win 'em all  //
//-----------------------------------------//

void choose_character() {
	cout << "bixiette" << endl;
}

void do_move() {
	srand(time(NULL));
	vector<string> valid_moves;
	valid_moves.clear();
	for (int dir = 0; dir < 4; dir++) {
		int nextx = darkMind.x + dx[dir];
		int nexty = darkMind.y + dy[dir];
		if (nextx >= 0 && nextx < width && nexty >= 0 && nexty < height) {
			if (matrixAdiacents[pairToNode(darkMind.y, darkMind.x)][pairToNode(nexty, nextx)] == 1) {
				valid_moves.push_back(moves[dir]);
			}
		}
	}
	if (valid_moves.size() == 0) {
		valid_moves.push_back("pass");
	}
	cout << valid_moves[rand() % valid_moves.size()] << endl;
}

float euclidianDistanceNode(const int & node1, const int & node2) {
	Point p1 = nodeToPair(node1);
	Point p2 = nodeToPair(node2);
	int sum1 = p1.x - p2.x;
	int sum2 = p1.y - p2.y;
	return sqrt(pow(sum1, 2) + pow(sum2, 2));
}
float euclidianDistanceCells(const int & x1, const int & y1, const int & x2, const int & y2) {
	int sum1 = x1 - x2;
	int sum2 = y1 - y2;
	return sqrt(pow(sum1, 2) + pow(sum2, 2));
}
int getClosestPlayer(const int & posBegin) {
	int closest = -1;
	float min = width * height; //nRighe*colonne, la distanza non potra mai essere maggiore
	for (int i = 0; i < 2; i++) {
		int posP = (i == darkMind.id) ? darkMind.node : enemy.node;
		float dist = euclidianDistanceNode(posBegin, posP);
		if (min > dist) {
			min = dist;
			closest = i;
		}
	}
	return closest;
}
int getClosestBug(const int & posBegin) {
	int closest = -1;
	float min = width * height; //nRighe*colonne, la distanza non potra mai essere maggiore
	for (unsigned int i = 0; i < bugs.size(); i++) {
		int posBugs = bugs[i].node;
		float dist = euclidianDistanceNode(posBegin, posBugs);
		if (min > dist) {
			min = dist;
			closest = i;
		}
	}
	return closest;
}
int getFartherPlayer(const int & posBegin) {
	int farther = -1;
	float max = -1;
	for (int i = 0; i < 2; i++) {
		int posP = (i == darkMind.id) ? darkMind.node : enemy.node;
		float dist = euclidianDistanceNode(posBegin, posP);
		if (max < dist) {
			max = dist;
			farther = i;
		}
	}
	return farther;
}
int getObjectiveRedBugsTL(const int & posBegin) {
	int closestPlayer = getClosestPlayer(posBegin);
	if (closestPlayer == darkMind.id) return darkMind.node;
	else return enemy.node;
}
int getObjectiveGreenBugsTR(const int & posBegin) {
	int closestPlayer = getClosestPlayer(posBegin);
	Player p = (closestPlayer == darkMind.id) ? darkMind : enemy;
	if (p.direction == "up") {
		int xNew = p.y - 4;
		if (xNew < 0) xNew = 0;
		return pairToNode(xNew, p.x);
	} else if (p.direction == "down") {
		int xNew = p.y + 4;
		if (xNew > height - 1) xNew = height - 1;
		return pairToNode(xNew, p.x);
	} else if (p.direction == "left") {
		int yNew = p.x - 4;
		if (yNew < 0) yNew = 0;
		return pairToNode(p.y, yNew);
	} else if (p.direction == "right") {
		int yNew = p.x + 4;
		if (yNew > width - 1) yNew = width - 1;
		return pairToNode(p.y, yNew);
	}
	return 0;
}
int getObjectiveYellowBugsBR(const int& posBegin) {
//	int closestPlayer=getClosestPlayer(nrPlayer,posBegin);
//	int closestBugToClPL=getClosestBug(nrBugs,closestPlayer);
//	int xVett=abs(players[closestPlayer].x-bugs[closestBugToClPL].x);
//	int yVett=abs(players[closestPlayer].y-bugs[closestBugToClPL].y);
//	return pairToNode(yVett*2,xVett*2);
	return 3;
}
int getObjectiveBlueBugsBL(const int& posBegin) {
	int fartherPlayer = getFartherPlayer(posBegin);
	if (fartherPlayer == darkMind.id) return darkMind.node;
	else return enemy.node;
	return 4;
}

