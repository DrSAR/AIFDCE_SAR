/* ***************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/FC2D_ANGIO/callbackDefs.h,v $
 *
 * Copyright (c) 1999-2002
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: callbackDefs.h,v 1.2.2.4 2005/10/25 15:25:34 sako Exp $
 *
 * ***************************************************************/

/* Encoding */
relations PVM_EncodingHandler backbone;

/* inplane geometry */

relations PVM_InplaneGeometryHandler  backbone;

/* slice geometry: */

relations PVM_SliceGeometryHandler  backbone;

/* digitizer parameters and bandwidth */
relations PVM_DigHandler       backbone;
relations PVM_EffSWh           EffSWhRel;

/* sequence atoms */

relations PVM_2dPhaseHandler          backbone;
relations PVM_FreqEncodingHandler     backbone;
relations PVM_ExSliceSelectionHandler backbone;
relations PVM_FlowComp2dPhaseHandler  backbone;
relations PVM_FlowCompSliceHandler    backbone;
relations PVM_FlowCompReadHandler     backbone;



/* other parameters */

relations PVM_NucleiHandler    backbone;
relations PVM_DeriveGains      backbone;
relations PVM_RepetitionTime   backbone;
//relations PVM_EchoTimeAIF      backbone;
//relations PVM_EchoTimeAIF1     LocalEchoTimeAIF1Relation;
relations PVM_EchoTime         backbone;
relations PVM_EchoTime1        LocalEchoTimeDCE1Relation;
relations PVM_NAverages        Local_NAveragesHandler;
relations PVM_NRepetitions     backbone;
relations PVM_ExcPulseAngle    ExcPulseAngleRelation;

/* modules */

relations PVM_SatSlicesHandler backbone;
relations PVM_FlowSatHandler   backbone;
relations PVM_AutoRgInitHandler MyRgInitRel;


/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/






