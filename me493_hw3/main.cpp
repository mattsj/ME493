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
	int prev_X;
	int prev_Y;
	int direction;
	int new_direction;
	int agent_value = 1;//for displaying agent on grid
	int place_agent[2] = { agentY,agentX };//matrix containing location of agent
	int skip_move;// for when agent hits bumper, only moves if = 0
	int choice;//tells agent to explore or greedy 0=random, 1=greedy
			   //int num_moves = 0;
	int num_moves;


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

	alpha = .1;
	gamma = .9;
}

void agent::move() {
	prev_X = agentX;
	prev_Y = agentY;
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
	int wall_state;
	void max_q(agent* plearn);
	void max_qnew(agent* plearn);
	int current_state;
	void current_state_fxn(agent* plearn);
	int prev_state;
	void prev_state_fxn(agent* plearn);
	double q_value;
	double max_qvalue;
	double max_qvalue_new;
	double thereward;
	int wall_hit;

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

	void create_wall();
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
	goalX = row - 1;
	goalY = column - 1;
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
//also acts as wall check
void gridworld::bumper_check(agent* plearn) {
	plearn->skip_move = 0;
	wall_hit = 0;
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

	//wall
	if (current_state == wall_state) {
		cout << "wall" << endl;
		current_state = prev_state;
		plearn->agentX = plearn->prev_X;
		plearn->agentY = plearn->prev_Y;
		//cout << "curr bump = " << current_state << endl;
		wall_hit = 1;
		plearn->skip_move = 1;
	}
}

void gridworld::current_state_fxn(agent* plearn) {
	current_state = state.at(tempmatrix[plearn->agentX][plearn->agentY]);
	//cout << "current state = " << current_state << endl;
}

void gridworld::prev_state_fxn(agent* plearn) {
	prev_state = state.at(tempmatrix[plearn->agentX][plearn->agentY]);
	//cout << "prev state = " << prev_state << endl;
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
	if (wall_hit == 1) {
		thereward = -100;
	}
	else if (current_state == prev_state) {
		//reward=-1 for hitting bumper
		thereward = -100;
	}
	else if (plearn->agentX == goalX && plearn->agentY == goalY) {
		thereward = goal_reward; //for reaching goal
	}
	else {
		thereward = -.001;//for moving to a different state
	}

	q_table[plearn->direction][prev_state] = q_table[plearn->direction][prev_state] + (plearn->alpha*thereward + plearn->gamma*max_qvalue - q_table[plearn->direction][prev_state]);
}

//determines max q value for the state http://www.cplusplus.com/forum/general/51452/
//determines what action/direction associated with the max q
void gridworld::max_q(agent* plearn) {
	int num_actions = 4;
	vector<double> temp(4, 0);
	max_qvalue = 0;
	for (int q = 0; q < 4; q++) {
		temp.at(q) = q_table[q][prev_state];
	}

	for (int i = 0; i < num_actions; i++) {
		if (temp.at(i) > max_qvalue) {
			max_qvalue = temp.at(i);
		}
	}
	//cout << "max q = " << max_qvalue << endl;

	if (max_qvalue == q_table[0][prev_state]) {
		plearn->direction = 0;
		//cout << "max q dir = 0" << endl;
	}
	else if (max_qvalue == q_table[1][prev_state]) {
		plearn->direction = 1;
		//cout << "max q dir = 1" << endl;
	}
	else if (max_qvalue == q_table[2][prev_state]) {
		plearn->direction = 2;
		//cout << "max q dir = 2" << endl;
	}
	else if (max_qvalue == q_table[3][prev_state]) {
		plearn->direction = 3;
		//cout << "max q dir = 3" << endl;
	}
	else {
		//cout << "error in max q" << endl;
	}
}

void gridworld::max_qnew(agent* plearn) {
	vector<double> temp1(4, 0);
	max_qvalue_new = 0;
	for (int a = 0; a < temp1.size(); a++) {
		temp1.at(a) = q_table[a][current_state];
	}
	if (current_state == prev_state) {
		q_table[plearn->direction][current_state] = -1;
		temp1.at(plearn->direction) = q_table[plearn->direction][current_state];
	}
	for (int i = 0; i < 4; i++) {
		if (temp1.at(i) > max_qvalue_new) {
			max_qvalue_new = temp1.at(i);
		}
	}
	//cout << "max q new = " << max_qvalue_new << endl;
	//cout << "s0 = " << q_table[0][current_state] << " s1 = " << q_table[1][current_state] << " s2 = " << q_table[2][current_state] << " s3 = " << q_table[3][current_state] << endl;
	if (max_qvalue_new == q_table[0][current_state]) {
		plearn->new_direction = 0;
		//cout << "new max q dir = 0" << endl;
	}
	else if (max_qvalue_new == q_table[1][current_state]) {
		plearn->new_direction = 1;
		//cout << "new max q dir = 1" << endl;
	}
	else if (max_qvalue_new == q_table[2][current_state]) {
		plearn->new_direction = 2;
		// << "new max q dir = 2" << endl;
	}
	else if (max_qvalue_new == q_table[3][current_state]) {
		plearn->new_direction = 3;
		//cout << "new max q dir = 3" << endl;
	}
	else {
		//cout << "error in new max q" << endl;
	}
}

void gridworld::create_wall() {
	//location of wall
	int goal_state = state.at(tempmatrix[goalX][goalY]);

	wall_state = goal_state-1;//puts wall to left of goal
	cout << "wall state = " << wall_state << endl;
}

//for first move
void fdecide(agent* plearn, gridworld* pmap) {
	if (plearn->choice == 0) {
		plearn->direction = rand() % 4;
		//cout << "explore" << endl;
		//cout << "explore direction = " << plearn->direction << endl;
	}
	else {
		pmap->max_q(plearn);
		//cout << "greedy" << endl;
	}
}


//decides what direction to pick 
void decide(agent* plearn, gridworld* pmap) {
	if (plearn->choice == 0) {
		plearn->direction = rand() % 4;
		//cout << "explore" << endl;
		//cout << "explore direction = " << plearn->direction << endl;
	}
	else {
		plearn->direction = plearn->new_direction;
		//cout << "greedy" << endl;
	}
}

//heuristic 
void testA(agent* plearn, gridworld* pmap) {
	if (plearn->agentX < pmap->goalX) {
		plearn->direction = 1;
		//cout << "down" << endl;
	}
	else if (plearn->agentX > pmap->goalX) {
		plearn->direction = 2;
		//cout << "up" << endl;
	}
	else if (plearn->agentY < pmap->goalY) {
		plearn->direction = 4;
		//cout << "right" << endl;
	}
	else if (plearn->agentY > pmap->goalY) {
		plearn->direction = 3;
		//cout << "left" << endl;
	}
	else {
		cout << "error" << endl;
	}
}

//Q-learner
void testB(agent* plearn, gridworld* pmap) {
	assert(plearn->agentX == pmap->goalX && plearn->agentY == pmap->goalY);
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
			  //int num_moves;
	grid.create_q_table(psmith);
	//grid.show_q_table(psmith);

	int pick_test;
	int hold;
	smith.num_moves = 0;

//	grid.show_grid();

	vector<int> vstep;

	for (int n = 0; n < 1; n++) {
		cout << "input: test A = 1, test B =2" << endl;
		cin >> pick_test;

		if (pick_test == 1) {
			stop = 0;
			while (stop < 1) {
				//cin >> hold;
				grid.prev_state = grid.current_state;
				grid.create_wall();
				testA(psmith, pgrid);
				grid.bumper_check(psmith);
				smith.move();
				//grid.current_state_fxn(psmith);
				smith.num_moves++;
				grid.clear_grid(psmith);
				//grid.show_grid();
				if (smith.num_moves >100) {
					stop = 2000;
					cout << "heuristic unable to find goal" << endl;
				}
			}
			//grid.show_grid();
		}
		else if (pick_test == 2) {
			grid.create_wall();
			stop = 0;
			smith.num_moves = 0;
			smith.agentX = 0;
			smith.agentY = 0;
			grid.prev_state_fxn(psmith);
			grid.current_state = 0;
			smith.epsilon = .25;
			smith.explore_or_greedy();
			fdecide(psmith, pgrid);
			grid.bumper_check(psmith);//checks if the move will hit bumper
			grid.prev_state = grid.current_state;
			smith.move();//moves the agent
			grid.clear_grid(psmith);
			//grid.show_grid();
			grid.current_state_fxn(psmith);
			grid.max_qnew(psmith);
			grid.update_q_table(psmith);//updates q table based on move
			//grid.show_q_table(psmith);
			//smith.epsilon_decay();
			smith.num_moves++;

			while (stop < 1) {

				//grid.current_state_fxn(psmith);
				smith.explore_or_greedy();//decides to do random or greedy
				decide(psmith, pgrid);//picks direction to move
				grid.bumper_check(psmith);//checks if the move will hit bumper
				grid.prev_state = grid.current_state;
				smith.move();//moves the agent
				grid.clear_grid(psmith);
				//grid.show_grid();
				grid.current_state_fxn(psmith);
				grid.max_qnew(psmith);
				grid.update_q_table(psmith);//updates q table based on move
				//grid.show_q_table(psmith);
				//smith.epsilon_decay();
				//cout << "epsilon = " << smith.epsilon << endl;
				//cin >> hold;
				//cout << endl;
				//cout << "agent X pos = " << smith.agentX << "  agent Y pos = " << smith.agentY << endl;
				//cout << "goal X pos = " << grid.goalX << "  goal Y pos = " << grid.goalY << endl;
				if (smith.agentX == grid.goalX && smith.agentY == grid.goalY) {
					testB(psmith, pgrid);
					stop = 1000;
					cout << "Goal Reached" << endl;
				}
				smith.num_moves++;
			}
		}
		else {
			cout << "not a valid test" << endl;
		}

	}
		//grid.show_q_table(psmith);
		//vstep.push_back(smith.num_moves);
		//cout << "moves = " << num_moves << endl;
	grid.show_q_table(psmith);

	return 0;
}