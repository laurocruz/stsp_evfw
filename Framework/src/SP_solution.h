#ifndef SP_SOLUTION_H_
#define SP_SOLUTION_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include "auxi/myutils.h"
#include <vector>
#include "SP_instance.h"


namespace EF {

class SP_solution {
private:
	SP_instance &problem;
	std::vector< std::vector<bool> >  used_in_scenario; //used_in_scenario[r][s]
	std::vector< bool >  used_in_first_stage;

	std::vector<double> partial_solution_values;
	std::vector<double> partial_solution_time;
	int partial_solution_counter;
	std::vector<string> partial_solution_location;

public:
	SP_solution(SP_instance &problem);
	virtual ~SP_solution();

	bool is_used_in_scenario(int r, int s){return used_in_scenario[r][s];};
	void set_used_in_scenario(int r, int s, bool b){used_in_scenario[r][s] = b;return;};

	bool is_used_in_first_stage(int r){return used_in_first_stage[r];};
	void set_used_in_first_stage(int r, bool b){used_in_first_stage[r] = b;return;};

	int number_resources_in_first_stage();
	int number_resources_in_scenario(int s);

	void set_new_partial_solution(double time, double value);
	void set_new_partial_solution_loc(double time, double value, string location);

	int get_number_of_partial_solutions(){return partial_solution_counter;}
	double get_partial_solutions_time(int i){return partial_solution_time[i];}
	double get_partial_solutions_value(int i){return partial_solution_values[i];}
	string get_partial_solutions_location(int i){return partial_solution_location[i];}

};

} /* namespace EF */

#endif /* SP_SOLUTION_H_ */
