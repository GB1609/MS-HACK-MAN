#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <ctime>
#include <queue>
#include <sstream>
#include <iterator>
using namespace std;
int width=-1;
int height=-1;
int numNodes=-1;
////////// let a pair of coordinates returns the corresponding node
int pairToNode(const int & r,const int & c)
{
  return r * width + c;
}
class Point;
class Point {
public:
	int x;
	int y;
	int node;
	Point() {}
	Point(int x, int y): x(x), y(y), node(pairToNode(y,x)) {}
	Point(int node) :
			x(node - ((node / width) * width)), y(node / width), node(node) {
	}

	Point(const Point& p):x(p.x),y(p.y),node(p.node){};
	Point(Point&& p):x(std::move(p.x)),y(std::move(p.y)),node(std::move(p.node)){};
	bool operator<(const Point &a) const {
		return x < a.x || (x == a.y && y < a.y);
	}
	bool operator==(const Point &a) const {
		return x == a.x && y == a.y;
	}

};
///////// let a node returns its coordinates
Point nodeToPair(const int & node)
{
  int r = node / width;
	int c = node - (r * width);
  return std::move(Point(c,r));
}
class Player : public Point {
public:
	string name;
	int id;
	int snippets;
	int bombs;
};
class SimpleObject: public Point {
public:
	SimpleObject(int x, int y) :
			Point(x, y) {
	}

	SimpleObject(int node) :
			Point(node) {
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
//class Cell: public Point{
template <char delimiter>
class StringDelimitedBy: public string{

	friend std::istream& operator>>(std::istream& is, StringDelimitedBy& output)
	{
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
Player me;
Player enemy;
vector<SimpleObject> snippets;
vector<ObjectWithRounds> weapons;
vector<ObjectWithRounds> bugs;
vector<ObjectWithRounds> spawn_points;
vector<ObjectWithRounds> gates;
//vector<ObjectWithRounds> cells;

void choose_character();
void do_move();

bool** matrixAdiacents;
bool isSetWalls=false;
int dx[4] = { 0, -1, 0, 1 };
int dy[4] = { -1, 0, 1, 0 };
string moves[4] = { "up", "left", "down", "right" };
void initAdiacents(){
	for(int r=0;r<height;r++){
		for(int c=0;c<width;c++){
			int currentNode=pairToNode(r,c);
			int adiacentNode;
			for(int i=0;i<4;i++){
				int adiacentR=r+dy[i];
				int adiacentC=c+dx[i];
				if(adiacentR<height&&adiacentR>=0&&adiacentC<width&&adiacentC>=0)
				{
					adiacentNode=pairToNode(adiacentR,adiacentC);
					matrixAdiacents[currentNode][adiacentNode]=1;
					matrixAdiacents[adiacentNode][currentNode]=1;
				}
			}
		}
	}
}
void removeAdiacents(int cell){
	Point cellCoordinate(cell);
	for(int i=0;i<4;i++){
		int adiacentR=cellCoordinate.y+dy[i];
		int adiacentC=cellCoordinate.x+dx[i];

		if(adiacentR<height&&adiacentR>=0&&adiacentC<width&&adiacentC>=0)
		{
			int adiacentNode=pairToNode(adiacentR,adiacentC);
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
	if (objList.size() == 1)
		parseSingleCharacter(objList[0], cell);
	else {
		std::istringstream issl(objList);
		std::vector<string> objs(
				(std::istream_iterator<StringDelimitedBy<';'>>(issl)),
				std::istream_iterator<StringDelimitedBy<';'>>());
		for (unsigned int i = 0; i < objs.size(); i++) {
			if (objs[i].size() == 1)
				parseSingleCharacter(objs[i][0], cell);
			else {
				switch (objs[i][0]) {
				case 'P': {
					int id = objs[i][1] - '0';
					if (id == me.id) {
						me.x = currentCoordinates.x;
						me.y = currentCoordinates.y;
					} else {
						enemy.x = currentCoordinates.x;
						enemy.y = currentCoordinates.y;
					}
				}
					break;
				case 'S': {
					spawn_points.push_back(
							ObjectWithRounds(cell, stoi(objs[i].erase(0, 1))));
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
					bugs.push_back(ObjectWithRounds(cell, objs[i][1] - '0'));
					break;
				case 'B':
					weapons.push_back(
							ObjectWithRounds(cell, stoi(objs[i].erase(0, 1))));
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
		if(width!=-1 && height!=-1){
			//int tmp=width;
			//width=height;
			//height=tmp;
			numNodes = width * height;
			matrixAdiacents = new bool*[numNodes];
			for(int r =0;r < numNodes;r++){
				matrixAdiacents[r]=new bool[numNodes]{0};
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
			bugs.clear();
			gates.clear();
			spawn_points.clear();
			string s;
			cin>>s;

			std::istringstream iss(s);
			std::vector<std::string> fields((std::istream_iterator<StringDelimitedBy<','>>(iss)),
		                                 std::istream_iterator<StringDelimitedBy<','>>());

			for(unsigned int ce=0;ce<fields.size();ce++){
				if(fields[ce][0]=='x'&&!isSetWalls)
					removeAdiacents(ce);
				else if (fields[ce][0]=='x')
						continue;
				parseObjects(fields[ce],ce);
			}
			if (!isSetWalls) //todo delete if the gates modify their direction
				addAdiacentsGates();
			isSetWalls = true;
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
	for (int i = 0; i < numNodes; i++) {
		delete[] matrixAdiacents[i];
	}
	for (int i = 0; i < numNodes; i++) {
		delete matrixAdiacents[i];
	}
	return 0;
}

//-----------------------------------------//
//  Improve the code below to win 'em all  //
//-----------------------------------------//



void choose_character() {
	 cout << "bixiette" << endl;
}

void do_move() {
	srand (time(NULL));
	vector<string> valid_moves;
	valid_moves.clear();
	for (int dir = 0; dir < 4; dir++) {
		int nextx = me.x + dx[dir];
		int nexty = me.y + dy[dir];
		if (nextx >= 0 && nextx < width && nexty >= 0 && nexty < height) {
			if (matrixAdiacents[pairToNode(me.y, me.x)][pairToNode(nexty, nextx)]
					== 1) {
				valid_moves.push_back(moves[dir]);
			}
		}
	}
	if (valid_moves.size() == 0) {
		valid_moves.push_back("pass");
	}
	cout << valid_moves[rand() % valid_moves.size()] << endl;
}