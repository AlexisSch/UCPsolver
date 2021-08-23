/** 
 * @class FormulationMasterUnitDecomposition
*/

#ifndef FormulationMasterUnitDecomposition_H
#define FormulationMasterUnitDecomposition_H


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
#include "Decomposition/FormulationMaster.h"
#include "Decomposition/VariableMaster.h"

// Unit Decomposition
#include "UnitDecomposition/FormulationMasterUnitDecomposition.h"



class FormulationMasterUnitDecomposition : public FormulationMaster
{

    public:
    
        /**
         *  constructor 
        */
        FormulationMasterUnitDecomposition( InstanceUCP* instance, SCIP* scip_master);

        /**
         *  destructor 
        */
        ~FormulationMasterUnitDecomposition();

        /**
         * @brief adds a column to the restricted master problem 
        */
        SCIP_RETCODE add_column( ProductionPlan* plan_of_new_column, bool initialization );
        
        SCIP_RETCODE create_variables();
        
        SCIP_RETCODE create_constraints();

        SCIP_RETCODE create_and_add_first_columns();

        ProductionPlan* get_production_plan_from_solution();


        //* gets

        SCIP_CONS** get_convexity_constraint();
        SCIP_CONS** get_complicating_constraints(int i_time_step);


    private:

        //* variables
        std::vector< VariableMaster* > m_vector_columns;

        //* constraints
        std::vector< SCIP_CONS* > m_complicating_constraints;
        SCIP_CONS* m_convexity_constraint;


};



#endif