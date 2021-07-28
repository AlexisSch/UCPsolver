
/**
 *  @file ProductionPlan.cpp
 *  Implement the class ProductionPlan 
 *  todo : Change the way it is working. Poor designed right now !
*/



//** Includes

//* Standart
#include <vector>
#include <iostream>


//* SCIP
#include <scip/scipdefplugins.h>
#include <scip/scip.h>


//* User

// general
#include "DataClasses/InstanceUCP.h"
#include "DataClasses/ProductionPlan.h"

// Other formulations
#include "OtherResolution/FormulationCompact.h"
#include "OtherResolution/FormulationLinearRelaxation.h"

// Unit Decomposition
#include "UnitDecomposition/FormulationPricerUnitDecomposition.h"


//** Namespaces
using namespace std; 




ProductionPlan::ProductionPlan( InstanceUCP* instance_ucp )
{
    p_instance = instance_ucp;

    // production plan with all the units working at full capacity at every time steps

    int number_units( p_instance-> get_units_number() );
    int number_time_steps( p_instance-> get_time_steps_number() );

    // up and down plan : 1 for every units at every time steps
    m_up_down_plan.resize( number_units );
    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        m_up_down_plan[i_unit].resize( number_time_steps, 1);
    }

    // switching plan : 1 if not active before, 0 if
    m_switch_plan.resize(number_units) ;
    vector<int> initial_state = p_instance->get_initial_state();
    for( int i_unit = 0; i_unit < number_units; i_unit ++ )
    {
        m_switch_plan[i_unit].resize( number_time_steps, 0);
        if( initial_state[i_unit] == 0 )
        {
            m_switch_plan[i_unit][0] = 1;
        }
    }

    // production values : maximum everytime
    m_quantity_plan.resize( number_units );
    vector<int> production_max( instance_ucp->get_production_max() );
    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        m_quantity_plan[i_unit].resize( number_time_steps, production_max[i_unit]);
    }


    computeCost();

}   


ProductionPlan::ProductionPlan( InstanceUCP* instance_ucp, FormulationPricerUnitDecomposition* formulation_pricer )
{
    p_instance = instance_ucp;
    transform_solution_in_plan( formulation_pricer );
    computeCost();
}


ProductionPlan::ProductionPlan( InstanceUCP* instance_ucp, FormulationCompact* formulation_compact )
{
    p_instance = instance_ucp;
    transform_solution_in_plan( formulation_compact );
    computeCost();
}


ProductionPlan::ProductionPlan( InstanceUCP* instance_ucp, FormulationLinearRelaxation* formulation_linear )
{
    p_instance = instance_ucp;
    transform_solution_in_plan( formulation_linear );
    computeCost();
}


ProductionPlan::~ProductionPlan()
{
}

void ProductionPlan::computeCost()
{
    m_cost = 0;

    int number_units( p_instance-> get_units_number() );
    int number_time_steps( p_instance-> get_time_steps_number() );
    vector<int> costs_fixed( p_instance->get_costs_fixed() );
    vector<int> costs_proportionnal( p_instance->get_costs_proportionnal() );
    vector<int> costs_startup( p_instance->get_costs_startup() );

    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step ++)
        {
            m_cost += m_up_down_plan[i_unit][i_time_step] * costs_fixed[i_unit] ;
            m_cost += m_switch_plan[i_unit][i_time_step] * costs_startup[i_unit] ;
            m_cost += m_quantity_plan[i_unit][i_time_step] * costs_proportionnal[i_unit] ;
        }
    }

    return;
}


void ProductionPlan::show()
{
    cout << "Printing the production plan : ";
    
    int number_units( p_instance-> get_units_number() );
    int number_time_steps( p_instance-> get_time_steps_number() );

    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        cout << "\nUnit " << i_unit << " :";
        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++ )
        {
            cout << " " << m_up_down_plan[i_unit][i_time_step];
        }
    }

    cout << "\nSwitch plan : " ;
    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        cout << "\nUnit " << i_unit << " :";
        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++ )
        {
            cout << " " << m_switch_plan[i_unit][i_time_step];
        }
    }

    cout << "\nProduction values : ";
    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        cout << "\nUnit " << i_unit << " :";
        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++ )
        {
            cout << " " << m_quantity_plan[i_unit][i_time_step];
        }
    }

    cout << "\nCost of the plan : " << m_cost << endl;

}


double ProductionPlan::get_cost()
{
    return( m_cost );
}


vector< vector< double > > ProductionPlan::get_quantity_plan()
{
    return( m_quantity_plan );
}


