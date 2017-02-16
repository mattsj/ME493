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

	double mu;
	double sigma;
	const double alpha = .1;
	double reward_of_arm;

	void init();
};

void arm::init(){
	mu = rand() % 5 + 1;
	sigma = rand() % 2 + 1;
	reward_of_arm = 0;
}

//found online
double average(vector<double>* pv) {
	double sum = 0;
	for (int i = 0; i < pv->size(); i++) {
		sum = sum + pv->at(i);
	}
	return sum / pv->size();
}

//found online
double stdev(vector<double>* pv, double avg) {
	double E = 0;
	double inverse = 1.0 / pv->size();
	for (int i = 0; i<pv->size(); i++)
	{
		E = E + pow(pv->at(i) - avg, 2);
	}
	//cout << "E = " << E << endl;
	//cout << "inverse = " << inverse << endl;
	return sqrt(inverse * E); //finds stdev population not sample stdev
}

int explore_or_greedy(double epsilon) {
	int choice;
	double number = rand() % 1+0;
	if (epsilon >=number) {
		choice = 0;//explore
	}
	else {
		choice = 1;//greedy, best option
	}
	return choice;
}

double epsilon_decay(double init_epsilon, vector<double>* pv) {
	double new_epsilon;
	double lambda = .01;
	double e = 2.71828;
	new_epsilon = init_epsilon * pow(e, -lambda*pv->size());

	return new_epsilon;
}

//value out through equation
double expected_value_out(vector<double>* pvec, double alpha, double arm_reward, int n) {
	double value = 0.0;
	//cout << "v size = " << pvec->size() << endl;
	//cout << "n = " << n << endl;
	if (pvec->at(1) == 0) {
		value = alpha*arm_reward;
	}
	else {
		//cout << "v at n = " << pvec->at(n - 1) << endl;
		value = alpha*arm_reward + (1 - alpha)*(pvec->at(n - 1));
	}

	return value;
}

//generates reward for the arm using Box Muller transform 
//code from wikipedia
double arm_pull(double mu, double sigma)
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

int main() {
	srand(time(NULL));

	arm left_arm;
	arm middle_arm;
	arm right_arm;

	left_arm.init();
	cout << "init mu = " << left_arm.mu << endl;
	cout << "init sigma = " << left_arm.sigma << endl;
	middle_arm.init();
	right_arm.init();

	int arm_input; // left=1, middle=2, right=3
	int arm_pulled;

	double total_left_reward = 0;
	double total_middle_reward = 0;
	double total_right_reward = 0;

	int length = 1000;//length of vector, # of trials

	vector<double> left(length,0);//creates vector filled with 0 to length
	vector<double> middle(length,0);
	vector<double> right(length,0);

	vector<double> left_exp_value;
	vector<double>right_exp_value;
	vector<double>middle_exp_value;
	
	vector<double> learn_curve;//stores each pull, used for excel
	vector<int> action_curve;//which arm is pulled, for excel

	double avgl;
	double avgm;
	double avgr;
	double devationl;
	double devationm;
	double devationr;
	double current_reward;

	//explore vs greedy 
	//double epsilon = rand() % 1;
	double init_epsilon = .5; //at start no knowledge 
	double epsilon = init_epsilon;
	explore_or_greedy(epsilon);
	if (int choice = 0) {
		arm_input = rand() % 3 + 1;
		if (arm_input == 1) {
			current_reward = arm_pull(left_arm.mu, left_arm.sigma);
			left_arm.reward_of_arm = expected_value_out(&left_exp_value, left_arm.alpha, current_reward,n);//fix,move into for loop
			left.at(n) = left_arm.reward_of_arm;//replaces value in vector to reward
			total_left_reward = total_left_reward + left_arm.reward_of_arm;
			cout << "left total reward = " << total_left_reward << endl;
			avgl = average(&left);
			cout << "left avg = " << avgl << endl;
			devationl = stdev(&left, avgl);
			cout << "left dev = " << devationl << endl;
			arm_pulled = 1;
			learn_curve.push_back(current_reward);
			action_curve.push_back(arm_pulled);
		}
		cout << "explore" << endl;
	}
	else {
		cout << "greedy" << endl;

	}

	//pull arm
	for (int n = 1; n < length; n++) {
			cout << "choose arm" << endl;
			cin >> arm_input;
			if (arm_input == 1) {
				left_arm.reward_of_arm = arm_pull(left_arm.mu, left_arm.sigma);
				total_left_reward = total_left_reward + left_arm.reward_of_arm;
				left.push_back(left_arm.reward_of_arm);
				cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&left);
				cout << "left avg = " << avgl << endl;
				devationl = stdev(&left, avgl);
				cout << "left dev = " << devationl << endl;
			}
			else if (arm_input == 2) {
				total_middle_reward = total_middle_reward + middle_arm.reward_of_arm;
				middle.push_back(total_middle_reward);
				cout << "reward = " << total_middle_reward << endl;
			}
			else if (arm_input == 3) {
				total_right_reward = total_right_reward + right_arm.reward_of_arm;
				right.push_back(total_right_reward);
				cout << "reward = " << total_right_reward << endl;
			}
			else {
				cout << "invalid arm" << endl;
			}
		
	}
	return 0;
}