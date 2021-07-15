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
#include "InstanceUCP.h"
#include "FormulationPricer.h"



class ProductionPlan
{

    public:

    ProductionPlan(InstanceUCP* instance_ucp);

    ProductionPlan(InstanceUCP* instance_ucp, FormulationPricer* formulation_pricer );

    ~ProductionPlan();
    void show();
    void computeCost();

    double get_cost();
    std::vector< std::vector< double > > get_quantity_plan(); 

    void transform_solution_in_plan( FormulationPricer* formulation_pricer );

    private:
    
    InstanceUCP* p_instance;
    
    std::vector < std::vector  < double > >    m_up_down_plan;
    std::vector < std::vector  < double > >    m_quantity_plan;
    std::vector < std::vector  < double > >    m_switch_plan;

    double m_cost;
};


#endif