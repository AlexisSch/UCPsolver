

/* standart includes */
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

/* SCIP includes */
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

/* user includes */
#include "InstanceUCP.h"
#include "FormulationLinearRelaxation.h"

/* namespace */
using namespace std;




FormulationLinearRelaxation::FormulationLinearRelaxation(InstanceUCP *instance, SCIP *scip): 
    p_scip(scip), p_ucp_instance(instance)
{

    create_variables();
    create_constraints();
    
}



SCIP_RETCODE FormulationLinearRelaxation::create_variables()
{
    ostringstream current_var_name;
    int unit_number = p_ucp_instance->get_units_number();
    int time_step_number = p_ucp_instance->get_time_steps_number();


    //* x 

    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_VAR*> variable_x_i;

        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            current_var_name.str("");
            current_var_name << "x_" << i_unit << "_" << i_time_step;
            SCIP_VAR* variable_x_i_t;
            int fixed_cost_unit_i(p_ucp_instance->get_costs_fixed()[i_unit]);
            SCIPcreateVarBasic(  p_scip,
                &variable_x_i_t,                    // pointer 
                current_var_name.str().c_str(),     // name
                0.,                                 // lowerbound
                1.,                                 // upperbound
                fixed_cost_unit_i,                  // coeff in obj function
                SCIP_VARTYPE_CONTINUOUS);               // type
            
            SCIP_CALL(SCIPaddVar(p_scip, variable_x_i_t));
            variable_x_i.push_back(variable_x_i_t);
        }

        m_variable_x.push_back(variable_x_i);

    }


    //* u

    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_VAR*> variable_u_i;
        int cost_start_i(p_ucp_instance->get_costs_startup()[i_unit]);

        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_VAR* variable_u_i_t;
            current_var_name.str("");
            current_var_name << "u_" << i_unit << "_" << i_time_step ;

            // Creation of the variable
            SCIP_CALL( SCIPcreateVarBasic( p_scip,
                &variable_u_i_t,                // pointer
                current_var_name.str().c_str(), // name
                0.,                             // lowerbound
                1.,                             // upperbound
                cost_start_i,                   // coeff in obj function
                SCIP_VARTYPE_CONTINUOUS));          // type

            // Adding the variable to the problem and the var matrix
            SCIP_CALL( SCIPaddVar(p_scip, variable_u_i_t));
            variable_u_i.push_back(variable_u_i_t);
        }
        m_variable_u.push_back(variable_u_i);
    }
   
   
    //* p

    vector<int> prod_max( p_ucp_instance->get_production_max() );
    vector<int> cost_prop( p_ucp_instance->get_costs_proportionnal() );
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {
        vector<SCIP_VAR*> variable_p_i;
        for(int i_time_step = 0; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_VAR* variable_p_i_t;
            current_var_name.str("");
            current_var_name << "p_" << i_unit << "_" << i_time_step ;

            // Creation of the variable
            SCIP_CALL( SCIPcreateVarBasic( p_scip,
                &variable_p_i_t,                // pointer
                current_var_name.str().c_str(), // name
                0,                     // lowerbound
                prod_max[i_unit],                              // upperbound
                cost_prop[i_unit],                    // coeff in obj function
                SCIP_VARTYPE_CONTINUOUS));      // type

            // Adding the variable to the problem and the var matrix
            SCIP_CALL( SCIPaddVar(p_scip, variable_p_i_t));
            variable_p_i.push_back(variable_p_i_t);
        }
        m_variable_p.push_back(variable_p_i);
    }
    
    return( SCIP_OKAY );
}




