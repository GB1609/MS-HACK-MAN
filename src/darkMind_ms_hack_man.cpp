#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <queue>
#include <sstream>
#include <iterator>
#include <cmath>
#include <list>
#include <climits>
#include <map>
using namespace std;
int width = -1;
int height = -1;
int numNodes = -1;
int centerNode = -1;
static int turn = 0;
static int around = 3;
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
string getMoveGB(int begin, int move) {
	if (begin - move == 1 || begin + (width - 1) == move) return "left";
	else if (begin + 1 == move || begin - move == width - 1) return "right";
	else if (begin + width == move) return "down";
	else if (begin - width == move) return "up";
	return "pass";
}
bool weightChange = true;
unsigned int precBugNumber = 0;
float precWeight = 0.0f;
vector<int> pathFindGB(const int & posBegin, const int & posEnd);
int numbersBonusNear(const int& node, const int& circle);
class NodeGB {
public:
	int node;
	vector<int> discendenza;
	float cost;
	NodeGB(int n, int c, vector<int> f) {
		node = n;
		cost = c;
		discendenza.resize(f.size());
		for (unsigned int i = 0; i < f.size(); i++)
			discendenza[i] = f[i];
	}
	NodeGB& operator=(const NodeGB &a) {
		cost = a.cost;
		node = a.node;
		discendenza.resize(a.discendenza.size());
		for (unsigned int b = 0; b < a.discendenza.size(); b++)
			discendenza[b] = a.discendenza[b];
		return *this;
	}

	float getCost() const {
		return cost;
	}

	const vector<int>& getDiscendenza() const {
		return discendenza;
	}

	int getNode() const {
		return node;
	}
};
class Point {
public:
	int x = -1;
	int y = -1;
	int node;
	Point() // @suppress("Class members should be properly initialized")
	{
	}
	Point(int x, int y) :
			x(x), y(y), node(pairToNode(y, x)) {
	}
	Point(int x, int y, int l, int p) :
			x(x), y(y), node(pairToNode(y, x)) {
	}
	Point(int node) :
			x(node - ((node / width) * width)), y(node / width), node(node) {
	}

	Point(const Point& p) :
			x(p.x), y(p.y), node(p.node) {
	}
	;
	Point(Point&& p) :
			x(std::move(p.x)), y(std::move(p.y)), node(std::move(p.node)) {
	}
	;
	bool operator<(const Point &a) const {
		return x < a.x || (x == a.y && y < a.y);
	}
	bool operator==(const Point &a) const {
		return x == a.x && y == a.y;
	}
	Point& operator=(const Point &a) {
		x = a.x;
		y = a.y;
		node = a.node;
		return *this;
	}
};
vector<int> getAdiacentsNode(const int& node);
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
		if (c > x) direction = "right";
		else if (c < x) direction = "left";
		else if (r < y) direction = "up";
		else if (r > y) direction = "down";
		else direction = "pass";
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
class Cell;
bool** matrixAdiacents;
Cell** matrixWeight;
bool inMatrix(int coordY, int coorX) {
	return coordY >= 0 && coordY < height && coorX >= 0 && coorX < width;
}
int dx[4] = { 0, -1, 0, 1 };
int dy[4] = { -1, 0, 1, 0 };
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
	bool operator<(const Cell &so) const {

		float sum1 = matrixWeight[this->y][this->x].weight;
		int node1 = matrixWeight[this->y][this->x].node;
		float sum2 = matrixWeight[so.y][so.x].weight;
		if (sum1 == sum2) {
			int nAdiacents2 = 0, nAdiacents1 = 0;
			for (int i = 0; i < 4; i++) {
				int myAdiacenty = this->y + dy[i], myAdiacentx = this->x + dx[i], soAdiaceny = so.y + dy[i], soAdiacentx = so.x + dx[i];
				if (inMatrix(myAdiacenty, myAdiacentx)) sum1 += matrixWeight[myAdiacenty][myAdiacentx].weight;
				if (inMatrix(soAdiaceny, soAdiacentx)) sum2 += matrixWeight[soAdiaceny][soAdiacentx].weight;
				int nodeMyAd = pairToNode(myAdiacenty, myAdiacentx);
				int nodeSoAd = pairToNode(soAdiaceny, soAdiacentx);
				if (matrixAdiacents[node1][nodeMyAd]) nAdiacents1++;
				if (matrixAdiacents[node1][nodeSoAd]) nAdiacents2++;
			}
			return sum1 / nAdiacents1 <= sum2 / nAdiacents2;
		}

		return sum1 < sum2;
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

		float sum1 = matrixWeight[this->y][this->x].weight;
		float sum2 = matrixWeight[so.y][so.x].weight;
		for (int i = 0; i < 4; i++) {
			int myAdiacenty = this->y + dy[i], myAdicantx = this->x + dx[i], soAdiaceny = so.y + dy[i], soAdiacentx = so.x + dx[i];
			if (inMatrix(myAdiacenty, myAdicantx)) sum1 += matrixWeight[myAdiacenty][myAdicantx].weight;
			if (inMatrix(soAdiaceny, soAdiacentx)) sum2 += matrixWeight[soAdiaceny][soAdiacentx].weight;
		}
		float dist1 = euclidianDistanceCells(darkMind.x, darkMind.y, this->x, this->y);
		float dist2 = euclidianDistanceCells(darkMind.x, darkMind.y, so.x, so.y);
		if (dist1 <= dist2) {
			return sum1 <= sum2;
		}
		return sum2 < sum1;
	}
};
vector<SimpleObject> precSnippetNumber;
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
vector<ObjectWithRounds> precWeaponNumber;
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
template<char delimiter>
class StringDelimitedBy: public string {

