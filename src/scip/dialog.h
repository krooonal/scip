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
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: dialog.h,v 1.16 2005/08/24 17:26:44 bzfpfend Exp $"

/**@file   dialog.h
 * @brief  internal methods for user interface dialog
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_DIALOG_H__
#define __SCIP_DIALOG_H__


#include "scip/def.h"
#include "scip/type_retcode.h"
#include "scip/type_set.h"
#include "scip/type_dialog.h"
#include "scip/pub_dialog.h"




/*
 * dialog handler
 */

/** creates a dialog handler */
extern
SCIP_RETCODE SCIPdialoghdlrCreate(
   SCIP_DIALOGHDLR**     dialoghdlr          /**< pointer to store dialog handler */
   );

/** frees a dialog handler and it's dialog tree */
extern
SCIP_RETCODE SCIPdialoghdlrFree(
   SCIP_DIALOGHDLR**     dialoghdlr          /**< pointer to dialog handler */
   );

/** executes the root dialog of the dialog handler */
extern
SCIP_RETCODE SCIPdialoghdlrExec(
   SCIP_DIALOGHDLR*      dialoghdlr,         /**< dialog handler */
   SCIP_SET*             set                 /**< global SCIP settings */
   );

/** makes given dialog the root dialog of dialog handler; captures dialog and releases former root dialog */
extern
SCIP_RETCODE SCIPdialoghdlrSetRoot(
   SCIP_DIALOGHDLR*      dialoghdlr,         /**< dialog handler */
   SCIP_DIALOG*          dialog              /**< dialog to be the root */
   );




/*
 * dialog
 */

/** creates and captures a user interface dialog */
extern
SCIP_RETCODE SCIPdialogCreate(
   SCIP_DIALOG**         dialog,             /**< pointer to store the dialog */
   SCIP_DECL_DIALOGEXEC  ((*dialogexec)),    /**< execution method of dialog */
   SCIP_DECL_DIALOGDESC  ((*dialogdesc)),    /**< description output method of dialog, or NULL */
   const char*           name,               /**< name of dialog: command name appearing in parent's dialog menu */
   const char*           desc,               /**< description of dialog used if description output method is NULL */
   SCIP_Bool             issubmenu,          /**< is the dialog a submenu? */
   SCIP_DIALOGDATA*      dialogdata          /**< user defined dialog data */
   );

/** captures a dialog */
extern
void SCIPdialogCapture(
   SCIP_DIALOG*          dialog              /**< dialog */
   );

/** releases a dialog */
extern
SCIP_RETCODE SCIPdialogRelease(
   SCIP_DIALOG**         dialog              /**< pointer to dialog */
   );

/** executes dialog */
extern
SCIP_RETCODE SCIPdialogExec(
   SCIP_DIALOG*          dialog,             /**< dialog */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_DIALOGHDLR*      dialoghdlr,         /**< dialog handler */
   SCIP_DIALOG**         nextdialog          /**< pointer to store the next dialog to process */
   );

/** adds a sub dialog to the given dialog as menu entry and captures the sub dialog */
extern
SCIP_RETCODE SCIPdialogAddEntry(
   SCIP_DIALOG*          dialog,             /**< dialog */
   SCIP_SET*             set,                /**< global SCIP settings */
   SCIP_DIALOG*          subdialog           /**< subdialog to add as menu entry in dialog */
   );


#endif
