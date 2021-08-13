/**
 * @file
 * 
*/

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


//** Namespaces
using namespace std;



FormulationPricerUnitDecomposition::FormulationPricerUnitDecomposition(InstanceUCP *instance, 
    SCIP *scip, 
    vector<SCIP_Real> reduced_costs_demand):
    FormulationPricer( instance, scip )
{

    //* create the variables
    SCIP_RETCODE retcode(SCIP_OKAY);
    retcode = create_variables(reduced_costs_demand);
    if ( retcode != SCIP_OKAY)
    {
        SCIPprintError( retcode );
    }

    //* create the constraints
    retcode = create_constraints();
    if ( retcode != SCIP_OKAY)
    {
        SCIPprintError( retcode );
    }

}


/* create all the variable and add them to the object */
SCIP_RETCODE FormulationPricerUnitDecomposition::create_variables(vector<SCIP_Real> reduced_costs_demand)
{
    ostringstream current_var_name;
    int unit_number = m_instance_ucp->get_units_number();
    int time_step_number = m_instance_ucp->get_time_steps_number();


    // Variables x

    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_VAR*> variable_x_i;

        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            current_var_name.str("");
            current_var_name << "x_" << i_unit << "_" << i_time_step;
            SCIP_VAR* variable_x_i_t;
            int fixed_cost_unit_i(m_instance_ucp->get_costs_fixed()[i_unit]);
            SCIPcreateVarBasic(  m_scip_pricer,
                &variable_x_i_t,                    // pointer 
                current_var_name.str().c_str(),     // name
                0.,                                 // lowerbound
                1.,                                 // upperbound
                fixed_cost_unit_i,                  // coeff in obj function
                SCIP_VARTYPE_BINARY);               // type
            
            SCIP_CALL(SCIPaddVar(m_scip_pricer, variable_x_i_t));
            variable_x_i.push_back(variable_x_i_t);
        }

        m_variable_x.push_back(variable_x_i);

    }


    // Variables u

    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_VAR*> variable_u_i;
        int cost_start_i(m_instance_ucp->get_costs_startup()[i_unit]);

        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_VAR* variable_u_i_t;
            current_var_name.str("");
            current_var_name << "u_" << i_unit << "_" << i_time_step ;

            // Creation of the variable
            SCIP_CALL( SCIPcreateVarBasic( m_scip_pricer,
                &variable_u_i_t,                // pointer
                current_var_name.str().c_str(), // name
                0.,                             // lowerbound
                1.,                             // upperbound
                cost_start_i,                   // coeff in obj function
                SCIP_VARTYPE_BINARY));          // type

            // Adding the variable to the problem and the var matrix
            SCIP_CALL( SCIPaddVar(m_scip_pricer, variable_u_i_t));
            variable_u_i.push_back(variable_u_i_t);
        }
        m_variable_u.push_back(variable_u_i);
    }
   
   
    // Variables p

    vector<int> prod_max( m_instance_ucp->get_production_max() );
    vector<int> cost_prop( m_instance_ucp->get_costs_proportionnal() );
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_VAR*> variable_p_i;
        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_VAR* variable_p_i_t;
            current_var_name.str("");
            current_var_name << "p_" << i_unit << "_" << i_time_step ;

            // Creation of the variable
            SCIP_CALL( SCIPcreateVarBasic( m_scip_pricer,
                &variable_p_i_t,                // pointer
                current_var_name.str().c_str(), // name
                0,                     // lowerbound
                prod_max[i_unit],                              // upperbound
                cost_prop[i_unit] - reduced_costs_demand[i_time_step],              // coeff in obj function
                SCIP_VARTYPE_CONTINUOUS));      // type

            // Adding the variable to the problem and the var matrix
            SCIP_CALL( SCIPaddVar(m_scip_pricer, variable_p_i_t));
            variable_p_i.push_back(variable_p_i_t);
        }
        m_variable_p.push_back(variable_p_i);
    }
    

    return( SCIP_OKAY );

}

