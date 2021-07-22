/** 
 * @class FormulationMaster
 ** An object from this class has the variable and constraints defining a Dantzig Wolfe reformulation.
 *  todo : improve the printing to get it with cout ?
*/

#ifndef FormulationMaster_H
#define FormulationMaster_H


/* standart includes */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"
#include "VariableMaster.h"

/* namespace */



class FormulationMaster
{

    public:
    
    FormulationMaster( InstanceUCP* instance, SCIP* scip_master);
    ~FormulationMaster();

    void addColumn( VariableMaster* variable_to_add );

    SCIP* get_scip_pointer();

    std::vector<int> get_dual_sol();

    SCIP_CONS* m_convexity_constraint;
    std::vector< SCIP_CONS* > m_complicating_constraints;
    std::vector< VariableMaster* > m_vector_columns;

    InstanceUCP* p_instance;
    SCIP* p_scip_master;



    ProductionPlan* get_production_plan_from_solution();

    private:
    // InstanceUCP *p_instance;
    // SCIP *p_scip_master;

    // SCIP_CONS* m_convexity_constraint;
    // std::vector< SCIP_CONS* > m_complicating_constraints;


};



#endif