	friend std::istream& operator>>(std::istream& is, StringDelimitedBy& output) {
		std::getline(is, output, delimiter);
		return is;
	}
};
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
bool checkChange() {
	if (precSnippetNumber.size() != snippets.size()) return true;
	for (unsigned int i = 0; i < snippets.size(); i++) {
		int count = 0;
		for (unsigned int j = 0; j < precSnippetNumber.size(); j++) {
			if (snippets[i].node == precSnippetNumber[j].node) {
				count++;
			}
		}
		if (count == 0) return true;
	}

	if (precWeaponNumber.size() != weapons.size()) return true;
	for (unsigned int i = 0; i < weapons.size(); i++) {
		int count = 0;
		for (unsigned int j = 0; j < precWeaponNumber.size(); j++) {
			if (weapons[i].node == precWeaponNumber[j].node) {
				count++;
			}
		}
		if (count == 0) return true;
	}
	return false;
}
void choose_character();
void do_move();
bool existWall(const int & r, const int & c, const int & bugR, const int & bugC) {
	int a = bugR + 1;
	if (a < height) {
		while (a < r) {
			if (!matrixAdiacents[pairToNode(a, bugC)][pairToNode(a - 1, bugC)]) return true;
			a++;
		}
	}
	a = bugR - 1;
	if (a > 0) {
		while (a > r) {
			if (!matrixAdiacents[pairToNode(a, bugC)][pairToNode(a + 1, bugC)]) return true;
			a--;
		}
	}
	a = bugC + 1;
	if (a < width) {
		while (a < c) {
			if (!matrixAdiacents[pairToNode(bugR, a)][pairToNode(bugR, a - 1)]) return true;
			a++;
		}
	}
	a = bugC - 1;
	if (a > 0) {
		while (a > c) {
			if (!matrixAdiacents[pairToNode(bugR, a)][pairToNode(bugR, a + 1)]) return true;
			a--;
		}
	}
	return false;
}
bool isSetWalls = false;
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
			cell.node = currentNode;
		}
	}
	for (int i = 0; i < numNodes; i++)
		matrixAdiacents[i][i] = 0;
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
//					if (spawn_points.back() == 1) bugs.push_back(spawn_points.back().node);
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
void weigths_cells() {
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			matrixWeight[r][c].weight = 0.0f;
			for (unsigned int bug = 0; bug < bugs.size(); bug++) {
				int bugCol = bugs[bug].x;
				int bugRow = bugs[bug].y;

				float cPeso = 0.0;
				if (r < bugRow) {
					float rPeso = 1.0 - ((bugRow - r - 1.0) / 10.0);
					if (c <= bugCol) {
						cPeso = rPeso - ((bugCol - c) / 10.0);
					} else {
						cPeso = rPeso - ((c - bugCol) / 10.0);

					}
				}
				if (r == bugRow) {
					if (c < bugCol) {
						cPeso = 1.0 - ((bugCol - c - 1.0) / 10.0);
					} else {
						if (c == bugCol) {
							if (matrixWeight[r][c].weight > 0.0) matrixWeight[r][c].weight = 1.5 + 0.1;
							else matrixWeight[r][c].weight = 1.5;
							continue;
						} else {
							cPeso = 1.0 - ((c - bugCol - 1.0) / 10.0);
						}
					}
				}

				if (r > bugRow) {
					float rPeso = 1.0 - ((r - bugRow - 1.0) / 10.0);
					if (c <= bugCol) {
						cPeso = rPeso - ((bugCol - c) / 10.0);
					} else {
						cPeso = rPeso - ((c - bugCol) / 10.0);

					}
				}

				//if (r > bugRow &&inMatrix(r-1,c)&& !matrixAdiacents[pairToNode(r - 1, c)][pairToNode(r, c)]) cPeso -= 0.1;
				//if (r < bugRow &&inMatrix(r+1,c)&& !matrixAdiacents[pairToNode(r + 1, c)][pairToNode(r, c)]) cPeso -= 0.1;
				//if (c > bugRow &&inMatrix(r,c-1)&& !matrixAdiacents[pairToNode(r, c - 1)][pairToNode(r, c)]) cPeso -= 0.1;
				//if (c > bugRow && inMatrix(r,c+1)&&!matrixAdiacents[pairToNode(r, c - 1)][pairToNode(r, c)]) cPeso -= 0.1;
				if (cPeso < 0.6) cPeso = 0.0f;
				if (matrixWeight[r][c].weight > cPeso) matrixWeight[r][c].weight += 0.1;
				else if (matrixWeight[r][c].weight > 0.0) matrixWeight[r][c].weight = cPeso > 0.0 ? cPeso + 0.1 : 0.0;
				else matrixWeight[r][c].weight = cPeso > 0.0 ? cPeso : 0.0;
			}
		}
	}
