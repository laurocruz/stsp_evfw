#ifndef EF_MANAGER_H_
#define EF_MANAGER_H_

#include "auxi/myutils.h"
#include "auxi/timer.hpp"
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>
#include <limits.h>

#include "EF_generic_solver.h"
#include "EF_local_search_solver.h"
#include "EF_second_stage_solver.h"
#include "SP_instance.h"
#include "SP_solution.h"

namespace EF {

class EF_manager : public EF_generic_solver{
private:
	SP_instance &problem;
	int verbose;
	int time_limit;
	int MAX_ITERATIONS_SINCE_LAST_IMPROVEMENT;
	int MAX_ITERATIONS_SINCE_BEST_SOLUTION;
	int TAIL_GENERATIONS_FACTOR;
	double MINIMUM_IMPROVEMENT_RATIO;

public:
	EF_manager(SP_instance &problem, int verbose, int time_limit,
			int MAX_ITERATIONS_SINCE_LAST_IMPROVEMENT, int MAX_ITERATIONS_SINCE_BEST_SOLUTION,
			double MINIMUM_IMPROVEMENT_RATIO);

	virtual ~EF_manager();

	int solve(double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&),
			SP_solution& solution);
};

}

#endif /* EF_MANAGER_H_ */
