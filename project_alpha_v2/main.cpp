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

#define MJRAND (double)rand()/RAND_MAX

using namespace std;

class arm {
public:

	double pull_count = 0.0;

	double mu;//actual
	double sigma;//actual
	double reward_of_arm;
	double new_reward_of_arm;
	double old_value;
	double value = 0;
	double calculated_reward_avg;//experimental
	double calculated_sigma;//experimental
	double avg_value_out;

	vector<double> vreward;
	vector<double> vvalue;

	void init();
	void expected_value(double alpha);
};

void arm::init() {
	mu = MJRAND * 10;
	sigma = MJRAND;
	calculated_reward_avg = MJRAND*.001;
	calculated_sigma = MJRAND*.001;
}

//new arm value
void arm::expected_value(double alpha) {
	old_value = value;
	if (pull_count == 0) {
		value = alpha*reward_of_arm;
	}
	else {
		value = alpha*reward_of_arm + (1 - alpha)*(old_value);
	}
	vvalue.push_back(value);
}




int explore_or_greedy(double epsilon) {
	int choice;
	if (epsilon >= 1 - MJRAND) {
		choice = 0;//explore
	}
	else {
		choice = 1;//greedy
	}
	return choice;
}

//exponential decay formula
double epsilon_decay(double epsilon, int num_pulls) {
	double new_epsilon;
	double lambda = .0001;
	double e = 2.71828;
	if (epsilon == 0) {
		new_epsilon = .0000001;
	}
	else {
		new_epsilon = epsilon * pow(e, -lambda*num_pulls);
	}
	return new_epsilon;
}


//returns calculated average for arm after being pulled
double average(vector<double> vreward) {
	double sum = 0;
	for (int i = 0; i < vreward.size(); i++) {
		sum = sum + vreward.at(i);
	}
	return sum / vreward.size();
}

//returns standard deviation
//http://www.cplusplus.com/forum/general/42032/
double stdev(vector<double> vreward, double reward_avg) {
	double E = 0;
	double inverse = 1.0 / vreward.size();
	for (int i = 0; i<vreward.size(); i++)
	{
		E = E + pow(vreward.at(i) - reward_avg, 2);
	}
	return sqrt(inverse * E); //finds stdev population not sample stdev
}


//generates reward for the arm using Box Muller transform 
//https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
double generate_reward(double mu, double sigma)
{
	const double epsilon = std::numeric_limits<double>::min();
	const double two_pi = 2.0*3.14159265358979323846;

	static double z0, z1;
	static bool generate;
	generate = !generate;

	if (!generate)
		return z1 * sigma + mu;

	double u1, u2;
	do
	{
		u1 = rand() * (1.0 / RAND_MAX);
		u2 = rand() * (1.0 / RAND_MAX);
	} while (u1 <= epsilon);

	z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
	z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);

	return z0 * sigma + mu;
}

//resets arms after each completed run
void reset_arms(vector<arm> varm) {
	for (int i = 0; i < varm.size(); i++) {
		varm.at(i).avg_value_out = 0;
		varm.at(i).calculated_reward_avg = 0;
		varm.at(i).calculated_sigma = 0;
		varm.at(i).new_reward_of_arm = 0;
		varm.at(i).old_value = 0;
		varm.at(i).pull_count = 0;
		varm.at(i).reward_of_arm = 0;
		varm.at(i).value = 0;
		for (int j = 0; j < varm.at(i).vreward.size(); j++) {
			varm.at(i).vreward.at(j) = 0;
			varm.at(i).vvalue.at(j) = 0;
		}
	}
}

//checks if an arm's calculated avg converges to its mu
int testA(vector<arm> varm) {
	int pass = 0;
	vector<int> temp;
	//finds which arm pulled the most
	for (int i = 0; i < varm.size(); i++) {
		temp.push_back(varm.at(i).pull_count);
	}
	int t = 0;
	int s;
	for (int j = 0; j < temp.size(); j++) {
		if (temp.at(j) > t) {
			t = temp.at(j);
			s = j;
		}
	}

	double tolerance = .02;
	if (varm.at(s).calculated_reward_avg <= varm.at(s).mu*(1 + tolerance)) {
		if (varm.at(s).calculated_reward_avg >= varm.at(s).mu*(1 - tolerance)) {
			pass = 1;
		}
	}
	assert(pass == 1);
	cout << "test A pass" << endl;

	return s;
}

//determines if the best arm is pulled the most by looking at the avg value out
void testB(vector<arm> varm, int a) {
	double t = 0.0;
	int b;
	for (int i = 0; i < varm.size(); i++) {
		if (varm.at(i).avg_value_out > t) {
			t = varm.at(i).avg_value_out;
			b = i;
		}
	}
	assert(b == a);
	cout << "test B pass" << endl;
}


