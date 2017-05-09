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
	double agentX;
	double agentY;
	double new_agentX;
	double new_agentY;
	double distance;
	double total_distance;

	void init();
	void calc_d();

};

void agent::init() {
	agentX = 0;
	agentY = 0;
}

//calculates distance for every move and total distance agent has travelled
void agent::calc_d() {
	double calcX = 0;
	double calcY = 0;
	calcX = new_agentX - agentX;
	calcY = new_agentY - agentY;
	double total = pow(calcX, 2) + pow(calcY, 2);
	distance = sqrt(total);
	//cout << "distance = " << distance << endl;
	total_distance = total_distance + distance;
	//cout << "total d = " << total_distance << endl;
}







class city {
public:
	double cityX;
	double cityY;
	int visit; //if city has been unvisited = 0
	int number;//city number 
	void init(double min, double max);
};

void city::init(double min, double max) {
	cityX = MJRAND*(max - min) + min;
	cityY = MJRAND*(max - min) + min;

	visit = 0;
}






class state {
public:
	int num_cities;
	double length;
	double xcorner;//min
	double ycorner;
	int state_num;
	double xlimit;//max
	double ylimit;
	int state_visit = 0;
	double midx;
	double midy;

	vector<city> vcity;//contains each city in the state

	void init();//creates each state to be a square, called after set min max
	void set_min_max(double xgap, double ygap);//generates the bottom left corner of the state
};

//set size of state
void state::set_min_max(double xgap, double ygap) {
	//hard code for 6 states
	double min = 0;//min x coord of the bottom left corner
	double max = 50;//max x coord
	xcorner = ((state_num + 1)*xgap) + (MJRAND* (max - min) + min);
	if (state_num <= 3) {
		ycorner = ygap + (MJRAND*(max - min) + min);
	}
	else {
		ycorner = (2 * ygap) + (MJRAND*(max - min) + min);
	}
}

void state::init() {
	double min = 50;//for the limits of a side of the state
	double max = 100;
	length = MJRAND*(max - min) + max;
	xlimit = length + xcorner;
	ylimit = length + ycorner;
	num_cities = rand() % 25 + 15;
	midx = (length / 2) + xcorner;
	midy = (length / 2) + ycorner;
}









//contains order of cities selected and total distance 
class policy {
public:
	vector<int> order;
	double total_d;

	void mutate();
};

//swaps the order of two cities, can't swap the starting city
void policy::mutate() {
	int rand1 = rand() % order.size();
	if (rand1 == 0) {
		rand1 = rand1 + 1;
	}
	int rand2 = rand() % order.size();
	if (rand2 == 0) {
		rand2 = rand2 + 1;
	}
	while (rand1 == rand2) {
		rand2 = rand() % order.size();
		if (rand2 == 0) {
			rand2 = rand2 + 1;
		}
	}
	int temp;
	temp = order.at(rand1);
	order.at(rand1) = order.at(rand2);
	order.at(rand2) = temp;
}








class state_policy {
public:
	vector<state> nation;//order of states visited
	double fitness;//total distance
	void mutate_state();
};

//changes the order in which the states are visited
void state_policy::mutate_state() {
	int rand1 = rand() % nation.size();
	int rand2 = rand() % nation.size();

	while (rand1 == rand2) {
		rand2 = rand() % nation.size();
	}

	state temp;
	temp = nation.at(rand1);
	nation.at(rand1) = nation.at(rand2);
	nation.at(rand2) = temp;
}









//only for initial guesses, chooses randomly where to move
int pick_city(vector<city> vcity) {
	int t;
	int test;

	t = rand() % vcity.size();//choose which city to move to
	while (vcity.at(t).visit >= 1) {
		t = rand() % vcity.size();
	}
	return t;
}

//only for inital guesses, chooses which state to move to
int pick_state(vector<state> vstate) {
	int t;
	int test;

	t = rand() % vstate.size();
	while (vstate.at(t).state_visit >= 1) {
		t = rand() % vstate.size();
	}
	return t;
}

double movex(agent* pagent, vector<city> vcity, int picked_city) {
	pagent->new_agentX = vcity.at(picked_city).cityX;
	return pagent->new_agentX;
}

