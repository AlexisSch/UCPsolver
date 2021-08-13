/**
 * @file main.cpp
 * 
 * * We pilote the solving of a UCP problem from here.
 * todo : understand what namespace is, errors catching ? 
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
#include <ctime>

//* SCIP includes
//#include <scip/scipdefplugins.h>
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

// Unit decomposition 2
#include "UnitDecomposition2/FormulationMasterUnitDecomposition2.h"
#include "UnitDecomposition2/ObjPricerUCPUnitDecomposition2.h"

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
    

    //* Get informations about the instance_ucp
    InstanceUCP *instance_ucp = new InstanceUCP( entry_file_name );

    clock_t begin_time = clock();
    
    //* Compact resolution

    SCIP* scip_compact(0);
    SCIPcreate( &scip_compact );
    SCIPincludeDefaultPlugins( scip_compact );
    SCIPcreateProb(scip_compact, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_compact, "display/verblevel", display_verbosity);
    
    FormulationCompact *formulation_compact = new FormulationCompact( instance_ucp, scip_compact );
    SCIPsolve( scip_compact );

    ProductionPlan* production_plan_compact = formulation_compact->get_production_plan_from_solution();
    cout << "Cost of integer optimum : " << production_plan_compact->get_cost() << endl;
    cout << "Resolution time : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

    //* Linear relaxation
    begin_time = clock();
    SCIP* scip_lr(0);
    SCIPcreate( &scip_lr );
    SCIPincludeDefaultPlugins( scip_lr );
    SCIPcreateProb(scip_lr, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_lr, "display/verblevel", display_verbosity);
    
    FormulationLinearRelaxation *formulation_lr = new FormulationLinearRelaxation( instance_ucp, scip_lr );
    SCIPsolve( scip_lr );

    ProductionPlan* production_plan_linear_relaxation = formulation_lr->get_production_plan_from_solution();
    cout << "Cost of linear relaxation : " << production_plan_linear_relaxation->get_cost() << endl;
    cout << "Resolution time : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;   

    
    //* Column generation 1

    cerr << "\nFirst column generation" << endl;
    
    // Create the master problem
    SCIP* scip_master(0);
    SCIPcreate( &scip_master );
    SCIPincludeDefaultPlugins( scip_master );
    SCIPcreateProb(scip_master, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_master, "display/verblevel", display_verbosity);
    FormulationMasterUnitDecomposition* formulation_master = new FormulationMasterUnitDecomposition( instance_ucp, scip_master );

    // Add the pricer
    static const char* PRICER_NAME = "Pricer_UCP";
    ObjPricerUCPUnitDecomposition *pricer_ucp = new ObjPricerUCPUnitDecomposition( scip_master, PRICER_NAME, 
        formulation_master, 
        instance_ucp
    );

    SCIPincludeObjPricer(scip_master, pricer_ucp, true);
    SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME));

    // Solve
    SCIPsolve( scip_master );
    ProductionPlan* production_plan_column_generation = formulation_master->get_production_plan_from_solution();
    // Print
    cout << "Cost of unit decomposition 2 : " << production_plan_column_generation->get_cost() << endl;
    cout << "Resolution time : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;   



    //* Column generation 2

    cerr << "\nSecond Column generation" << endl;
    begin_time = clock();
    // Create the master problem
    SCIP* scip_master2(0);
    SCIPcreate( &scip_master2 );
    SCIPincludeDefaultPlugins( scip_master2 );
    SCIPcreateProb(scip_master2, "UCP", 0, 0, 0, 0, 0, 0, 0);
    SCIPsetIntParam(scip_master2, "display/verblevel", display_verbosity);
    FormulationMasterUnitDecomposition2* formulation_master2 = new FormulationMasterUnitDecomposition2( instance_ucp, scip_master2 );
    // Add the pricer
    static const char* PRICER_NAME2 = "Pricer_UCP2";
    ObjPricerUCPUnitDecomposition2 *pricer_ucp2 = new ObjPricerUCPUnitDecomposition2( scip_master2, PRICER_NAME2, 
        formulation_master2, 
        instance_ucp
    );
    SCIPincludeObjPricer(scip_master2, pricer_ucp2, true);
    SCIPactivatePricer(scip_master2, SCIPfindPricer(scip_master2, PRICER_NAME2));

    // Solve
    SCIPsolve( scip_master2 );
    ProductionPlan* production_plan_column_generation2 = formulation_master2->get_production_plan_from_solution();
   
    // Print
    //SCIPprintBestSol(scip_master2, NULL, FALSE) ;
    cout << "Cost of unit decomposition 2 : " << production_plan_column_generation2->get_cost() << endl;


    cout << "Resolution time : " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;




    cerr << "Program ends here. See you later ! " << endl;
    return 0;
}



