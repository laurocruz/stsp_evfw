#ifndef SP_INSTANCE_H_
#define SP_INSTANCE_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include "auxi/mylemonutils.h"
#include "auxi/myutils.h"
#include <vector>

namespace EF {

class SP_instance {
private:

public:
	std::vector<int> id_final_to_reading_position;

	std::vector<double>  cost_first_stage;
	std::vector< std::vector<double> >  cost_in_scenario; //cost_in_scenario[R][S]

	std::vector<double> probabilities_scenario;

	int number_of_resources;
	int number_of_scenarios;

	//this vector is used for optimization purposes
	std::vector<std::vector<double> * >  resource_cost_in_scenario; //resource_cost_in_scenario[S][R]

	double (*decode_Ap)(EF::SP_instance &, int, std::vector <double> &,std::vector <bool>&);

	//Constructors
	SP_instance(){};

	//Destructors
	virtual ~SP_instance(){};

	double get_cost_first_stage(int resource_id){return cost_first_stage[resource_id];};
	int get_number_of_scenarios(){return number_of_scenarios;};
	int get_number_of_resources(){return number_of_resources;};

	double get_probabilities_scenario(int s){return probabilities_scenario[s];};
	double get_cost_in_scenario(int resource_id, int s){return (*resource_cost_in_scenario[s])[resource_id];};

	std::vector<double>  * get_vector_costs_in_scenario(int s){return resource_cost_in_scenario[s];};
	std::vector<double>  * get_vector_costs_in_first_stage(){return &cost_first_stage;};

};

} /* namespace EF */
#endif /* SP_INSTANCE_H_ */
