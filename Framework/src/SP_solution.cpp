#include "SP_solution.h"

namespace EF {

SP_solution::SP_solution(SP_instance &p):problem(p),partial_solution_counter(0){
	used_in_first_stage.resize(problem.get_number_of_resources());
	used_in_scenario.resize(problem.get_number_of_resources());
	for (int i = 0; i < problem.get_number_of_resources(); i++){
		used_in_scenario[i].resize(p.get_number_of_scenarios());
	}
}

SP_solution::~SP_solution() {
}

int SP_solution::number_resources_in_first_stage(){
	int counter = 0;
	for (int i = 0; i < problem.get_number_of_resources(); i++){
		if (used_in_first_stage[i]) counter++;
	}
	return counter;
}

int SP_solution::number_resources_in_scenario(int s){
	int counter = 0;
	for (int i = 0; i < problem.get_number_of_resources(); i++){
		if (used_in_scenario[i][s]) counter++;
	}
	return counter;
}

void SP_solution::set_new_partial_solution(double time, double value){
	if(partial_solution_counter >= 10000){
		std::cerr << "Number of partial solution improvements exceeded." << std::endl;
		exit(0);
	}
	partial_solution_time.push_back(time);
	partial_solution_values.push_back(value);
	stringstream ss;
	ss << partial_solution_counter;
	string str = ss.str();
	partial_solution_location.push_back(str);
	partial_solution_counter++;
	return;
}

void SP_solution::set_new_partial_solution_loc(double time, double value, string location){
	if(partial_solution_counter >= 10000){
		std::cerr << "Number of partial solution improvements exceeded." << std::endl;
		exit(0);
	}
	if (partial_solution_counter == 0 || partial_solution_values[partial_solution_counter-1] > value){
		partial_solution_time.push_back(time);
		partial_solution_values.push_back(value);
		partial_solution_location.push_back(location);
		partial_solution_counter++;
	}
	return;
}

} /* namespace EF */