/* create all the constraints, and add them to the scip object and formulation object */
SCIP_RETCODE FormulationPricerUnitDecomposition::create_constraints()
{
    ostringstream current_cons_name;
    int unit_number = m_instance_ucp->get_units_number();
    int time_step_number = m_instance_ucp->get_time_steps_number();


    //* demand constraint : not in pricing problem
    
    //* startup constraints 

    // the first initial state is different from the rest
    // we need to get the initial state, so we will define it first

    vector<int> initial_state(m_instance_ucp->get_initial_state());
    for(int i_unit = 0; i_unit < unit_number; i_unit++)
    {
        SCIP_CONS* cons_startup_i_0;
        current_cons_name.str("");
        current_cons_name << "cons_startup_" << i_unit << "_0";

        SCIP_CALL(SCIPcreateConsBasicLinear(m_scip_pricer, 
            &cons_startup_i_0,                  /* constraint pointer */ 
            current_cons_name.str().c_str(),    /* constraint name */
            0,                                  /* number of variable added */
            nullptr,                            /* array of variable */
            nullptr,                            /* array of coefficient */
            -SCIPinfinity(m_scip_pricer),              /* LHS */
            +initial_state[i_unit] ));          /* RHS */

        SCIP_CALL( SCIPaddCoefLinear( m_scip_pricer,
            cons_startup_i_0, 
            m_variable_x[i_unit][0],       /* variable to add */
            1));                /* coefficient */
        SCIP_CALL( SCIPaddCoefLinear( m_scip_pricer,
            cons_startup_i_0,
            m_variable_u[i_unit][0],                           /* variable to add */
            -1));                                    /* coefficient */

        SCIP_CALL( SCIPaddCons( m_scip_pricer, cons_startup_i_0));

    }

    // we define the rest of the time steps, which are all the same
    for(int i_unit = 0; i_unit < unit_number; i_unit++)
    {
        vector<SCIP_CONS*> cons_startup_i;
        
        for(int i_time_step = 1; i_time_step < time_step_number; i_time_step++)
        {
            SCIP_CONS* cons_startup_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_startup_" << i_unit << "_" << i_time_step;

            SCIP_CALL(SCIPcreateConsBasicLinear(m_scip_pricer, 
                &cons_startup_i_t,                  /* constraint pointer */ 
                current_cons_name.str().c_str(),    /* constraint name */
                0,                                  /* number of variable added */
                nullptr,                            /* array of variable */
                nullptr,                            /* array of coefficient */
                -SCIPinfinity(m_scip_pricer),                /* LHS */
                0 ));           /* RHS */   

            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_startup_i_t,
                m_variable_x[i_unit][i_time_step],      /* variable to add */
                1));                                    /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_startup_i_t,
                m_variable_x[i_unit][i_time_step - 1],  /* variable to add */
                -1));                                   /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_startup_i_t,
                m_variable_u[i_unit][i_time_step],      /* variable to add */
                -1));                                   /* coefficient */
            SCIP_CALL( SCIPaddCons( m_scip_pricer, cons_startup_i_t));
            cons_startup_i.push_back(cons_startup_i_t);
        }
    m_constraint_startup.push_back(cons_startup_i);
    }


    //* production constraints

    // p < pmax
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_CONS*> cons_p_i;

        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_CONS* cons_p_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_p" << i_unit << "_" << i_time_step;

            // create the constraint
            SCIP_CALL(SCIPcreateConsBasicLinear( m_scip_pricer, 
                &cons_p_i_t,                      /** constraint pointer */ 
                current_cons_name.str().c_str(),             /** constraint name */
                0,                                  /** number of variable added */
                nullptr,                            /** array of variable */
                nullptr,                            /** array of coefficient */
                -SCIPinfinity(m_scip_pricer),                /** LHS */
                0));                                /** RHS */

            // add the variables
            int prod_max_i( m_instance_ucp->get_production_max()[i_unit]);

            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_p_i_t,
                m_variable_x[i_unit][i_time_step],                           /** variable to add */
                - prod_max_i ));     /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear( m_scip_pricer,
                cons_p_i_t,
                m_variable_p[i_unit][i_time_step],                           /** variable to add */
                1));                                    /** coefficient */

            // add the constraint
            SCIP_CALL( SCIPaddCons(m_scip_pricer, cons_p_i_t));
        }
    }

    // pmin < p
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_CONS*> cons_pmin_i;

        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_CONS* cons_pmin_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_pmin_" << i_unit << "_" << i_time_step;

            // create the constraint
            SCIP_CALL(SCIPcreateConsBasicLinear( m_scip_pricer, 
                &cons_pmin_i_t,                      /** constraint pointer */ 
                current_cons_name.str().c_str(),             /** constraint name */
                0,                                  /** number of variable added */
                nullptr,                            /** array of variable */
                nullptr,                            /** array of coefficient */
                0,                /** LHS */
                SCIPinfinity(m_scip_pricer)));                                /** RHS */

            // add the variables
            int prod_min_i( m_instance_ucp->get_production_min()[i_unit] );

            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_pmin_i_t,
                m_variable_x[i_unit][i_time_step],                           /** variable to add */
                - prod_min_i ));     /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear( m_scip_pricer,
                cons_pmin_i_t,
                m_variable_p[i_unit][i_time_step],                           /** variable to add */
                1));                                    /** coefficient */

            // add the constraint
            SCIP_CALL( SCIPaddCons(m_scip_pricer, cons_pmin_i_t));
        }
    }



    // //* Minimum uptime constraint
    vector<int> min_uptime = m_instance_ucp->get_min_uptime();
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {

        vector<SCIP_CONS*> cons_min_uptime_i;



        for(int i_time_step = min_uptime[i_unit]; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_CONS* cons_min_uptime_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_min_uptime_" << i_unit << "_" << i_time_step;

            // create the constraint
            SCIP_CALL( SCIPcreateConsBasicLinear( m_scip_pricer, 
                &cons_min_uptime_i_t,                /** constraint pointer */ 
                current_cons_name.str().c_str(),         /** constraint name */
                0,                              /** number of variable added */
                nullptr,                        /** array of variable */
                nullptr,                        /** array of coefficient */
                -SCIPinfinity(m_scip_pricer),                              /** LHS */
                0));           /** RHS */
            // add the variables
            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_min_uptime_i_t,
                m_variable_x[i_unit][i_time_step],                           /** variable to add */
                -1));                                    /** coefficient */


            for(int i_time_step2 = i_time_step - min_uptime[i_unit] + 1 ; i_time_step2 < i_time_step + 1; i_time_step2++)
            {
                SCIP_CALL( SCIPaddCoefLinear( m_scip_pricer,
                    cons_min_uptime_i_t,
                    m_variable_u[i_unit][i_time_step2],             /** variable to add */
                    1));                                    /** coefficient */
            }
            // adding it to the problem
            SCIP_CALL( SCIPaddCons(m_scip_pricer, cons_min_uptime_i_t));
            cons_min_uptime_i.push_back(cons_min_uptime_i_t);
        }
        m_constraint_min_up_time.push_back(cons_min_uptime_i);
    }
    

    //* Minimum downtime constraint
    vector<int> min_downtime = m_instance_ucp->get_min_downtime();
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {

        vector<SCIP_CONS*> cons_min_downtime_i;

        for(int i_time_step = min_downtime[i_unit]; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_CONS* cons_min_downtime_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_min_downtime_" << i_unit << "_" << i_time_step;

            // create the constraint
            SCIP_CALL( SCIPcreateConsBasicLinear( m_scip_pricer, 
                &cons_min_downtime_i_t,                /** constraint pointer */ 
                current_cons_name.str().c_str(),         /** constraint name */
                0,                              /** number of variable added */
                nullptr,                        /** array of variable */
                nullptr,                        /** array of coefficient */
                - SCIPinfinity(m_scip_pricer),                              /** LHS */
                1));           /** RHS */
            // add the variables
            SCIP_CALL( SCIPaddCoefLinear(m_scip_pricer,
                cons_min_downtime_i_t,
                m_variable_x[i_unit][i_time_step - min_downtime[i_unit]],                           /** variable to add */
                1));                                    /** coefficient */
            for(int i_time_step2 = i_time_step - min_downtime[i_unit] + 1 ; i_time_step2 < i_time_step + 1; i_time_step2++)
            {
                SCIP_CALL( SCIPaddCoefLinear( m_scip_pricer,
                    cons_min_downtime_i_t,
                    m_variable_u[i_unit][i_time_step2],             /** variable to add */
                    1));                                    /** coefficient */
            }

            // adding it to the problem
            SCIP_CALL( SCIPaddCons(m_scip_pricer, cons_min_downtime_i_t));
        }
     }


    return( SCIP_OKAY );
}


