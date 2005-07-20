/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2005 Tobias Achterberg                              */
/*                                                                           */
/*                  2002-2005 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic License.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: cmain.c,v 1.80 2005/07/20 16:35:13 bzfpfend Exp $"

/**@file   cmain.c
 * @brief  main file for C compilation
 * @author Tobias Achterberg
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <stdio.h>

#include "scip/scip.h"
#include "scip/scipdefplugins.h"


static
RETCODE readParams(
   SCIP*            scip,               /**< SCIP data structure */
   const char*      filename            /**< parameter file name, or NULL */
   )
{
   if( filename != NULL )
   {
      if( SCIPfileExists(filename) )
      {
         SCIPinfoMessage(scip, NULL, "reading parameter file <%s>\n", filename);
         CHECK_OKAY( SCIPreadParams(scip, filename) );
      }
      else
         SCIPinfoMessage(scip, NULL, "parameter file <%s> not found - using default parameters\n", filename);
   }
   else if( SCIPfileExists("scip.set") )
   {
      SCIPinfoMessage(scip, NULL, "reading parameter file <scip.set>\n");
      CHECK_OKAY( SCIPreadParams(scip, "scip.set") );
   }

   return SCIP_OKAY;
}

static
RETCODE fromCommandLine(
   SCIP*            scip,               /**< SCIP data structure */
   const char*      filename            /**< input file name */
   )
{
   /********************
    * Problem Creation *
    ********************/

   SCIPinfoMessage(scip, NULL, "\nread problem <%s>\n", filename);
   SCIPinfoMessage(scip, NULL, "============\n\n");
   CHECK_OKAY( SCIPreadProb(scip, filename) );


   /*******************
    * Problem Solving *
    *******************/

   /* solve problem */
   SCIPinfoMessage(scip, NULL, "\nsolve problem\n");
   SCIPinfoMessage(scip, NULL, "=============\n\n");
   CHECK_OKAY( SCIPsolve(scip) );

   SCIPinfoMessage(scip, NULL, "\nprimal solution:\n");
   SCIPinfoMessage(scip, NULL, "================\n\n");
   CHECK_OKAY( SCIPprintBestSol(scip, NULL) );


   /**************
    * Statistics *
    **************/

   SCIPinfoMessage(scip, NULL, "\nStatistics\n");
   SCIPinfoMessage(scip, NULL, "==========\n\n");

   CHECK_OKAY( SCIPprintStatistics(scip, NULL) );

   return SCIP_OKAY;
}

static
RETCODE interactive(
   SCIP*            scip                /**< SCIP data structure */
   )
{
   /* start user interactive mode */
   CHECK_OKAY( SCIPstartInteraction(scip) );

   return SCIP_OKAY;
}

static
RETCODE runSCIP(
   int              argc,
   char**           argv
   )
{
   SCIP* scip = NULL;


   /***********************
    * Version information *
    ***********************/

   SCIPprintVersion(NULL);


   /*********
    * Setup *
    *********/

   /* initialize SCIP */
   CHECK_OKAY( SCIPcreate(&scip) );
   SCIPinfoMessage(scip, NULL, "\n");

   /* include default SCIP plugins */
   CHECK_OKAY( SCIPincludeDefaultPlugins(scip) );


   /**************
    * Parameters *
    **************/

   if( argc >= 3 )
   {
      CHECK_OKAY( readParams(scip, argv[2]) );
   }
   else
   {
      CHECK_OKAY( readParams(scip, NULL) );
   }
   /*CHECK_OKAY( SCIPwriteParams(scip, "scip.set", TRUE) );*/


   /**************
    * Start SCIP *
    **************/

   if( argc >= 2 )
   {
      CHECK_OKAY( fromCommandLine(scip, argv[1]) );
   }
   else
   {
      SCIPinfoMessage(scip, NULL, "\n");

      CHECK_OKAY( interactive(scip) );
   }

   
   /********************
    * Deinitialization *
    ********************/

   CHECK_OKAY( SCIPfree(&scip) );

   checkEmptyMemory();

   return SCIP_OKAY;
}

int
main(
   int              argc,
   char**           argv
   )
{
   RETCODE retcode;

   /**@todo implement remaining events */
   /**@todo avoid addition of identical rows */
   /**@todo avoid addition of identical constraints */
   /**@todo pricing for pseudo solutions */
   /**@todo it's a bit ugly, that user call backs may be called before the nodequeue was processed */
   /**@todo unboundness detection in presolving -> convert problem into feasibility problem to decide
    *       unboundness/infeasibility */
   /**@todo variable event PSSOLCHANGED, update pseudo activities in constraints to speed up checking of pseudo solutions */
   /**@todo branching rule acting as a filter by temporary changing the branching priority of variables and returning 
    *       SCIP_DIDNOTFIND to let the next branching rule select the branching variable */
   /**@todo use aging in all constraint handlers */
   /**@todo try to not use the first but the shortest constraint as reason for a deduction */

   retcode = runSCIP(argc, argv);
   if( retcode != SCIP_OKAY )
   {
      SCIPprintError(retcode, stderr);
      return -1;
   }

   return 0;
}
