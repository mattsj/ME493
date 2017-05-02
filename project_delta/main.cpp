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
#include "LY_NN.h"

#define MJRAND (double)rand()/RAND_MAX
#define pi 3.14159265

using namespace std;

class ship {
public:
	double velocity;
	double timestep;
	double T;
	double control_signal; //radians per second
	double xpos;
	double prev_xpos;
	double ypos;
	double prev_ypos;
	double theta;//radians
	double prev_theta;
	double omega;//radians/s
	double prev_omega;
	double dist;

	double startx;
	double starty;
	double start_theta;
	double start_omega;

	void init();
	void calc_x_pos();
	void calc_y_pos();
	void calc_theta();
	void calc_omega();
	void simulate();
};

void ship::init() {
	velocity = 3;
	timestep = .2;
	T = 5;
	int max = 15;
	int min = -15;
	control_signal = (MJRAND*(max - min) + min)*pi / 180;

	int grid_max = 1000;
	int grid_min = 0;
	//xpos = MJRAND*(grid_max - grid_min) + grid_min;
	//ypos = MJRAND*(grid_max - grid_min) + grid_min;
	xpos = 100;
	ypos = 100;
	startx = xpos;
	starty = ypos;
	double theta_max = 360 * pi / 180;
	double theta_min = 0;
	theta = MJRAND*(theta_max - theta_min) + theta_min;
	//theta = pi / 4;
	start_theta = theta;
	int omega_max = 10;
	int omega_min = 1;
	omega = MJRAND*(omega_max - omega_min) + omega_min;
	start_omega = omega;
	
}

void ship::calc_x_pos() {
	prev_xpos = xpos;
	xpos = xpos + (velocity*sin(theta)*timestep);
}

void ship::calc_y_pos() {
	prev_ypos = ypos;
	ypos = ypos + (velocity*cos(theta)*timestep);
}

void ship::calc_theta() {
	prev_theta = theta;
	theta = theta + (omega*timestep);
}

void ship::calc_omega() {
	prev_omega = omega;
	omega = omega + ((control_signal - omega)*timestep / T);
}

void ship::simulate() {
	calc_omega();
	calc_theta();
	calc_x_pos();
	calc_y_pos();
}




class policy {
public:
	double fitness;
	vector<double> weights;//weights for the boat
	vector<double> pathx;//boat's x coordinates are at each move
	vector<double> pathy;//boat's y coordinates are at each move

	double xpos;
	double ypos;
	double theta;
	double omega;

	void init(ship* pboat);
	void reset();

};

void policy::init(ship* pboat) {
	xpos = pboat->startx;
	ypos = pboat->starty;
	theta = pboat->start_theta;
	omega = pboat->start_omega;
}

void policy::reset() {
	pathx.clear();
	pathy.clear();
	weights.clear();
}






class goal {
public:
	double goal_x1;
	double goal_y1;
	double goal_x2;
	double goal_y2;
	double goal_line;

	void create_goal(int grid_min,int grid_max);
};

void goal::create_goal(int min, int max) {
	goal_x1 = 1000;
	goal_y1 = 900;
	goal_x2 = 900;
	goal_y2 = 1000;
}





//checks if the boat goes outside the boundary
int boundary_check(ship* boat, int min, int max) {
	int stop = 0;
	if ((boat->xpos < min) || (boat->xpos > max)) {
		stop = 1;//outside grid
	}
	if ((boat->ypos < min) || (boat->ypos > max)) {
		stop = 1;//outside grid
	}
	return stop;
}

//http://flassari.is/2008/11/line-line-intersection-in-cplusplus/ if want to use line intersection
int goal_check(ship* b, goal* g) {
	int check = 0;
	//if the boat crosses the goal then check = 1
	if ((b->xpos >= g->goal_x2) && (b->ypos >= g->goal_y1)) {
		check = 1;
	}

	return check;
}


vector<double> EA_mutate(vector<double> weights) {
	vector<double> w;
	w = weights;
	for (int i = 0; i < weights.size(); i++) {
		if (rand() % 2 == 0) {
			w.at(i) = (w.at(i)*MJRAND) - (.2*MJRAND);
		}
	}

	return w;
}

vector<policy> EA_replicate(vector<policy> P) {
	vector<policy> population;
	population = P;
	while (population.size() < P.size() * 2) {
		int spot = rand() % population.size();
		policy A;
		A = population.at(spot);
		A.weights = EA_mutate(A.weights);
		population.push_back(A);
	}

	return population;
}



//binary tournament
vector<policy> EA_downselect(vector<policy> P) {
	vector<policy> population;

	while (population.size() < P.size() / 2) {
		int spot1 = rand() % P.size();
		int spot2 = rand() % P.size();
		while (spot1 == spot2) {
			spot2 = rand() % P.size();
		}
		double fit1 = P.at(spot1).fitness;
		double fit2 = P.at(spot2).fitness;
		policy A;
		if (fit1 > fit2) {
			A = P.at(spot1);
		}
		else if (fit2 >= fit1) {
			A = P.at(spot2);
		}

		population.push_back(A);
	}

	return population;
}

