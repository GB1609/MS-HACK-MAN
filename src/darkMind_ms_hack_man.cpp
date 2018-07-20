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
int canPutABomb(int currentNode);
int getNextNode(int currentNode,string direction);
bool checkSafeConditionExplosion(int currentNode,int candidateSafeNode);
int getintesectionNodeBeetweenMeAndEnemy(int);
int numberOfWayOut(int bugNode);
//int getSafeAdiacent(int currentNode,vector<int>nodes);
int iNeedToProgramABomb();
bool weightChange = true;
unsigned int precBugNumber = 0;
float precWeight = 0.0f;
vector<int>falseNode;
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
        direction = getMoveGB(node, p.node);
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
bool ** matrixWall;
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
bool inMatrixNode(const int& node) {
    for(int i=0;i<falseNode.size();i++)
        if(falseNode[i] == node)
            return false;

    return (node >= 0 && node < numNodes);
}
bool notInAdiacents(const int& node, const int& toVer) {
    vector<int> ad = getAdiacentsNode(node, false);
    for (unsigned int a = 0; a < ad.size(); a++)
        if (ad[a] == toVer) return true;
    return false;
}
float getWeight(const int& node) {
    if (node < 0 || node >= numNodes) return INT_MAX;
    return matrixWeight[nodeToPair(node).y][nodeToPair(node).x].weight;
}
bool thereIsBugNear(const int& node) {
    int circle = 2;
    int toReturn = 0;
    for (int i = -circle; i < circle + 1; i++)
        for (int c = -circle; c < circle + 1; c++) {
            for (unsigned int a = 0; a < bugs.size(); a++) {
                int cell = (node + (width * c)) + i;
                if (cell == bugs[a].node) toReturn++;
            }
        }
    return toReturn > 1;
}
int iTsPressed(Player player);
vector<NodeGB> bugsPaths;
vector<Point> objBugs;
int currentBug = 0;
map<ObjectWithRounds, vector<int> > checkDangerNode();
void weigths_cells() {
    // 0-> rosso
    // 1->verde
    //2 ->giallo
    //3 ->viola
    bugsPaths.clear();
    objBugs.clear();
    for (unsigned int i = 0; i < bugs.size(); i++) {
        switch (bugs[i].type) {
        case 0:
            objBugs.push_back(getObjectiveRedBugsTL(bugs[i].node));
            currentBug = i;
            bugsPaths.push_back(std::move(pathFindGB(bugs[i].node, objBugs[i].node, true)));
            break;
        case 1:
            objBugs.push_back(getObjectiveGreenBugsTR(bugs[i].node));
            currentBug = i;
            bugsPaths.push_back(std::move(pathFindGB(bugs[i].node, objBugs[i].node, true)));
            break;
        case 2:
            objBugs.push_back(getObjectiveYellowBugsBR(bugs[i].node));
            currentBug = i;
            bugsPaths.push_back(std::move(pathFindGB(bugs[i].node, objBugs[i].node, true)));
            break;
        case 3:
            objBugs.push_back(getObjectiveBlueBugsBL(bugs[i].node));
            currentBug = i;
            bugsPaths.push_back(std::move(pathFindGB(bugs[i].node, objBugs[i].node, true)));
            break;
        default:
            break;
        }
    }
    for (int r = 0; r < height; r++)
        for (int c = 0; c < width; c++)
            matrixWeight[r][c].weight = 0.0f;
    for (unsigned int i = 0; i < bugs.size(); i++) {
        //        cerr << "bug " << i << " type " << bugs[i].type << " [ " << bugs[i].node << "] to obj" << objBugs[i].node;
        vector<int> & path = bugsPaths[i].discendenza;
        string dirBug = getMoveGB(bugs[i].node, path[1]);
        //        cerr << dirBug << endl;
        for (unsigned int node = 0; node < path.size(); node++) {
            Point p(nodeToPair(path[node]));
            if (node == 0) {
                //                cerr << "path to " << objBugs[i].node << ": ";
                for (unsigned int a = 0; a < path.size(); a++)
                    //                    cerr << path[a] << ",";
                    //cerr << endl;
                    matrixWeight[p.y][p.x].weight = 1.5f;
            } else {
                //                cerr << "node" << node << "=" << (1.0f - ((node - 1.0f) / 10.0)) << endl;
                float weight = 1.0f - ((node - 1.0f) / 10.0);
                if (weight < 0.0) weight = 0.0f;
                if (matrixWeight[p.y][p.x].weight < weight) matrixWeight[p.y][p.x].weight = weight;

            }
        }
    }

    //modificato per le bombe controllate magari se non faccio casino
    map<ObjectWithRounds,vector<int> > m=checkDangerNode();
    for( map<ObjectWithRounds, vector<int> >::iterator it=m.begin();it!=m.end();it++)
        for(int i=0;i<it->second.size();i++)
        {
            Point p=nodeToPair(it->second[i]);
            matrixWeight[p.y][p.x].weight+=10;
        }



    // if (bugs.size() != 0) {
    //     cerr << turn << endl;
    //    for (unsigned int i = 0; i < bugs.size(); i++)
    //            cerr << bugs[i].y << "---" << bugs[i].x << endl;
    //  cerr << "---------------------------" << endl;
    // for (int r = 0; r < height; r++) {
    //    for (int c = 0; c < width; c++)
    //       cerr << matrixWeight[r][c].weight << "-";
    //  cerr << endl;
    //}
    // }
}
bool inAdiacentDiAdiacent(int node) {
    vector<int> ad = getAdiacentsNode(node, false);
    for (unsigned int i = 0; i < ad.size(); i++) {
        for (unsigned int bug = 0; bug < bugs.size(); bug++) {
            if (bugs[bug].node == ad[i]) {
                vector<int> ad2 = getAdiacentsNode(bugs[bug].node, false);
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
            matrixWall = new bool*[height];
            for (int i = 0; i < height; i++) {
                matrixWeight[i] = new Cell[width];
                matrixWall[i] = new bool[width] { 0 };
            }
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
                if (fields[ce][0] == 'x' && !isSetWalls) {
                    Point p = nodeToPair(ce);
                    matrixWall[p.y][p.x] = true;
                    removeAdiacents(ce);
                } else if (fields[ce][0] == 'x') continue;
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
        delete[] matrixWall[i];
    }
    delete[] matrixWeight;
    delete[] matrixWall;
    return 0;
}
string getSafeDir(int currentNode) {
    int dm = currentNode;
    int l1 = currentNode - 1, l2 = currentNode - 2, r1 = currentNode + 1, r2 = currentNode + 2;
    int u1 = currentNode - width, u2 = currentNode - width * 2, d1 = currentNode + width, d2 = currentNode + width * 2;
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
    vector<int> nodes = getAdiacentsNode(currentNode, false);
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
    return getMoveGB(currentNode, adiacents[toR].node);
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
bool sureToGo(const int& toGo) {
    if (bugs.empty()) return true;
    float weightToGo = getWeight(toGo);
    float weightDM = getWeight(darkMind.node);
    if (weightToGo <= 0.70 || (weightDM >= weightToGo && weightDM < 0.9)) return true;
    return false;
}

vector<int> getDangerNodes(ObjectWithRounds bomb);
vector<SimpleObject> toScan;
void do_move() {
    int bomb = -1;
    cerr << "start turn: " << turn << endl;
    cerr << "my pos:" << darkMind.node << "     enemy pos:" << enemy.node << endl;
    int toPrint = -1, toGO, aroundV;
    unsigned int min = INT_MAX, pathChoicher, sizeEN, sizeDM, minCost = INT_MAX;
    float eN, eDM;
    //	if ((inAdiacentDiAdiacent(darkMind.node)) ? getWeight(darkMind.node) >= 0.0 : getWeight(darkMind.node) >= 0.8) cout << getSafeDir() << endl;
    //	if (getWeight(darkMind.node) >= 0.8) {
    //		cout << getSafeDir() << endl;
    //		cerr << "save my ass" << endl;
    //	} else {
    if (weightChange) {
        toScan = vector<SimpleObject>(snippets);
        for (unsigned int a = 0; a < weapons.size(); a++) {
            if (weapons[a].rounds == -1) toScan.push_back(SimpleObject(weapons[a].node));
        }
        sort(toScan.begin(), toScan.end());
    }
    for (unsigned int i = 0; i < toScan.size(); i++) {
        NodeGB temp = pathFindGB(darkMind.node, toScan[i].node, false);
        sizeDM = temp.discendenza.size();
        //        cerr << "path to " << toScan[i].node << ": ";
        //        for (unsigned int a = 0; a < sizeDM; a++)
        //            cerr << temp.discendenza[a] << ",";
        //        cerr << "with length " << sizeDM;
        NodeGB tempEn = pathFindGB(enemy.node, toScan[i].node, false);
        toGO = temp.discendenza[1];
        aroundV = numbersBonusNear(toScan[i].node, around) - 1;
        pathChoicher = sizeDM - aroundV;
        //        cerr << " updated to " << pathChoicher << endl;
        sizeEN = tempEn.discendenza.size();
        eN = euclidianDistanceNode(enemy.node, toScan[i].node);
        eDM = euclidianDistanceNode(darkMind.node, toScan[i].node);
        if (temp.node != numNodes && (sizeEN >= sizeDM) && pathChoicher <= min) {
            if (pathChoicher == min && temp.cost < minCost) {
                toPrint = toGO;
                minCost = temp.cost;
                min = pathChoicher;
            } else if (pathChoicher < min) {
                min = pathChoicher;
                toPrint = toGO;
                minCost = temp.cost;
            }
        }
    }
    bomb = iNeedToProgramABomb();
    cerr << "CHOICHE: ";
    if (toPrint == -1) {

        NodeGB toCe = pathFindGB(darkMind.node, centerNode, false);
        if(bomb != -1)
            (toCe.node != numNodes && sureToGo(toCe.discendenza[1])) ? cout << getMoveGB(darkMind.node, toCe.discendenza[1]) <<";drop_bomb "<<bomb<<endl : cout << getSafeDir(darkMind.node)<<";drop_bomb "<<bomb<<endl;
        else
            (toCe.node != numNodes && sureToGo(toCe.discendenza[1])) ? cout << getMoveGB(darkMind.node, toCe.discendenza[1]) << endl : cout << getSafeDir(darkMind.node) << endl;

        cerr << "GO TO CENTER or SAVE" << endl;
    } else if (!sureToGo(toPrint)) {

        if(bomb != -1)
            cout << getSafeDir(darkMind.node) <<";drop_bomb "<<bomb<< endl;
        else
            cout << getSafeDir(darkMind.node) << endl;
        cerr << "save my ass " << endl;
    } else {
        if(bomb != -1)
            cout << getMoveGB(darkMind.node, toPrint) <<";drop_bomb "<<bomb<< endl;
        else
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
    Point b(posBegin);
    if (p.direction == "up") {

        int xNew = p.y - 4;
        if (xNew < 0) xNew = 0;
        if (b.x == p.x && b.y + 1 == p.y - 1) {
            xNew = b.y + 1;
        }
        if (xNew > height - 1) xNew = height - 1;
        return pairToNode(xNew, p.x);
    } else if (p.direction == "down") {
        int xNew = p.y + 4;
        if (xNew > height - 1) xNew = height - 1;
        if (b.x == p.x && b.y - 1 == p.y + 1) {
            xNew = b.y - 1;
        }
        if (xNew < 0) xNew = 0;
        return pairToNode(xNew, p.x);
    } else if (p.direction == "left") {
        int yNew = p.x - 4;
        if (yNew < 0) yNew = 0;
        if (b.y == p.y && b.x + 1 == p.x - 1) {
            yNew = b.x + 1;
        }
        if (yNew > width) yNew = width - 1;
        return pairToNode(p.y, yNew);
    } else if (p.direction == "right") {
        int yNew = p.x + 4;
        if (yNew > width - 1) yNew = width - 1;
        if (b.y == p.y && b.x - 1 == p.x + 1) {
            yNew = b.x - 1;
        }
        if (yNew < 0) yNew = 0;
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
vector<int> getAdiacentsNode(const int& node, const bool & isforBug) {
    vector<int> toReturn;
    if (node - width >= 0 && (matrixAdiacents[node][node - width] || (isforBug && node - width == objBugs[currentBug].node))) toReturn.push_back(node - width);
    if (node + width < numNodes && (matrixAdiacents[node][node + width] || (isforBug && node + width == objBugs[currentBug].node))) toReturn.push_back(node + width);
    if (node + 1 < numNodes && (matrixAdiacents[node][node + 1] || (isforBug && node + 1 == objBugs[currentBug].node))) toReturn.push_back(node + 1);
    if (node - 1 >= 0 && (matrixAdiacents[node][node - 1] || (isforBug && node - 1 == objBugs[currentBug].node))) toReturn.push_back(node - 1);
    if (!isforBug) {
        if (node == gates[0].node) toReturn.push_back(gates[1].node);
        if (node == gates[1].node) toReturn.push_back(gates[0].node);
    }
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
NodeGB pathFindGB(const int & posBegin, const int & posEnd, bool isForBug) {
    bool found = false;
    vector<int> disc;
    NodeGB bestNode(numNodes, 0, disc);
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
                float cost = euclidianDistanceNode(ad[i], posEnd) + s.cost;
                int w = getWeight(ad[i]);
                if (!isForBug && w >= 0.75 && dis.size() < 5) cost *= (cost * w);
                NodeGB temp(ad[i], cost, dis);
                if (found && temp.cost < bestNode.cost) queue.push(temp);
                else if (!found) queue.push(temp);
            }
        }
    }
    bestNode.discendenza.push_back(posEnd);
    return bestNode;
}
bool checkSafeConditionExplosion(int currentNode,int candidateSafeNode)
{
    Point current = nodeToPair(currentNode);
    Point candidateSafe = nodeToPair(candidateSafeNode);

    return ((current.x != candidateSafe.x and current.y != candidateSafe.y) ||
            (current.x == candidateSafe.x and current.y != candidateSafe.y and
            existWall(current.y,current.x,candidateSafe.y,candidateSafe.x)) ||
            (current.x != candidateSafe.x and current.y == candidateSafe.y and
            existWall(current.y,current.x,candidateSafe.y,candidateSafe.x)));
}
int canPutABomb(int currentNode)
{
    int FALSE = -1;
    if(darkMind.bombs == 0 || darkMind.node==133 || darkMind.node ==151)
        return FALSE;
cerr<<"1"<<endl;
    map<ObjectWithRounds,vector<int> > m=checkDangerNode();
    for( map<ObjectWithRounds, vector<int> >::iterator it=m.begin();it!=m.end();it++)
        if(it->first.rounds!=-1)
            for(int i=0;i<it->second.size();i++)
                if(it->second[i] == currentNode)
                    return FALSE;

    falseNode.push_back(currentNode);
    cerr<<"fine  1"<<endl;
    string safeDir = getSafeDir(currentNode);
    cerr<<"fine  2"<<endl;
    int nextSafeNode = getNextNode(currentNode,safeDir);
    cerr<<"fine  3"<<endl;
    falseNode.push_back(nextSafeNode);
    int maxCellNext = 6;
    int currentCellStep = 1;
    int initialNode = currentNode;

    while(!checkSafeConditionExplosion(initialNode,nextSafeNode)&&currentCellStep<maxCellNext)
    {
        cerr<<"OK   "<<currentCellStep<<endl;
        currentNode = nextSafeNode;
        safeDir = getSafeDir(currentNode);
        nextSafeNode = getNextNode(currentNode,safeDir);
        falseNode.push_back(nextSafeNode);
        currentCellStep++;
        cerr<<"NODIIII   "<<endl;
        for(int i=0;i<falseNode.size();i++)
            cerr<<falseNode[i]<<"   ";

    }

    falseNode.clear();
    cerr<<"finito il while"<<endl;
    if(currentCellStep == maxCellNext || currentCellStep < 2)
        return FALSE;

    return currentCellStep;



}
int getNextNode(int currentNode,string direction)
{
    int nextNode = 0;

    if(direction =="up")
        nextNode = currentNode-width;

    else if(direction =="down")
        nextNode = currentNode+width;

    else if(direction =="left")
        nextNode = currentNode-1;

    else if(direction =="right")
        nextNode = currentNode+1;

    if(nextNode<0)
        nextNode=0;
    if(nextNode> width*height)
        nextNode=width*height;

    return nextNode;
}



int iNeedToProgramABomb()
{
    int FALSE=-1;
    if(darkMind.bombs==0)
        return FALSE;

    //0-> rosso
    //1->verde
    //2 ->giallo
    //3 ->viola
    //    int node = -1;

    int nextDarkMindNode = getNextNode(darkMind.node,getSafeDir(darkMind.node));
    int intersectionNode = getintesectionNodeBeetweenMeAndEnemy(nextDarkMindNode);
    int nextDarkMindX = nodeToPair(nextDarkMindNode).x;
    int nextDarkMindY = nodeToPair(nextDarkMindNode).y;

    if(intersectionNode != -1) // vuol dire che si sta avvicinando
    {
        Point intersectionPoint= nodeToPair(intersectionNode);
        if((nextDarkMindX == enemy.x || nextDarkMindY == enemy.y)&&!existWall(nextDarkMindY,nextDarkMindX,intersectionPoint.y,intersectionPoint.x) &&
                !existWall(enemy.y,enemy.x,intersectionPoint.y,intersectionPoint.x))
        {
            int bugNodeValue=iTsPressed(enemy);
            if(bugNodeValue != -1 and numberOfWayOut(intersectionNode) + numberOfWayOut(bugNodeValue) == 0)
            {
                int roundsDarkMind = canPutABomb(nextDarkMindNode);
                if(roundsDarkMind != -1)
                {
                    int minRound = euclidianDistanceNode(nextDarkMindNode,intersectionNode);
                    if(roundsDarkMind == minRound)
                        return roundsDarkMind;
                    if(minRound > roundsDarkMind)
                        return minRound;
                }

            }

        }
    }



    int bomb=FALSE;
    bomb = canPutABomb(nextDarkMindNode);

    if((nextDarkMindX == enemy.x || nextDarkMindY == enemy.y)&&!existWall(nextDarkMindY,nextDarkMindX,enemy.y,enemy.x)
            && canPutABomb(enemy.node) < bomb)
        return bomb;

    bomb=FALSE;

    int value = iTsPressed(darkMind);

    if(value != -1 )
        bomb = canPutABomb(nextDarkMindNode);
    cerr<<value<<"      porca miseriaaa   bombaa "<<bomb<<endl;
    return bomb;
}
int numberOfWayOut(int bugNode)
{
    int xBug=nodeToPair(bugNode).x;
    int yBug=nodeToPair(bugNode).y;
    int x=enemy.x;
    int y=enemy.y;

    int number=0;

    if(yBug==y)
    {
        if(x < xBug)
            for(int i=x+1;i<xBug;i++)
                if(!existWall(y,i,y-1,i) || !existWall(y,i,y+1,i))
                    number++;

        if(x > xBug)
            for(int i=x-1;i>xBug;i--)
                if(!existWall(y,i,y-1,i) || !existWall(y,i,y+1,i))
                    number++;

    }
    if(xBug==x)
    {
        if(y < yBug)
            for(int i=y+1;i<yBug;i++)
                if(!existWall(x,i,x-1,i) || !existWall(x,i,x+1,i))
                    number++;

        if(y > yBug)
            for(int i=y-1;i>yBug;i--)
                if(!existWall(x,i,x-1,i) || !existWall(x,i,x+1,i))
                    number++;

    }


    return number;
}

int getintesectionNodeBeetweenMeAndEnemy(int darkMindNode)
{
    int x=0;
    int y=0;

    int enemyNewNode = getNextNode(enemy.node,getSafeDir(enemy.node));

    if(euclidianDistanceNode(enemyNewNode,darkMindNode) > euclidianDistanceNode(enemy.node,darkMindNode))
        return -1;

    if(enemyNewNode == enemy.node -1 || enemyNewNode == enemy.node +1)
    {
        x=nodeToPair(darkMindNode).x;
        y=enemy.y;

    }
    else if (enemyNewNode == enemy.node - width || enemyNewNode == enemy.node + width)
    {
        x=enemy.x;
        y=nodeToPair(darkMindNode).y;

    }


    return pairToNode(y,x);

}

int iTsPressed(Player enemy1)
{
    Player enemy=enemy1;
    enemy.node=getNextNode(enemy.node, getSafeDir(enemy.node));
    enemy.x=nodeToPair(enemy.node).x;
    enemy.y=nodeToPair(enemy.node).y;

    for(int i=0;i<bugs.size();i++)
    {
        if(bugs[i].type==0 and getObjectiveRedBugsTL(bugs[i].node) == enemy1.node)
            if((bugs[i].x == enemy.x and !existWall(bugs[i].y,bugs[i].x,enemy.y,enemy.x))
                    ||
                    (bugs[i].y == enemy.y and !existWall(bugs[i].y,bugs[i].x,enemy.y,enemy.x)))
                return bugs[i].node;

        if(bugs[i].type==1 and getClosestPlayer(bugs[i].node) == enemy1.node)
            if((bugs[i].x == enemy.x and !existWall(bugs[i].y,bugs[i].x,enemy.y,enemy.x) /*and nodeToPair(getObjectiveGreenBugsTR(bugs[i].node)).x == enemy.x*/)
                    ||
                    (bugs[i].y == enemy.y and !existWall(bugs[i].y,bugs[i].x,enemy.y,enemy.x) /*and nodeToPair(getObjectiveGreenBugsTR(bugs[i].node)).y == enemy.y*/))
                return bugs[i].node;

        if(bugs[i].type==2 and getClosestPlayer(bugs[i].node) == enemy1.node)
            if((bugs[i].x == enemy.x and !existWall(bugs[i].y,bugs[i].x,enemy.y,enemy.x) /*and nodeToPair(getObjectiveYellowBugsBR(bugs[i].node)).x == enemy.x*/)
                    ||
                    (bugs[i].y == enemy.y and !existWall(bugs[i].y,bugs[i].x,enemy.y,enemy.x) /*and nodeToPair(getObjectiveYellowBugsBR(bugs[i].node)).y == enemy.y*/))
                return bugs[i].node;

    }


    return -1;
}