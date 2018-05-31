/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/empty/method.h,v $
 *
 * Copyright (c) 1995-2008
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: method.h,v 1.7.2.1 2008/03/07 09:54:52 mawi Exp $
 *
 ****************************************************************/

#ifndef METHOD_H
#define METHOD_H

/*--------------------------------------------------------------*
 * Prototypes of relations functions...
 *--------------------------------------------------------------*/


/* --------------------------------------------------------------
 *   Include files...
 *--------------------------------------------------------------*/

#include "machine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/*--------------------------------------------------------------*
 * include files required at the start by Parx...
 *--------------------------------------------------------------*/

#include "brukdef.h"
#include "bruktyp.h"
#include "Parx/Parx.h"

/*--------------------------------------------------------------*
 * Uxnmr include files...
 *--------------------------------------------------------------*/

#include "ta_config.h"
#include "debugdef.h"
#include "lib/libPvPath.h"
#include "lib/libDatPath.h"

#include "acqutyp.h"
#include "acqumtyp.h"
#include "autocmd.h"
#include "recotyp.h"
#include "subjtyp.h"
#include "autotyp.h"
#include "methodTypes.h"
#include "ovl_toolbox/AdjTools.h"
#include "lib/PvSysinfoClient.h"
/*--------------------------------------------------------------*
 * Global include files...
 *--------------------------------------------------------------*/
#include "Pvnl/PvnlSpherical.h"
#include "Pvnl/PvnlComplex.h"
#include "Pvnl/PvnlError.h"
#include "Pvnl/PvnlFT.h"
#include "Pvnl/PvnlSpherical.h"
#include "Pvnl/PvnlFit.h"
#include "Pvnl/PvnlLinalg.h"
#include "Pvnl/PvnlMatrix.h"
#include "Pvnl/PvnlUlp.h"


#include "PVMTools.h"

/*--------------------------------------------------------------*
 * Method include files...
 *--------------------------------------------------------------*/

#include "parsTypes.h"
#include "relProtos.h"


#endif

/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/

