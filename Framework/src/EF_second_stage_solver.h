#ifndef EF_SECOND_STAGE_SOLVER_H_
#define EF_SECOND_STAGE_SOLVER_H_

#include "auxi/myutils.h"
#include "auxi/timer.hpp"
#include <list>
#include <vector>
#include <algorithm>
#include <math.h>

#include "EF_generic_solver.h"
#include "SP_instance.h"
#include "SP_solution.h"

namespace EF {

class EF_second_stage_solver : public EF_generic_solver{
private:
	SP_instance &problem;
	int verbose;
	int time_limit;

public:
	EF_second_stage_solver(SP_instance &problem, int verbose, int time_limit);
	virtual ~EF_second_stage_solver();

	int solve_exact(int scenario, std::vector<bool>& selected_resources, std::vector<double>& chromosome);

	int solve(SP_solution& solution_final,
			std::vector<bool>& selected_resources, std::vector< std::vector <double> >& chromosomes);
};

} /* namespace EF */


#endif /* EF_SECOND_STAGE_SOLVER_H_ */
