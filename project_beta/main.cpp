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

	epsilon = 0;
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
	void max_q_old(agent* plearn);
	void max_q_new(agent* plearn);
	int current_state;
	void prev_state_fxn(agent* plearn);
	int prev_state;
	double q_value;
	double max_qvalue_old = 0;
	double max_qvalue_new = 0;
	double thereward;

	vector<vector<int>> matrix;//gridworld
	vector<vector<int>> tempmatrix; //for looking up state coordinates
	vector<vector<double>> q_table_old;//q table
	vector<vector<double>> q_table_new;
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
	void copy_table();
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
	goalX = row-1;
	goalY = column-1;
	//places goal in grid
	matrix[goalX][goalY] = goal_value;

	reward.at(state.at(tempmatrix[goalX][goalY])) = 100;//sets reward of reaching goal 100

	//cout << " goal state is " << state.at(tempmatrix[goalX][goalY]) << endl;

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
			//cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentX == row - 1) {
		if (plearn->direction == 1) {
			plearn->skip_move = 1;
			//cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentY == 0) {
		if (plearn->direction == 2) {
			plearn->skip_move = 1;
			//cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentY == column - 1) {
		if (plearn->direction == 3) {
			plearn->skip_move = 1;
			//cout << "bumper hit" << endl;
		}
	}
	if (plearn->agentX == 0 && plearn->agentY == 0) {
		if (plearn->direction == 0 || plearn->direction == 2) {
			plearn->skip_move = 1;
			//cout << "corner hit" << endl;
		}
	}
	if (plearn->agentX == row - 1 && plearn->agentY == 0) { //must subtract 1 since matrix starts at 0
		if (plearn->direction == 1 || plearn->direction == 2) {
			plearn->skip_move = 1;
			//cout << "corner hit" << endl;
		}
	}
	if (plearn->agentX == 0 && plearn->agentY == column - 1) {
		if (plearn->direction == 0 || plearn->direction == 3) {
			plearn->skip_move = 1;
			//cout << "corner hit" << endl;
		}
	}
	if (plearn->agentX == row - 1 && plearn->agentY == column - 1) {
		if (plearn->direction == 1 || plearn->direction == 3) {
			plearn->skip_move = 1;
			//cout << "corner hit" << endl;
		}
	}
}

void gridworld::prev_state_fxn(agent* plearn) {
	/*cout << "x=" << plearn->agentY << " y=" << plearn->agentX << endl;
	prev_state = state.at(tempmatrix[plearn->agentX][plearn->agentY]);
	cout << "prev state = " << prev_state << endl;*/

	if (plearn->skip_move == 1) {
		prev_state = current_state;
	}
	else if (plearn->direction == 0) {
		prev_state = state.at(tempmatrix[plearn->agentX+1][plearn->agentY]);
	}
	else if (plearn->direction == 1) {
		prev_state = state.at(tempmatrix[plearn->agentX-1][plearn->agentY]);
	}
	else if (plearn->direction == 2) {
		prev_state = state.at(tempmatrix[plearn->agentX][plearn->agentY+1]);
	}
	else if(plearn->direction == 3) {
		prev_state = state.at(tempmatrix[plearn->agentX][plearn->agentY -1]);
	}
	else {
		//cout << "error in prev state fxn" << endl;
	}
	//cout << "prev state = " << prev_state << endl;
}

//creates initial q table
void gridworld::create_q_table(agent* plearn) {
	int action = 4;//0=up, 1=down, 2=left, 3=right
				   //num of states = row * column
	q_table_old.resize(action);
	q_table_new.resize(action);
	for (int i = 0; i < action; i++) {
		q_table_old[i].resize(state.size());
		q_table_new[i].resize(state.size());
	}

	//initializes each square in q_table to a small number to avoid ties
	for (int a = 0; a < action; a++) {
		for (int b = 0; b < state.size(); b++) {
			//q_table[a][b] = b;
			q_table_old[a][b] = MJRAND*.001;
			q_table_new[a][b] = q_table_old[a][b];
		}
	}

}

void gridworld::show_q_table(agent* plearn) {
	//cout << "state size = " << state.size() << endl;
	//cout << "new q table" << endl;
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < state.size(); b++) {
			cout << q_table_new[a][b] << "\t";
		}
		cout << endl;
	}
}

