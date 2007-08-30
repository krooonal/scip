#!/bin/sh
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#*                                                                           *
#*                  This file is part of the program and library             *
#*         SCIP --- Solving Constraint Integer Programs                      *
#*                                                                           *
#*    Copyright (C) 2002-2007 Tobias Achterberg                              *
#*                                                                           *
#*                  2002-2007 Konrad-Zuse-Zentrum                            *
#*                            fuer Informationstechnik Berlin                *
#*                                                                           *
#*  SCIP is distributed under the terms of the ZIB Academic License.         *
#*                                                                           *
#*  You should have received a copy of the ZIB Academic License              *
#*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      *
#*                                                                           *
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# $Id: allcmpres.sh,v 1.13 2007/08/30 14:21:06 bzfpfend Exp $

AWKARGS=""
FILES=""
for i in $@
do
    if test ! -e $i
    then
	AWKARGS="$AWKARGS $i"
    else
	FILES="$FILES $i"
    fi
done

TESTSETS=""
for i in `ls -1 --color=none $FILES | sed 's!\(.*\)check\.\([^ .]*\)\.\([^ ]*\)\.res!\2!g' | sort -u`
do
    TESTSETS="$TESTSETS $i"
done

for i in $TESTSETS
do
    echo
    echo ====vvvv==== $i ====vvvv====
    cmpres.awk $AWKARGS texcmpfilename="cmpres.$i.tex" `ls -1 --color=none $FILES | grep "$i\..*\.res"`
    echo ====^^^^==== $i ====^^^^====
done