SCIP_RETCODE FormulationLinearRelaxation::create_constraints()
{
    // some usefull stuff
    ostringstream current_cons_name;
    int unit_number = p_ucp_instance->get_units_number();
    int time_step_number = p_ucp_instance->get_time_steps_number();


    // let's define all the constraints

    //* demand constraint

    for(int i_time_step = 0; i_time_step < time_step_number; i_time_step++)
    {
        SCIP_CONS* cons_demand_t;
        current_cons_name.str("");
        current_cons_name << "cons_demand_" << i_time_step;
        // creating the constraint
        SCIP_CALL(SCIPcreateConsBasicLinear(p_scip, 
            &cons_demand_t,                     /* constraint pointer */ 
            current_cons_name.str().c_str(),    /* constraint name */
            0,                                  /* number of variable added */
            nullptr,                            /* array of variable */
            nullptr,                            /* array of coefficient */
            p_ucp_instance->get_demand()[i_time_step],         /* LHS */
            +SCIPinfinity(p_scip)));              /* RHS */        
        // adding the variable
        for(int i_unit = 0; i_unit < unit_number; i_unit++)
        {
            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_demand_t,
                m_variable_p[i_unit][i_time_step],  /* variable to add */
                1.));                               /* coefficient */
        }
        SCIP_CALL( SCIPaddCons(p_scip, cons_demand_t));
        m_constraint_demand.push_back(cons_demand_t);
    }
    
    


    //* startup constraints 

    // the first initial state is different from the rest
    // we need to get the initial state, so we will define it first

    vector<int> initial_state(p_ucp_instance->get_initial_state());
    for(int i_unit = 0; i_unit < unit_number; i_unit++)
    {
        SCIP_CONS* cons_startup_i_0;
        current_cons_name.str("");
        current_cons_name << "cons_startup_" << i_unit << "_0";

        SCIP_CALL(SCIPcreateConsBasicLinear(p_scip, 
            &cons_startup_i_0,                  /* constraint pointer */ 
            current_cons_name.str().c_str(),    /* constraint name */
            0,                                  /* number of variable added */
            nullptr,                            /* array of variable */
            nullptr,                            /* array of coefficient */
            -SCIPinfinity(p_scip),              /* LHS */
            +initial_state[i_unit] ));          /* RHS */

        SCIP_CALL( SCIPaddCoefLinear( p_scip,
            cons_startup_i_0, 
            m_variable_x[i_unit][0],       /* variable to add */
            1));                /* coefficient */
        SCIP_CALL( SCIPaddCoefLinear( p_scip,
            cons_startup_i_0,
            m_variable_u[i_unit][0],                           /* variable to add */
            -1));                                    /* coefficient */

        SCIP_CALL( SCIPaddCons( p_scip, cons_startup_i_0));

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

            SCIP_CALL(SCIPcreateConsBasicLinear(p_scip, 
                &cons_startup_i_t,                  /* constraint pointer */ 
                current_cons_name.str().c_str(),    /* constraint name */
                0,                                  /* number of variable added */
                nullptr,                            /* array of variable */
                nullptr,                            /* array of coefficient */
                -SCIPinfinity(p_scip),                /* LHS */
                0 ));           /* RHS */   

            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_startup_i_t,
                m_variable_x[i_unit][i_time_step],      /* variable to add */
                1));                                    /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_startup_i_t,
                m_variable_x[i_unit][i_time_step - 1],  /* variable to add */
                -1));                                   /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_startup_i_t,
                m_variable_u[i_unit][i_time_step],      /* variable to add */
                -1));                                   /* coefficient */
            SCIP_CALL( SCIPaddCons( p_scip, cons_startup_i_t));
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
            SCIP_CALL(SCIPcreateConsBasicLinear( p_scip, 
                &cons_p_i_t,                      /** constraint pointer */ 
                current_cons_name.str().c_str(),             /** constraint name */
                0,                                  /** number of variable added */
                nullptr,                            /** array of variable */
                nullptr,                            /** array of coefficient */
                -SCIPinfinity(p_scip),                /** LHS */
                0));                                /** RHS */

            // add the variables
            int prod_max_i( p_ucp_instance->get_production_max()[i_unit]);

            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_p_i_t,
                m_variable_x[i_unit][i_time_step],                           /** variable to add */
                - prod_max_i ));     /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear( p_scip,
                cons_p_i_t,
                m_variable_p[i_unit][i_time_step],                           /** variable to add */
                1));                                    /** coefficient */

            // add the constraint
            SCIP_CALL( SCIPaddCons(p_scip, cons_p_i_t));
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
            SCIP_CALL(SCIPcreateConsBasicLinear( p_scip, 
                &cons_pmin_i_t,                      /** constraint pointer */ 
                current_cons_name.str().c_str(),             /** constraint name */
                0,                                  /** number of variable added */
                nullptr,                            /** array of variable */
                nullptr,                            /** array of coefficient */
                0,                /** LHS */
                SCIPinfinity(p_scip)));                                /** RHS */

            // add the variables
            int prod_min_i( p_ucp_instance->get_production_min()[i_unit] );

            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_pmin_i_t,
                m_variable_x[i_unit][i_time_step],                           /** variable to add */
                - prod_min_i ));     /* coefficient */
            SCIP_CALL( SCIPaddCoefLinear( p_scip,
                cons_pmin_i_t,
                m_variable_p[i_unit][i_time_step],                           /** variable to add */
                1));                                    /** coefficient */

            // add the constraint
            SCIP_CALL( SCIPaddCons(p_scip, cons_pmin_i_t));
        }
    }




    //* Minimum uptime constraint
    vector<int> min_uptime = p_ucp_instance->get_min_uptime();
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {

        vector<SCIP_CONS*> cons_min_uptime_i;



        for(int i_time_step = min_uptime[i_unit]; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_CONS* cons_min_uptime_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_min_uptime_" << i_unit << "_" << i_time_step;

            // create the constraint
            SCIP_CALL( SCIPcreateConsBasicLinear( p_scip, 
                &cons_min_uptime_i_t,                /** constraint pointer */ 
                current_cons_name.str().c_str(),         /** constraint name */
                0,                              /** number of variable added */
                nullptr,                        /** array of variable */
                nullptr,                        /** array of coefficient */
                -SCIPinfinity(p_scip),                              /** LHS */
                0));           /** RHS */
            // add the variables
            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_min_uptime_i_t,
                m_variable_x[i_unit][i_time_step],                           /** variable to add */
                -1));                                    /** coefficient */


            for(int i_time_step2 = i_time_step - min_uptime[i_unit] + 1 ; i_time_step2 < i_time_step + 1; i_time_step2++)
            {
                SCIP_CALL( SCIPaddCoefLinear( p_scip,
                    cons_min_uptime_i_t,
                    m_variable_u[i_unit][i_time_step2],             /** variable to add */
                    1));                                    /** coefficient */
            }
            // adding it to the problem
            SCIP_CALL( SCIPaddCons(p_scip, cons_min_uptime_i_t));
            cons_min_uptime_i.push_back(cons_min_uptime_i_t);
        }
        m_constraint_min_up_time.push_back(cons_min_uptime_i);
    }
    



    //* Minimum downtime constraint
    vector<int> min_downtime = p_ucp_instance->get_min_downtime();
    for(int i_unit = 0; i_unit < unit_number; i_unit ++)
    {

        vector<SCIP_CONS*> cons_min_downtime_i;

        for(int i_time_step = min_downtime[i_unit]; i_time_step < time_step_number; i_time_step ++)
        {
            SCIP_CONS* cons_min_downtime_i_t;
            current_cons_name.str("");
            current_cons_name << "cons_min_downtime_" << i_unit << "_" << i_time_step;

            // create the constraint
            SCIP_CALL( SCIPcreateConsBasicLinear( p_scip, 
                &cons_min_downtime_i_t,                /** constraint pointer */ 
                current_cons_name.str().c_str(),         /** constraint name */
                0,                              /** number of variable added */
                nullptr,                        /** array of variable */
                nullptr,                        /** array of coefficient */
                - SCIPinfinity(p_scip),                              /** LHS */
                1));           /** RHS */
            // add the variables
            SCIP_CALL( SCIPaddCoefLinear(p_scip,
                cons_min_downtime_i_t,
                m_variable_x[i_unit][i_time_step - min_downtime[i_unit]],                           /** variable to add */
                1));                                    /** coefficient */
            for(int i_time_step2 = i_time_step - min_downtime[i_unit] + 1 ; i_time_step2 < i_time_step + 1; i_time_step2++)
            {
                SCIP_CALL( SCIPaddCoefLinear( p_scip,
                    cons_min_downtime_i_t,
                    m_variable_u[i_unit][i_time_step2],             /** variable to add */
                    1));                                    /** coefficient */
            }

            // adding it to the problem
            SCIP_CALL( SCIPaddCons(p_scip, cons_min_downtime_i_t));
        }
     }

    return( SCIP_OKAY );
}





/* gets */

SCIP* FormulationLinearRelaxation::get_scip_pointer()
{
    return( p_scip );
}

std::vector< std::vector< SCIP_VAR* >> FormulationLinearRelaxation::get_variable_u()
{
    return( m_variable_u );
}

std::vector< std::vector< SCIP_VAR* >> FormulationLinearRelaxation::get_variable_x()
{
    return( m_variable_x );
}

std::vector< std::vector< SCIP_VAR* >> FormulationLinearRelaxation::get_variable_p()
{
    return( m_variable_p );
}