double movey(agent* pagent, vector<city> vcity, int picked_city) {
	pagent->new_agentY = vcity.at(picked_city).cityY;
	return pagent->new_agentY;
}



//only for mutating
//calcs total distance for new city order
double mutate_move(agent* pagent, policy p, vector<city> city_info, int start_city) {
	//cout << "mutate move" << endl;
	pagent->distance = 0;
	pagent->total_distance = 0;
	//cout << "size = " << p.order.size() << endl;
	//for (int t = 0; t < p.order.size(); t++) {
	//	cout << p.order.at(t) << "\t";
	//}
	//cout << endl;
	int city1 = 0;//city agent is at
	int city2 = 1;//city agent will travel to
	int test = 0;
	for (int i = 0; i < p.order.size() - 1; i++) {
		int temp_loc1 = 0;
		int temp_loc2 = 0;
		for (int i = 0; i < city_info.size(); i++) {
			if (p.order.at(city1) == city_info.at(i).number) {
				temp_loc1 = i;
			}
			if (p.order.at(city2) == city_info.at(i).number) {
				temp_loc2 = i;
			}
		}
		city info1 = city_info.at(temp_loc1);
		city info2 = city_info.at(temp_loc2);
		pagent->agentX = info1.cityX;
		pagent->agentY = info1.cityY;
		pagent->new_agentX = info2.cityX;
		pagent->new_agentY = info2.cityY;
		//checks that the agent begins at the start city
		while (test == 0) {
			city start = city_info.at(start_city);
			assert(pagent->agentX == start.cityX);
			assert(pagent->agentY == start.cityY);
			test = 10;
		}
		//cout << "x = " << pagent->agentX << " y= " << pagent->agentY << endl;
		//cout << "newx = " << pagent->new_agentX << " newy =" << pagent->new_agentY << endl;
		pagent->calc_d();
		city1++;
		city2++;
	}
	//cout << "done" << endl;
	return pagent->total_distance;
}

//creates  copy of a policy and mutates
vector<policy> replicate(vector<policy> vP, agent* pagent, vector<city> city_info, int start_city) {
	//cout << "replicate" << endl;
	vector<policy> pop;//new vector of policies
	pop = vP;
	while (pop.size() < vP.size() * 2) {
		int spot = rand() % pop.size();
		policy p;
		p = pop.at(spot);
		//cout << "mutate" << endl;
		p.mutate();
		//for (int i = 0; i < p.order.size(); i++) {
		//	cout << p.order.at(i) << "\t";
		//}
		p.total_d = mutate_move(pagent, p, city_info, start_city);
		pop.push_back(p);
	}
	assert(pop.size() == vP.size() * 2);
	return pop;
}

//reduces amount of policies to initial amount
vector<policy> downselect(vector<policy> vP) {
	//cout << "downselect" << endl;
	//binary touranment
	vector<policy> population;//new vector of policies
	policy p;
	while (population.size() < vP.size() / 2) {
		int spot1 = rand() % vP.size();
		int spot2 = rand() % vP.size();
		while (spot1 == spot2) {
			spot2 = rand() % vP.size();
		}
		//keeps the policy with the lowest total distance
		double fit1 = vP.at(spot1).total_d;
		double fit2 = vP.at(spot2).total_d;

		if (fit1 < fit2) {
			p = vP.at(spot1);
		}
		else {
			p = vP.at(spot2);
		}
		population.push_back(p);
	}
	//checks if there are the same amount of new policies as what the program started with
	assert(vP.size() / 2 == population.size());
	return population;
}




vector<state_policy> replicate_state(vector<state_policy> vS) {
	//cout << "replicate" << endl;
	vector<state_policy> pop;//new vector of policies
	pop = vS;
	while (pop.size() < vS.size() * 2) {
		int spot = rand() % pop.size();
		state_policy s;
		s = pop.at(spot);

		//mutate
		s.mutate_state();

		//recalculate neww fitness for the mutated policy
		s.fitness = calc_state_dist(s.nation);

		pop.push_back(s);
	}

	assert(pop.size() == vS.size() * 2);
	return pop;
}


