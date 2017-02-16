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
	double m = rand() % 2;
	double s = rand() % 2;
	double msign;
	double ssign;
	if (m >= 1) {
		msign = 0;
	}
	else {
		msign = -10;
	}

	if (s >= 1) {
		ssign = 0;
	}
	else {
		ssign = -3;
	}
	mu = msign + rand() % 10 + 1;
	sigma = ssign + MJRAND+rand() % 4;
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
	double number = MJRAND;
	//cout << "num = " << number << endl;
	if (epsilon >=number) {
		choice = 0;//explore
	}
	else {
		choice = 1;//greedy, best option
	}
	return choice;
}

//exponential decay formula
double epsilon_decay(double init_epsilon, vector<double>* pv) {
	double new_epsilon;
	double lambda = .001;
	double e = 2.71828;
	new_epsilon = init_epsilon * pow(e, -lambda*pv->size());

	return new_epsilon;
}

//value out through equation
double expected_value_out(vector<double>* pvec, double alpha, double arm_reward, int n) {
	double value = 0.0;
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

//implement at end of testing or won't converge
void tetstA(arm arm, double avg, double average) {
	double mu = arm.mu;//actual average value
	double test_avg = average*1.1;//calculated from pulls, averages won't be exact adds tolerance
	assert(test_avg == mu);
}

//implement at end of testing
void testB(arm arml, arm armm, arm armr) {
	double mul = arml.mu;
	double mum = armm.mu;
	double mur = armr.mu;

	double varl = arml.sigma;
	double varm = armm.sigma;
	double varr = armr.sigma;

	//assert (which arm was pulled the most = if that arm had the highest mean and lowest variance)

}

int main() {
	srand(time(NULL));

	arm left_arm;
	arm middle_arm;
	arm right_arm;

	left_arm.init();
	//cout << "init mu = " << left_arm.mu << endl;
	//cout << "init sigma = " << left_arm.sigma << endl;
	middle_arm.init();
	right_arm.init();

	int arm_input; // left=1, middle=2, right=3
	int arm_pulled;
	int lcount = 0;
	int mcount = 0;
	int rcount = 0;
	int explore_count = 0;
	int greedy_count = 0;

	double total_left_reward = 0;
	double total_middle_reward = 0;
	double total_right_reward = 0;

	int length = 1000;//length of vector, # of trials

	vector<double> left(length,0);//creates vector filled with 0 to length
	vector<double> middle(length,0);
	vector<double> right(length,0);

	vector<double> left_exp_value(length, 0); //stores V(T)
	vector<double> right_exp_value(length, 0);
	vector<double> middle_exp_value(length, 0);
	
	vector<double> learn_curve;//stores current reward value for each pull, used for excel
	vector<double> action_curve;//stores which arm is pulled every iteration, for excel

	double avgl=1;
	double avgm=1;
	double avgr=1;
	double devationl=1;
	double devationm=1;
	double devationr=1;
	double current_reward=1;

	//explore vs greedy 
	//double epsilon = rand() % 1;
	double init_epsilon = .3; //at start no knowledge 
	double epsilon = init_epsilon;
	//agent pulling arm
	for (int n = 1; n < length; n++) {
		/*cout << "choose arm" << endl;
		cin >> arm_input;*/
		int choice;
		choice = explore_or_greedy(epsilon);

		if (choice == 0) {//explore
			cout << "explore" << endl;
			explore_count++;
			arm_input = rand() % 3 + 1;
			if (arm_input == 1) {
				current_reward = arm_pull(left_arm.mu, left_arm.sigma);// R in the equation
				learn_curve.push_back(current_reward);
				left_arm.reward_of_arm = expected_value_out(&left_exp_value, left_arm.alpha, current_reward,n);// V(T)
				left.at(n) = left_arm.reward_of_arm;//replaces value in vector to reward
				total_left_reward = total_left_reward + left_arm.reward_of_arm;//total reward of the arm
				//cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&learn_curve);//check for which vector
				//cout << "left avg = " << avgl << endl;
				devationl = stdev(&learn_curve, avgl);
				//cout << "left dev = " << devationl << endl;
				arm_pulled = 1;
				//cout << "left arm pulled" << endl;
				action_curve.push_back(arm_pulled);
				epsilon = epsilon_decay(init_epsilon, &action_curve);//generates lower epsilon
				lcount++;
			}
			else if (arm_input == 2) {
				current_reward = arm_pull(middle_arm.mu, middle_arm.sigma);// R in the equation
				learn_curve.push_back(current_reward);
				middle_arm.reward_of_arm = expected_value_out(&middle_exp_value, middle_arm.alpha, current_reward,n);// V(T)
				middle.at(n) = middle_arm.reward_of_arm;//replaces value in vector to reward
				total_middle_reward = total_middle_reward + middle_arm.reward_of_arm;//total reward of the arm
				//cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&learn_curve);//check for which vector
				//cout << "left avg = " << avgl << endl;
				devationl = stdev(&learn_curve, avgl);
				//cout << "left dev = " << devationl << endl;
				arm_pulled = 2;
				//cout << "mid arm pulled" << endl;
				action_curve.push_back(arm_pulled);
				epsilon = epsilon_decay(init_epsilon, &action_curve);//generates lower epsilon
				mcount++;
			}
			else if (arm_input == 3) {
				current_reward = arm_pull(right_arm.mu, right_arm.sigma);// R in the equation
				learn_curve.push_back(current_reward);
				right_arm.reward_of_arm = expected_value_out(&right_exp_value, right_arm.alpha, current_reward,n);// V(T)
				right.at(n) = right_arm.reward_of_arm;//replaces value in vector to reward
				total_right_reward = total_right_reward + right_arm.reward_of_arm;//total reward of the arm
				//cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&learn_curve);//check for which vector
				//cout << "left avg = " << avgl << endl;
				devationl = stdev(&learn_curve, avgl);
				//cout << "left dev = " << devationl << endl;
				arm_pulled = 3;
				//cout << "left arm pulled" << endl;
				action_curve.push_back(arm_pulled);
				epsilon = epsilon_decay(init_epsilon, &action_curve);//generates lower epsilon
				rcount++;
			}
			else {
				cout << "invalid arm" << endl;
			}
			
			}

		else {
			greedy_count++;
			cout << "greedy" << endl;
			if (avgl > avgm && avgl > avgr) {
				arm_input = 1;
			}
			else if (avgm > avgl && avgm > avgr) {
				arm_input = 2;
			}
			else if (avgr > avgl && avgr > avgm) {
				arm_input = 3;
			}
			else {
				arm_input = rand() % 3 + 1;
			}


			if (arm_input == 1) {
				current_reward = arm_pull(left_arm.mu, left_arm.sigma);// R in the equation
				learn_curve.push_back(current_reward);
				left_arm.reward_of_arm = expected_value_out(&left_exp_value, left_arm.alpha, current_reward,n);// V(T)
				left.at(n) = left_arm.reward_of_arm;//replaces value in vector to reward
				total_left_reward = total_left_reward + left_arm.reward_of_arm;//total reward of the arm
				//cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&learn_curve);//check for which vector
				//cout << "left avg = " << avgl << endl;
				devationl = stdev(&learn_curve, avgl);
				//cout << "left dev = " << devationl << endl;
				arm_pulled = 1;
				//cout << "left arm pulled" << endl;
				action_curve.push_back(arm_pulled);
				epsilon = epsilon_decay(init_epsilon, &action_curve);//generates lower epsilon
				lcount++;
			}
			else if (arm_input == 2) {
				current_reward = arm_pull(middle_arm.mu, middle_arm.sigma);// R in the equation
				learn_curve.push_back(current_reward);
				middle_arm.reward_of_arm = expected_value_out(&middle_exp_value, middle_arm.alpha, current_reward,n);// V(T)
				middle.at(n) = middle_arm.reward_of_arm;//replaces value in vector to reward
				total_middle_reward = total_middle_reward + middle_arm.reward_of_arm;//total reward of the arm
				//cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&learn_curve);//check for which vector
				//cout << "left avg = " << avgl << endl;
				devationl = stdev(&learn_curve, avgl);
				//cout << "left dev = " << devationl << endl;
				arm_pulled = 2;
				//cout << "mid arm pulled" << endl;
				action_curve.push_back(arm_pulled);
				epsilon = epsilon_decay(init_epsilon, &action_curve);//generates lower epsilon
				mcount++;
			}
			else if (arm_input == 3) {
				current_reward = arm_pull(right_arm.mu, right_arm.sigma);// R in the equation
				learn_curve.push_back(current_reward);
				right_arm.reward_of_arm = expected_value_out(&right_exp_value, right_arm.alpha, current_reward,n);// V(T)
				right.at(n) = right_arm.reward_of_arm;//replaces value in vector to reward
				total_right_reward = total_right_reward + right_arm.reward_of_arm;//total reward of the arm
				//cout << "left total reward = " << total_left_reward << endl;
				avgl = average(&learn_curve);//check for which vector
				//cout << "left avg = " << avgl << endl;
				devationl = stdev(&learn_curve, avgl);
				//cout << "left dev = " << devationl << endl;
				arm_pulled = 3;
				//cout << "left arm pulled" << endl;
				action_curve.push_back(arm_pulled);
				epsilon = epsilon_decay(init_epsilon, &action_curve);//generates lower epsilon
				rcount++;
			}
			else {
				cout << "invalid arm" << endl;
			}
		}
		
	}
	cout << "l arm avg = " << left_arm.mu << " var = " << left_arm.sigma << endl;
	cout << "m arm avg = " << middle_arm.mu << " var = " << middle_arm.sigma << endl;
	cout << "r arm avg = " << right_arm.mu << " var = " << right_arm.sigma << endl;
	cout << "left pulls = " << lcount << endl;
	cout << "mid pulls = " << mcount << endl;
	cout << "right pulls = " << rcount << endl;
	cout << "explore count = " << explore_count << endl;
	cout << "greedy count = " << greedy_count << endl;
	//testA************
	//testB****************
	
	ofstream project_alpha;
	project_alpha.open("project_alpha.txt");
	for (int m = 0; m < learn_curve.size(); m++) {
		project_alpha << "reward = " << learn_curve.at(m) << endl;
	}
	project_alpha.close();

	return 0;
}