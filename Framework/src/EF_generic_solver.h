#ifndef EF_GENERIC_SOLVER_H_
#define EF_GENERIC_SOLVER_H_

namespace EF {

class EF_generic_solver {
public:
	double time_used;
	double solution_value;
	int number_of_selected_resources;
	virtual ~EF_generic_solver() {};

	virtual int solve(){return -1;};
	double get_time_used(){return time_used;};
	double get_solution_value(){return solution_value;};
	int get_number_of_selected_resources(){return number_of_selected_resources;}
};

} /* namespace EF */

#endif /* EF_GENERIC_SOLVER_H_ */