//evaluate
double calc_fitness(ship* pboat, goal* pgoal) {

	//centroid of the goal line
	double midx;
	double midy;
	midx = (pgoal->goal_x1 + pgoal->goal_x2) / 2;
	midy = (pgoal->goal_y1 + pgoal->goal_y2) / 2;
	//distance of the boat to the centroid
	double distance;
	distance = sqrt(pow(pboat->xpos - midx, 2) + pow(pboat->ypos - midy, 2));

	return distance;
}

//moves in straight line up
void mr1(ship* pboat) {
	pboat->init();
	pboat->xpos = 100;
	pboat->ypos = 100;
	pboat->theta = 0;
	pboat->omega = 0;
	cout << "initial pos" << endl;
	cout << pboat->xpos << "\t" << pboat->ypos << endl;
	double inity = pboat->ypos;
	int count = 0;
	while (count<20) {
		pboat->simulate();
		cout << pboat->xpos << "\t" << pboat->ypos << endl;
		count++;
	}
	assert(pboat->ypos > inity);
	cout << "done moving in straight line" << endl;
	int a;
	cout << "enter any number to continue" << endl;
	cin >> a;
}

void mr2(ship* pboat, goal* pgoal) {
	pboat->init();
	pboat->xpos = pgoal->goal_x2 - 5;
	pboat->ypos = pgoal->goal_y1 - 5;
	pboat->theta = 359;
	pboat->omega = 0;
	cout << "initial pos" << endl;
	cout << pboat->xpos << "\t" << pboat->ypos << endl;
	cout << "goal area \t" << "x in range of " << pgoal->goal_x2 << " to " << pgoal->goal_x1 << "\t" << "y in range of " << pgoal->goal_y1 << " to " << pgoal->goal_y2 << endl;
	int temp = 0;
	while (temp == 0) {
		pboat->simulate();
		cout << pboat->xpos << "\t" << pboat->ypos << endl;
		temp = goal_check(pboat, pgoal);
	}
	assert(temp == 1);
	cout << "passed goal" << endl;
	int a;
	cout << "enter any number to continue" << endl;
	cin >> a;
}