int main() {
	srand(time(NULL));



	//creates n number of arms
	int num_arms;
	cout << "enter # of arms" << endl;
	cin >> num_arms;

	int iterations = num_arms * 250;
	int runs = 30;
	int complete_run = 0;

	vector<arm> varm;

	for (int a = 0; a < num_arms; a++) {
		arm thearm;
		thearm.init();
		varm.push_back(thearm);
	}
	assert(varm.size() == num_arms);

	/*for (int p = 0; p < num_arms; p++) {
		cout << "actual values" << endl;
		cout << "arm " << p << "  avg = " << varm.at(p).mu << "  var = " << varm.at(p).sigma << endl;
	}*/

	vector<double> learn_curve;
	vector<vector<double>> action_curve;
	action_curve.resize(num_arms);
	for (int b = 0; b < num_arms; b++) {
		action_curve[b].resize(iterations);
	}


	

	int arm_input;
	double epsilon;
	double alpha = .1;
	int decide;
	int explore_count;
	int greedy_count;

	ofstream action;
	action.open("action_curve.txt");


	//does x number runs
	for (int o = 0; o < runs; o++) {
		epsilon = .5;
		explore_count = 0;
		greedy_count = 0;
		reset_arms(varm);

		for (int e = 0; e < num_arms; e++) {
			for (int f = 0; f < iterations; f++) {
				action_curve[e][f] = 0;
			}
		}

		for (int aa = 0; aa < num_arms; aa++) {
			varm.at(aa).pull_count = 0;
		}

		for (int n = 0; n < iterations; n++) {
			decide = explore_or_greedy(epsilon);
			if (decide == 0) {
				//explore
				explore_count++;
				arm_input = rand() % num_arms;
				varm.at(arm_input).reward_of_arm = generate_reward(varm.at(arm_input).mu, varm.at(arm_input).sigma);//gets reward of arm
				learn_curve.push_back(varm.at(arm_input).reward_of_arm);
				varm.at(arm_input).vreward.push_back(varm.at(arm_input).reward_of_arm);
				varm.at(arm_input).calculated_reward_avg = average(varm.at(arm_input).vreward);//calculates average reward of the arm
				varm.at(arm_input).expected_value(alpha);//gets new value of arm
				varm.at(arm_input).calculated_sigma = stdev(varm.at(arm_input).vreward, varm.at(arm_input).calculated_reward_avg);//calculates variance
				epsilon = epsilon_decay(epsilon, n + 1);
				varm.at(arm_input).pull_count++;
				for (int u = 0; u < num_arms; u++) {
					action_curve[u][n] = varm.at(u).pull_count / iterations;
				}
			}
			else if (decide == 1) {
				//greedy
				greedy_count++;

				//determine which arm has the highest value out in order to pull that arm
				vector<double> vtemp;
				for (int c = 0; c < num_arms; c++) {
					vtemp.push_back(varm.at(c).value);
				}
				double temp = 0;
				double best_arm = 0;
				for (int d = 0; d < num_arms; d++) {
					if (vtemp.at(d) > temp) {
						temp = vtemp.at(d);
						best_arm = d;
					}
				}
				varm.at(best_arm).reward_of_arm = generate_reward(varm.at(best_arm).mu, varm.at(best_arm).sigma);//gets reward of arm
				learn_curve.push_back(varm.at(best_arm).reward_of_arm);
				varm.at(best_arm).vreward.push_back(varm.at(best_arm).reward_of_arm);
				varm.at(best_arm).calculated_reward_avg = average(varm.at(best_arm).vreward);//calculates average reward of the arm
				varm.at(best_arm).expected_value(alpha);//gets new value of arm
				varm.at(best_arm).calculated_sigma = stdev(varm.at(best_arm).vreward, varm.at(best_arm).calculated_reward_avg);//calculates variance
				epsilon = epsilon_decay(epsilon, n + 1);
				varm.at(best_arm).pull_count++;
				for (int v = 0; v < num_arms; v++) {
					action_curve[v][n] = varm.at(v).pull_count / iterations;
				}
			}
			else {
				cout << "error in decide" << endl;
			}
		}

		for (int r = 0; r < num_arms; r++) {
			varm.at(r).avg_value_out = average(varm.at(r).vvalue);
		}

		int s = testA(varm);//runs test A and returns which arm was pulled the most
		testB(varm, s);
		for (int g = 0; g < iterations; g++) {
			action << action_curve[s][g] << "\t";
		}
		action << endl;

		

		complete_run++;
		cout << "completed runs  = " << complete_run << endl;

		/*cout << "explore count = " << explore_count << endl;
		cout << "greedy count = " << greedy_count << endl;*/

		/*for (int k = 0; k < num_arms; k++) {
			cout << endl;
			cout << "calculated values" << endl;
			cout << "arm " << k << "  avg = " << varm.at(k).calculated_reward_avg << "  var = " << varm.at(k).calculated_sigma << "  pulls = " << varm.at(k).pull_count << endl;
		}*/
	}


	ofstream learn;
	learn.open("learn_curve.txt");
	for (int m = 0; m < iterations*runs; m++) {
		learn << learn_curve.at(m) << "\t";
		if ((m - (iterations - 1)) % (iterations) == 0 && m != 0 && m != 1) {
			learn << endl;
		}
	}
	learn.close();
	action.close();


	return 0;
}