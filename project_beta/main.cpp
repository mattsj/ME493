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
#include <climits>

#define MJRAND (double)rand()/RAND_MAX

using namespace std;

class agent {
public:
	int agentX;
	int agentY;
	int direction;
	int agent_value = 1;//for displaying agent on grid
	int place_agent[2] = { agentY,agentX };//matrix containing location of agent
	int skip_move;// for when agent hits bumper, only moves if = 0
	int choice;//tells agent to explore or greedy 0=random, 1=greedy
	int num_moves = 0;


	double epsilon;//exploration rate
	double alpha;//learning rate
	double gamma;//discount rate

	void init();
	void move();
	void explore_or_greedy();
	void epsilon_decay();

};

void agent::init() {
	agentX = 0; //x location
	agentY = 0;//y location
	place_agent[1] = agentX;
	place_agent[0] = agentY;
	agent_value = 1;//represents where agent is when matrix is shown

	epsilon = .1;
	alpha = .1;
	gamma = .9;
}

void agent::move() {
	//agent will only move if skip_move = 0
	if (skip_move == 0) {
		if (direction == 0) {
			//up
			agentX = agentX - 1;
		}
		else if (direction == 1) {
			//down
			agentX = agentX + 1;
		}
		else if (direction == 2) {
			//left
			agentY = agentY - 1;
		}
		else if (direction == 3) {
			//right
			agentY = agentY + 1;
		}
		else {
			cout << "invalid move" << endl;
		}
	}
}

void agent::explore_or_greedy() {
	//cout << "num = " << number << endl;
	if (epsilon >= 1 - MJRAND) {
		choice = 0;//explore
	}
	else {
		choice = 1;//greedy
	}
}

//exponential decay formula, agent gains more knowledge
void agent::epsilon_decay() {
	double lambda = .01;
	double e = 2.71828;
	epsilon = epsilon * pow(e, -lambda*num_moves);
}




class gridworld {
public:

	int row;//x, refers to second element of matrix
	int column;//y, refers to fist element of matrix
	int goalX;
	int goalY;
	int goal_value = 5;//for displaying goal on grid
	int goal_reward = 100;//reward for reaching the goal
	int grid_number;
	int rewardX;
	int rewardY;
	void max_q(agent* plearn);
	int current_state;
	void current_state_fxn(agent* plearn);
	int prev_state;
	double q_value;
	double max_qvalue = 0;

	vector<vector<int>> matrix;//gridworld
	vector<vector<int>> tempmatrix; //for looking up state coordinates
	vector<vector<double>> q_table;//q table
	vector<vector<int>> tempreward;
	vector<int> reward;
	vector<int> state;

	void init();
	void clear_grid(agent* plearn);
	void show_grid();
	void bumper_check(agent* plearn);

	void create_q_table(agent* plearn);
	void update_q_table(agent* plearn);
	void show_q_table(agent* plearn);
};

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
	tempmatrix.resize(row);
	tempreward.resize(row);
	for (int i = 0; i < row; i++) {
		matrix[i].resize(column);
		tempmatrix[i].resize(column);
		tempreward[i].resize(column);
	}


	//matrix initialized
	for (int o = 0; o < row; o++) {
		for (int p = 0; p < column; p++) {
			matrix[o][p] = 0;
			tempmatrix[o][p] = p + (o*column);
			state.push_back(tempmatrix[o][p]); //use vector to rename pair of coordinates to integers, starts at 0

			tempreward[o][p] = 0;
			reward.push_back(tempreward[o][p]);

		}
	}


	/*for (int r = 0; r < state.size(); r++) {
	cout << state.at(r) << endl;
	}*/


	//creates location of the goal
	//goalX = rand() % row;
	//goalY = rand() % column;
	goalX = 1;
	goalY = 1;
	//places goal in grid
	matrix[goalX][goalY] = goal_value;

	reward.at(state.at(tempmatrix[goalX][goalY])) = 100;//sets reward of reaching goal 100

	cout << " goal state is " << state.at(tempmatrix[goalX][goalY]) << endl;

	/*for (int r = 0; r < reward.size(); r++) {
	cout << reward.at(r) << endl;
	}*/

}

