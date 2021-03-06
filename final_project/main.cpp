//Matthew Jew, Alton Ta
//ME 493
//Final Project



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

	void init();
};

void agent::init() {
	agentX = 0;
	agentY = 0;
}








class city {
public:
	double cityX;
	double cityY;
	int visit; //if city has been unvisited = 0
	int number;//city number 
	void init(double xmin, double xmax, double ymin, double ymax);
};

void city::init(double xmin, double xmax, double ymin, double ymax) {
	cityX = MJRAND*(xmax - xmin) + xmin;
	cityY = MJRAND*(ymax - ymin) + ymin;

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
	double length_min = 50;//for the limits of a side of the state
	double length_max = 100;

	vector<city> vcity;//contains each city in the state

	void init();//creates each state to be a square, called after set min max
	void set_min_max(double xgap, double ygap);//generates the bottom left corner of the state
};

//set size of state
void state::set_min_max(double xgap, double ygap) {
	//hard code for 6 states
	double min = 0;//min x coord of the bottom left corner
	double max = 50;//max x coord
	if (state_num < 3) {
		if (state_num > 0) {
			xcorner = ((state_num + 1)*xgap) + (MJRAND* (max - min) + min) + (length_max * state_num);
		}
		else {
			xcorner = ((state_num + 1)*xgap) + (MJRAND* (max - min) + min);
		}
	}
	else {
		if (state_num > 3) {
			xcorner = ((state_num - 2)*xgap) + (MJRAND* (max - min) + min) + (length_max*(state_num - 3));
		}
		else {
			xcorner = ((state_num - 2)*xgap) + (MJRAND* (max - min) + min);
		}
	}

	if (state_num < 3) {
			ycorner = ygap + (MJRAND*(max - min) + min);
	}
	else {
		ycorner = (2 * ygap) + (MJRAND*(max - min) + min) + length_max;
	}
}

void state::init() {
	
	length = MJRAND*(length_max - length_min) + length_max;
	xlimit = length + xcorner;
	ylimit = length + ycorner;
	num_cities = rand() % 15 + 35;
	midx = (length / 2) + xcorner;
	midy = (length / 2) + ycorner;
}









//contains order of cities selected and total distance 
class policy {
public:
	vector<city> order;
	double total_d;

	void mutate(int num_states, vector<state> vstate);
	void mutate_gamma();
};



//swaps the order of two cities
void policy::mutate(int num_states, vector<state> vstate) {
	//for (int i = 0; i < vstate.size(); i++) {
	//	cout << vstate.at(i).num_cities << endl;
	//}
	int state = rand() % num_states;
	//cout << "state " << state << endl;
	int t = 0;
	//calculate min for rand
	if (state == 0) {
		t = 0;
	}
	else {
		for (int i = 0; i < state; i++) {
			t = t + vstate.at(i).num_cities;
		}
	}
	//cout<<"t " << t << endl;
	//set limits for which cities can be mutated based on the state picked
	int rand1 = rand() % vstate.at(state).num_cities + t;
	int rand2 = rand() % vstate.at(state).num_cities + t;
	//cout << "rand1 " << rand1 << endl;
	//cout<<"bbbbb";
	while (rand1==rand2) {
		rand2 = rand() % vstate.at(state).num_cities + t;
		//cout << "rand2 " << rand2 << endl;
	}
	//cout << "aaaa";
	//cout << "rand1 " << rand1<< "\t rand2 " << rand2 << endl;
	city temp;
	temp = order.at(rand1);
	order.at(rand1) = order.at(rand2);
	order.at(rand2) = temp;
}

void policy::mutate_gamma() {
	int rand1 = rand() % order.size();
	int rand2 = rand() % order.size();
	while (rand1 == rand2) {
		rand2 = rand() % order.size();
	}
	city temp;
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

double calc_city_dist(vector<city> vcity) {
	agent a;
	a.agentX = 0;
	a.agentY = 0;
	double d = 0;
	double total = 0;
	for (int i = 0; i < vcity.size(); i++) {
		a.new_agentX = vcity.at(i).cityX;
		a.new_agentY = vcity.at(i).cityY;
		double x = pow(a.new_agentX - a.agentX, 2);
		double y = pow(a.new_agentY - a.agentY, 2);
		d = sqrt(x + y);
		total = total + d;
		a.agentX = a.new_agentX;
		a.agentY = a.new_agentY;
	}
	return total;
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
		a.agentX = a.new_agentX;
		a.agentY = a.new_agentY;
	}
	return total;
}

