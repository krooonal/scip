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
#pragma ident "@(#) $Id: type_prop.h,v 1.3 2005/01/21 09:17:12 bzfpfend Exp $"

/**@file   type_prop.h
 * @brief  type definitions for propagators
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __TYPE_PROP_H__
#define __TYPE_PROP_H__


typedef struct Prop PROP;               /**< propagator */
typedef struct PropData PROPDATA;       /**< locally defined propagator data */


/** destructor of propagator to free user data (called when SCIP is exiting)
 *
 *  input:
 *  - scip            : SCIP main data structure
 *  - prop            : the propagator itself
 */
#define DECL_PROPFREE(x) RETCODE x (SCIP* scip, PROP* prop)

/** initialization method of propagator (called after problem was transformed)
 *
 *  input:
 *  - scip            : SCIP main data structure
 *  - prop            : the propagator itself
 */
#define DECL_PROPINIT(x) RETCODE x (SCIP* scip, PROP* prop)

/** deinitialization method of propagator (called before transformed problem is freed)
 *
 *  input:
 *  - scip            : SCIP main data structure
 *  - prop            : the propagator itself
 */
#define DECL_PROPEXIT(x) RETCODE x (SCIP* scip, PROP* prop)

/** execution method of propagator
 *
 *  Searches for domain propagations. The method is called in the node processing loop.
 *
 *  input:
 *  - scip            : SCIP main data structure
 *  - prop            : the propagator itself
 *  - result          : pointer to store the result of the propagation call
 *
 *  possible return values for *result:
 *  - SCIP_CUTOFF     : the current node is infeasible for the current domains
 *  - SCIP_REDUCEDDOM : at least one domain reduction was found
 *  - SCIP_DIDNOTFIND : the propagator searched, but did not find a domain reduction
 *  - SCIP_DIDNOTRUN  : the propagator was skipped
 */
#define DECL_PROPEXEC(x) RETCODE x (SCIP* scip, PROP* prop, RESULT* result)


/** propagation conflict resolving method of propagator
 *
 *  This method is called during conflict analysis. If the propagator wants to support conflict analysis,
 *  it should call SCIPinferVarLbProp() or SCIPinferVarUbProp() in domain propagation instead of SCIPchgVarLb() or
 *  SCIPchgVarUb() in order to deduce bound changes on variables.
 *  In the SCIPinferVarLbProp() and SCIPinferVarUbProp() calls, the propagator provides a pointer to itself
 *  and an integer value "inferinfo" that can be arbitrarily chosen.
 *  The propagation conflict resolving method must then be implemented, to provide the "reasons" for the bound
 *  changes, i.e. the bounds of variables at the time of the propagation, that forced the propagator to set the
 *  conflict variable's bound to its current value. It can use the "inferinfo" tag to identify its own propagation
 *  rule and thus identify the "reason" bounds. The bounds that form the reason of the assignment must then be provided
 *  by calls to SCIPaddConflictLb() and SCIPaddConflictUb() in the propagation conflict resolving method.
 *
 *  See the description of the propagation conflict resulving method of constraint handlers for further details.
 *
 *  input:
 *  - scip            : SCIP main data structure
 *  - prop            : the propagator itself
 *  - infervar        : the conflict variable whose bound change has to be resolved
 *  - inferinfo       : the user information passed to the corresponding SCIPinferVarLbProp() or SCIPinferVarUbProp() call
 *  - boundtype       : the type of the changed bound (lower or upper bound)
 *  - bdchgidx        : the index of the bound change, representing the point of time where the change took place
 *
 *  output:
 *  - result          : pointer to store the result of the propagation conflict resolving call
 *
 *  possible return values for *result:
 *  - SCIP_SUCCESS    : the conflicting bound change has been successfully resolved by adding all reason bounds
 *  - SCIP_DIDNOTFIND : the conflicting bound change could not be resolved and has to be put into the conflict set
 */
#define DECL_PROPRESPROP(x) RETCODE x (SCIP* scip, PROP* prop, VAR* infervar, int inferinfo, \
      BOUNDTYPE boundtype, BDCHGIDX* bdchgidx, RESULT* result)


#include "def.h"
#include "type_retcode.h"
#include "type_result.h"
#include "type_scip.h"


#endif
