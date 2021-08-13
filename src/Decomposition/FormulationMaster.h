/** 
 * @class FormulationMaster
*/

#ifndef FormulationMaster_H
#define FormulationMaster_H



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



class FormulationMaster
{

    public:
    
        /**
         *  constructor 
         * @param instance
         * @param scip
         * 
        */
        FormulationMaster( InstanceUCP* instance, SCIP* scip_master);

        /**
         * destructor
        */
        virtual ~FormulationMaster()
        {}

        /**
         * @brief goes over the columns selected to get the current best plan 
         * @return a pointer to the production plan
        */
        ProductionPlan* get_production_plan_from_solution();

        /**
         * 
         * @brief adds a column to the restricted master problem . Pure virtual function 
         * 
        */
        virtual void add_column( VariableMaster* variable_to_add ) = 0;


        virtual void create_constraints() = 0;

        virtual void create_and_add_first_columns() = 0;



        //* gets

        SCIP* get_scip_pointer();
        InstanceUCP* get_instance();

        std::vector< VariableMaster* > get_vector_columns();



    protected:


        InstanceUCP *m_instance_ucp;
        SCIP *m_scip_master;

        //* variables
        std::vector< VariableMaster* > m_vector_columns;

        //* constraints : defined in derived class

};



#endif