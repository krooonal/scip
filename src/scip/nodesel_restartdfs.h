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
#pragma ident "@(#) $Id: nodesel_restartdfs.h,v 1.5 2005/01/21 09:16:58 bzfpfend Exp $"

/**@file   nodesel_restartdfs.h
 * @brief  node selector for depth first search with periodical selection of the best node
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __NODESEL_RESTARTDFS_H__
#define __NODESEL_RESTARTDFS_H__


#include "scip.h"


/** creates the node selector for restarting depth first search and includes it in SCIP */
extern
RETCODE SCIPincludeNodeselRestartdfs(
   SCIP*            scip                /**< SCIP data structure */
   );

#endif
