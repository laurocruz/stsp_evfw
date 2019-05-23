#include "auxi/MTRand.h"
#include "auxi/BRKGA.h"
#include <limits>
#include "EF_second_stage_solver.h"

namespace EF {

EF_second_stage_solver::EF_second_stage_solver(SP_instance &problem, int verbose, int time_limit):
										problem(problem),verbose(verbose),time_limit(time_limit){
}

EF_second_stage_solver::~EF_second_stage_solver() {
}

int EF_second_stage_solver::solve_exact(int scenario,
		std::vector<bool>& selected_resources, std::vector<double>& chromosome){

	Timer timer_global;
	timer_global.restart();

	double total_cost = 0;
	std::vector<bool> resource_map(problem.get_number_of_resources(), false);
	std::vector<double> allele_cost(problem.get_number_of_resources());
	std::vector<double> * costs_in_scenario = problem.get_vector_costs_in_scenario(scenario);

	for (int i = 0; i < problem.get_number_of_resources(); i++){
		if (selected_resources[i]) allele_cost[i] = 0;
		else allele_cost[i] = (*costs_in_scenario)[i];
	}

	total_cost += problem.decode_Ap(problem, scenario, allele_cost, resource_map);

	// Created fake chromosome.
	// If resource is used: Allele = 0
	// Otherwise:           Allele = 1
	for (int i = 0; i < problem.get_number_of_resources(); i++){
		if (resource_map[i] == true) {
			chromosome[i] = 0;
			/*
			if (selected_resources[i] == false) {
				total_cost += (*costs_in_scenario)[i];
			}
			*/
		} else chromosome[i] = 1;
	}

	solution_value = total_cost;
	return solution_value;
}

int EF_second_stage_solver::solve(SP_solution& solution_final,
		std::vector<bool>& selected_resources, std::vector< std::vector <double> >& chromosomes){

	Timer timer;
	timer.restart();

	double total_cost = 0;

	EF_second_stage_solver P_solver(problem, verbose, 36000);

	for (int s = 0; s < problem.get_number_of_scenarios(); s++){
		P_solver.solve_exact(s, selected_resources, chromosomes[s]);
		total_cost += P_solver.get_solution_value() * problem.get_probabilities_scenario(s);
	}

  for (int i = 0; i < problem.get_number_of_resources(); i++){
		if(selected_resources[i] == true) total_cost += problem.get_cost_first_stage(i);
	}

	solution_value = total_cost;
	time_used = timer.elapsed();
	return 0;
}


} /* namespace EF */