//creates  copy of a policy and mutates
vector<policy> replicate_city(vector<policy> vP, int num_states, vector<state> vstate) {
	//cout << "replicate" << endl;
	vector<policy> pop;//new vector of policies
	pop = vP;
	while (pop.size() < vP.size() * 2) {
		//cout << "pop " << pop.size() << "\t vp " << vP.size()*2 << endl;
		int spot = rand() % pop.size();
		policy p;
		p = pop.at(spot);
		//cout << "mutate" << endl;
		p.mutate(num_states, vstate);
		//cout << "done" << endl;
		//calc new fitness
		p.total_d = calc_city_dist(p.order);

		pop.push_back(p);
	}
	assert(pop.size() == vP.size() * 2);
	return pop;
}

//creates  copy of a policy and mutates
vector<policy> replicate_city_gamma(vector<policy> vP) {
	//cout << "replicate" << endl;
	vector<policy> pop;//new vector of policies
	pop = vP;
	while (pop.size() < vP.size() * 2) {
		//cout << "pop " << pop.size() << "\t vp " << vP.size()*2 << endl;
		int spot = rand() % pop.size();
		policy p;
		p = pop.at(spot);
		//cout << "mutate" << endl;
		p.mutate_gamma();
		//cout << "done" << endl;
		//calc new fitness
		p.total_d = calc_city_dist(p.order);

		pop.push_back(p);
	}
	assert(pop.size() == vP.size() * 2);
	return pop;
}

