/**
 * @file main.cpp
 * 
 * * We pilote the solving of a UCP problem from here.
 * todo : understand what namespace is, errors catching ? 
 * todo : put on github
 * change the class productionplan, it has two use at the same time, circular dependance
 * create the destructors and the gets, use smard pointers, add commentaries, etc.
*/


/**
 * * Welcome to this code used to solve the UCP
 * 
 * 
 * 
 * More informations in the README.md
 * 
 * some notes : using visual studio with a bunch of extensions to make it look better
*/





//* Standart includes
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

//* SCIP includes
#include <scip/scipdefplugins.h>
#include <scip/scip.h>

//* User includes

// Data includes
#include "DataClasses/InstanceUCP.h"
#include "DataClasses/ProductionPlan.h"

// Other Resolutions includes
#include "OtherResolution/FormulationCompact.h"
#include "OtherResolution/FormulationLinearRelaxation.h"

// Decomposition
#include "Decomposition/FormulationMaster.h"
#include "Decomposition/VariableMaster.h"

// Unit decomposition
#include "UnitDecomposition/FormulationMasterUnitDecomposition.h"
#include "UnitDecomposition/ObjPricerUCPUnitDecomposition.h"


//* Namespaces
using namespace std;


/* read arguments from the console entry */
SCIP_RETCODE read_arguments( int argc, 
    char** argv, 
    char* &entry_file_name, 
    char* &exit_file_name, 
    bool &must_write_data_out, 
    int &time_limit, 
    int &display_verbosity )
{
    must_write_data_out = false;
    time_limit = 100;
    display_verbosity = 0;

    if(argc < 2)
    {
        cerr << "ERROR : no file provided" << endl;    
        return(SCIP_READERROR);
    }   
    if (argc >= 2)
    {
        entry_file_name = argv[1];
    } 
    if (argc >= 3)
    {
        exit_file_name = argv[2];
        must_write_data_out = true;
    }
    if (argc >= 4)
    {
        time_limit = atof(argv[3]);
    }
    if( argc >= 5 )
    {
        display_verbosity = atof( argv[4] );
    }
    if (argc >= 6)
    {
        cerr << "Warning, too many arguments provided..." << endl;
    }



    return(SCIP_OKAY);
}




/* main function */
int main(
    int argc,
    char** argv
    )
{

    cerr << "Welcome to this UCP solver ! " << endl << endl;

    //* Definition of usefull parameters
    SCIP_RETCODE retcode;
    char* entry_file_name;
    char* exit_file_name;
    bool must_write_data_out;
    int time_limit;
    int display_verbosity;

    retcode = read_arguments(argc, argv, entry_file_name, exit_file_name, must_write_data_out, time_limit, display_verbosity );
    if ( retcode != SCIP_OKAY )
    {
        return -1;
    }
    

    //* Get informations about the instance
    InstanceUCP *instance = new InstanceUCP( entry_file_name );


    //* Compact resolution

    SCIP* scip_compact(0);
    SCIPcreate( &scip_compact );
    SCIPincludeDefaultPlugins( scip_compact );
    SCIPcreateProb(scip_compact, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_compact, "display/verblevel", display_verbosity);
    
    FormulationCompact *formulation_compact = new FormulationCompact( instance, scip_compact );
    SCIPsolve( scip_compact );
    ProductionPlan production_plan_compact( instance, formulation_compact);
    cerr << "Cost of integer optimum : " << production_plan_compact.get_cost() << endl;


    //* Linear relaxation

    SCIP* scip_lr(0);
    SCIPcreate( &scip_lr );
    SCIPincludeDefaultPlugins( scip_lr );
    SCIPcreateProb(scip_lr, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_lr, "display/verblevel", display_verbosity);
    
    FormulationLinearRelaxation *formulation_lr = new FormulationLinearRelaxation( instance, scip_lr );
    SCIPsolve( scip_lr );
    ProductionPlan production_plan_linear_relaxation( instance, formulation_lr);
    cerr << "Cost of linear relaxation : " << production_plan_linear_relaxation.get_cost() << endl;



    //* Column generation

    
    // Create the master problem and add the pricer
    SCIP* scip_master(0);
    SCIPcreate( &scip_master );
    SCIPincludeDefaultPlugins( scip_master );
    SCIPcreateProb(scip_master, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_master, "display/verblevel", display_verbosity);

    FormulationMasterUnitDecomposition* formulation_master = new FormulationMasterUnitDecomposition( instance, scip_master );

    static const char* PRICER_NAME = "Pricer_UCP";

    ObjPricerUCPUnitDecomposition *pricer_ucp = new ObjPricerUCPUnitDecomposition( scip_master, PRICER_NAME, 
        formulation_master, 
        instance
    );
    
    SCIPincludeObjPricer(scip_master, pricer_ucp, true);
    SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME));


    // create and add first column, with the worst plan, but working
    ProductionPlan* first_production_plan = new ProductionPlan( instance );
    first_production_plan->computeCost();
    string column_name = "column_0" ; 
    SCIP_VAR* p_variable;
    SCIPcreateVar(  scip_master,
        &p_variable,                            // pointer 
        column_name.c_str(),                            // name
        0.,                                     // lowerbound
        +SCIPinfinity( scip_master),            // upperbound
        first_production_plan->get_cost(),          // coeff in obj function
        SCIP_VARTYPE_CONTINUOUS,
        true, false, NULL, NULL, NULL, NULL, NULL);
    SCIPaddVar(scip_master, p_variable);
    VariableMaster* first_column = new VariableMaster( p_variable, first_production_plan);
    formulation_master->add_column( first_column );


    // solve
    SCIPsolve( scip_master );

    SCIPprintBestSol(scip_master, NULL, FALSE) ;

    ProductionPlan* production_plan_column_generation = formulation_master->get_production_plan_from_solution();
    production_plan_column_generation->show();



    cerr << "Program ends here. See you later ! " << endl;
    return 0;
}