int main() {
	srand(time(NULL));

	int grid_max = 1000;
	int grid_min = 0;

	ship boat;
	ship* pboat = &boat;

	goal g;
	goal* pg = &g;
	g.create_goal(grid_min, grid_max);


	vector<double> inputs;//for NN, state variables
	vector<double> weights;//for NN
	neural_network NN;
	NN.setup(2, 5, 1);
	NN.set_in_min_max(grid_min-10,900*sqrt(2));//distance of boat to midpoint of goal
	NN.set_in_min_max(-pi / 2, (2 * pi) + (pi / 2));//theta
	NN.set_out_min_max(-5, 30);

	int m1;
	cout << "enter 0 to test mr1 else input 1" << endl;
	cin >> m1;
	if (m1 == 0) {
		mr1(pboat);
	}
	int m2;
	cout << "enter 0 to test mr2 else input 1" << endl;
	cin >> m2;
	if (m2 == 0) {
		mr2(pboat, pg);
	}

	policy P;
	vector<policy> vP;

	int num_policies = 100;
	//generate initial policies
	for (int z = 0; z < num_policies; z++) {
		//cout << "policy " << z << endl;
		boat.init();
		P.init(pboat);

		P.pathx.push_back(boat.xpos);
		P.pathy.push_back(boat.ypos);

		int num_weights;
		inputs.clear();
		//possibly only need theta and distance to the goal for state info
		boat.dist = calc_fitness(pboat, pg);
		inputs.push_back(boat.dist);
		inputs.push_back(boat.theta);
		//NN setup 

		NN.set_vector_input(inputs);
		num_weights = NN.get_number_of_weights();
		for (int i = 0; i < num_weights; i++) {
			weights.push_back(MJRAND-MJRAND);//sets the inital weights 
		}
		P.weights = weights;
		NN.set_weights(weights, true);
		int goal;
		goal = goal_check(pboat, pg);
		int boundary;
		boundary = boundary_check(pboat, grid_min, grid_max);

		int count = 0;
		//boat will stop moving if it crosses the goal or goes outside the grid

		while ((goal == 0) && (boundary == 0)) {
			NN.execute();
			//cout << "before" << endl << boat.xpos << "\t" << boat.ypos << "\t" << boat.omega << "\t" << boat.theta << "\t" << boat.control_signal << endl;
			double NNval = NN.get_output(0);
			boat.control_signal = NNval;
			boat.simulate();//obtain boat's new state
			P.pathx.push_back(boat.xpos);
			P.pathy.push_back(boat.ypos);
			//cout << "after" << endl << boat.xpos << "\t" << boat.ypos << "\t" << boat.omega << "\t" << boat.theta << "\t" << boat.control_signal << endl;
			goal = goal_check(pboat, pg);
			if (goal == 1) {
				cout << "goal" << endl;
			}
			boundary = boundary_check(pboat, grid_min, grid_max);
			if (boundary == 1) {
				cout << "outside grid" << endl;
			}

			count++;
			//stops simulation after large number of time steps
			if (count > 2000) {
				//cout << "too many time steps" << endl;
				break;
			}
			inputs.clear();
			boat.dist = calc_fitness(pboat, pg);
			inputs.push_back(boat.dist);
			inputs.push_back(boat.theta);
			NN.set_vector_input(inputs);
		}
		//cout << count << endl;
		//cout << boat.xpos << "\t" << boat.ypos << endl;
		double fitness = calc_fitness(pboat, pg);
		//cout << fitness << endl;
		P.fitness = fitness;
		vP.push_back(P);
		//cout << P.fitness << "\t" << P.pathx.size() << "\t" << P.pathy.size() << endl;
		//for (int y = 0; y < P.weights.size(); y++) {
		//	cout << P.weights.at(y);
		//}
		P.reset();
		weights.clear();
		//cout << vP.size();
		
	}




	//generations
	int num_generations = 5;
	for (int r = 0; r < num_generations; r++) {
		cout << "generation " << r << endl;
		//downselect
		vP = EA_downselect(vP);
		assert(vP.size() == num_policies / 2);

		//replicate and mutate
		vP = EA_replicate(vP);
		assert(vP.size() == num_policies);

		//simulate
		for (int v = 0; v < vP.size(); v++) {
			/*int tt;
			cin >> tt;*/
			//cout << "v " << v << endl;
			ship simb;
			simb.init();
			ship* psimb = &simb;
			policy simp;
			simp = vP.at(v);
			simb.xpos = simp.xpos;
			simb.ypos = simp.ypos;
			//cout << "init \t" << simb.xpos << "\t" << simb.ypos << endl;
			simb.theta = simp.theta;
			simb.omega = simp.theta;
			
			simp.pathx.clear();
			simp.pathy.clear();
			simp.pathx.push_back(simb.xpos);
			simp.pathy.push_back(simb.ypos);

			int num_weights;
			inputs.clear();
			//possibly only need theta and distance to the goal for state info
			simb.dist = calc_fitness(psimb, pg);
			inputs.push_back(simb.dist);
			inputs.push_back(simb.theta);

			//NN setup 
			NN.set_vector_input(inputs);
			num_weights = NN.get_number_of_weights();
			weights.clear();
			weights = simp.weights;
			NN.set_weights(weights, true);
			int goal;
			goal = goal_check(psimb, pg);
			int boundary;
			boundary = boundary_check(psimb, grid_min, grid_max);

			int count = 0;
			//boat will stop moving if it crosses the goal or goes outside the grid

			while ((goal == 0) && (boundary == 0)) {
				NN.execute();
				//cout << "before" << endl << boat.xpos << "\t" << boat.ypos << "\t" << boat.omega << "\t" << boat.theta << "\t" << boat.control_signal << endl;
				double NNval = NN.get_output(0);
				simb.control_signal = NNval;
				simb.simulate();//obtain boat's new state
				simp.pathx.push_back(simb.xpos);
				simp.pathy.push_back(simb.ypos);
				//cout << "after" << endl << boat.xpos << "\t" << boat.ypos << "\t" << boat.omega << "\t" << boat.theta << "\t" << boat.control_signal << endl;
				goal = goal_check(psimb, pg);
				if (goal == 1) {
					cout << "goal" << endl;
				}
				boundary = boundary_check(psimb, grid_min, grid_max);
				if (boundary == 1) {
					cout << "outside grid" << endl;
				}

				count++;
				//stops simulation after large number of time steps
				if (count > 2000) {
					//cout << "too many time steps" << endl;
					break;
				}
				inputs.clear();
				simb.dist = calc_fitness(psimb, pg);
				inputs.push_back(simb.dist);
				inputs.push_back(simb.theta);
				NN.set_vector_input(inputs);
			}
			//cout << count << endl;
			//cout <<"final \t"<< simb.xpos << "\t" << simb.ypos << endl;
			double fit = calc_fitness(psimb, pg);
			//cout << fit<<endl;
			simp.fitness = fit;
			vP.at(v) = simp;
			//cout << P.fitness << "\t" << P.pathx.size() << "\t" << P.pathy.size() << endl;
			//for (int y = 0; y < P.weights.size(); y++) {
			//	cout << P.weights.at(y);
			//}
			simp.reset();
			//cout << vP.size();
		}
		
		/*double temp = 0;
		int loc = 0;
		for (int a = 0; a < vP.size(); a++) {
			if (vP.at(a).fitness >= 0) {d
				temp = vP.at(a).fitness;
				loc = a;
			}
		}
		cout << "smallest fitness = " << temp << endl;*/
	}

	cout << "final fitness" << endl;
	for (int b = 0; b < vP.size(); b++) {
		cout << vP.at(b).fitness << endl;
	}

	return 0;
}