//reduces amount of policies to initial amount
vector<policy> downselect_city(vector<policy> vP) {
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



double average_d(vector<double> v) {
	double sum = 0;
	for (int i = 0; i < v.size(); i++) {
		sum = sum + v.at(i);
	}
	return sum / v.size();
}

double calc_overall_d(state_policy sp, policy p, vector<state> original_state) {
	//int count = 0;
	agent a;
	a.init();
	double total = 0;
	for (int i = 0; i < sp.nation.size(); i++) {
		int temp = sp.nation.at(i).state_num;
		int t = 0;//min
		//cout << sp.nation.at(i).state_num;
		//for the first state the min is 0
		if (i == 0) {
			t = 0;
		}
		else {
			for (int z = 0; z < temp; z++) {
				t = t + original_state.at(z).num_cities;
			}
			
		}
		//cout <<"num cities "<<sp.nation.at(i).num_cities<< "\t t " << t << endl;
		int c = 0;
		for (int j = 0; j < sp.nation.at(i).num_cities; j++) {
			//cout << t + c << endl;
			a.new_agentX = p.order.at(t+c).cityX;
			a.new_agentY = p.order.at(t+c).cityY;
			double x = pow(a.new_agentX - a.agentX, 2);
			double y = pow(a.new_agentY - a.agentY, 2);
			total = total + sqrt(x + y);
			a.agentX = a.new_agentX;
			a.agentY = a.new_agentY;
			//count++;
			c++;
		}
		//cout << endl;
	}
	/*int r;
	cin >> r;*/
	//cout << count << endl;

	return total;
}




//state optimization then cities
void top_down(int num_states, double xgap, double ygap, int num_city_policies, int num_state_policies, int state_generations, int city_generations, vector<state> vstate, vector<state> original_state) {
	agent smith;
	
	ofstream learn_curve_state;
	learn_curve_state.open("learn_state_td.txt");

	cout << "creating inital state policies" << endl;
	vector<state_policy> vsp;
	//creates initial state policies
	for (int i = 0; i < num_state_policies; i++) {
		//reset agent and states
		smith.init();
		for (int z = 0; z < vstate.size(); z++) {
			vstate.at(z).state_visit = 0;
		}

		state_policy sp;
		//guess
		for (int j = 0; j < num_states; j++) {
			int a = pick_state(vstate);
			vstate.at(a).state_visit = 1;//sets the state to have been visited
			sp.nation.push_back(vstate.at(a));
		}
		sp.fitness = calc_state_dist(sp.nation);//calcs fitness for the state policy
		vsp.push_back(sp);
	}
	vector<double> vtemp1;
	for (int i = 0; i < vsp.size(); i++) {
		vtemp1.push_back(vsp.at(i).fitness);
	}
	learn_curve_state << average_d(vtemp1) << endl;
	


	cout << "EA state optimization" << endl;
	//EA for state optimization 
	for (int y = 0; y < state_generations; y++) {
		vtemp1.clear();
		//replicate, also mutates and evaluates
		vsp = replicate_state(vsp);
		assert(vsp.size() == num_state_policies * 2);

		//downselect
		vsp = downselect_state(vsp);
		assert(vsp.size() == num_state_policies);

		for (int i = 0; i < vsp.size(); i++) {
			vtemp1.push_back(vsp.at(i).fitness);
		}
		
		learn_curve_state << average_d(vtemp1) << endl;
	}
	//state path should now be optimized
	learn_curve_state.close();

	//finds state policy with lowest fitness to calculate final overall total distance at the end
	double temp1 = 1000000000;
	double loc1 = 0;
	for (int i = 0; i < vsp.size(); i++) {
		if (vsp.at(i).fitness < temp1) {
			temp1 = vsp.at(i).fitness;
			loc1 = i;
		}

	}



	ofstream learn_curve_city;
	learn_curve_city.open("learn_city_td.txt");
	cout << "creating inital city policies" << endl;
	vector<policy> vp;
	vector<double> vtemp2;

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
		//goes through each state and makes guess path for that city
		for (int s = 0; s < vstate.size(); s++) {
			for (int t = 0; t < vstate.at(s).num_cities; t++) {
				int a = pick_city(vstate.at(s).vcity);
				vstate.at(s).vcity.at(a).visit = 1;
				p.order.push_back(vstate.at(s).vcity.at(a));
			}
		}
		p.total_d = calc_city_dist(p.order);
		vp.push_back(p);
	}
	for (int i = 0; i < vp.size(); i++) {
		vtemp2.push_back(vp.at(i).total_d);
	}
	learn_curve_city << average_d(vtemp2) << endl;


	cout << "EA city optimization" << endl;
	//EA for city optimization
	for (int i = 0; i < city_generations; i++) {
		//cout << "gen " << i << endl;
		vtemp2.clear();
		
		//replicate
		//cout << "r";
		vp = replicate_city(vp, num_states, vstate);
		assert(vp.size() == num_city_policies * 2);
	
		//downselect
		//cout << "d";
		vp = downselect_city(vp);
		assert(vp.size() == num_city_policies);

		for (int j = 0; j < vp.size(); j++) {
			vtemp2.push_back(vp.at(j).total_d);
		}
		learn_curve_city << average_d(vtemp2) << endl;
		//cout << "a";
	}
	learn_curve_city.close();

	double temp2 = 100000000;
	double loc2 = 0;
	for (int i = 0; i < vp.size(); i++) {
		if (vp.at(i).total_d < temp2) {
			temp2 = vp.at(i).total_d;
			loc2 = i;
		}

	}

	vector<double> vtempx;
	vector<double> vtempy;

	
	for (int i = 0; i < vsp.at(loc1).nation.size(); i++) {
		int temp = vsp.at(loc1).nation.at(i).state_num;
		int t = 0;
		if (i == 0) {
			t = 0;
		}
		else {
			for (int z = 0; z < temp; z++) {
				t = t + original_state.at(z).num_cities;
			}
		}
		int c = 0;
		//cout << "aaaaa" << endl;
		for (int j = 0; j < vsp.at(loc1).nation.at(i).num_cities; j++) {
			//cout << "bbbbbb" << endl;
			vtempx.push_back(vp.at(loc2).order.at(t + c).cityX);
			vtempy.push_back(vp.at(loc2).order.at(t + c).cityY);
			c++;
		}
	}


	ofstream path_td;
	path_td.open("path_td.txt");
	for (int i = 0; i < vtempx.size(); i++) {
		path_td << vtempx.at(i) << "\t" << vtempy.at(i) << endl;
	}
	path_td.close();

	double count_chocula = 0;
	for (int i = 0; i < vstate.size(); i++) {
		count_chocula = count_chocula + vstate.at(i).num_cities;
	}
	cout << "total cities " << count_chocula << endl;

	//calculate total distance visintg state and cities in order
	double overall_total = calc_overall_d(vsp.at(loc1), vp.at(loc2),original_state);
	cout << "total overall distance " << overall_total << endl;

}