vector<state_policy> downselect_state(vector<state_policy> vS) {
	//cout << "downselect" << endl;
	//binary touranment
	vector<state_policy> population;//new vector of policies
	state_policy s;
	while (population.size() < vS.size() / 2) {
		int spot1 = rand() % vS.size();
		int spot2 = rand() % vS.size();
		while (spot1 == spot2) {
			spot2 = rand() % vS.size();
		}
		//keeps the policy with the lowest total distance
		double fit1 = vS.at(spot1).fitness;
		double fit2 = vS.at(spot2).fitness;

		if (fit1 < fit2) {
			s = vS.at(spot1);
		}
		else {
			s = vS.at(spot2);
		}
		population.push_back(s);
	}
	//checks if there are the same amount of new policies as what the program started with
	assert(vS.size() / 2 == population.size());
	return population;
}



double average_d(vector<policy> vpolicy) {
	double sum = 0;
	for (int i = 0; i < vpolicy.size(); i++) {
		sum = sum + vpolicy.at(i).total_d;
	}
	return sum / vpolicy.size();
}

double calc_state_dist(vector<state> nation) {
	agent a;
	a.agentX = 0;
	a.agentY = 0;
	double d = 0;
	double total = 0;
	for (int i = 0; i < nation.size(); i++) {
		a.new_agentX = nation.at(i).midx;
		a.new_agentY = nation.at(i).midy;
		double x = pow(a.new_agentX - a.agentX, 2);
		double y = pow(a.new_agentY - a.agentY, 2);
		d = sqrt(x + y);
		total = total + d;
	}
	return d;
}




//state optimization then cities
void top_down(int num_states, double xgap, double ygap, int num_city_policies, int num_state_policies, int state_generations, int city_generations) {
	agent smith;
	vector<state> vstate;
	//creates all the states and places them in a vector of states for reference
	for (int i = 0; i < num_states; i++) {
		state county;
		county.state_num = i;
		county.set_min_max(xgap,ygap);
		county.init();
		vstate.push_back(county);
	}
	//states are currently cityless

	vector<state_policy> vsp;
	//creates initial state policies
	for (int i = 0; i < num_state_policies; i++) {
		//reset agent and states
		smith.init();
		for (int z = 0; z < vstate.size(); z++) {
			vstate.at(z).state_visit = 0;
		}

		state_policy sp;
		//guesses
		for (int j = 0; j < num_states; j++) {
			int a = pick_state(vstate);
			vstate.at(a).state_visit = 1;//sets the state to have been visited
			sp.nation.push_back(vstate.at(a));
		}
		sp.fitness = calc_state_dist(sp.nation);//calcs fitness for the state policy
	}


	//EA for state optimization 
	for (int y = 0; y < state_generations; y++) {
		//replicate, also mutates and evaluates
		vsp = downselect_state(vsp);
		assert(vsp.size() == num_state_policies * 2);

		//downselect
		vsp = downselect_state(vsp);
		assert(vsp.size() == num_state_policies);
	}
	//state path should now be optimized


	//creates the cities for each state
	for (int i = 0; i < vstate.size(); i++) {
		for (int j = 0; j < vstate.at(i).num_cities; j++) {
			city town;
			town.init();//fix
			town.number = j;
			vstate.at(i).vcity.push_back(town);
		}
	}


	//creates initial city policies
	for (int i = 0; i < num_city_policies; i++) {
		//reset agent and cities
		smith.init();
		for (int j = 0; j < vstate.size(); j++) {
			for (int k = 0; k < vstate.at(j).vcity.size(); k++) {
				vstate.at(j).vcity.at(k).visit = 0;
			}
		}

		policy p;
		//guesses
		//goes through each state and makes guess path for that state
	}


	//EA for city optimization
}

//city optimization the states
void down_up() {

}


int main() {
	srand(time(NULL));

	//int num_cities = 100;
	int num_states = 6;
	int grid_min = 0;
	int grid_max = 1000;
	double xgap = grid_max / 4;
	double ygap = grid_max / 3;
	int num_city_policies = 100;
	int num_state_policies = 100;
	int state_generations = 100;
	int city_generations = 200;



	return 0;
}