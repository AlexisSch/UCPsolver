/**
 * @file main.cpp
 * 
 * * We pilote the solving of a UCP problem from here.=
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

// Unit decomposition 3
#include "UnitDecomposition3/FormulationMasterUnitDecomposition3.h"
#include "UnitDecomposition3/ObjPricerUCPUnitDecomposition3.h"

// Time decomposition
#include "TimeDecomposition/FormulationMasterTimeDecomposition.h"
#include "TimeDecomposition/ObjPricerUCPTimeDecomposition.h"

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
    
    //* solving preparation : data importation

    SCIP_RETCODE retcode;
    char* entry_file_name;
    char* exit_file_name;
    bool must_write_data_out = false;
    int time_limit(100);
    int display_verbosity(1); // 0 for nothing, 1 for basic stuff, 2 for EVERYTHING
 
    retcode = read_arguments(argc, argv, entry_file_name, exit_file_name, must_write_data_out, time_limit, display_verbosity );
    if ( retcode != SCIP_OKAY )
    {
        cerr << "Invalid filename entry; solving default instance. " << endl;
        entry_file_name = "/home/schlegel/Documents/UCPsolver/data/instance3.txt" ;
    }
    InstanceUCP *instance_ucp = new InstanceUCP( entry_file_name );


    clock_t begin_time = clock();
    if(display_verbosity > 0)
    {
        cout << "Welcome to this UCP solver !" << endl;
    }

    ofstream file;
    if( must_write_data_out )
    {
        file.open( exit_file_name , ios::out | ios::app);
        if( !file )
        {
            cerr << "Your filename is not working !" << endl;
            must_write_data_out = false;    
        } 
        else
        {
            file << instance_ucp->get_units_number() << ", " << instance_ucp->get_time_steps_number() << ", "   ;
        }   
    }





    //* SOLVING


    //* Compact 
    bool solve_compact = true;
    if( solve_compact )
    {
        begin_time = clock();

        // creation of the problem and solving
        SCIP* scip(0);
        SCIPcreate( &scip );
        SCIPincludeDefaultPlugins( scip );
        SCIPcreateProb(scip, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip, "display/verblevel", 0);
        FormulationCompact *formulation_compact = new FormulationCompact( instance_ucp, scip );
        SCIPsolve( scip );

        ProductionPlan* production_plan_compact = formulation_compact->get_production_plan_from_solution();

        double solution_cost( production_plan_compact->get_cost());
        double solving_time( float( clock () - begin_time ) /  CLOCKS_PER_SEC );
        
        // display
        if( display_verbosity > 0)
        {
            cout << "\n Compact Resolution : \n";
            cout << "Solution ost : " << solution_cost << "\n";
            cout << "Resolution time : " << solving_time << "\n";
        }
        if( display_verbosity > 1)
        {
            SCIPprintBestSol(scip, NULL, FALSE) ;
        }

        // write in the document
        if( must_write_data_out )
        {
            file << solution_cost << ", " << solving_time << ", " ;
        }

    }


    //* Linear relaxation
    bool solve_linear = true;
    if( solve_linear )
    {
        begin_time = clock();

        SCIP* scip(0);
        SCIPcreate( &scip );
        SCIPincludeDefaultPlugins( scip );
        SCIPcreateProb(scip, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip, "display/verblevel", 0);
        
        FormulationLinearRelaxation *formulation = new FormulationLinearRelaxation( instance_ucp, scip );
        SCIPsolve( scip );
        
        ProductionPlan* production_plan = formulation->get_production_plan_from_solution();
        
        double solution_cost( production_plan->get_cost());
        double solving_time( float( clock () - begin_time ) /  CLOCKS_PER_SEC );

        // display
        if( display_verbosity > 0)
        {
            cout << "\nLinear Relaxation : \n";
            cout << "Solution ost : " << solution_cost << "\n";
            cout << "Resolution time : " << solving_time << "\n";
        }
        if( display_verbosity > 1)
        {
            SCIPprintBestSol(scip, NULL, FALSE) ;
        }

        // write in the document
        if( must_write_data_out )
        {
            file << solution_cost << ", " << solving_time << ", " ;
        }

    }
 
    

    // //* Unit Decomposition 2
    bool solve_unit_decomposition2 = false;
    if( solve_unit_decomposition2 )
    {
        begin_time = clock();

        // Create the master problem
        SCIP* scip_master(0);
        SCIPcreate( &scip_master );
        SCIPincludeDefaultPlugins( scip_master );
        SCIPcreateProb(scip_master, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip_master, "display/verblevel", 0);
        FormulationMasterUnitDecomposition2* formulation_master = new FormulationMasterUnitDecomposition2( instance_ucp, scip_master );
        // Add the pricer
        static const char* PRICER_NAME = "pricer_ucp2";
        ObjPricerUCPUnitDecomposition2 *pricer_ucp = new ObjPricerUCPUnitDecomposition2( scip_master, PRICER_NAME, 
            formulation_master, 
            instance_ucp
        );
        SCIPincludeObjPricer(scip_master, pricer_ucp, true);
        SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME));

        // Solve
        SCIPsolve( scip_master );
        ProductionPlan* production_plan = formulation_master->get_production_plan_from_solution();
    
        // Post solving

        double cost( production_plan->get_cost());
        double solving_time( float( clock () - begin_time ) /  CLOCKS_PER_SEC );
        double number_of_columns( formulation_master->get_columns_number());
        if( display_verbosity > 0)
        {
            cout << "\nColumn Generation \nwith production constraints in subproblems \n";
            cout << "Solution ost : " << cost << "\n";
            cout << "Resolution time : " << solving_time << "\n";
            cout << "Number of columns : " << number_of_columns << endl;
        }
        if( display_verbosity > 1)
        {
            SCIPprintBestSol(scip_master, NULL, FALSE) ;
        }
        if( must_write_data_out )
        {
            file << cost << ", " << solving_time << ", " << number_of_columns << ", "  ;
        }

    }



 
    //* Unit Decomposition 3
    bool solve_unit_decomposition3 = true;
    if( solve_unit_decomposition3 )
    {

        begin_time = clock();

        // Create the master problem
        SCIP* scip_master(0);
        SCIPcreate( &scip_master );
        SCIPincludeDefaultPlugins( scip_master );
        SCIPcreateProb(scip_master, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip_master, "display/verblevel", 0);
        FormulationMasterUnitDecomposition3* formulation_master = new FormulationMasterUnitDecomposition3( instance_ucp, scip_master );
        // Add the pricer
        static const char* PRICER_NAME3 = "pricer";
        ObjPricerUCPUnitDecomposition3 *pricer = new ObjPricerUCPUnitDecomposition3( scip_master, PRICER_NAME3, 
            formulation_master, 
            instance_ucp
        );
        SCIPincludeObjPricer(scip_master, pricer, true);
        SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME3));
        // Solve 
        SCIPsolve( scip_master );
        ProductionPlan* production_plan = formulation_master->get_production_plan_from_solution();
    
        // Print
        double solution_cost( production_plan->get_cost());
        double solving_time( float( clock () - begin_time ) /  CLOCKS_PER_SEC );
        double columns_number( formulation_master->get_columns_number());

        if( display_verbosity > 0)
        {
            cout << "\nColumn Generation \nwith production constraints in master \n";
            cout << "Solution cost : " << solution_cost << "\n";
            cout << "Number of columns : " << columns_number << endl;
            cout << "Resolution time : " << solving_time << "\n";
            pricer->print_times();
        }
        if( display_verbosity > 1)
        {
            SCIPprintBestSol(scip_master, NULL, FALSE) ;
        }
        if( must_write_data_out )
        {
            file << solution_cost << ", " << solving_time << ", " << columns_number << ", "  ;
        }

    }



    //* Time Decomposition
    bool solve_time_decomposition = true;
    if (solve_time_decomposition)
    {
        begin_time = clock();
        // Create the master problem
        SCIP* scip_master(0);
        SCIPcreate( &scip_master );
        SCIPincludeDefaultPlugins( scip_master );
        SCIPcreateProb(scip_master, "UCP", 0, 0, 0, 0, 0, 0, 0);
        SCIPsetIntParam(scip_master, "display/verblevel", 0);
        FormulationMasterTimeDecomposition* formulation_master = new FormulationMasterTimeDecomposition( instance_ucp, scip_master );
        // Add the pricer
        static const char* PRICER_NAME_TIME = "pricer";
        ObjPricerUCPTimeDecomposition *pricer = new ObjPricerUCPTimeDecomposition( scip_master, PRICER_NAME_TIME, 
            formulation_master, 
            instance_ucp
        );
        SCIPincludeObjPricer(scip_master, pricer, true);
        SCIPactivatePricer(scip_master, SCIPfindPricer(scip_master, PRICER_NAME_TIME));
        // Solve 
        SCIPsolve( scip_master );
        ProductionPlan* production_plan = formulation_master->get_production_plan_from_solution();
    
        double solution_cost( production_plan->get_cost());
        double solving_time( float( clock () - begin_time ) /  CLOCKS_PER_SEC );
        double columns_number( formulation_master->get_columns_number());

        // display
        if( display_verbosity > 0)
        {
            cout << "\n Time decomposition \n";
            cout << "Solution ost : " << solution_cost << "\n";
            cout << "Resolution time : " << solving_time << "\n";
            cout << "Number of columns : " << columns_number << endl;
        }
        if( display_verbosity > 1)
        {
            SCIPprintBestSol(scip_master, NULL, FALSE) ;
        }

        // write in document
        if( must_write_data_out )
        {
            file << solution_cost << ", " << solving_time << ", " << columns_number << "," ;
        }
    }


    if( must_write_data_out )
    {
        file << endl;
    }


    if( display_verbosity > 0)
    {
        cerr << "\n\nProgram ends here. See you later ! " << endl;
    }
    



    return 0;
}