//city optimization then states
void down_up(int num_states, double xgap, double ygap, int num_city_policies, int num_state_policies, int state_generations, int city_generations, vector<state> vstate, vector<state> original_state) {
	agent smith;

	ofstream learn_curve_city;
	learn_curve_city.open("learn_city_du.txt");
	cout << "creating inital city policies" << endl;
	vector<policy> vp;
	vector<double> vtemp1;

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
		for (int s = 0; s < vstate.size(); s++) {
			for (int t = 0; t < vstate.at(s).num_cities; t++) {
				int a = pick_city(vstate.at(s).vcity);
				vstate.at(s).vcity.at(a).visit = 1;
				p.order.push_back(vstate.at(s).vcity.at(a));
			}
		}
		p.total_d = calc_city_dist(p.order);
		vp.push_back(p);
	}
	for (int i = 0; i < vp.size(); i++) {
		vtemp1.push_back(vp.at(i).total_d);
	}
	learn_curve_city << average_d(vtemp1) << endl;


	cout << "EA city optimization" << endl;
	//EA for city optimization
	for (int i = 0; i < city_generations; i++) {
		//cout << "gen " << i << endl;
		vtemp1.clear();

		//replicate
		//cout << "r";
		vp = replicate_city(vp, num_states, vstate);
		assert(vp.size() == num_city_policies * 2);

		//downselect
		//cout << "d";
		vp = downselect_city(vp);
		assert(vp.size() == num_city_policies);

		for (int j = 0; j < vp.size(); j++) {
			vtemp1.push_back(vp.at(j).total_d);
		}
		learn_curve_city << average_d(vtemp1) << endl;
		//cout << "a";
	}
	learn_curve_city.close();

	double temp2 = 100000000;
	double loc2 = 0;
	for (int i = 0; i < vp.size(); i++) {
		if (vp.at(i).total_d < temp2) {
			temp2 = vp.at(i).total_d;
			loc2 = i;
		}

	}




	ofstream learn_curve_state;
	learn_curve_state.open("learn_state_du.txt");

	cout << "creating inital state policies" << endl;
	vector<state_policy> vsp;
	//creates initial state policies
	for (int i = 0; i < num_state_policies; i++) {
		//reset agent and states
		smith.init();
		for (int z = 0; z < vstate.size(); z++) {
			vstate.at(z).state_visit = 0;
		}

		state_policy sp;
		//guess
		for (int j = 0; j < num_states; j++) {
			int a = pick_state(vstate);
			vstate.at(a).state_visit = 1;//sets the state to have been visited
			sp.nation.push_back(vstate.at(a));
		}
		sp.fitness = calc_state_dist(sp.nation);//calcs fitness for the state policy
		vsp.push_back(sp);
	}
	vector<double> vtemp2;
	for (int i = 0; i < vsp.size(); i++) {
		vtemp2.push_back(vsp.at(i).fitness);
	}
	learn_curve_state << average_d(vtemp2) << endl;



	cout << "EA state optimization" << endl;
	//EA for state optimization 
	for (int y = 0; y < state_generations; y++) {
		vtemp2.clear();
		//replicate, also mutates and evaluates
		vsp = replicate_state(vsp);
		assert(vsp.size() == num_state_policies * 2);

		//downselect
		vsp = downselect_state(vsp);
		assert(vsp.size() == num_state_policies);

		for (int i = 0; i < vsp.size(); i++) {
			vtemp2.push_back(vsp.at(i).fitness);
		}

		learn_curve_state << average_d(vtemp2) << endl;
	}
	//state path should now be optimized
	learn_curve_state.close();

	double temp1 = 1000000000;
	double loc1 = 0;
	for (int i = 0; i < vsp.size(); i++) {
		if (vsp.at(i).fitness < temp1) {
			temp1 = vsp.at(i).fitness;
			loc1 = i;
		}

	}


	double count_chocula = 0;
	for (int i = 0; i < vstate.size(); i++) {
		count_chocula = count_chocula + vstate.at(i).num_cities;
	}
	cout << "total cities " << count_chocula << endl;

	//calculate total distance visintg state and cities in order
	double overall_total = calc_overall_d(vsp.at(loc1), vp.at(loc2),original_state);
	cout << "total overall distance " << overall_total << endl;


	vector<double> vtempx;
	vector<double> vtempy;


	for (int i = 0; i < vsp.at(loc1).nation.size(); i++) {
		int temp = vsp.at(loc1).nation.at(i).state_num;
		int t = 0;
		if (i == 0) {
			t = 0;
		}
		else {
			for (int z = 0; z < temp; z++) {
				t = t + original_state.at(z).num_cities;
			}
		}
		int c = 0;
		//cout << "aaaaa" << endl;
		for (int j = 0; j < vsp.at(loc1).nation.at(i).num_cities; j++) {
			//cout << "bbbbbb" << endl;
			vtempx.push_back(vp.at(loc2).order.at(t + c).cityX);
			vtempy.push_back(vp.at(loc2).order.at(t + c).cityY);
			c++;
		}
	}

	ofstream path_du;
	path_du.open("path_du.txt");
	for (int i = 0; i < vtempx.size(); i++) {
		path_du << vtempx.at(i) << "\t" << vtempy.at(i) << endl;
	}
	path_du.close();
}

