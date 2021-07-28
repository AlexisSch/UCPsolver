/** 
 * @class FormulationMasterUnitDecomposition
 ** An object from this class has the variable and constraints defining a Dantzig Wolfe reformulation.
 *  todo : improve the printing to get it with cout ?
*/

#ifndef FormulationMasterUnitDecomposition_H
#define FormulationMasterUnitDecomposition_H


/* standart includes */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

/* SCIP includes */
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"

#include "../Decomposition/VariableMaster.h"
#include "../Decomposition/FormulationMaster.h"


/* namespace */



class FormulationMasterUnitDecomposition : public FormulationMaster
{

    public:
    
        /* constructor */
        FormulationMasterUnitDecomposition( InstanceUCP* instance, SCIP* scip_master);

        /* destructor */
        ~FormulationMasterUnitDecomposition();

        /* adds a column to the restricted master problem */
        void add_column( VariableMaster* variable_to_add );

        /* goes over the columns selected to get the current best plan */
        //ProductionPlan* get_production_plan_from_solution();




        /* gets */

        SCIP_CONS** get_convexity_constraint();
        SCIP_CONS** get_complicating_constraints(int i_time_step);


    private:

        /* constraints */
        std::vector< SCIP_CONS* > m_complicating_constraints;
        SCIP_CONS* m_convexity_constraint;


};



#endif