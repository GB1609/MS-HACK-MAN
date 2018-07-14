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
using namespace std;
int width = -1;
int height = -1;
int numNodes = -1;
static int turn = 0;
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
string getMoveGB(int begin, int move) {
	if (begin - move == 1 || begin + (width - 1) == move) return "left";
	else if (begin + 1 == move || begin - move == width - 1) return "right";
	else if (begin + width == move) return "down";
	else if (begin - width == move) return "up";
	return "pass";
}
vector<int> pathFindGB(const int & posBegin, const int & posEnd);
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

void choose_character();
void do_move();

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
		weigths_cells();

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

vector<Cell> adiacents;
for (int i = 0; i < 4; i++) {
	if (inMatrix(darkMind.y + dy[i], darkMind.x + dx[i]) && matrixAdiacents[darkMind.node][pairToNode(darkMind.y + dy[i], darkMind.x + dx[i])])
		adiacents.push_back(matrixWeight[darkMind.y + dy[i]][darkMind.x + dx[i]]);
}
sort(adiacents.begin(), adiacents.end());
Cell & cSafe = adiacents[0];
cerr << cSafe.x << "--" << cSafe.y << endl;
if (cSafe.x > darkMind.x) return "right";
if (cSafe.x < darkMind.x) return "left";
if (cSafe.y > darkMind.y) return "down";
return "up";
}
Point dirToCell(string s) {

if (s == "left") return std::move(Point(darkMind.x - 1, darkMind.y));
if (s == "up") return std::move(Point(darkMind.x, darkMind.y - 1));
if (s == "right") return std::move(Point(darkMind.x + 1, darkMind.y));
if (s == "down") return std::move(Point(darkMind.x, darkMind.y + 1));
return std::move(Point(darkMind.x, darkMind.y));

}
void do_move() {

cerr << "start turn: " << turn << endl;
cerr << "myPOS:" << darkMind.node << endl;
int toPrint = -1;
unsigned int min = width * height;
for (unsigned int i = 0; i < snippets.size(); i++) {
	vector<int> temp = pathFindGB(darkMind.node, snippets[i].node);
	vector<int> tempEn = pathFindGB(enemy.node, snippets[i].node);
	if (temp.size() < min && temp.size() > 0) {
		min = temp.size();
		toPrint = temp[temp.size() - 1];
	}
}
if (toPrint == -1 || getMoveGB(darkMind.node, toPrint) == "pass") {
	cout << "pass" << endl;
} else cout << getMoveGB(darkMind.node, toPrint) << endl;
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
vector<int> pathFindGB(const int & posBegin, const int & posEnd) {
bool found = false;
vector<int> disc;
NodeGB bestNode(numNodes, 0, disc);
// Mark all the vertices as not visited
bool visited[numNodes];
for (int i = 0; i < numNodes; i++)
	visited[i] = false;
// Create a queue for BFS
queue<NodeGB> queue;
// Mark the current node as visited and enqueue it
visited[posBegin] = true;
NodeGB start(posBegin, 0, disc);
queue.push(start);
while (!queue.empty()) {
	// Dequeue a vertex from queue and print it
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
			vector<int> dis(s.discendenza.size());
			for (unsigned int a = 0; a < dis.size(); a++)
				dis[a] = s.discendenza[a];
			dis.push_back(s.node);
			NodeGB temp(ad[i], euclidianDistanceNode(ad[i], posEnd) + s.cost, dis);
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
