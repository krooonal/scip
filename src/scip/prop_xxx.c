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
#pragma ident "@(#) $Id: prop_xxx.c,v 1.3 2005/01/21 09:17:02 bzfpfend Exp $"

/**@file   prop_xxx.c
 * @brief  xxx propagator
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>

#include "prop_xxx.h"


#define PROP_NAME              "xxx"
#define PROP_DESC              "propagator template"
#define PROP_PRIORITY                 0
#define PROP_FREQ                    10




/*
 * Data structures
 */

/* TODO: fill in the necessary propagator data */

/** propagator data */
struct PropData
{
};




/*
 * Local methods
 */

/* put your local methods here, and declare them static */




/*
 * Callback methods of propagator
 */

/* TODO: Implement all necessary propagator methods. The methods with an #if 0 ... #else #define ... are optional */

/** destructor of propagator to free user data (called when SCIP is exiting) */
#if 0
static
DECL_PROPFREE(propFreeXxx)
{  /*lint --e{715}*/
   errorMessage("method of xxx propagator not implemented yet\n");
   abort(); /*lint --e{527}*/

   return SCIP_OKAY;
}
#else
#define propFreeXxx NULL
#endif


/** initialization method of propagator (called after problem was transformed) */
#if 0
static
DECL_PROPINIT(propInitXxx)
{  /*lint --e{715}*/
   errorMessage("method of xxx propagator not implemented yet\n");
   abort(); /*lint --e{527}*/

   return SCIP_OKAY;
}
#else
#define propInitXxx NULL
#endif


/** deinitialization method of propagator (called before transformed problem is freed) */
#if 0
static
DECL_PROPEXIT(propExitXxx)
{  /*lint --e{715}*/
   errorMessage("method of xxx propagator not implemented yet\n");
   abort(); /*lint --e{527}*/

   return SCIP_OKAY;
}
#else
#define propExitXxx NULL
#endif


/** execution method of propagator */
static
DECL_PROPEXEC(propExecXxx)
{  /*lint --e{715}*/
   errorMessage("method of xxx propagator not implemented yet\n");
   abort(); /*lint --e{527}*/

   return SCIP_OKAY;
}


/** propagation conflict resolving method of propagator */
static
DECL_PROPRESPROP(propRespropXxx)
{  /*lint --e{715}*/
   errorMessage("method of xxx propagator not implemented yet\n");
   abort(); /*lint --e{527}*/

   return SCIP_OKAY;
}




/*
 * propagator specific interface methods
 */

/** creates the xxx propagator and includes it in SCIP */
RETCODE SCIPincludePropXxx(
   SCIP*            scip                /**< SCIP data structure */
   )
{
   PROPDATA* propdata;

   /* create xxx propagator data */
   propdata = NULL;
   /* TODO: (optional) create propagator specific data here */

   /* include propagator */
   CHECK_OKAY( SCIPincludeProp(scip, PROP_NAME, PROP_DESC, PROP_PRIORITY, PROP_FREQ,
         propFreeXxx, propInitXxx, propExitXxx, propExecXxx, propRespropXxx,
         propdata) );

   /* add xxx propagator parameters */
   /* TODO: (optional) add propagator specific parameters with SCIPaddTypeParam() here */

   return SCIP_OKAY;
}
