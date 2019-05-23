#include "EF_local_search_solver.h"

namespace EF {

EF_local_search_solver::EF_local_search_solver(SP_instance &problem, int verbose, int time_limit):
		problem(problem),verbose(verbose),time_limit(time_limit){
}

EF_local_search_solver::~EF_local_search_solver() {
}

double EF_local_search_solver::solve_all_scenarios(
		double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&),
		std::vector <bool>& selected_resources,
		std::vector<std::vector <double> * >&  perturbed_costs){

	//solution of P
	std::vector <bool> solution(problem.get_number_of_resources());

	double total_cost = 0;

	for(int s = 0; s < problem.get_number_of_scenarios() ; s++){
		double scenario_total_cost = 0;
		decode_Ap(problem, s, *perturbed_costs[s], solution);
		std::vector<double> * costs_in_scenario = problem.get_vector_costs_in_scenario(s);
		for (int i = 0; i < problem.get_number_of_resources(); i++){
			if(solution[i] == true && selected_resources[i] != true){
				scenario_total_cost += (*costs_in_scenario)[i];
			}
		}
		total_cost +=  scenario_total_cost * problem.get_probabilities_scenario(s);
	}

	return total_cost;
}

int EF_local_search_solver::solve(
		double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&),
		std::vector <bool>& selected_resources,
		SP_solution &partial_times,
		Timer &timer_global, std::vector<std::vector <double> * >&  perturbed_costs,
		const std::vector< std::vector< double > >& chromosome){

	Timer timer;
	timer.restart();

	if(verbose >= 2)
		std::cout << endl << "** Determining the upper bound **" << endl;

	for (int i = 0; i < problem.get_number_of_resources(); i++){
		selected_resources[i] = false;
	}

	double current_sol_val = -1;
	current_sol_val = solve_all_scenarios(decode_Ap, selected_resources, perturbed_costs);
	partial_times.set_new_partial_solution_loc(timer_global.elapsed(),current_sol_val,"Upper bound");

	if(verbose >= 2){
		std::cout << "Upper bound = " << current_sol_val << std::endl;
	}


	std::vector<pair<double,int> > average_chromosome(problem.get_number_of_resources());

	for (int i = 0; i < problem.get_number_of_resources(); i++){
		average_chromosome[i].first = 0 + ((double) problem.id_final_to_reading_position[i] / (100000 * problem.get_number_of_resources())); //To correct edges order
		average_chromosome[i].second = i;
	}

	for(int s = 0;  s < problem.get_number_of_scenarios(); s++){
		double prob_scenario = problem.get_probabilities_scenario(s);
		for (int count = 0; count < problem.get_number_of_resources(); count++){
			average_chromosome[count].first += chromosome[s][count] * prob_scenario;
		}
	}

	if(verbose >= 2){
		for(int i = 0; i < problem.get_number_of_resources(); i++){
			std::cout << " " << average_chromosome[i].second << "(" << average_chromosome[i].first << ") "  ;
		}
		std::cout << std::endl;
	}

	sort(average_chromosome.begin(), average_chromosome.end());

	if(verbose >= 2){
		for(int i = 0; i < problem.get_number_of_resources(); i++){
			std::cout << " " << average_chromosome[i].second << "(" << average_chromosome[i].first << ") "  ;
		}
		std::cout << std::endl;
	}


	double aux_costs[problem.get_number_of_scenarios()];

	//Local Search main loop
	for(int iterator = 0 ; iterator < problem.get_number_of_resources(); iterator++){
		int i = average_chromosome[iterator].second;
		double new_sol_val;
		selected_resources[i] = true;
		for(int s = 0;  s < problem.get_number_of_scenarios(); s++){
			aux_costs[s] = (*perturbed_costs[s])[i];
			(*perturbed_costs[s])[i] = 0;
		}

		new_sol_val = solve_all_scenarios(decode_Ap, selected_resources, perturbed_costs);

		double reduced_cost = - current_sol_val + new_sol_val + problem.get_cost_first_stage(i);

		if(reduced_cost > 0){
			//backtracking
			selected_resources[i] = false;
			for(int s = 0;  s < problem.get_number_of_scenarios(); s++){
				(*perturbed_costs[s])[i] = aux_costs[s];
			}
		}else{
			current_sol_val = new_sol_val;
		}
	}//End of LS main loop

	for (int i = 0; i < problem.get_number_of_resources(); i++){
		if(selected_resources[i] == true)
			current_sol_val += problem.get_cost_first_stage(i);
	}
	solution_value = current_sol_val;

	if(verbose >= 1){
		std::cout << "Local search solution cost = " << current_sol_val << std::endl;
		std::cout << "Local search time = " << timer.elapsed() << std::endl;
	}
	time_used = timer.elapsed();
	number_of_selected_resources = 0;
	for (int i = 0; i < problem.get_number_of_resources(); i++){
		if(selected_resources[i]) number_of_selected_resources++;
	}
	return 0;
}

} /* namespace EF */
