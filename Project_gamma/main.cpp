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
	total_distance = total_distance + distance;
}



class city {
public:
	double max = 100;
	double min = 0;
	double cityX;
	double cityY;
	int visit = 0; //if city has been unvisited = 0
	int number;//city number 
	void init();
};

void city::init() {
	cityX = MJRAND*(max - min) + min;
	cityY = MJRAND*(max - min) + min;
}

//contains order of cities selected and total distance 
class policy {
public:
	vector<int> order;
	double total_d;

	void mutate();
};

//swaps the city order of two cities, can't swap the starting city
void policy::mutate() {
	//cout << "mutate" << endl;
	//cout << "size = " << order.size() << endl;
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
	/*cout << "r1 = " << rand1 << endl;
	cout << "r2 = " << rand2 << endl;
	for (int i = 0; i < order.size(); i++) {
		cout << order.at(i) << endl;
	}*/
	//cout << "break" << endl;
	int temp;
	temp = order.at(rand1);
	//cout << "temp = " << temp << endl;
	//cout << "1 = "<<order.at(rand1) << endl;
	//cout << "2 = "<<order.at(rand2) << endl;
	order.at(rand1) = order.at(rand2);
	order.at(rand2) = temp;
	//cout << "done mutate" << endl;
}

//only for initial guesses
int pick_city(vector<city> vcity) {
	int t;
	int test;

	t = rand() % vcity.size();//choose which city to move to
	while (vcity.at(t).visit == 1) {
		t = rand() % vcity.size();
	}
	return t;
}
vector<city> move(agent* pagent, vector<city> vcity, int picked_city) {
	vcity.at(picked_city).visit = 1;
	pagent->new_agentX = vcity.at(picked_city).cityX;
	pagent->new_agentY = vcity.at(picked_city).cityY;
	return vcity;
}

//only for mutating
double mutate_move(agent* pagent, vector<city> vcity, vector<policy> vpolicy) {
	pagent->distance = 0;
	pagent->total_distance = 0;
	for (int i = 0; i < vpolicy.size() - 1; i++) {
		int city1 = 0;
		int city2 = 1;
		pagent->agentX = vcity.at(city1).cityX;
		pagent->agentY = vcity.at(city1).cityY;
		pagent->new_agentX = vcity.at(city2).cityX;
		pagent->new_agentY = vcity.at(city2).cityY;
		pagent->calc_d();
		pagent->total_distance = pagent->total_distance + pagent->distance;
		city1++;
		city2++;
	}
	return pagent->total_distance;
}

//resets agent and cities to be unvisited
//int start(vector<city> vcity, int start_city, agent* pagent) {
//	for (int i = 0; i < vcity.size(); i++) {
//		vcity.at(i).visit = 0;
//	}
//	vcity.at(start_city).visit = 1;
//
//	pagent->agentX = vcity.at(start_city).cityX;
//	pagent->agentY = vcity.at(start_city).cityY;
//	pagent->distance = 0;
//	pagent->total_distance = 0;
//
//	return vcity.at(start_city).visit;
//}

//creates  copy of a policy and mutates
vector<policy> replicate(vector<policy> vP, agent* pagent, vector<city> vcity) {
	//cout << "replicate" << endl;
	vector<policy> pop;//new vector of policies
	pop = vP;
	while (pop.size() < vP.size() * 2) {
		int spot = rand() % pop.size();
		policy p;
		p = pop.at(spot);
		p.mutate();
		p.total_d = mutate_move(pagent, vcity, pop);
		pop.push_back(p);
	}
	assert(pop.size() == vP.size() * 2);
	return pop;
}

//reduces amount of policies
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
	assert(vP.size() / 2 == population.size());
	return population;
}

int main() {
	srand(time(NULL));
	agent smith;
	agent* psmith = &smith;
	smith.init();
	city town;
	vector<city> vcity;
	int num_cities;
	cout << "enter number of cities" << endl;
	cin >> num_cities;
	int count = 0;
	for (int a = 0; a < num_cities; a++) {
		town.init();
		//checks for cities in same loc
		if (a >= 1) {
			for (int g = 0; g < vcity.size(); g++) {
				while ((town.cityX == vcity.at(g).cityX) && (town.cityY == vcity.at(g).cityY)) {
					town.init();
				}
			}
		}
		town.number = count;
		vcity.push_back(town);
		count++;
	}
	assert(num_cities == vcity.size());
	for (int z = 0; z < vcity.size(); z++) {
		cout << "x = " << vcity.at(z).cityX << " y = " << vcity.at(z).cityY << " visit = " << vcity.at(z).visit << endl;
	}

	policy P;
	vector<policy> vpolicy;

	int max_cities = num_cities + 1;
	int start_city = 0;//city where the agent will start
	int chosen_city;

	int generations = 5;//number of RED cycles
	int num_policies = 10;//number of total policies

	//initial guesses
	cout << "begin guess" << endl;
	for (int e = 0; e < num_policies; e++) {
		//reset agent and cities
		for (int p = 0; p < vcity.size(); p++) {
			vcity.at(p).visit = 0;
		}
		vcity.at(start_city).visit = 1;
		smith.agentX = vcity.at(start_city).cityX;
		smith.agentY = vcity.at(start_city).cityY;
		smith.distance = 0;
		smith.total_distance = 0;
		/*for (int z = 0; z < vcity.size(); z++) {
			cout << "x = " << vcity.at(z).cityX << " y = " << vcity.at(z).cityY << " visit = " << vcity.at(z).visit << endl;
		}*/
		//cout << endl;
		P.order.clear();
		P.total_d = 0;
		P.order.push_back(vcity.at(start_city).number);

		//guesses
		//cout << "guess" << endl;
		int gar;
		//cin >> gar;
		for (int f = 0; f < num_cities-1; f++) {
			chosen_city = pick_city(vcity);
			vcity = move(psmith, vcity, chosen_city);
			smith.calc_d(); 
			//cout << "d = " << smith.distance << endl;
			P.order.push_back(vcity.at(chosen_city).number);
		}
		P.total_d = smith.total_distance;
		vpolicy.push_back(P);

	}
	cout << "finished guess" << endl;
	assert(vpolicy.size() == num_policies);

	//generation
	for (int current_g = 0; current_g < generations; current_g++) {
		//replicate/mutate
		vpolicy = replicate(vpolicy, psmith, vcity);
			//for (int j = 0; j < vpolicy.at(0).order.size(); j++) {
			//	//cout << vpolicy.at(i).order.at(j) << endl;
			//}
			//cout << endl;
		//downselect
		vpolicy = downselect(vpolicy);
		for (int i = 0; i < vpolicy.size(); i++) {
			cout << vpolicy.at(i).total_d << endl;
		}
		cout << endl;
	}
	assert(vpolicy.size() == num_policies);

	double min_value = 1000000000;
	int min_loc;
	for (int r = 0; r < vpolicy.size(); r++) {
		cout << "d = " << vpolicy.at(r).total_d << endl;
		if (vpolicy.at(r).total_d < min_value) {
			min_value = vpolicy.at(r).total_d;
			min_loc = r;
		}
	}
	cout << "min loc = " << min_loc << endl;
	cout << "total d = " << min_value << endl;
	for (int w = 0; w < vpolicy.at(min_loc).order.size(); w++) {
		cout << vpolicy.at(min_loc).order.at(w) << endl;
	}
	return 0;
}