/** 
 * @class FormulationPricerUnitDecomposition
*/

#ifndef FormulationPricerUnitDecomposition_H
#define FormulationPricerUnitDecomposition_H

//** Includes

//* Standart
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


//* SCIP
#include <scip/scipdefplugins.h>
#include <scip/scip.h>


//* User

// general
#include "DataClasses/InstanceUCP.h"
#include "DataClasses/ProductionPlan.h"

// Decomposition
#include "Decomposition/FormulationPricer.h"

// Unit Decomposition
#include "UnitDecomposition/FormulationPricerUnitDecomposition.h"


/* namespace */

class FormulationPricerUnitDecomposition : public FormulationPricer
{

    public : 

        //* constructor
        FormulationPricerUnitDecomposition(InstanceUCP *instance, SCIP *scip, 
        std::vector<SCIP_Real> reduced_costs_demand );

        //* destructor
        ~FormulationPricerUnitDecomposition()
        {};


        //* virtual functions
        SCIP_RETCODE create_variables( std::vector<SCIP_Real> reduced_costs_demand );
        SCIP_RETCODE create_constraints();


        //* gets
        
        std::vector< std::vector< SCIP_VAR* >> get_variable_u();
        std::vector< std::vector< SCIP_VAR* >> get_variable_x();
        std::vector< std::vector< SCIP_VAR* >> get_variable_p();




    private:

        //* variables
        std::vector< std::vector< SCIP_VAR* >> m_variable_u;
        std::vector< std::vector< SCIP_VAR* >> m_variable_x;
        std::vector< std::vector< SCIP_VAR* >> m_variable_p;

        //* contraintes
        std::vector< SCIP_CONS* > m_constraint_demand;
        std::vector< std::vector< SCIP_CONS* >> m_constraint_production;
        std::vector< std::vector< SCIP_CONS* >> m_constraint_min_up_time;
        std::vector< std::vector< SCIP_CONS* >> m_constraint_min_down_time;
        std::vector< std::vector< SCIP_CONS* >> m_constraint_startup;
        

};

#endif