ProductionPlan* FormulationPricerUnitDecomposition::get_production_plan_from_solution()
{
    SCIP_SOL *solution = SCIPgetBestSol( m_scip_pricer );
    int number_of_units( m_instance_ucp->get_units_number());
    int number_of_time_steps( m_instance_ucp->get_time_steps_number());

    // We start by creating an empty plan
    vector< vector < double > > up_down_plan;
    vector< vector < double > > switch_plan;
    vector< vector < double > > quantity_plan;
    up_down_plan.resize(number_of_units);
    switch_plan.resize(number_of_units);
    quantity_plan.resize(number_of_units);
    for(int i_unit = 0; i_unit < number_of_units; i_unit ++)
    {
        up_down_plan[i_unit].resize(number_of_time_steps);
        switch_plan[i_unit].resize(number_of_time_steps);
        quantity_plan[i_unit].resize(number_of_time_steps);
    
        for( int i_time_step = 0; i_time_step < number_of_time_steps; i_time_step++)
        {
            up_down_plan[i_unit][i_time_step] = SCIPgetSolVal( m_scip_pricer, solution, m_variable_x[i_unit][i_time_step]);
            switch_plan[i_unit][i_time_step] = SCIPgetSolVal( m_scip_pricer, solution, m_variable_u[i_unit][i_time_step]);
            quantity_plan[i_unit][i_time_step] = SCIPgetSolVal( m_scip_pricer, solution, m_variable_p[i_unit][i_time_step]);
        }   
    } 

    ProductionPlan* new_plan = new ProductionPlan( m_instance_ucp, up_down_plan, switch_plan, quantity_plan );
    new_plan->compute_cost();

    return( new_plan );
}


