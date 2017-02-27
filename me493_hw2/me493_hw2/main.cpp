#include <iostream>
#include <assert.h>
#include <random>
#include <time.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <limits>

using namespace std;

class agent {
public:
	int agentX;
	int agentY;
	int direction;
	int agent_value = 1;//for displaying agent on grid
	int place_agent[2] = { agentY,agentX };//matrix containing location of agent
	int skip_move;// for when agent hits bumper, only moves if = 0

	void init();
	void move();
};

class gridworld {
public:

	int row;//x, refers to second element of matrix
	int column;//y, refers to fist element of matrix
	int goalX;
	int goalY;
	int goal_value = 5;//for displaying goal on grid

	vector<vector<int>> matrix;

	void init();
	void clear_grid(agent* plearn);
	void show_grid();
	void bumper_check(agent* plearn);
};

void agent::init() {
	agentX = 0; //x location
	agentY = 0;//y location
	place_agent[1] = agentX;
	place_agent[0] = agentY;
	agent_value = 1;//represents where agent is when matrix is shown
}

void agent::move() {
	cout << "skip move = " << skip_move << endl;
	if (skip_move == 0) {
		if (direction == 1) {
			//up
			agentX = agentX - 1;
		}
		else if (direction == 2) {
			//down
			agentX = agentX + 1;
		}
		else if (direction == 3) {
			//left
			agentY = agentY - 1;
		}
		else if (direction == 4) {
			//right
			agentY = agentY + 1;
		}
		else {
			cout << "invalid move" << endl;
		}
	}
}


void gridworld::init() {
	cout << "# of rows" << endl;
	cin >> row;
	cout << "# of columns" << endl;
	cin >> column;
	//row = 3;
	//column = 3;

	//http://stackoverflow.com/questions/1403150/how-do-you-dynamically-allocate-a-matrix
	//for creating the matrix/grid

	matrix.resize(row);
	for (int i = 0; i < row; i++) {
		matrix[i].resize(column);
	}

	//every element filled to 0, 0 represents empty square
	for (int o = 0; o < row; o++) {
		for (int p = 0; p < column; p++) {
			matrix[o][p] = 0;
		}
	}

	goalX = rand() % row;
	goalY = rand() % column;
	//goalX = 1;
	//goalY = 1;

	matrix[goalX][goalY] = goal_value;
}

//displays grid on console
void gridworld::show_grid() {
	for (int a = 0; a < row; a++) {
		for (int b = 0; b < column; b++) {
			cout << matrix[a][b] << "\t";
		}
		cout << endl;
	}

}

//clears grid of previous movement, have to call before showing grid
void gridworld::clear_grid(agent* plearn) {
	for (int a = 0; a < row; a++) {
		for (int b = 0; b < column; b++) {
			matrix[a][b] = 0;
		}
	}
	matrix[goalX][goalY] = goal_value;
	matrix[plearn->agentX][plearn->agentY] = plearn->agent_value;

}

//checks if agents hits edge of grid
void gridworld::bumper_check(agent* plearn) {
	plearn->skip_move = 0;
	if (plearn->agentX == 0) {
		if (plearn->direction == 1) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentX == row - 1) {
		if (plearn->direction == 2) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentY == 0) {
		if (plearn->direction == 3) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentY == column - 1) {
		if (plearn->direction == 4) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentX == 0 && plearn->agentY == 0) {
		if (plearn->direction == 1 || plearn->direction == 3) {
			plearn->skip_move = 1;
		}
	}
	if (plearn->agentX == row - 1 && plearn->agentY == 0) { //must subtract 1 since matrix starts at 0
		if (plearn->direction == 2 || plearn->direction == 3) {
			plearn->skip_move = 1;
		}
	}
	if (plearn->agentX == 0 && plearn->agentY == column - 1) {
		if (plearn->direction == 1 || plearn->direction == 4) {
			plearn->skip_move = 1;
		}
	}
	if (plearn->agentX == row - 1 && plearn->agentY == column - 1) {
		if (plearn->direction == 2 || plearn->direction == 4) {
			plearn->skip_move = 1;
		}
	}
}

//places agent at 0,0 if starts out of bounds
void testA(agent* plearn, gridworld* pmap) {
	if (plearn->agentX > pmap->row && plearn->agentY > pmap->column) {
		plearn->agentX = 0;
		plearn->agentY = 0;
	}
}

//allows for human input
void testB(agent* plearn) {
	cout << endl;
	cout << "up = 1, down = 2, left = 3, right = 4" << endl;
	cout << "input direction" << endl;
	cin >> plearn->direction;
}

//autonomous movement
void testC(agent* plearn, gridworld* pmap) {
	if (plearn->agentX < pmap->goalX) {
		plearn->direction = 2;
		cout << "down" << endl;
	}
	else if (plearn->agentX > pmap->goalX) {
		plearn->direction = 1;
		cout << "up" << endl;
	}
	else if (plearn->agentY < pmap->goalY) {
		plearn->direction = 4;
		cout << "right" << endl;
	}
	else if (plearn->agentY > pmap->goalY) {
		plearn->direction = 3;
		cout << "left" << endl;
	}
	else {
		cout << "error" << endl;
	}
}

int main() {
	srand(time(NULL));
	gridworld grid;
	agent smith;
	agent* psmith = &smith;
	gridworld* pgrid = &grid;
	grid.init();
	smith.init();
	testA(psmith, pgrid);
	grid.clear_grid(psmith);
	int stop = 0; //break out of loop when goal reached
				  //int m;
	int pick_test;

	grid.show_grid();
	cout << "value of 1 = agent's location	value of 5 = goal's location" << endl;
	cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
	cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
	cout << "Pick a test to run: Test B=1, Test C=2" << endl;
	cin >> pick_test;
	if (pick_test == 1) {
		while (stop < 1) {
			testB(psmith);
			grid.bumper_check(psmith);
			smith.move();
			grid.clear_grid(psmith);
			grid.show_grid();
			cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
			cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
			if (smith.agentX == grid.goalX && smith.agentY == grid.goalY) {
				stop = 1000;
				cout << "Goal Reached" << endl;
			}
		}
	}
	else if (pick_test == 2) {
		while (stop < 1) {
			testC(psmith, pgrid);
			grid.bumper_check(psmith);
			smith.move();
			grid.clear_grid(psmith);
			grid.show_grid();
			cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
			cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
			if (smith.agentX == grid.goalX && smith.agentY == grid.goalY) {
				stop = 1000;
				cout << "Goal Reached" << endl;
			}
		}
	}
	else {
		cout << "invalid test" << endl;
	}

	return 0;
}