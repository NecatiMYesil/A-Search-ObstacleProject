#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using std::cout;
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;
using std::abs;
using std::sort;


//##############################################################################
enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
/*
ParseLine reads the row and converts 0,1 s into type State.
@param line, row of the board
return converted board
*/
vector<State> ParseLine(string line) {
    istringstream sline(line);
    int n;
    char c;
  
    vector<State> row;
    while (sline >> n >> c && c == ',') {
      if(n == 0){ //Checks if the coord. is obstacle.
        State s = State::kEmpty; 
        row.push_back(s);
      }
      else //if it is obstacle it converts it into a mountain.
      {
        State s = State::kObstacle;
        row.push_back(s);
      }
    }
    return row;
}

/*
ReadBoardFile reads file with vectors.
@param path, file that its going to read.
return: board
*/
vector<vector<State>> ReadBoardFile(string path) {
  ifstream myfile (path);

  vector<vector<State>> board{};
  if (myfile) { //checks for success.
    string line;
    while (getline(myfile, line)) { //while there is a new row.
      //converts vector of 0,1 into State type 
      vector<State> row = ParseLine(line); 
      board.push_back(row);
    }
  }
  return board;
}

/*
CellString convers corressponding State types into graphical obstacle.
@param cell, it a coordinates of the board in State type.
return graphical representation.
*/
string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    case State::kStart: return "🚦   ";
    case State::kFinish: return "🏁   ";
    default: return "0   "; 
  }
}
/*
PrintBoard Prints the board to screen
@param board, vector of vectors int.
*/
void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}
//###########################################################################
/*
AddToOpen, it adds the x,y,g,h to openlist vector and it closes that coord.
*/
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist, vector<vector<State>> &grid) {
  openlist.push_back(vector<int>{x, y, g, h});
  grid[x][y] = State::kClosed;
}

//calculates the f values of current and possible next step's
//return the comparison
bool Compare(const vector<int> a, const vector<int> b) {
  int f1 = a[2] + a[3]; // f1 = g1 + h1
  int f2 = b[2] + b[3]; // f2 = g2 + h2
  return f1 > f2; 
}
//don't know what the heck is happing here. soon to be learned
void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}
//calculates the Heuristic value.
//returns the h
int Heuristic(int x1, int y1, int x2, int y2) {
  return abs(x2 - x1) + abs(y2 - y1);
}

//checks if potential next step is on the grid and empty.
bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
  bool on_grid_x = (x >= 0 && x < grid.size());
  bool on_grid_y = (y >= 0 && y < grid[0].size());
  if (on_grid_x && on_grid_y)
    return grid[x][y] == State::kEmpty;
  return false;
}
/*
it takes current coord and loops thru its potential next steps. if it does it closes grid on open vector.

*/
void ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist, vector<vector<State>> &grid) {
  // Get current node's data.
  int x = current[0];
  int y = current[1];
  int g = current[2];

  // Loop through current node's potential neighbors.
  for (int i = 0; i < 4; i++) {
    int x2 = x + delta[i][0];
    int y2 = y + delta[i][1];

    // Check that the potential neighbor's x2 and y2 values are on the grid and not closed.
    if (CheckValidCell(x2, y2, grid)) {
      // Increment g value and add neighbor to open list.
      int g2 = g + 1;
      int h2 = Heuristic(x2, y2, goal[0], goal[1]); //Heuristic of the current step.
      AddToOpen(x2, y2, g2, h2, openlist, grid);
    }
  }
}

/*
it takes the grid, initial and goal points, finds the optiminal path to take from
initial to goal.
Returns vector of States graphically represents. 
*/

vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2]){
  
  vector<vector<int>> open {};
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(x, y, goal[0], goal[1]);
  AddToOpen(x, y, g, h, open, grid);

  while(open.size() > 0){
    CellSort(&open);
    auto current = open.back();
    open.pop_back();
    x = current[0];
    y = current[1];
    grid[x][y] = State::kPath;
    
    if(x == goal[0] && y == goal [1]){
      grid[0][0] = State::kStart;
      grid[x][y] = State::kFinish;
      return grid;
    }
    ExpandNeighbors(current, goal, open, grid);
  }
 
  cout << "No Path found!\n";
  return vector<vector<State>> {};
}



int main() {
  //read a vector from chosen file and assign to vector of vector int called board.
  int init[2]{0,0};
  int goal[2]{4,5};
  auto board = ReadBoardFile("/Users/muratyesil/Udacity/c++/A-Search-ObstacleProject/ObstacleProject/src/1.board");
  auto solution = Search(board, init, goal);
  PrintBoard(solution);
}