//displays grid on console
void gridworld::show_grid() {
	for (int a = 0; a < row; a++) {
		for (int b = 0; b < column; b++) {
			cout << matrix[a][b] << "\t";
			//cout << grid_number << "\t";
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
		if (plearn->direction == 0) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentX == row - 1) {
		if (plearn->direction == 1) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentY == 0) {
		if (plearn->direction == 2) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentY == column - 1) {
		if (plearn->direction == 3) {
			plearn->skip_move = 1;
			cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentX == 0 && plearn->agentY == 0) {
		if (plearn->direction == 0 || plearn->direction == 2) {
			plearn->skip_move = 1;
			cout << "corner hit" << endl;
		}
	}
	if (plearn->agentX == row - 1 && plearn->agentY == 0) { //must subtract 1 since matrix starts at 0
		if (plearn->direction == 1 || plearn->direction == 2) {
			plearn->skip_move = 1;
			cout << "corner hit" << endl;
		}
	}
	if (plearn->agentX == 0 && plearn->agentY == column - 1) {
		if (plearn->direction == 0 || plearn->direction == 3) {
			plearn->skip_move = 1;
			cout << "corner hit" << endl;
		}
	}
	if (plearn->agentX == row - 1 && plearn->agentY == column - 1) {
		if (plearn->direction == 1 || plearn->direction == 3) {
			plearn->skip_move = 1;
			cout << "corner hit" << endl;
		}
	}
}

void gridworld::current_state_fxn(agent* plearn) {
	cout << "x=" << plearn->agentX << " y=" << plearn->agentY << endl;
	current_state = state.at(tempmatrix[plearn->agentY][plearn->agentX]);
	cout << "current state = " << current_state << endl;
}

//creates initial q table
void gridworld::create_q_table(agent* plearn) {
	int action = 4;//0=up, 1=down, 2=left, 3=right
				   //num of states = row * column
	q_table.resize(action);
	for (int i = 0; i < action; i++) {
		q_table[i].resize(state.size());
	}

	//initializes each square in q_table to a small number to avoid ties
	for (int a = 0; a < action; a++) {
		for (int b = 0; b < state.size(); b++) {
			//q_table[a][b] = b;
			q_table[a][b] = MJRAND*.001;
		}
	}

}

void gridworld::show_q_table(agent* plearn) {
	//cout << "state size = " << state.size() << endl;
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < state.size(); b++) {
			cout << q_table[a][b] << "\t";
		}
		cout << endl;
	}
}

//updates Q table every move, agent moves first by looking at the q table and then updates q table
//called after the agent moves
void gridworld::update_q_table(agent* plearn) {
	//current_state = state.at(tempmatrix[plearn->agentY][plearn->agentX]);//coordinates flipped due to the state equation
	cout << "update ax = " << plearn->agentX << "  update ay = " << plearn->agentY << endl;
	prev_state = state.at(tempmatrix[plearn->agentY][plearn->agentX]);//coordinates flipped due to the state equation
	cout << "prev state = " << prev_state << endl;
	//q_value = q_table[plearn->direction][current_state];
	//cout << "q_value = " << q_value << endl;
	cout << "update q direction = " << plearn->direction << endl;

	if (plearn->skip_move == 0) {
		q_table[plearn->direction][prev_state] = q_table[plearn->direction][prev_state] + (plearn->alpha*reward.at(current_state) + plearn->gamma*max_qvalue - q_table[plearn->direction][prev_state]);
	}
	else {
		//if hits bumper, q value for that action becomes -1
		//top left corner
		if (plearn->agentX == 0 && plearn->agentY == 0) {
			if (plearn->direction == 0 || plearn->direction == 2) {
				q_table[0][prev_state] = -1;
				q_table[2][prev_state] = -1;
			}
		}
		//bottom left corner
		if (plearn->agentX == row - 1 && plearn->agentY == 0) { //must subtract 1 since matrix starts at 0
			if (plearn->direction == 1 || plearn->direction == 2) {
				q_table[1][prev_state] = -1;
				q_table[2][prev_state] = -1;
			}
		}
		//top left corner
		if (plearn->agentX == 0 && plearn->agentY == column - 1) {
			if (plearn->direction == 0 || plearn->direction == 3) {
				q_table[0][prev_state] = -1;
				q_table[3][prev_state] = -1;
			}
		}
		//bottom right corner
		if (plearn->agentX == row - 1 && plearn->agentY == column - 1) {
			if (plearn->direction == 1 || plearn->direction == 3) {
				q_table[1][prev_state] = -1;
				q_table[3][prev_state] = -1;
			}
		}
		q_table[plearn->direction][prev_state] = -1;
	}
}