//updates Q table every move, agent moves first by looking at the q table and then updates q table
//runs previous state function to obtain what previous state was
//called after the agent moves
void gridworld::update_q_table(agent* plearn) {
	/*current_state = state.at(tempmatrix[plearn->agentX][plearn->agentY]);
	cout << "current state = " << current_state << endl;*/
	//q_value = q_table[plearn->direction][current_state];
	//cout << "q_value = " << q_value << endl;
	//cout << "update q direction = " << plearn->direction << endl;

	if (current_state == prev_state) {
		//reward=-1 for hitting bumper
		thereward = -1;
	}
	else if (plearn->agentX == goalX && plearn->agentY == goalY) {
		thereward = goal_reward; //for reaching goal
	}
	else {
		thereward = .001;//for moving to a different state
	}

	//if (plearn->skip_move == 0) {
		q_table_new[plearn->direction][prev_state] = q_table_new[plearn->direction][prev_state] + (plearn->alpha*thereward + plearn->gamma*max_qvalue_new - q_table_new[plearn->direction][prev_state]);
	//}
	//else {
	//	//if hits bumper, q value for that action becomes 0
	//	//top left corner
	//	if (plearn->agentX == 0 && plearn->agentY == 0) {
	//		if (plearn->direction == 0 || plearn->direction == 2) {
	//			q_table_new[0][prev_state] = 0;
	//			q_table_new[2][prev_state] = 0;
	//		}
	//	}
	//	//bottom left corner
	//	if (plearn->agentX == row - 1 && plearn->agentY == 0) { //must subtract 1 since matrix starts at 0
	//		if (plearn->direction == 1 || plearn->direction == 2) {
	//			q_table_new[1][prev_state] = 0;
	//			q_table_new[2][prev_state] = 0;
	//		}
	//	}
	//	//top left corner
	//	if (plearn->agentX == 0 && plearn->agentY == column - 1) {
	//		if (plearn->direction == 0 || plearn->direction == 3) {
	//			q_table_new[0][prev_state] = 0;
	//			q_table_new[3][prev_state] = 0;
	//		}
	//	}
	//	//bottom right corner
	//	if (plearn->agentX == row - 1 && plearn->agentY == column - 1) {
	//		if (plearn->direction == 1 || plearn->direction == 3) {
	//			q_table_new[1][prev_state] = 0;
	//			q_table_new[3][prev_state] = 0;
	//		}
	//	}
	//	q_table_new[plearn->direction][prev_state] = 0;
	//}
		if (max_qvalue_new == q_table_new[0][current_state]) {
			//plearn->direction = 0;
			cout << "new max q dir = 0" << endl;
		}
		else if (max_qvalue_new == q_table_new[1][current_state]) {
			//plearn->direction = 1;
			cout << "new max q dir = 1" << endl;
		}
		else if (max_qvalue_new == q_table_new[0][current_state]) {
			//plearn->direction = 2;
			cout << "new max q dir = 2" << endl;
		}
		else if (max_qvalue_new == q_table_new[0][current_state]) {
			//plearn->direction = 3;
			cout << "new max q dir = 3" << endl;
		}
		else {
			cout << "error in max q new" << endl;
		}
}

//determines max q value for the state http://www.cplusplus.com/forum/general/51452/
//determines what action/direction associated with the max q
void gridworld::max_q_old(agent* plearn) {
	int num_actions = 4;
	vector<double> temp(4,0);
	max_qvalue_old = 0;
	//int temp = 1;
	//current_state = state.at(tempmatrix[plearn->agentY][plearn->agentX]);//coordinates flipped due to the state equation

	for (int q = 0; q < 4; q++) {
		temp.at(q) = q_table_old[q][prev_state];
	}
	/*cout << "old vector" << endl;
	for (int z = 0; z < temp.size(); z++) {
		cout << temp.at(z) << "\t";
	}
	cout << endl;*/

	for (int i = 0; i < num_actions; i++) {
		if (temp.at(i) > max_qvalue_old) {
			max_qvalue_old = temp.at(i);
			//temp = i;
		}
	}
	//plearn->direction = temp;
	cout << "max q old = " << max_qvalue_old << endl;


	if (max_qvalue_old == q_table_old[0][prev_state]) {
		plearn->direction = 0;
		cout << "old max q dir = 0" << endl;
	}
	else if (max_qvalue_old == q_table_old[1][prev_state]) {
		plearn->direction = 1;
		cout << "old max q dir = 1" << endl;
	}
	else if (max_qvalue_old == q_table_old[2][prev_state]) {
		plearn->direction = 2;
		cout << "old max q dir = 2" << endl;
	}
	else if (max_qvalue_old == q_table_old[3][current_state]) {
		plearn->direction = 3;
		cout << "old max q dir = 3" << endl;
	}
	else {
		cout << "error in max q" << endl;
	}
}

