/** 
 * @class ProductionPlan
 ** Objects from this class stores the information about a production plan. We can then put it inside a master variable.
*/

#ifndef PRODUCTIONPLAN_H
#define PRODUCTIONPLAN_H

/* standart includes */
#include <vector>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */

// Basics includes
#include "InstanceUCP.h"
#include "FormulationCompact.h"
#include "FormulationLinearRelaxation.h"

// Unit decomposition
#include "UnitDecomposition/FormulationPricerUnitDecomposition.h"


class ProductionPlan
{

    public:

    ProductionPlan(InstanceUCP* instance_ucp);

    ProductionPlan(InstanceUCP* instance_ucp, FormulationPricerUnitDecomposition* formulation_pricer );
    ProductionPlan(InstanceUCP* instance_ucp, FormulationLinearRelaxation* formulation_linear );
    ProductionPlan(InstanceUCP* instance_ucp, FormulationCompact* formulation_compact );


    ~ProductionPlan();
    void show();
    void computeCost();

    void empty_plan();

    double get_cost();
    std::vector< std::vector< double > > get_quantity_plan(); 

    void transform_solution_in_plan( FormulationPricerUnitDecomposition* formulation_pricer );
    void transform_solution_in_plan( FormulationCompact* formulation_compact );
    void transform_solution_in_plan( FormulationLinearRelaxation* formulation_linear );

    /* allows to create a plan for the master problem solution.
    Was hard to code, probably better things / smarter to be done */
    void add_column_values( ProductionPlan* production_plan_to_add, SCIP_Real coefficient_column );


    
    private:
    
    InstanceUCP* p_instance;
    
    std::vector < std::vector  < double > >    m_up_down_plan;
    std::vector < std::vector  < double > >    m_quantity_plan;
    std::vector < std::vector  < double > >    m_switch_plan;

    double m_cost;
};


#endif