//determines max q value for the state http://www.cplusplus.com/forum/general/51452/
//determines what action/direction associated with the max q
void gridworld::max_q(agent* plearn) {
	int num_actions = 4;
	//int temp = 1;
	//current_state = state.at(tempmatrix[plearn->agentY][plearn->agentX]);//coordinates flipped due to the state equation
	//double array[4] = { };
	//array[4] = { q_table[0][current_state],q_table[1][current_state],q_table[2][current_state],q_table[3][current_state] };
	vector<double> temp;
	temp.push_back(q_table[0][current_state]);
	temp.push_back(q_table[1][current_state]);
	temp.push_back(q_table[2][current_state]);
	temp.push_back(q_table[3][current_state]);

	for (int i = 0; i < num_actions; i++) {
		if (temp.at(i) > max_qvalue) {
			max_qvalue = temp.at(i);
			//temp = i;
		}
	}
	//plearn->direction = temp;
	cout << "max q = " << max_qvalue << endl;


	if (max_qvalue == q_table[0][current_state]) {
		plearn->direction = 0;
		cout << "max q dir = 0" << endl;
	}
	else if (max_qvalue == q_table[1][current_state]) {
		plearn->direction = 1;
		cout << "max q dir = 1" << endl;
	}
	else if (max_qvalue == q_table[2][current_state]) {
		plearn->direction = 2;
		cout << "max q dir = 2" << endl;
	}
	else if (max_qvalue == q_table[3][current_state]) {
		plearn->direction = 3;
		cout << "max q dir = 3" << endl;
	}
	else {
		cout << "error in max q" << endl;
	}
}

//decides what direction to pick 
void decide(agent* plearn, gridworld* pmap) {
	if (plearn->choice == 0) {
		plearn->direction = rand() % 4;
		cout << "explore" << endl;
		cout << "explore direction = " << plearn->direction << endl;
	}
	else {
		pmap->max_q(plearn);
		cout << "max q 1111111" << endl;
		cout << "greedy" << endl;
	}
}



int main() {
	srand(time(NULL));
	gridworld grid;
	agent smith;
	agent* psmith = &smith;
	grid.init();
	smith.init();
	//creates new goal if goal and agent start at the same spot
	while (smith.agentX == grid.goalX && smith.agentY == grid.goalY) {
		cout << "goal and agent at same spot" << endl;
		cout << "enter matrix size again" << endl;
		grid.init();
	}
	gridworld* pgrid = &grid;
	grid.clear_grid(psmith);
	int stop; //break out of loop when goal reached
	int num_moves;
	grid.create_q_table(psmith);
	grid.show_q_table(psmith);

	int pick_test;

	grid.show_grid();

	vector<int> vstep;

	for (int n = 0; n < 50; n++) {
		stop = 0;
		num_moves = 0;
		while (stop < 1) {
			cout << endl;
			cout << endl;
			grid.current_state_fxn(psmith);
			smith.explore_or_greedy();//decides to do random or greedy
			decide(psmith, pgrid);//picks direction to move
			grid.bumper_check(psmith);//checks if the move will hit bumper
			smith.move();//moves the agent
			cout << "move has been made" << endl;
			cin >> pick_test;
			grid.max_q(psmith);
			cout << "max q 2222222" << endl;
			grid.update_q_table(psmith);//updates q table based on move
			grid.show_q_table(psmith);
			grid.clear_grid(psmith);
			grid.show_grid();
			cout << endl;
			//cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
			//cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
			if (smith.agentX == grid.goalX && smith.agentY == grid.goalY) {
				stop = 1000;
				cout << "Goal Reached" << endl;
			}
			num_moves++;
		}
		vstep.push_back(num_moves);
		cout << "moves = " << num_moves << endl;
	}


	//cout << "value of 1 = agent's location	value of 5 = goal's location" << endl;
	//cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
	//cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
	//cout << "Pick a test to run: Test A=0, B=1, Test C=2" << endl;
	//cin >> pick_test;

	ofstream project_beta;
	project_beta.open("project_beta.txt");
	for (int m = 0; m < vstep.size(); m++) {
		project_beta << vstep.at(m) << endl;
	}
	project_beta.close();

	return 0;
}