//already copied old to new in create q table
void gridworld::copy_table() {
	assert(q_table_old.size() == q_table_new.size());

	for (int a = 0; a < q_table_old.size(); a++) {
		for (int b = 0; b < q_table_old.size(); b++) {
			q_table_old[a][b] = q_table_new[a][b];
		}
	}
}

void gridworld::max_q_new(agent* plearn) {
	int num_actions = 4;
	//int temp = 1;
	//current_state = state.at(tempmatrix[plearn->agentY][plearn->agentX]);//coordinates flipped due to the state equation
	vector<double> temp1(4,0);
	max_qvalue_new=0;
	for (int q = 0; q < 4; q++) {
		temp1.at(q) = q_table_new[q][current_state];
	}
	//cout << "new vector" << endl;
	/*for (int z = 0; z < temp1.size(); z++) {
		cout << temp1.at(z) << "\t";
	}*/

	for (int i = 0; i < num_actions; i++) {
		if (temp1.at(i) > max_qvalue_new) {
			max_qvalue_new = temp1.at(i);
			//temp = i;
		}
	}
	//plearn->direction = temp;
	cout << "new max q = " << max_qvalue_new << endl;
	//cout << "current state max q = " << current_state << endl;
	//if (max_qvalue_new == q_table_new[0][current_state]) {
	//	//plearn->direction = 0;
	//	cout << "new max q dir = 0" << endl;
	//}
	//else if (max_qvalue_new == q_table_new[1][current_state]) {
	//	//plearn->direction = 1;
	//	cout << "new max q dir = 1" << endl;
	//}
	//else if (max_qvalue_new == q_table_new[0][current_state]) {
	//	//plearn->direction = 2;
	//	cout << "new max q dir = 2" << endl;
	//}
	//else if (max_qvalue_new == q_table_new[0][current_state]) {
	//	//plearn->direction = 3;
	//	cout << "new max q dir = 3" << endl;
	//}
	//else {
	//	cout << "error in max q new" << endl;
	//}
}
//decides what direction to pick 
void decide(agent* plearn, gridworld* pmap) {
	if (plearn->choice == 0) {
		plearn->direction = rand() % 4;
		//cout << "explore" << endl;
		cout << "explore direction = " << plearn->direction << endl;
	}
	else {
		pmap->max_q_old(plearn);
		//cout << "greedy" << endl;
	}
}

//void testD(gridworld* pmap) {
//	for (int a = 0; a < pmap->q_table_new.size(); a++) {
//		if (pmap->goal_reward < pmap->q_table_new.at(a)) {
//			cout << "large" << endl;
//		}
//	}
//}

void testE(agent* plearn, gridworld* pmap) {
	assert(plearn->agentX == 0);
	assert(plearn->agentY == 0);
	
}

void testF(agent* plearn, gridworld* pmap) {
	int min_moves;
	min_moves = 1;
	
}

void testG() {

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

	grid.prev_state = 0;

	for (int n = 0; n < 1; n++) {
		stop = 0;
		num_moves = 0;
		smith.agentX = 0;
		smith.agentY = 0;
		smith.epsilon = .1;
		while (stop < 1) {
			//cout << endl;
			//cout << endl;
			cin >> pick_test;
			//grid.current_state = grid.prev_state;
			smith.explore_or_greedy();//decides to do random or greedy
			decide(psmith, pgrid);//picks direction to move
			grid.bumper_check(psmith);//checks if the move will hit bumper
			smith.move();//moves the agent
			//cout << "move has been made" << endl;
			//cin >> pick_test;
			grid.prev_state_fxn(psmith);
			grid.current_state = grid.state.at(grid.tempmatrix[smith.agentX][smith.agentY]);
			cout << "current state = " << grid.current_state << endl;
			grid.max_q_new(psmith);
			//cout << "max q new = "<<grid.max_qvalue_new << endl;
			grid.update_q_table(psmith);//updates q table based on move
			grid.copy_table();
			grid.show_q_table(psmith);
			grid.clear_grid(psmith);
			//cout << endl;
			grid.show_grid();
			smith.epsilon_decay();
			//cout << endl;
			//cout << "old q table" << endl;
			/*for (int a = 0; a < 4; a++) {
				for (int b = 0; b < grid.state.size(); b++) {
					cout << grid.q_table_old[a][b] << "\t";
				}
				cout << endl;
			}*/
			//cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
			//cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
			if (smith.agentX == grid.goalX && smith.agentY == grid.goalY) {
				stop = 1000;
				cout << "Goal Reached" << endl;
			}
			num_moves++;
			//cout << num_moves << endl;
		}
		vstep.push_back(num_moves);
		//cout << "moves = " << num_moves << endl;
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