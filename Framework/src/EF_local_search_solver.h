#ifndef EF_LOCAL_SEARCH_SOLVER_H_
#define EF_LOCAL_SEARCH_SOLVER_H_


#include <limits>
#include "auxi/timer.hpp"
#include <algorithm>

#include "EF_generic_solver.h"
#include "EF_second_stage_solver.h"
#include "SP_instance.h"
#include "SP_solution.h"

namespace EF {

class EF_local_search_solver  : public EF_generic_solver{
private:
	SP_instance &problem;
	int verbose;
	int time_limit;

	double solve_all_scenarios(double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&),
			std::vector <bool>& selected_resources, std::vector<std::vector <double> * >&  perturbed_costs);

public:

	EF_local_search_solver(SP_instance &problem, int verbose, int time_limit);
	virtual ~EF_local_search_solver();

	int solve(double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&),
			std::vector <bool>& selected_resources, SP_solution &partial_times,
			Timer &timer_global, std::vector<std::vector <double> * >&  perturbed_costs,
			const std::vector< std::vector< double > >& chromosome);
};



} /* namespace EF */
#endif /* EF_LOCAL_SEARCH_SOLVER_H_ */