//  if(bugs.size()!=0){
//      for(int i=0;i<bugs.size();i++)
//      cerr<<bugs[i].y<<"---"<<bugs[i].x<<endl;
//  cerr<<"llllllllllllllllllllllllllllll"<<endl;
//  for (int r = 0; r < height; r++)
//  {
//    for (int c = 0; c < width; c++)
//      cerr << matrixWeight[r][c].weight << "-";
//    cerr << endl;
//    }}
}
bool inAdiacentDiAdiacent(int node) {
	vector<int> ad = getAdiacentsNode(node);
	for (unsigned int i = 0; i < ad.size(); i++) {
		for (unsigned int bug = 0; bug < bugs.size(); bug++) {
			if (bugs[bug].node == ad[i]) {
				vector<int> ad2 = getAdiacentsNode(bugs[bug].node);
				ad2.push_back(ad[i]);
				for (unsigned int j = 0; j < ad2.size(); j++) {
					for (unsigned int bug2 = 0; bug2 < bugs.size(); bug2++) {
						if (bug != bug2 && bugs[bug2].node == ad2[j]) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
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
			centerNode = pairToNode(height / 2, width / 2);
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
			if (!isSetWalls) addAdiacentsGates();
			isSetWalls = true;
			sort(snippets.begin(), snippets.end());
			weigths_cells();
			if (bugs.size() != precBugNumber || checkChange() || current_round < 2
					|| (inAdiacentDiAdiacent(darkMind.node) ?
							(matrixWeight[darkMind.y][darkMind.x].weight >= 0.9 && precWeight < 0.9)
									|| (matrixWeight[darkMind.y][darkMind.x].weight >= 0.9 && matrixWeight[darkMind.y][darkMind.x].weight > precWeight) :
							(matrixWeight[darkMind.y][darkMind.x].weight >= 0.8 && precWeight < 0.8)
									|| (matrixWeight[darkMind.y][darkMind.x].weight >= 0.8 && matrixWeight[darkMind.y][darkMind.x].weight > precWeight))) {
				precBugNumber = bugs.size();
				weightChange = true;
			} else weightChange = false;
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
void choose_character() {
	cout << "bixiette" << endl;
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
string getSafeDir() {

	vector<int> nodes = getAdiacentsNode(darkMind.node);
	vector<Cell> adiacents;
	for (unsigned int i = 0; i < nodes.size(); i++)
		adiacents.push_back(matrixWeight[nodeToPair(nodes[i]).y][nodeToPair(nodes[i]).x]);
	sort(adiacents.begin(), adiacents.end());
	int toR = 0;
	if (adiacents[0].weight == adiacents[1].weight || abs(adiacents[0].weight - adiacents[1].weight) == 0.1) {
		float min = INT_MAX;
		for (unsigned int a = 0; a < gates.size(); a++) {
			float e0 = euclidianDistanceNode(adiacents[0].node, gates[a].node);
			float e1 = euclidianDistanceNode(adiacents[1].node, gates[a].node);
			if (e0 < e1 && e0 < min) toR = 0;
			if (e1 < e0 && e1 < min) toR = 1;
		}
	}
	Cell & cSafe = adiacents[toR];
//	cerr << cSafe.x << "--" << cSafe.y << endl;
	return getMoveGB(darkMind.node, cSafe.node);
}
Point dirToCell(string s) {
	if (s == "left") return std::move(Point(darkMind.x - 1, darkMind.y));
	if (s == "up") return std::move(Point(darkMind.x, darkMind.y - 1));
	if (s == "right") return std::move(Point(darkMind.x + 1, darkMind.y));
	if (s == "down") return std::move(Point(darkMind.x, darkMind.y + 1));
	return std::move(Point(darkMind.x, darkMind.y));
}
bool isOnRight(const int &node1, const int& node2) {
	return node1 == node2 - 1 || node1 == node2 - (width - 1);
}
bool isOnLeft(const int &node1, const int& node2) {
	return node1 == node2 + 1 || node1 == node2 + (width - 1);
}
bool isOnUp(const int &node1, const int& node2) {
	return node1 == node2 + width;
}
bool isOnDown(const int &node1, const int& node2) {
	return node1 == node2 - width;
}
int weightToLeft(const int &node1) {
	int x = nodeToPair(node1).x;
	int y = nodeToPair(node1).y;
	(x - 1 < 0) ? x = width - 1 : x - 1;
	return matrixWeight[y][x].weight;
}
bool weightToRight(const int &node1, const int& node2) {
	int x = nodeToPair(node1).x;
	int y = nodeToPair(node1).y;
	(x + 1 == width) ? x = 0 : x + 1;
	return matrixWeight[y][x].weight;
}
int weightToUp(const int &node1) {
	int x = nodeToPair(node1).x;
	int y = nodeToPair(node1).y - 1;
	return matrixWeight[y][x].weight;
}
int weightToDown(const int &node1) {
	int x = nodeToPair(node1).x;
	int y = nodeToPair(node1).y + 1;
	return matrixWeight[y][x].weight;
}
float getWeight(const int& node) {
	return matrixWeight[nodeToPair(node).y][nodeToPair(node).x].weight;
}
bool sureToGo(const int& toGo, const int& dm) {
	float weightToGo = getWeight(toGo);
	float weightDM = getWeight(dm);
	if (bugs.empty()) return true;
	if (weightToGo <= 0.70 || weightDM >= weightToGo) return true;
	return false;
}
map<ObjectWithRounds, vector<int> > checkDangerNode();
vector<int> getDangerNodes(ObjectWithRounds bomb);
vector<SimpleObject> toScan;
void do_move() {
	cerr << "start turn: " << turn << endl;
	cerr << "my pos:" << darkMind.node << "     enemy pos:" << enemy.node << endl;
	int toPrint = -1, toPrintEN = -1, toGO, aroundV;
	unsigned int min = INT_MAX, minEN = INT_MAX, pathChoicher, sizeEN;
	float eN, eDM;
//	if ((inAdiacentDiAdiacent(darkMind.node)) ? getWeight(darkMind.node) >= 0.0 : getWeight(darkMind.node) >= 0.8) cout << getSafeDir() << endl;
	if (getWeight(darkMind.node) >= 0.8) cout << getSafeDir() << endl;
	else {
		if (weightChange) {
			toScan = vector<SimpleObject>(snippets);
			for (unsigned int a = 0; a < weapons.size(); a++) {
				if (weapons[a].rounds == -1) toScan.push_back(SimpleObject(weapons[a].node));
			}
			sort(toScan.begin(), toScan.end());
		}
		for (unsigned int i = 0; i < toScan.size(); i++) {
			vector<int> temp = pathFindGB(darkMind.node, toScan[i].node);
			cerr << "path to " << toScan[i].node << ": ";
			for (unsigned int a = 0; a < temp.size(); a++)
				cerr << temp[a] << ",";
			cerr << "with length " << temp.size();
			vector<int> tempEn = pathFindGB(enemy.node, toScan[i].node);
			if (tempEn.size() < minEN || minEN == INT_MAX) {
				minEN = tempEn.size();
				toPrintEN = temp[tempEn.size() - 1];
			}
			toGO = temp[temp.size() - 1];
			aroundV = numbersBonusNear(toGO, around) - 1;
			pathChoicher = temp.size() - aroundV;
			cerr << " updated to " << temp.size() << endl;
			sizeEN = tempEn.size();
			eN = euclidianDistanceNode(toPrintEN, toScan[i].node);
			eDM = euclidianDistanceNode(toGO, toScan[i].node);
			if (temp.size() > 0 && (sizeEN >= temp.size() || eN > eDM) && pathChoicher < min) {
				toPrint = toGO;
				min = pathChoicher;
			}
		}
		cerr << "CHOICHE: ";
		if (toPrint == -1) {
			vector<int> toCe = pathFindGB(darkMind.node, centerNode);
			(toCe.size() > 0) ? cout << getMoveGB(darkMind.node, toCe[toCe.size() - 1]) << endl : cout << getSafeDir() << endl;
			cerr << "GO TO CENTER or PASS" << endl;
		} else {
			cout << getMoveGB(darkMind.node, toPrint) << endl;
			cerr << "GO TO " << toPrint << endl;
		}
	}
	cerr << "END TURN: " << turn << endl;
	turn++;
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
	int playerX = 0;
	int playerY = 0;
	int closestPlayer = getClosestPlayer(posBegin);
	int closestPlayerNode = darkMind.node;
	playerX = darkMind.x;
	playerY = darkMind.y;
	if (closestPlayer != darkMind.id) {
		closestPlayerNode = enemy.node;
		playerX = enemy.x;
		playerY = enemy.y;
	}
	int closestBug = getClosestBug(closestPlayerNode);
	int bugX = bugs[closestBug].x;
	int bugY = bugs[closestBug].y;
	int x = -bugX + playerX;
	int y = -bugY + playerY;
	x = playerX + x;
	y = playerY + y;
	if (y > height - 1) y = height - 1;
	if (y < 0) y = 0;
	if (x > width - 1) x = width - 1;
	if (x < 0) x = 0;

	return pairToNode(y, x);
}
int getObjectiveBlueBugsBL(const int& posBegin) {
	int fartherPlayer = getFartherPlayer(posBegin);
	if (fartherPlayer == darkMind.id) return darkMind.node;
	else return enemy.node;
	return 4;
}
map<ObjectWithRounds, vector<int> > checkDangerNode() {
	map<ObjectWithRounds, vector<int> > dangers;
	for (unsigned int i = 0; i < weapons.size(); i++)
		if (weapons[i].rounds != -1) dangers[weapons[i]] = getDangerNodes(weapons[i]);
	return dangers;
}
vector<int> getDangerNodes(ObjectWithRounds bomb) {
	vector<int> nodes;
	int x = bomb.x;
	int y = bomb.y;
	int bombNode = bomb.node;
	nodes.push_back(bombNode);
	for (int i = y; i < height - 1; i++) {
		int currentNode = pairToNode(i, x);
		int nextNode = pairToNode(i + 1, x);
		if (matrixAdiacents[currentNode][nextNode] == 0) break;
		nodes.push_back(nextNode);
	}
	for (int i = y; i > 0; i--) {
		int currentNode = pairToNode(i, x);
		int nextNode = pairToNode(i - 1, x);
		if (matrixAdiacents[currentNode][nextNode] == 0) break;
		nodes.push_back(nextNode);
	}
	for (int j = x; j < width - 1; j++) {
		int currentNode = pairToNode(y, j);
		int nextNode = pairToNode(y, j + 1);
		if (matrixAdiacents[currentNode][nextNode] == 0) break;
		nodes.push_back(nextNode);
	}
	for (int j = x; j > 0; j--) {
		int currentNode = pairToNode(y, j);
		int nextNode = pairToNode(y, j - 1);
		if (matrixAdiacents[currentNode][nextNode] == 0) break;
		nodes.push_back(nextNode);
	}
	return nodes;
}
vector<int> getAdiacentsNode(const int& node) {
	vector<int> toReturn;
	if (node - width >= 0 && matrixAdiacents[node][node - width]) toReturn.push_back(node - width);
	if (node + width < numNodes && matrixAdiacents[node][node + width]) toReturn.push_back(node + width);
	if (node + 1 < numNodes && matrixAdiacents[node][node + 1]) toReturn.push_back(node + 1);
	if (node - 1 >= 0 && matrixAdiacents[node][node - 1]) toReturn.push_back(node - 1);
	if (node == gates[0].node) toReturn.push_back(gates[1].node);
	if (node == gates[1].node) toReturn.push_back(gates[0].node);
	return toReturn;
}
int numbersBonusNear(const int& node, const int& circle) {
	int toReturn = 0;
	for (int i = -circle; i < circle + 1; i++)
		for (int c = -circle; c < circle + 1; c++) {
			for (unsigned int a = 0; a < snippets.size(); a++) {
				int cell = (node + (width * c)) + i;
				if (cell == snippets[a].node) toReturn++;
			}
			for (unsigned int a = 0; a < weapons.size(); a++) {
				int cell = (node + (width * c)) + i;
				if (cell == weapons[a].node && weapons[a].rounds == -1) toReturn++;
			}
		}
	return toReturn;
}
vector<int> pathFindGB(const int & posBegin, const int & posEnd) {
	bool found = false;
	vector<int> disc;
	NodeGB bestNode(numNodes, 0, disc);
// Mark all the vertices as not visited
	bool visited[numNodes];
	for (int i = 0; i < numNodes; i++)
		visited[i] = false;
	queue<NodeGB> queue;
	visited[posBegin] = true;
	NodeGB start(posBegin, 0, disc);
	queue.push(start);
	while (!queue.empty()) {
		NodeGB s = queue.front();
		if (s.node == posEnd) {
			if (found && s.cost < bestNode.cost) {
				bestNode = s;
			} else if (!found) {
				found = true;
				bestNode = s;
			}
		}
		queue.pop();
		vector<int> ad = getAdiacentsNode(s.node);
		for (unsigned int i = 0; i < ad.size(); i++) {
			if (!visited[ad[i]]) {
				visited[ad[i]] = true;
				vector<int> dis(s.discendenza);
				dis.push_back(s.node);
				float cost = euclidianDistanceNode(ad[i], posEnd) + s.cost;
				int w = getWeight(ad[i]);
				if (w >= 0.75 && dis.size() < 5) cost *= (cost * w);
				NodeGB temp(ad[i], cost, dis);
				if (found && temp.cost < bestNode.cost) queue.push(temp);
				else if (!found) queue.push(temp);
			}
		}
	}
	vector<int> toReturn;
	if (bestNode.node == numNodes) return toReturn;
	else {
		toReturn.push_back(posEnd);
		for (int i = bestNode.discendenza.size() - 1; i > 0; i--)
			toReturn.push_back(bestNode.discendenza[i]);
		return toReturn;
	}
	return toReturn;
}