void ProductionPlan::transform_solution_in_plan( FormulationPricerUnitDecomposition* formulation_pricer )
{

    SCIP* scip_pricer = formulation_pricer->get_scip_pointer();
    SCIP_SOL *solution = SCIPgetBestSol( scip_pricer );

    vector< vector < SCIP_VAR* > > variable_x = formulation_pricer->get_variable_x();
    vector< vector< SCIP_VAR* > > variable_u = formulation_pricer->get_variable_u();
    vector< vector< SCIP_VAR* > > variable_p = formulation_pricer->get_variable_p();

    int number_unit( variable_x.size() );
    int number_time_steps( variable_x[0].size() );

    m_up_down_plan.resize(number_unit);
    m_switch_plan.resize(number_unit);
    m_quantity_plan.resize(number_unit);

    for( int i_unit = 0; i_unit < number_unit; i_unit ++)
    {
        m_up_down_plan[i_unit].resize(number_time_steps);
        m_switch_plan[i_unit].resize(number_time_steps);
        m_quantity_plan[i_unit].resize(number_time_steps);

        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++)
        {
            m_up_down_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_x[i_unit][i_time_step]);
            m_switch_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_u[i_unit][i_time_step]);
            m_quantity_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_p[i_unit][i_time_step]);
        }   
    } 

}


void ProductionPlan::transform_solution_in_plan( FormulationCompact* formulation_compact )
{

    SCIP* scip_pricer = formulation_compact->get_scip_pointer();
    SCIP_SOL *solution = SCIPgetBestSol( formulation_compact->get_scip_pointer() );

    vector< vector < SCIP_VAR* > > variable_x = formulation_compact->get_variable_x();
    vector< vector< SCIP_VAR* > > variable_u = formulation_compact->get_variable_u();
    vector< vector< SCIP_VAR* > > variable_p = formulation_compact->get_variable_p();

    int number_unit( variable_x.size() );
    int number_time_steps( variable_x[0].size() );

    m_up_down_plan.resize(number_unit);
    m_switch_plan.resize(number_unit);
    m_quantity_plan.resize(number_unit);

    for( int i_unit = 0; i_unit < number_unit; i_unit ++)
    {
        m_up_down_plan[i_unit].resize(number_time_steps);
        m_switch_plan[i_unit].resize(number_time_steps);
        m_quantity_plan[i_unit].resize(number_time_steps);

        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++)
        {
            m_up_down_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_x[i_unit][i_time_step]);
            m_switch_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_u[i_unit][i_time_step]);
            m_quantity_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_p[i_unit][i_time_step]);
        }   
    } 

}


void ProductionPlan::transform_solution_in_plan( FormulationLinearRelaxation* formulation_linear )
{

    SCIP* scip_pricer = formulation_linear->get_scip_pointer();
    SCIP_SOL *solution = SCIPgetBestSol( formulation_linear->get_scip_pointer() );

    vector< vector < SCIP_VAR* > > variable_x = formulation_linear->get_variable_x();
    vector< vector< SCIP_VAR* > > variable_u = formulation_linear->get_variable_u();
    vector< vector< SCIP_VAR* > > variable_p = formulation_linear->get_variable_p();

    int number_unit( variable_x.size() );
    int number_time_steps( variable_x[0].size() );

    m_up_down_plan.resize(number_unit);
    m_switch_plan.resize(number_unit);
    m_quantity_plan.resize(number_unit);

    for( int i_unit = 0; i_unit < number_unit; i_unit ++)
    {
        m_up_down_plan[i_unit].resize(number_time_steps);
        m_switch_plan[i_unit].resize(number_time_steps);
        m_quantity_plan[i_unit].resize(number_time_steps);

        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++)
        {
            m_up_down_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_x[i_unit][i_time_step]);
            m_switch_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_u[i_unit][i_time_step]);
            m_quantity_plan[i_unit][i_time_step] = SCIPgetSolVal( scip_pricer, solution, variable_p[i_unit][i_time_step]);
        }   
    } 

}



void ProductionPlan::empty_plan()
{
    int number_units( m_up_down_plan.size());
    int number_time_steps( m_up_down_plan[0].size());

    for( int i_units(0); i_units < number_units; i_units ++ )
    {
        for( int i_time_step(0); i_time_step < number_time_steps; i_time_step ++ )
        {
            m_up_down_plan[i_units][i_time_step] = 0;
            m_switch_plan[i_units][i_time_step] = 0;
            m_quantity_plan[i_units][i_time_step] = 0;
        }
    }
}



void ProductionPlan::add_column_values( ProductionPlan* production_plan_to_add, SCIP_Real coefficient_column )
{
    int number_units( p_instance->get_units_number());
    int number_time_steps( p_instance->get_time_steps_number());

    for( int i_unit = 0; i_unit < number_units; i_unit ++)
    {
        for( int i_time_step = 0; i_time_step < number_time_steps; i_time_step++ )
        {
            m_up_down_plan[i_unit][i_time_step] += production_plan_to_add->m_up_down_plan[i_unit][i_time_step]*coefficient_column;
            m_switch_plan[i_unit][i_time_step] += production_plan_to_add->m_switch_plan[i_unit][i_time_step]*coefficient_column;
            m_quantity_plan[i_unit][i_time_step] += production_plan_to_add->m_quantity_plan[i_unit][i_time_step]*coefficient_column; 
        }
    }
}