void gamma(vector<city> vcity, int num_city_policies, int num_city_gen) {
	vector<double> learn;

	//initial guesses
	vector<policy> vp;
	for (int i = 0; i < num_city_policies; i++) {
		//reset cities and agent
		for (int z = 0; z < vcity.size(); z++) {
			vcity.at(z).visit = 0;
		}

		policy p;

		for (int j = 0; j < vcity.size(); j++) {
			int a = pick_city(vcity);
			vcity.at(a).visit = 1;
			p.order.push_back(vcity.at(a));
		}

		p.total_d = calc_city_dist(p.order);
		vp.push_back(p);
	}

	vector<double> v;
	for (int i = 0; i < vp.size(); i++) {
		v.push_back(vp.at(i).total_d);
	}

	learn.push_back(average_d(v));


	//EA
	for (int i = 0; i < num_city_gen; i++) {
		v.clear();
		//replicate, mutate, and evaluate
		vp = replicate_city_gamma(vp);

		//downselect
		vp = downselect_city(vp);

		for (int i = 0; i < vp.size(); i++) {
			v.push_back(vp.at(i).total_d);
		}
		learn.push_back(average_d(v));
	}
	
	ofstream gamma_learn;
	gamma_learn.open("gamma_learn.txt");
	for (int i = 0; i < learn.size(); i++) {
		gamma_learn << learn.at(i) << endl;
	}

	vector<double> vtempx;
	vector<double> vtempy;

	//find policy with lowest fitness
	double temp = 1000000000;
	int loc = 0;
	for (int i = 0; i < vp.size(); i++) {
		if (vp.at(i).total_d < temp) {
			temp = vp.at(i).total_d;
			loc = i;
		}
	}
	
	for (int j = 0; j < vp.at(loc).order.size(); j++) {
		vtempx.push_back(vp.at(loc).order.at(j).cityX);
		vtempy.push_back(vp.at(loc).order.at(j).cityY);
	}

	//write path
	ofstream gamma_path;
	gamma_path.open("gamma_path.txt");
	for (int i = 0; i < vtempx.size(); i++) {
		gamma_path << vtempx.at(i) << "\t" << vtempy.at(i) << endl;
	}
	gamma_path.close();

	cout << "total distance " << vp.at(loc).total_d << endl;
}




int main() {
	srand(time(NULL));

	state temp_state;
	int num_states = 6;
	int grid_min = 0;
	int grid_max = 1000;
	double xgap = (grid_max - (num_states / 2 * temp_state.length_max)) / 4;
	double ygap = (grid_max - (num_states / 3 * temp_state.length_max)) / 3;
	int num_city_policies = 100;
	int num_state_policies = 100;
	int state_generations = 50;
	int city_generations = 300;


	vector<state> vstate;
	//creates all the states and places them in a vector of states for reference
	for (int i = 0; i < num_states; i++) {
		state county;
		county.state_num = i;
		county.set_min_max(xgap, ygap);
		county.init();
		//cout << county.midx << "\t" << county.midy << endl;
		vstate.push_back(county);
	}
	assert(vstate.size() == num_states);
	vector<state> original_state = vstate;
	//states are currently cityless

	vector<city> vcity;//jsut for gamma
	
	//creates the cities for each state
	for (int i = 0; i < vstate.size(); i++) {
		for (int j = 0; j < vstate.at(i).num_cities; j++) {
			city town;
			town.init(vstate.at(i).xcorner, vstate.at(i).xlimit, vstate.at(i).ycorner, vstate.at(i).ylimit);
			//checks for duplicate cities
			if (j > 0) {
				for (int k = 0; k < vstate.at(i).vcity.size(); k++) {
					while ((vstate.at(i).vcity.at(k).cityX == town.cityX) && (vstate.at(i).vcity.at(k).cityY == town.cityY)) {
						town.init(vstate.at(i).xcorner, vstate.at(i).xlimit, vstate.at(i).ycorner, vstate.at(i).ylimit);
					}
				}
			}

			town.number = j;
			vstate.at(i).vcity.push_back(town);
			vcity.push_back(town);
		}
	}

	cout << "TD" << endl;
	top_down(num_states, xgap, ygap, num_city_policies, num_state_policies, state_generations, city_generations, vstate, original_state);
	cout << "DU" << endl;
	down_up(num_states, xgap, ygap, num_city_policies, num_state_policies, state_generations, city_generations, vstate, original_state);
	cout << "GAMMA" << endl;
	gamma(vcity, num_city_policies, city_generations);


	return 0;
}