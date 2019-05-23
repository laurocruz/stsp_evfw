#include "EF_manager.h"

namespace EF {

EF_manager::EF_manager(SP_instance &problem, int verbose, int time_limit,
		int MAX_ITERATIONS_SINCE_LAST_IMPROVEMENT, int MAX_ITERATIONS_SINCE_BEST_SOLUTION,
		double MINIMUM_IMPROVEMENT_RATIO):
						problem(problem), verbose(verbose), time_limit(time_limit),
						MAX_ITERATIONS_SINCE_LAST_IMPROVEMENT(MAX_ITERATIONS_SINCE_LAST_IMPROVEMENT),
						MAX_ITERATIONS_SINCE_BEST_SOLUTION(MAX_ITERATIONS_SINCE_BEST_SOLUTION),
						MINIMUM_IMPROVEMENT_RATIO(MINIMUM_IMPROVEMENT_RATIO){
}

EF_manager::~EF_manager() {
}

//This if the main function of EF
int EF_manager::solve(double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&),
		SP_solution& solution) {

	Timer timer;
	timer.restart();


	double previous_time = timer.elapsed();

	std::vector<bool> selected_resources(problem.get_number_of_resources());
	std::vector<bool> best_selected_resources(problem.get_number_of_resources());
	//Vectors perturbed_costs is used only by the local search
	std::vector<std::vector<double> *> perturbed_costs(problem.get_number_of_scenarios());
	std::vector<std::vector<double> > cromo(problem.get_number_of_scenarios());
	std::vector<std::vector<double> > best_cromo(problem.get_number_of_scenarios());

	//Memory allocation
	perturbed_costs.resize(problem.get_number_of_scenarios());
	for (int s = 0; s < problem.get_number_of_scenarios(); s++) {
		perturbed_costs[s] = new std::vector<double>(problem.get_number_of_resources());
		cromo[s].resize(problem.get_number_of_resources());
		best_cromo[s].resize(problem.get_number_of_resources());
	}

	// "Perturbed costs"
	for (int s = 0; s < problem.get_number_of_scenarios(); s++) {
		std::vector<double> * costs_in_scenario = problem.get_vector_costs_in_scenario(s);
		for(int i = 0; i < problem.get_number_of_resources(); i++){
			(*perturbed_costs[s])[i] = (*costs_in_scenario)[i];
		}
	}

	//Setting initial chromosome
	for (int s = 0; s < problem.get_number_of_scenarios(); s++) {
		for (int i = 0; i < problem.get_number_of_resources(); i++){
			cromo[s][i] = 0.5;
		}
	}

	double previous_cycle_cost = std::numeric_limits<double>::max();
	double best_global_solution = std::numeric_limits<double>::max();
	int current_cycle = 0;
	int best_solution_cycle = 0;
	int last_improvement_cycle = 0;

	//EF main loop
	while ((timer.elapsed() < time_limit || time_limit < 0)
			&& (current_cycle - best_solution_cycle
					<= MAX_ITERATIONS_SINCE_BEST_SOLUTION)
					&& (current_cycle - last_improvement_cycle
							<= MAX_ITERATIONS_SINCE_LAST_IMPROVEMENT )) {
		previous_time = timer.elapsed();

		//Update perturbed costs
		/*
		for (int s = 0; s < problem.get_number_of_scenarios(); s++) {
			std::vector<double> * costs_in_scenario = problem.get_vector_costs_in_scenario(s);
			for(int i = 0; i < problem.get_number_of_resources(); i++){
				(*perturbed_costs[s])[i] = (*costs_in_scenario)[i];
						EF_second_stage_solver::compute_perturbed_cost(alpha, cromo[s][i], (*costs_in_scenario)[i]);
			}
		}
		*/

		EF_local_search_solver local_search_solver(problem, verbose,
				time_limit);
		local_search_solver.solve(decode_Ap, selected_resources, solution, timer,
				perturbed_costs, cromo);

		stringstream ss_r;
		ss_r << "local search - EF main loop cycle " << current_cycle;
		string str_r = ss_r.str();
		solution.set_new_partial_solution_loc(timer.elapsed(), local_search_solver.get_solution_value(), str_r);

		if (verbose >= 1) {
			std::cout << timer.elapsed() << " local search - EF main loop cycle " << current_cycle << " solution cost " << local_search_solver.get_solution_value() << std::endl;
		}

		if (verbose >= 2) {
			std::cout << " local search - EF main loop cycle " << current_cycle << " fixed resourced: "<< std::endl;
			for (int i = 0; i < problem.get_number_of_resources(); i++)
				if (selected_resources[i] == true)	std::cout << i << "(cost: " << problem.get_cost_first_stage(i) << ") ";
		}

		EF::EF_second_stage_solver second_stage_solver(problem, verbose, -1);
		second_stage_solver.solve(solution, selected_resources, cromo);

		//Update convergence counters
		if (previous_cycle_cost > (second_stage_solver.get_solution_value() * (1 + MINIMUM_IMPROVEMENT_RATIO))) {
			last_improvement_cycle = current_cycle;
		}
		if (best_global_solution > (second_stage_solver.get_solution_value() * (1 + MINIMUM_IMPROVEMENT_RATIO))) {
			best_solution_cycle = current_cycle;
		}

		//Update best found solutions
		if (best_global_solution > second_stage_solver.get_solution_value()) {
			best_global_solution = second_stage_solver.get_solution_value();
			for(int i = 0; i < problem.get_number_of_resources(); i++)
				best_selected_resources[i] = selected_resources[i];
			for (int s = 0; s < problem.get_number_of_scenarios(); s++) {
				for(int i = 0; i < problem.get_number_of_resources(); i++)
					best_cromo[s][i] = cromo[s][i];
			}
		}

		previous_cycle_cost = second_stage_solver.get_solution_value();

		if (verbose >= 1){
			std::cout << timer.elapsed() <<"s Cost of SP solution after second stage solver "
					<< second_stage_solver.get_solution_value() << std::endl;
		}

		stringstream ss;
		ss << " second stage solver - EF main loop cycle " << current_cycle;
		string str = ss.str();
		solution.set_new_partial_solution_loc(timer.elapsed(), second_stage_solver.get_solution_value(), str);
		current_cycle++;

	}//End of EF main loop

	previous_time = timer.elapsed();

	if (verbose >= 1) {
		std::cout << "Number of cycles " << current_cycle << std::endl;
		std::cout << "Time for each cycle " << previous_time / current_cycle << std::endl;
	}
	if (verbose >= 2) {
		std::cout << previous_time << "s current_cycle " << current_cycle << " | best_solution_cycle "
				<< best_solution_cycle << " | last_improvement_cycle "
				<< last_improvement_cycle << std::endl;
	}

	//Tail step
	if (time_limit < 0 || time_limit - previous_time > 0) {
		if (verbose >= 2) { std::cout << "Running the tail step" << std::endl; }
		EF::EF_second_stage_solver second_stage_solver(problem, verbose, time_limit - previous_time);
		second_stage_solver.solve(solution, best_selected_resources, best_cromo);

		if (best_global_solution > second_stage_solver.get_solution_value()) {
			best_global_solution = second_stage_solver.get_solution_value();
		}

		if (verbose >= 1){
			std::cout << timer.elapsed() <<"s Cost of SP solution after tail step "
					<< second_stage_solver.get_solution_value() << std::endl;
		}

		stringstream ss;
		ss << "tail step ";
		string str = ss.str();
		solution.set_new_partial_solution_loc(timer.elapsed(), second_stage_solver.get_solution_value(), str);
	}

	std::cout << "SELECTED RESOURCES: ";
	for (int i = 0; i < problem.get_number_of_resources(); ++i) {
		if (best_selected_resources[i]) {
			std::cout << problem.id_final_to_reading_position[i] << " ";
		}
	}
	std::cout << std::endl;
	std::cout << "BEST CROMOS: \n";
	std::vector<double> crom(problem.get_number_of_resources());
	for (int s = 0; s < problem.get_number_of_scenarios(); s++) {
		std::cout << "S " << s << ": ";
		for(int i = 0; i < problem.get_number_of_resources(); i++) {
			crom[problem.id_final_to_reading_position[i]] = best_cromo[s][i];
		}
		for(int i = 0; i < problem.get_number_of_resources(); i++) {
			std::cout << crom[i] << " ";
		}
		std::cout << std::endl;
	}

	std::vector<bool> aux_solution(problem.get_number_of_resources(), false);
	std::vector<double> p_costs(problem.get_number_of_resources());
	for (int s = 0; s < problem.get_number_of_scenarios(); ++s) {
		for (int i = 0; i < problem.get_number_of_resources(); ++i) {
			if (best_selected_resources[i]) p_costs[i] = 0;
			else p_costs[i] = problem.cost_in_scenario[i][s];
		}
		const int tour_cost = decode_Ap(problem, s, p_costs, aux_solution);
		std::cout << "S " << s << ": " << tour_cost << " | ";
		for (int i = 0; i < problem.get_number_of_resources(); ++i) {
			if (aux_solution[i]) {
				std::cout << problem.id_final_to_reading_position[i] << " ";
				aux_solution[i] = false;
			}
		}
		std::cout << std::endl;
	}


	time_used = timer.elapsed();
	solution_value = best_global_solution;
	return 0;
}



}
