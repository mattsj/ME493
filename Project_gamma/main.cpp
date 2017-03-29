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

//swaps the city order of two cities 
void policy::mutate() {
	cout << "mutate" << endl;
	int rand1 = rand() % order.size();
	int rand2 = rand() % order.size();
	while (rand1 == rand2) {
		rand2 = rand() % order.size();
	}
	int temp;
	temp = order.at(rand1);
	order.at(rand1) = order.at(rand2);
	order.at(rand2) = temp;
}

//only for initial guesses
int pick_city(vector<city> vcity) {
	int t;
	int test;

	t = rand() % vcity.size();//choose which city to move to
	cout << "t = " << t << endl;
	cout << "v = " << vcity.at(t).visit << endl;
	while (vcity.at(t).visit = 1) {
		cin >> test;
		t = rand() % vcity.size();
		cout << "t = " << t << endl;
		cout << "v = " << vcity.at(t).visit << endl;
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
void mutate_move(agent* pagent, vector<city> vcity, vector<policy> vpolicy) {
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
}

//resets agent and cities to be unvisited
int start(vector<city> vcity, int start_city, agent* pagent) {
	for (int i = 0; i < vcity.size(); i++) {
		vcity.at(i).visit = 0;
	}
	vcity.at(start_city).visit = 1;

	pagent->agentX = vcity.at(start_city).cityX;
	pagent->agentY = vcity.at(start_city).cityY;
	pagent->distance = 0;
	pagent->total_distance = 0;

	return vcity.at(start_city).visit;
}

//creates  copy of a policy and mutates
vector<policy> replicate(vector<policy> vP, agent* pagent, vector<city> vcity) {
	vector<policy> pop;
	pop = vP;
	while (pop.size() < vP.size() * 2) {
		int spot = rand() % pop.size();
		policy p;
		p = pop.at(spot);
		p.mutate();
		mutate_move(pagent,vcity, pop);
		pop.push_back(p);
	}

	return pop;
}

//reduces amount of policies
vector<policy> downselect(vector<policy> vP) {
	cout << "downselect" << endl;
	//binary touranment
	vector<policy> population;
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
				while (town.cityX == vcity.at(g).cityX) {
					while (town.cityY == vcity.at(g).cityY) {
						town.init();
					}
				}
			}
		}
		town.number = count;
		vcity.push_back(town);
		count++;
	}
	assert(num_cities == vcity.size());

	policy P;
	vector<policy> vpolicy;

	int max_cities = num_cities + 1;
	int start_city = 0;//city where the agent will start
	int chosen_city;

	int generations = 1;
	int iterations = 2;
	//initial guesses
	cout << "begin guess" << endl;
	for (int e = 0; e < iterations; e++) {
		vcity.at(start_city).visit = start(vcity, start_city, psmith);
		for (int z = 0; z < vcity.size(); z++) {
			cout << "x = " << vcity.at(z).cityX << " y = " << vcity.at(z).cityY << " visit = " << vcity.at(z).visit << endl;
		}
		P.order.clear();
		P.total_d = 0;
		cout << "guess" << endl;
		for (int f = 0; f < num_cities-1; f++) {
			chosen_city = pick_city(vcity);
			vcity = move(psmith, vcity, chosen_city);
			smith.calc_d(); 
			cout << "d = " << smith.distance << endl;
			P.order.push_back(vcity.at(chosen_city).number);
			cout << "3333" << endl;
		}
		cout << "2222" << endl;
		P.total_d = smith.total_distance;
		vpolicy.push_back(P);

	}
	cout << "finished guess" << endl;
	assert(vpolicy.size() == iterations);

	//generation
	for (int current_g = 0; current_g < generations; current_g++) {
		//replicate/mutate
		vpolicy = replicate(vpolicy, psmith, vcity);
		//downselect
		vpolicy = downselect(vpolicy);
	}

	return 0;
}