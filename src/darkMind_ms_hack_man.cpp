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
int getObjectiveVioletBugsBL(const int& posBegin);
int getMaxIDBug();
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
int numbersBonusNear(const int& node, const int& circle);
class NodeGB {
public:
	int node;
	vector<int> discendenza;
	float cost;
	NodeGB() { // @suppress("Class members should be properly initialized")
	} //serve ad elena
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
};
NodeGB pathFindGB(const int & posBegin, const int & posEnd, bool isForBug);
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
vector<int> getAdiacentsNode(const int& node, const bool & isForBug);
vector<int> getAlternative(const int& node);
void cleanBugs();
///////// let a node returns its coordinates
Point nodeToPair(const int & node) {
	int r = node / width;
	int c = node - (r * width);
	return std::move(Point(c, r));
}
class Cell;
bool** matrixAdiacents;
Cell** matrixWeight;
bool ** matrixWall;
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
const int tl = 0;
const int tr = 18;
const int br = 284;
const int bl = 266;
template<char delimiter>
class StringDelimitedBy: public string {

	friend std::istream& operator>>(std::istream& is, StringDelimitedBy& output) {
		std::getline(is, output, delimiter);
		return is;
	}
};
class Player: public Point {
public:
	string name;
	string direction;
	int id;
	int snippets = 0;
	int bombs;
	void setPosition(int node) {
		Point tmp(nodeToPair(node));
		setPosition(tmp);
	}
	Point getDirectionVector() {
		int newX, newY;
		if (direction == "up") {
			newY = y - 1;
			newX = x;
		} else if (direction == "down") {
			newY = y + 1;
			newX = x;
		} else if (direction == "left") {
			newY = y;
			newX = x - 1;
		} else if (direction == "right") {
			newY = y;
			newX = x + 1;
		} else {
			newY = y;
			newX = x;
		}
		return std::move(Point(newX - x, newY - y));
	}
	void setPosition(Point & p) {
		x = p.x;
		y = p.y;
		direction = getMoveGB(node, p.node);
		node = p.node;
	}
};
Player darkMind;
Player enemy;
class Bug: public Point {
public:
	bool founded;
	int type;
	string direction;
	int objective;
	int id;
	int nextNode = -1;
	bool toUpdate = false;
	vector<int> alternative;
	Point directionVector;
	bool fakeBug = true;
	Bug() { // @suppress("Class members should be properly initialized")
	}
	Bug(int node) :
			Point(node) {
		switch (node) {
		case tl:
			type = 0;
			objective = tl;
			break;
		case tr:
			type = 1;
			objective = tr;
			break;
		case br:
			type = 2;
			objective = br;
			break;
		case bl:
			type = 3;
			objective = bl;
			break;
		default:
			type = -1;
			break;
		}
		id = getMaxIDBug() + 1;
		direction = "pass";
		nextNode = node;
		founded = true;
	}
	bool notBackPass(int node) {
		Point n(nodeToPair(node));
		Point dir(x - n.x, y - n.y);
		return ((directionVector.x + dir.x) == 0 && (directionVector.y + dir.y) == 0);
	}
	void setDirection(const string& direction) {
		this->direction = direction;
	}
	void setAlternative(const int & n) {
		alternative.clear();
		alternative = getAlternative(n);
	}
	void setPosition(int node) {
		if (fakeBug && node != this->node) fakeBug = false;
		this->node = node;
		Point tmp(std::move(Point(node)));
		directionVector = std::move(Point(this->x - tmp.x, this->y - tmp.y));
		this->x = tmp.x;
		this->y = tmp.y;
		setAlternative(node);
	}
	bool operator==(const Bug &a) {
		if (type != a.type) return false;
		if (nextNode == a.node) {
			setPosition(a.node);
			return true;
		}
		for (unsigned int v = 0; v < alternative.size(); v++)
			if (alternative[v] == a.node) {
				setPosition(a.node);
				toUpdate = true;
				return true;
			}
		return false;
	}
};
list<Bug> bugsList;
int timebank;
int time_per_move;
int time_remaining;
int max_rounds;
int current_round;
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
vector<SimpleObject> snippets;
vector<ObjectWithRounds> weapons;
vector<ObjectWithRounds> spawn_points;
vector<ObjectWithRounds> gates;
int getMaxIDBug() {
	int max = 0;
	::list<Bug>::iterator it = bugsList.begin();
	for (; it != bugsList.end(); it++)
		if (it->id > max) max = it->id;
	return max;
}
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
					if (spawn_points.back().rounds == 2) {
						cerr << "add bug" << endl;
						bugsList.push_back(std::move(Bug(cell)));
					}
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
				case 'E': {
					Bug b(cell);
					b.type = objs[i][1] - '0';
					for (std::list<Bug>::iterator bug = bugsList.begin(); bug != bugsList.end(); bug++)
						if (*bug == b) bug->founded = true;
				}
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
bool inMatrixNode(const int& node) {
	return (node >= 0 && node < numNodes);
}
float getWeight(const int& node) {
	if (node < 0 || node >= numNodes) return INT_MAX;
	return matrixWeight[nodeToPair(node).y][nodeToPair(node).x].weight;
}
map<int, NodeGB> pathsOfBugs;
int currentBug = 0;
vector<Point> objBugs;
bool newBugPatIsCreate = false;
Bug currentBugObj;
void weigths_cells() {
	cerr << "SITUATION AT TURN: " << turn << endl;
	std::list<Bug>::iterator i;
	int obTEMP = -1;
	NodeGB tempPATH;
	objBugs.clear();
	int cBug = 0;
	for (i = bugsList.begin(); i != bugsList.end(); cBug++, i++) {
		if (!i->fakeBug || i->node == i->objective || i->objective == -1 || i->toUpdate) {
			currentBugObj = *i;
			newBugPatIsCreate = true;
			i->toUpdate = false;
			switch (i->type) {
			case 0: //RED
				objBugs.push_back(getObjectiveRedBugsTL(i->node));
				currentBug = cBug;
				tempPATH = pathFindGB(i->node, objBugs[currentBug].node, true);
				break;
			case 1: //GREEN
				objBugs.push_back(getObjectiveGreenBugsTR(i->node));
				currentBug = cBug;
				tempPATH = pathFindGB(i->node, objBugs[currentBug].node, true);
				break;
			case 2: //YELLOW
				objBugs.push_back(getObjectiveYellowBugsBR(i->node));
				currentBug = cBug;
				tempPATH = pathFindGB(i->node, objBugs[currentBug].node, true);
				break;
			case 3: //VIOLET
				objBugs.push_back(getObjectiveYellowBugsBR(i->node));
				currentBug = cBug;
				tempPATH = pathFindGB(i->node, objBugs[currentBug].node, true);
				break;
			default:
				break;
			}
			vector<int>& pathTmp = tempPATH.discendenza;
			obTEMP = pathTmp[pathTmp.size() - 1];
			if (pathTmp.size() > 1) for (unsigned int a = 1; a < pathTmp.size(); a++) {
				if (getAdiacentsNode(pathTmp[a], true).size() == 3) {
					obTEMP = pathTmp[a];
					break;
				}
			}
			if (!i->fakeBug) {
				i->objective = obTEMP;
				int nextNode = tempPATH.discendenza[1];
				i->setAlternative(i->node);
				i->setDirection(getMoveGB(i->node, nextNode));
				i->nextNode = nextNode;
			} else {
				i->nextNode = i->node;
				i->objective = i->node;
				i->setDirection(getMoveGB(i->node, i->node));
			}
			std::map<int, NodeGB>::iterator it = pathsOfBugs.find(i->id);
			if (it == pathsOfBugs.end()) pathsOfBugs.insert(std::pair<int, NodeGB>(i->id, tempPATH));
			else it->second = tempPATH;
		} else {
			vector<int>& temp = pathsOfBugs.at(i->id).discendenza;
			int c = 0;
			for (unsigned int a = 0; a < temp.size(); a++)
				if (temp[a] == i->node) c = a;
			temp = vector<int>(temp.begin() + c, temp.end());
			i->setDirection(getMoveGB(i->node, temp[1]));
			i->nextNode = temp[1];
			i->setAlternative(i->node);
		}
	}
	for (int r = 0; r < height; r++)
		for (int c = 0; c < width; c++)
			matrixWeight[r][c].weight = 0.0f;
	for (std::list<Bug>::iterator i = bugsList.begin(); i != bugsList.end(); i++) {
		cerr << "bug " << i->id << " type " << i->type << " [ " << i->node << "] to obj in dir: " << i->direction << " to objective " << i->objective << endl;
		vector<int>& temp = pathsOfBugs.at(i->id).discendenza;
		unsigned int c = 0;
		for (unsigned int a = 0; a < temp.size(); a++)
			if (temp[a] == i->node) c = a;
		vector<int> & path = pathsOfBugs.at(i->id).discendenza;
		for (unsigned int node = c; node < path.size(); node++) {
			Point p(nodeToPair(path[node]));
			if (node == c) {
				cerr << "path to " << i->objective << ": ";
				for (unsigned int a = 0; a < path.size(); a++)
					cerr << path[a] << ",";
				cerr << endl;
				matrixWeight[p.y][p.x].weight = 1.5f;
			} else {
				float weight = 1.0f - ((node - 1.0f) / 10.0);
				if (weight < 0.0) weight = 0.0f;
				if (matrixWeight[p.y][p.x].weight < weight) matrixWeight[p.y][p.x].weight = weight;
			}
		}
	}
	if (bugsList.size() != 0) {
		cerr << turn << endl;
		for (std::list<Bug>::iterator i = bugsList.begin(); i != bugsList.end(); i++)
			cerr << i->y << "---" << i->x << endl;
		cerr << "---------------------------" << endl;
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++)
				cerr << matrixWeight[r][c].weight << "  |  ";
			cerr << endl;
		}
	}
}
bool inAdiacentDiAdiacent(int node) {
	vector<int> ad = getAdiacentsNode(node, false);
	for (unsigned int i = 0; i < ad.size(); i++) {
		std::list<Bug>::iterator bug;
		for (bug = bugsList.begin(); bug != bugsList.end(); bug++) {
			if (bug->node == ad[i]) {
				vector<int> ad2 = getAdiacentsNode(bug->node, false);
				ad2.push_back(ad[i]);
				for (unsigned int j = 0; j < ad2.size(); j++) {
					std::list<Bug>::iterator bug2;
					for (bug2 = bugsList.begin(); bug2 != bugsList.end(); bug2++) {
						if (bug->id != bug2->id && bug2->node == ad2[j]) {
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
			matrixWall = new bool*[height];
			for (int i = 0; i < height; i++) {
				matrixWeight[i] = new Cell[width];
				matrixWall[i] = new bool[width] { 0 };
			}
			initAdiacents();
		}

	} else if (command == "update") {
		string player_name, type;
		cin >> player_name >> type;
		if (type == "round") {
			cin >> current_round;
		} else if (type == "field") {
			snippets.clear();
			weapons.clear();
			gates.clear();
			spawn_points.clear();
			string s;
			cin >> s;
			std::istringstream iss(s);
			std::vector<std::string> fields((std::istream_iterator<StringDelimitedBy<','>>(iss)), std::istream_iterator<StringDelimitedBy<','>>());
			for (unsigned int ce = 0; ce < fields.size(); ce++) {
				if (fields[ce][0] == 'x' && !isSetWalls) {
					Point p = nodeToPair(ce);
					matrixWall[p.y][p.x] = true;
					removeAdiacents(ce);
				} else if (fields[ce][0] == 'x') continue;
				parseObjects(fields[ce], ce);
			}
			cleanBugs();
			if (!isSetWalls) addAdiacentsGates();
			isSetWalls = true;
			sort(snippets.begin(), snippets.end());
			weigths_cells();
			if (bugsList.size() != precBugNumber || checkChange() || current_round < 2
					|| (inAdiacentDiAdiacent(darkMind.node) ?
							(matrixWeight[darkMind.y][darkMind.x].weight >= 0.9 && precWeight < 0.9)
									|| (matrixWeight[darkMind.y][darkMind.x].weight >= 0.9 && matrixWeight[darkMind.y][darkMind.x].weight > precWeight) :
							(matrixWeight[darkMind.y][darkMind.x].weight >= 0.8 && precWeight < 0.8)
									|| (matrixWeight[darkMind.y][darkMind.x].weight >= 0.8 && matrixWeight[darkMind.y][darkMind.x].weight > precWeight))) {
				precBugNumber = bugsList.size();
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
		delete[] matrixWall[i];
	}
	delete[] matrixWeight;
	delete[] matrixWall;
	return 0;
}
int countBugToUp() {
	int toReturn = 0;
	vector<int> ad = getAdiacentsNode(darkMind.node, false);
	bool found = false;
	for (unsigned int a = 0; a < ad.size(); a++)
		if (ad[a] == darkMind.node - width) found = true;
	if (!found) return INT_MAX;
	ad = getAdiacentsNode(darkMind.node - width, false);
	int temp = darkMind.node - width;
	while (ad.size() == 2) {
		int index = (ad[0] == temp) ? 1 : 0;
		temp = ad[index];
		if (getWeight(ad[index]) >= 1.0) toReturn++;
		ad = getAdiacentsNode(ad[index], false);
	}
	return toReturn;
}
int countBugToDown() {
	int toReturn = 0;
	vector<int> ad = getAdiacentsNode(darkMind.node, false);
	bool found = false;
	for (unsigned int a = 0; a < ad.size(); a++)
		if (ad[a] == darkMind.node + width) found = true;
	if (!found) return INT_MAX;
	ad = getAdiacentsNode(darkMind.node + width, false);
	int temp = darkMind.node + width;
	while (ad.size() == 2) {
		int index = (ad[0] == temp) ? 1 : 0;
		temp = ad[index];
		if (getWeight(ad[index]) >= 1.0) toReturn++;
		ad = getAdiacentsNode(ad[index], false);
	}
	return toReturn;
}
int countBugToRight() {
	int toReturn = 0;
	vector<int> ad = getAdiacentsNode(darkMind.node, false);
	bool found = false;
	for (unsigned int a = 0; a < ad.size(); a++)
		if (ad[a] == darkMind.node + 1) found = true;
	if (!found) return INT_MAX;
	ad = getAdiacentsNode(darkMind.node + 1, false);
	int temp = darkMind.node + 1;
	while (ad.size() == 2) {
		int index = (ad[0] == temp) ? 1 : 0;
		temp = ad[index];
		if (getWeight(ad[index]) >= 1.0) toReturn++;
		ad = getAdiacentsNode(ad[index], false);
	}
	return toReturn;
}
int countBugToLeft() {
	int toReturn = 0;
	vector<int> ad = getAdiacentsNode(darkMind.node, false);
	bool found = false;
	for (unsigned int a = 0; a < ad.size(); a++)
		if (ad[a] == darkMind.node - 1) found = true;
	if (!found) return INT_MAX;
	ad = getAdiacentsNode(darkMind.node - 1, false);
	int temp = darkMind.node - 1;
	while (ad.size() == 2) {
		int index = (ad[0] == temp) ? 1 : 0;
		temp = ad[index];
		if (getWeight(ad[index]) >= 1.0) toReturn++;
		ad = getAdiacentsNode(ad[index], false);
	}
	return toReturn;
}
string getSafeDir() {
	int dm = darkMind.node;
//	int costDir[4] { countBugToDown(), countBugToUp(), countBugToRight(), countBugToLeft() };
//	int min = INT_MAX;
//	int nodeToMove = 0;
//	for (int i = 0; i < 4; i++)
//		if (costDir[i] < min) {
//			min = costDir[i];
//			switch (i) {
//			case 0:
//				nodeToMove = dm + width;
//				break;
//			case 1:
//				nodeToMove = dm - width;
//				break;
//			case 2:
//				nodeToMove = dm + 1;
//				break;
//			case 3:
//				nodeToMove = dm - 1;
//				break;
//			}
//		}
//	if (min == INT_MAX) {
//		vector<int> nodes = getAdiacentsNode(darkMind.node, false);
//		vector<Cell> adiacents;
//		for (unsigned int i = 0; i < nodes.size(); i++)
//			adiacents.push_back(matrixWeight[nodeToPair(nodes[i]).y][nodeToPair(nodes[i]).x]);
//		sort(adiacents.begin(), adiacents.end());
//		int toR = 0;
//		if (adiacents[0].weight == adiacents[1].weight || abs(adiacents[0].weight - adiacents[1].weight) == 0.1) {
//			float min = INT_MAX;
//			for (unsigned int a = 0; a < gates.size(); a++) {
//				float e0 = euclidianDistanceNode(adiacents[0].node, gates[a].node);
//				float e1 = euclidianDistanceNode(adiacents[1].node, gates[a].node);
//				if (e0 < e1 && e0 < min) toR = 0;
//				if (e1 < e0 && e1 < min) toR = 1;
//			}
//		}
//		return getMoveGB(darkMind.node, adiacents[toR].node);
//	} else {
//		string move = getMoveGB(darkMind.node, nodeToMove);
//		cerr << " with move " << move << endl;
//		return move;
//	}

	int l1 = darkMind.node - 1, l2 = darkMind.node - 2, r1 = darkMind.node + 1, r2 = darkMind.node + 2;
	int u1 = darkMind.node - width, u2 = darkMind.node - width * 2, d1 = darkMind.node + width, d2 = darkMind.node + width * 2;
	int gate = (euclidianDistanceNode(dm, gates[0].node) < euclidianDistanceNode(dm, gates[1].node)) ? gates[0].node : gates[1].node;
	if (((inMatrixNode(u1) && getWeight(u1) >= 1.5) || (inMatrixNode(u2) && getWeight(u2) >= 1.5))   //ATTACK U ,D and R
	&& ((inMatrixNode(d1) && getWeight(d1) >= 1.5) || (inMatrixNode(d2) && getWeight(d2) >= 1.5))
			&& ((inMatrixNode(r1) && getWeight(r1) >= 1.5) || (inMatrixNode(r2) && getWeight(r2) >= 1.5))) {
		if (inMatrixNode(l1) && matrixAdiacents[dm][l1]) return "left";
	} else if (((inMatrixNode(u1) && getWeight(u1) >= 1.5) || (inMatrixNode(u2) && getWeight(u2) >= 1.5))   //ATTACK U ,D and L
	&& ((inMatrixNode(d1) && getWeight(d1) >= 1.5) || (inMatrixNode(d2) && getWeight(d2) >= 1.5))
			&& ((inMatrixNode(l1) && getWeight(l1) >= 1.5) || (inMatrixNode(l2) && getWeight(l2) >= 1.5))) {
		if (inMatrixNode(r1) && matrixAdiacents[dm][r1]) return "right";
	} else if (((inMatrixNode(l1) && getWeight(l1) >= 1.5) || (inMatrixNode(l2) && getWeight(l2) >= 1.5)) //ATTACK R,L and D
	&& ((inMatrixNode(r1) && getWeight(r1) >= 1.5) || (inMatrixNode(r1) && getWeight(r2) >= 1.5))
			&& ((inMatrixNode(d1) && getWeight(d1) >= 1.5) || (inMatrixNode(d2) && getWeight(d2) >= 1.5))) {
		if (inMatrixNode(u1) && matrixAdiacents[dm][u1]) return "up";
	} else if (((inMatrixNode(l1) && getWeight(l1) >= 1.5) || (inMatrixNode(l2) && getWeight(l2) >= 1.5)) //ATTACK R,L and U
	&& ((inMatrixNode(r1) && getWeight(r1) >= 1.5) || (inMatrixNode(r1) && getWeight(r2) >= 1.5))
			&& ((inMatrixNode(u1) && getWeight(u1) >= 1.5) || (inMatrixNode(u2) && getWeight(u2) >= 1.5))) {
		if (inMatrixNode(d1) && matrixAdiacents[dm][d1]) return "down";
	} else if (((inMatrixNode(l1) && getWeight(l1) >= 1.5) || (inMatrixNode(l2) && getWeight(l2) >= 1.5))  //ATTACK R AND L
	&& ((inMatrixNode(r1) && getWeight(r1) >= 1.5) || (inMatrixNode(r1) && getWeight(r2) >= 1.5))) {
		if (inMatrixNode(u1) && inMatrixNode(d2) && matrixAdiacents[dm][u1] && matrixAdiacents[dm][d1]) {
			if (getWeight(d1) == getWeight(u1)) {
				if (euclidianDistanceNode(d1, gate) <= euclidianDistanceNode(u1, gate)) return "down";
				else return "up";
			} else if (getWeight(d1) > getWeight(u1)) return "up";
			else return "down";
		} else if (inMatrixNode(u1) && matrixAdiacents[dm][u1] && (!inMatrixNode(d1) || !matrixAdiacents[dm][d1])) return "up";
		else if ((!inMatrixNode(u1) || !matrixAdiacents[dm][u1]) && inMatrixNode(d1) && matrixAdiacents[dm][d1]) return "down";
	} else if (((inMatrixNode(u1) && getWeight(u1) >= 1.5) || (inMatrixNode(u2) && getWeight(u2) >= 1.5))   //ATTACK U AND D
	&& ((inMatrixNode(d1) && getWeight(d1) >= 1.5) || (inMatrixNode(d2) && getWeight(d2) >= 1.5))) {
		if (inMatrixNode(l1) && inMatrixNode(r1) && matrixAdiacents[dm][l1] && matrixAdiacents[dm][r1]) {
			if (getWeight(l1) == getWeight(r1)) {
				if (euclidianDistanceNode(l1, gate) <= euclidianDistanceNode(r1, gate)) return "left";
				else return "right";
			} else if (getWeight(l1) < getWeight(r1)) return "left";
			else return "right";
		} else if (inMatrixNode(l1) && matrixAdiacents[dm][l1] && (!matrixAdiacents[dm][r1] || !inMatrixNode(r1))) return "left";
		else if ((!inMatrixNode(l1) || !matrixAdiacents[dm][l1]) && inMatrixNode(r1) && matrixAdiacents[dm][r1]) return "right";
	} else if (((inMatrixNode(u1) && getWeight(u1) >= 1.5) || (inMatrixNode(u2) && getWeight(u2) >= 1.5))   //ATTACK U AND L
	&& ((inMatrixNode(l1) && getWeight(l1) >= 1.5) || (inMatrixNode(l2) && getWeight(l2) >= 1.5))) {
		if (inMatrixNode(d1) && inMatrixNode(r1) && matrixAdiacents[dm][d1] && matrixAdiacents[dm][r1]) {
			if (getWeight(d1) == getWeight(r1)) {
				if (euclidianDistanceNode(d1, gate) < euclidianDistanceNode(r1, gate)) return "down";
				else return "right";
			} else if (getWeight(d1) < getWeight(r1)) return "down";
			else return "right";
		} else if (inMatrixNode(d1) && matrixAdiacents[dm][d1] && (!matrixAdiacents[dm][r1] || !inMatrixNode(r1))) return "down";
		else if ((!inMatrixNode(d1) || !matrixAdiacents[dm][d1]) && inMatrixNode(r1) && matrixAdiacents[dm][r1]) return "right";
	} else if (((inMatrixNode(u1) && getWeight(u1) >= 1.5) || (inMatrixNode(u2) && getWeight(u2) >= 1.5))   //ATTACK U AND R
	&& ((inMatrixNode(r1) && getWeight(r1) >= 1.5) || (inMatrixNode(r2) && getWeight(r2) >= 1.5))) {
		if (inMatrixNode(d1) && inMatrixNode(l1) && matrixAdiacents[dm][d1] && matrixAdiacents[dm][l1]) {
			if (getWeight(d1) == getWeight(l1)) {
				if (euclidianDistanceNode(d1, gate) < euclidianDistanceNode(l1, gate)) return "down";
				else return "left";
			} else if (getWeight(d1) < getWeight(l1)) return "down";
			else return "left";
		} else if (inMatrixNode(d1) && matrixAdiacents[dm][d1] && (!matrixAdiacents[dm][l1] || !inMatrixNode(l1))) return "down";
		else if ((!inMatrixNode(d1) || !matrixAdiacents[dm][d1]) && inMatrixNode(l1) && matrixAdiacents[dm][l1]) return "left";
	} else if (((inMatrixNode(d1) && getWeight(d1) >= 1.5) || (inMatrixNode(d2) && getWeight(d2) >= 1.5))   //ATTACK D AND L
	&& ((inMatrixNode(l1) && getWeight(l1) >= 1.5) || (inMatrixNode(l2) && getWeight(l2) >= 1.5))) {
		if (inMatrixNode(u1) && inMatrixNode(r1) && matrixAdiacents[dm][u1] && matrixAdiacents[dm][r1]) {
			if (getWeight(r1) == getWeight(u1)) {
				if (euclidianDistanceNode(r1, gate) <= euclidianDistanceNode(u1, gate)) return "right";
				else return "up";
			} else if (getWeight(u1) < getWeight(r1)) return "up";
			else return "right";
		} else if (inMatrixNode(u1) && matrixAdiacents[dm][u1] && (!matrixAdiacents[dm][r1] || !inMatrixNode(r1))) return "up";
		else if ((!inMatrixNode(u1) || !matrixAdiacents[dm][u1]) && inMatrixNode(r1) && matrixAdiacents[dm][r1]) return "right";
	} else if (((inMatrixNode(d1) && getWeight(d1) >= 1.5) || (inMatrixNode(d2) && getWeight(d2) >= 1.5))   //ATTACK D AND R
	&& ((inMatrixNode(r1) && getWeight(r1) >= 1.5) || (inMatrixNode(r2) && getWeight(r2) >= 1.5))) {
		if (inMatrixNode(u1) && inMatrixNode(l1) && matrixAdiacents[dm][u1] && matrixAdiacents[dm][l1]) {
			if (getWeight(l1) == getWeight(u1)) {
				if (euclidianDistanceNode(l1, gate) <= euclidianDistanceNode(u1, gate)) return "left";
				else return "up";
			} else if (getWeight(u1) < getWeight(l1)) return "up";
			else return "left";
		} else if (inMatrixNode(u1) && matrixAdiacents[dm][u1] && (!matrixAdiacents[dm][l1] || !inMatrixNode(l1))) return "up";
		else if ((!inMatrixNode(u1) || !matrixAdiacents[dm][u1]) && inMatrixNode(l1) && matrixAdiacents[dm][l1]) return "left";
	}
	vector<int> nodes = getAdiacentsNode(darkMind.node, false);
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
	return getMoveGB(darkMind.node, adiacents[toR].node);
}
Point dirToCell(string s) {
	if (s == "left") return std::move(Point(darkMind.x - 1, darkMind.y));
	if (s == "up") return std::move(Point(darkMind.x, darkMind.y - 1));
	if (s == "right") return std::move(Point(darkMind.x + 1, darkMind.y));
	if (s == "down") return std::move(Point(darkMind.x, darkMind.y + 1));
	return std::move(Point(darkMind.x, darkMind.y));
}
bool sureToGo(const int& toGo) {
	if (bugsList.empty()) return true;
	float weightToGo = getWeight(toGo);
	float weightDM = getWeight(darkMind.node);
	if (weightToGo <= 0.70 || (weightDM >= weightToGo && weightDM <= 0.9)) return true;
	return false;
}
map<ObjectWithRounds, vector<int> > checkDangerNode();
vector<int> getDangerNodes(ObjectWithRounds bomb);
vector<SimpleObject> toScan;
bool canGo(int toGo, int toGoGo) {
	if (getWeight(toGo) == 0.9 && getWeight(toGoGo) == 1.0) return false;
	return true;
}
void do_move() {
	cerr << "start turn: " << turn << endl;
	cerr << "my pos:" << darkMind.node << "     enemy pos:" << enemy.node << endl;
	int toPrint = -1, toGO, aroundV;
	unsigned int min = INT_MAX, pathChoicher, sizeEN, sizeDM, minCost = INT_MAX;
	if (weightChange || newBugPatIsCreate) {
		toScan = vector<SimpleObject>(snippets);
		for (unsigned int a = 0; a < weapons.size(); a++) {
			if (weapons[a].rounds == -1) toScan.push_back(SimpleObject(weapons[a].node));
		}
		sort(toScan.begin(), toScan.end());
	}
	bool canGoo = false;
	for (unsigned int i = 0; i < toScan.size(); i++) {
		NodeGB temp = pathFindGB(darkMind.node, toScan[i].node, false);
		sizeDM = temp.discendenza.size();
		cerr << "path to " << toScan[i].node << ": ";
		for (unsigned int a = 0; a < sizeDM; a++)
			cerr << temp.discendenza[a] << ",";
		cerr << "with length " << sizeDM;
		NodeGB tempEn = pathFindGB(enemy.node, toScan[i].node, false);
		toGO = temp.discendenza[1];
		aroundV = numbersBonusNear(toScan[i].node, around) - 1;
		pathChoicher = sizeDM - aroundV;
		cerr << " updated to " << pathChoicher << endl;
		sizeEN = tempEn.discendenza.size();
		if (temp.node != numNodes && (sizeEN >= sizeDM) && pathChoicher <= min) {
			if (pathChoicher == min && temp.cost < minCost) {
				toPrint = toGO;
				if (temp.discendenza.size() > 2) canGoo = canGo(toGO, temp.discendenza[2]);
				else canGoo = true;
				minCost = temp.cost;
				min = pathChoicher;
			} else if (pathChoicher < min) {
				min = pathChoicher;
				toPrint = toGO;
				if (temp.discendenza.size() > 2) canGoo = canGo(toGO, temp.discendenza[2]);
				else canGoo = true;
				minCost = temp.cost;
			}
		}
	}
	cerr << "CHOICHE: ";
	if (toPrint == -1) {
		NodeGB toCe = pathFindGB(darkMind.node, centerNode, false);
		if (toCe.node != numNodes && sureToGo(toCe.discendenza[1])) {
			cout << getMoveGB(darkMind.node, toCe.discendenza[1]) << endl;
			cerr << "mi muovo secondo path" << endl;
		} else {
			cout << getSafeDir() << endl;
			cerr << "save my ass -1 " << endl;
		}
	} else if (!sureToGo(toPrint) && canGoo) {
		cout << getSafeDir() << endl;
		cerr << "save my ass " << endl;
	} else {
		cout << getMoveGB(darkMind.node, toPrint) << endl;
		cerr << "GO TO " << toPrint << endl;
	}
	cerr << "CHOICHE: ";
	if (toPrint == -1) {
		NodeGB toCe = pathFindGB(darkMind.node, centerNode, false);
		(toCe.node != numNodes && sureToGo(toCe.discendenza[1])) ? cout << getMoveGB(darkMind.node, toCe.discendenza[1]) << endl : cout << getSafeDir() << endl;
		cerr << "GO TO CENTER or SAVE" << endl;
	} else if (!sureToGo(toPrint)) {
		cout << getSafeDir() << endl;
		cerr << "save my ass " << endl;
	} else {
		cout << getMoveGB(darkMind.node, toPrint) << endl;
		cerr << "GO TO " << toPrint << endl;
	}
//	}
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
	std::list<Bug>::iterator it;
	for (it = bugsList.begin(); it != bugsList.end(); ++it) {
		int posBugs = it->node;
		float dist = euclidianDistanceNode(posBegin, posBugs);
		if (min > dist) {
			min = dist;
			closest = it->node;
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
int getAdiacentsWithMinimuED(const int& posBegin, const int & posEnd) {
	Point p(posEnd);
	float eDistance = euclidianDistanceNode(posBegin, posEnd);
	int toReturn = 0;
	float minDifference = INTMAX_MAX;
	for (int i = 0; i < 4; i++) {
		if (inMatrix(p.y + dy[i], p.x + dx[i]) && !matrixWall[p.y + dy[i]][p.x + dx[i]]) {
			float minDiff;
			if ((minDiff = abs((eDistance - euclidianDistanceNode(posBegin, pairToNode(p.y + dy[i], p.x + dx[i]))))) < minDifference) {
				minDifference = minDiff;
				toReturn = i;
			}
		}
	}
	return pairToNode(p.y + dy[toReturn], p.x + dx[toReturn]);

}
int getObjectiveGreenBugsTR(const int & posBegin) {
	int closestPlayer = getClosestPlayer(posBegin);
	Player p = (closestPlayer == darkMind.id) ? darkMind : enemy;
	Point direction(p.getDirectionVector());
	int newX = p.x + direction.x * 4;
	int newY = p.y + direction.y * 4;
	if (newX < 0) newX = 0;
	if (newX >= width) newX = width - 1;
	if (newY < 0) newY = 0;
	if (newY >= height) newY = height - 1;
	return pairToNode(newY, newX);
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
	int bugX = nodeToPair(closestBug).x;
	int bugY = nodeToPair(closestBug).y;
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
int getObjectiveVioletBugsBL(const int& posBegin) {
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
vector<int> getAdiacentsNode(const int& node, const bool & isforBug) {
	vector<int> toReturn;
	if (node - width >= 0 && (matrixAdiacents[node][node - width] || (isforBug && node - width == objBugs[currentBug].node))
			&& !(isforBug && currentBugObj.notBackPass(node - width))) toReturn.push_back(node - width);
	if (node + width < numNodes && (matrixAdiacents[node][node + width] || (isforBug && node + width == objBugs[currentBug].node))
			&& !(isforBug && currentBugObj.notBackPass(node + width))) toReturn.push_back(node + width);
	if (node + 1 < numNodes && (matrixAdiacents[node][node + 1] || (isforBug && node + 1 == objBugs[currentBug].node)) && !(isforBug && currentBugObj.notBackPass(node + 1)))
		toReturn.push_back(node + 1);
	if (node - 1 >= 0 && (matrixAdiacents[node][node - 1] || (isforBug && node - 1 == objBugs[currentBug].node)) && !(isforBug && currentBugObj.notBackPass(node - 1)))
		toReturn.push_back(node - 1);
	if (!isforBug) {
		if (node == gates[0].node) toReturn.push_back(gates[1].node);
		if (node == gates[1].node) toReturn.push_back(gates[0].node);
	}
	return toReturn;
}
vector<int> getAlternative(const int& node) {
	vector<int> toReturn;
	if (node - width >= 0 && matrixAdiacents[node][node - width]) toReturn.push_back(node - width);
	if (node + width < numNodes && matrixAdiacents[node][node + width]) toReturn.push_back(node + width);
	if (node + 1 < numNodes && matrixAdiacents[node][node + 1]) toReturn.push_back(node + 1);
	if (node - 1 >= 0 && matrixAdiacents[node][node - 1]) toReturn.push_back(node - 1);
	return toReturn;
}
void cleanBugs() {
	for (list<Bug>::iterator a = bugsList.begin(); a != bugsList.end(); a++) {
		if (!a->founded && !a->fakeBug) {
			a = bugsList.erase(a);
			cerr << "DELETED BUG" << endl;
		} else a->founded = false;
	}
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
NodeGB pathFindGB(const int & posBegin, const int & posEnd, bool isForBug) {
	bool found = false;
	vector<int> disc;
	NodeGB bestNode(numNodes, 0, disc);
	if (posEnd == -1) {
		cerr << "EORRRE" << endl;
		return bestNode;
	}
	if (posBegin == posEnd) {
		bestNode.discendenza.push_back(posBegin);
		bestNode.discendenza.push_back(posBegin);
		return bestNode;
	}
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
		vector<int> ad = getAdiacentsNode(s.node, isForBug);
		for (unsigned int i = 0; i < ad.size(); i++) {
			if (!visited[ad[i]]) {
				visited[ad[i]] = true;
				vector<int> dis(s.discendenza);
				dis.push_back(s.node);
				float cost = euclidianDistanceNode(ad[i], posEnd) + (isForBug ? 0 : s.cost);
				int w = getWeight(ad[i]);
				if (!isForBug && w >= 0.75 && dis.size() < 5) cost *= (cost * w);
				NodeGB temp(ad[i], cost, dis);
				bool notPushNode = false;
				if (!isForBug) {
					for (std::map<int, NodeGB>::iterator bug = pathsOfBugs.begin(); bug != pathsOfBugs.end(); bug++) {
						vector<int>& temp1 = bug->second.discendenza;
						if (temp1.size() > dis.size()) {
							if (temp1[dis.size()] == ad[i] && getWeight(ad[i]) > 0.2) {
								notPushNode = true;
							}
						}
					}
				}
				if (found && temp.cost < bestNode.cost && !notPushNode) queue.push(temp);
				else if (!found && !notPushNode) queue.push(temp);
			}
		}
	}
	bestNode.discendenza.push_back(posEnd);
	return bestNode;
}

