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
#pragma ident "@(#) $Id: prop_pseudoobj.c,v 1.5 2005/01/21 09:17:02 bzfpfend Exp $"

/**@file   prop_pseudoobj.c
 * @brief  pseudoobj propagator
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>

#include "prop_pseudoobj.h"


#define PROP_NAME              "pseudoobj"
#define PROP_DESC              "pseudo objective function propagator"
#define PROP_PRIORITY                 0
#define PROP_FREQ                     1

#define DEFAULT_MAXCANDS            100 /**< maximal number of variables to look at in a single propagation round
                                         *   (-1: process all variables) */




/*
 * Data structures
 */

/** propagator data */
struct PropData
{
   int              maxcands;           /**< maximal number of variables to look at in a single propagation round */
   int              lastvarnum;         /**< last variable number that was looked at */
};




/*
 * Local methods
 */

/* put your local methods here, and declare them static */




/*
 * Callback methods of propagator
 */

/** destructor of propagator to free user data (called when SCIP is exiting) */
static
DECL_PROPFREE(propFreePseudoobj)
{  /*lint --e{715}*/
   PROPDATA* propdata;

   /* free branching rule data */
   propdata = SCIPpropGetData(prop);
   SCIPfreeMemory(scip, &propdata);
   SCIPpropSetData(prop, NULL);

   return SCIP_OKAY;
}


/** initialization method of propagator (called after problem was transformed) */
#define propInitPseudoobj NULL


/** deinitialization method of propagator (called before transformed problem is freed) */
#define propExitPseudoobj NULL


/** execution method of propagator */
static
DECL_PROPEXEC(propExecPseudoobj)
{  /*lint --e{715}*/
   PROPDATA* propdata;
   VAR** vars;
   VAR* var;
   Real pseudoobjval;
   Real cutoffbound;
   Real obj;
   Real lb;
   Real ub;
   int ncands;
   int nvars;
   int c;
   int v;

   *result = SCIP_DIDNOTRUN;

   propdata = SCIPpropGetData(prop);
   assert(propdata != NULL);

   /* get current pseudo objective value and cutoff bound */
   pseudoobjval = SCIPgetPseudoObjval(scip);
   if( SCIPisInfinity(scip, -pseudoobjval) )
      return SCIP_OKAY;
   cutoffbound = SCIPgetCutoffbound(scip);
   if( SCIPisInfinity(scip, cutoffbound) )
      return SCIP_OKAY;
   if( SCIPisGE(scip, pseudoobjval, cutoffbound) )
   {
      *result = SCIP_CUTOFF;
      return SCIP_OKAY;
   }

   debugMessage("propagating pseudo objective function\n");

   *result = SCIP_DIDNOTFIND;

   /* tighten domains, if they would increase the pseudo objective value above the upper bound */
   vars = SCIPgetVars(scip);
   nvars = SCIPgetNVars(scip);
   ncands = (propdata->maxcands >= 0 ? MIN(propdata->maxcands, nvars) : nvars);
   v = propdata->lastvarnum;
   for( c = 0; c < ncands; ++c )
   {
      v++;
      if( v >= nvars )
         v = 0;
      var = vars[v];
      lb = SCIPvarGetLbLocal(var);
      ub = SCIPvarGetUbLocal(var);
      if( SCIPisFeasEQ(scip, lb, ub) )
         continue;
      obj = SCIPvarGetObj(var);

      if( SCIPisPositive(scip, obj) )
      {
         Real newub;
         Bool infeasible;
         Bool tightened;

         newub = lb + (cutoffbound - pseudoobjval)/obj;
         if( SCIPisUbBetter(scip, newub, ub) )
         {
            debugMessage(" -> new upper bound of variable <%s>[%g,%g]: %g\n", SCIPvarGetName(var), lb, ub, newub);
            CHECK_OKAY( SCIPinferVarUbProp(scip, var, newub, prop, 0, &infeasible, &tightened) );
            assert(!infeasible);
            assert(tightened);
            *result = SCIP_REDUCEDDOM;
         }
      }
      else if( SCIPisNegative(scip, obj) )
      {
         Real newlb;
         Bool infeasible;
         Bool tightened;

         newlb = ub + (cutoffbound - pseudoobjval)/obj;
         if( SCIPisLbBetter(scip, newlb, lb) )
         {
            debugMessage(" -> new lower bound of variable <%s>[%g,%g]: %g\n", SCIPvarGetName(var), lb, ub, newlb);
            CHECK_OKAY( SCIPinferVarLbProp(scip, var, newlb, prop, 0, &infeasible, &tightened) );
            assert(!infeasible);
            assert(tightened);
            *result = SCIP_REDUCEDDOM;
         }
      }
   }
   propdata->lastvarnum = v;

   return SCIP_OKAY;
}


/** propagation conflict resolving method of propagator */
static
DECL_PROPRESPROP(propRespropPseudoobj)
{  /*lint --e{715}*/
   VAR** vars;
   VAR* var;
   Real obj;
   int nvars;
   int v;

   /* the variables responsible for the propagation are the ones with
    *  - obj > 0 and local lb > global lb
    *  - obj < 0 and local ub < global ub
    */
   vars = SCIPgetVars(scip);
   nvars = SCIPgetNVars(scip);
   for( v = 0; v < nvars; ++v )
   {
      var = vars[v];
      if( var == infervar )
         continue;

      obj = SCIPvarGetObj(var);
      if( SCIPisPositive(scip, obj) )
      {
         Real loclb;
         Real glblb;

         glblb = SCIPvarGetLbGlobal(var);
         loclb = SCIPvarGetLbAtIndex(var, bdchgidx, FALSE);
         if( SCIPisGT(scip, loclb, glblb) )
         {
            CHECK_OKAY( SCIPaddConflictLb(scip, var, bdchgidx) );
         }
      }
      else if( SCIPisNegative(scip, obj) )
      {
         Real locub;
         Real glbub;

         glbub = SCIPvarGetUbGlobal(var);
         locub = SCIPvarGetUbAtIndex(var, bdchgidx, FALSE);
         if( SCIPisLT(scip, locub, glbub) )
         {
            CHECK_OKAY( SCIPaddConflictUb(scip, var, bdchgidx) );
         }
      }
   }

   *result = SCIP_SUCCESS;

   return SCIP_OKAY;
}




/*
 * propagator specific interface methods
 */

/** creates the pseudo objective function propagator and includes it in SCIP */
RETCODE SCIPincludePropPseudoobj(
   SCIP*            scip                /**< SCIP data structure */
   )
{
   PROPDATA* propdata;

   /* create pseudoobj propagator data */
   CHECK_OKAY( SCIPallocMemory(scip, &propdata) );
   propdata->lastvarnum = -1;

   /* include propagator */
   CHECK_OKAY( SCIPincludeProp(scip, PROP_NAME, PROP_DESC, PROP_PRIORITY, PROP_FREQ,
         propFreePseudoobj, propInitPseudoobj, propExitPseudoobj, propExecPseudoobj, propRespropPseudoobj,
         propdata) );

   /* add pseudoobj propagator parameters */
   CHECK_OKAY( SCIPaddIntParam(scip,
         "propagating/pseudoobj/maxcands", 
         "maximal number of variables to look at in a single propagation round (-1: process all variables)",
         &propdata->maxcands, DEFAULT_MAXCANDS, -1, INT_MAX, NULL, NULL) );

   return SCIP_OKAY;
}
