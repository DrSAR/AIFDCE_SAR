/* ***************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/FC2D_ANGIO/initMeth.c,v $ 
 *
 * Copyright (c) 2002
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: initMeth.c,v 1.9.2.3 2005/10/20 10:55:26 sako Exp $
 *
 * ***************************************************************/

static const char resid[] = "$Id: initMeth.c,v 1.9.2.3 2005/10/20 10:55:26 sako Exp $ (C) 2002 Bruker BioSpin MRI GmbH";

#define DEBUG           0
#define DB_MODULE       1
#define DB_LINE_NR      1


#include "method.h"

/*:=MPB=:=======================================================*
 *
 * Global Function: initMeth
 *
 * Description: This procedure is implicitly called when this
 *	method is selected.
 *
 * Error History: 
 *
 * Interface:							*/

void initMeth()
/*:=MPE=:=======================================================*/
{
  int dimRange[2] = { 2,2 };
  int lowMat[3] = { 32, 32, 8 };
  int upMat[3]  = { 2048, 2048, 256 };


  DB_MSG(( "-->initMeth\n" ));

  /* which version of toolboxes should be active */
  PTB_VersionRequirement( Yes,20050101,"");


  /* Initialize RF spoiling parameters */
  RFSpoilDelayRange();
  RFSpoilPhiRange();

  
  /*  Initialize NA ( see code in parsRelations ) */
  Local_NAveragesRange();

  if(ParxRelsParHasValue("PVM_NRepetitions") == No)
    PVM_NRepetitions = 1;

  /*  Initialise dummy scans */
  /* dsRange(); */

  /*
   * Which parameter classes (see parDefs.h) should be
   * hidden in routine mode
   */
  PTB_SetUserTypeClasses( "Nuclei,"
			  "Sequence_Details,"
			  "RF_Pulses" );

  
  /* Initialisation of rf pulse parameters */

  /* 1: flip angle in the scan edidor */
  if(ParxRelsParHasValue("PVM_ExcPulseAngle") == No)
      PVM_ExcPulseAngle = 90.0;
  ParxRelsShowInEditor("PVM_ExcPulseAngle");

  /* 2: pulses declared in parDefinitions.h 
   *  in this case - ExcPulse. We initalize it to default name, 
   *  1ms, and the flip angle given in PVM_ExcPulseAngle
   */
  if(ParxRelsParHasValue("ExcPulse") == No)
  {
     STB_InitRFPulse(&ExcPulse,
		     CFG_RFPulseDefaultShapename(LIB_EXCITATION),
		     1.0,
		     PVM_ExcPulseAngle);
  }
  ExcPulseRange();
  
  /* 3: the corresponding pulse enums */
  STB_InitExcPulseEnum("ExcPulseEnum");


  /* Initialisation of nucleus */  
  STB_InitNuclei(1);

  /* Gradient limits in % of max. Value 57 (1/sqrt(3))
     is needed for arbitrary oblique slices. */
  PVM_LimReadFCGradient = 57.0;
  PVM_Lim2dPhaseFCGradient = 57.0;

  /* Initialisation of atoms */

  /* 1: method specific initialisation */
  if(ParxRelsParHasValue("PVM_ReadDephaseTime") == No){
    //PVM_ReadFCTime = 1.5;
    PVM_ReadDephaseTime = 1.5;
  }

  if(ParxRelsParHasValue("PVM_ExSliceRephaseTime") == No)
    PVM_ExSliceRephaseTime = 1.5;
    
  if(ParxRelsParHasValue("PVM_FCtau") == No){
    PVM_FCtau = 1.5;
  }

  PVM_AcqStartWaitTime = 0.0;

  /* 2: call init functions to make sure all atoms have legal values */
  STB_InitReadAtoms();  
  STB_InitExSliceAtoms();
  STB_Init2dPhaseAtoms();

  /* call init functions of flow compensated gradient points */
  STB_Init2dPhaseFCAtoms();  
  STB_InitReadFCAtoms();  
  STB_InitExSliceFCAtoms();

  /* Initialisation of spoilers */
  if(ParxRelsParHasValue("ReadSpoilerDuration") == No)
    ReadSpoilerDuration = 2;
  if(ParxRelsParHasValue("ReadSpoilerStrength") == No)
    ReadSpoilerStrength = 20;
  if(ParxRelsParHasValue("SliceSpoilerDuration") == No)
    SliceSpoilerDuration = 2;
  if(ParxRelsParHasValue("SliceSpoilerStrength") == No)
    SliceSpoilerStrength = 20;
  if(ParxRelsParHasValue("PVM_DeriveGains") == No)
    PVM_DeriveGains = Yes;  

  /* Initialisation of geometry parameters */
  /* A: in-plane */
  STB_InitStandardInplaneGeoPars(2,dimRange,lowMat,upMat,No);
 
  /* B: slice geometry */
  STB_InitSliceGeoPars(0,0,0);

 /* Encoding */
  STB_InitEncoding();

  /* initialize digitizer parameter */
  STB_InitDigPars();
  EffSWhRange();
   
  /* not a csi experiment */
  PTB_SetSpectrocopyDims( 0, 0 );

  /* Preparation modules */
  STB_InitSatSlicesModule();
  STB_InitFlowSaturationModule();

  /* Set Flow Compensation flag */
  PARX_hide_pars(NOT_HIDDEN,"PVM_FlowCompOnOff");
  PARX_noedit(TRUE,"PVM_FlowCompOnOff");
  PVM_FlowCompOnOff = On;

  /* RF Soiling */
  if(ParxRelsParHasValue("RFSpoilerOnOff") == 0)
    RFSpoilerOnOff = Off;

  /*
   * Once all parameters have initial values, the backbone is called
   * to assure they are consistent 
   */
  ParxRelsHideClassInEditor("ScanEditorInterface");


  if(ParxRelsParHasValue("NAngles") == 0)
   NAngles = 233;
  if(ParxRelsParHasValue("AIF_Angle_Increment") == 0)
    AIF_Angle_Increment = 111.244635;
  if(ParxRelsParHasValue("AIF_Increment_Choice") == 0)
  {
   AIF_Increment_Choice = GoldenAngle;
   ParxRelsMakeNonEditable("NAngles");
   ParxRelsMakeNonEditable("AIF_Angle_Increment");
  }


  if(ParxRelsParHasValue("NDCESlices") == 0)
   NDCESlices = GTB_NumberOfSlices(PVM_NSPacks,PVM_SPackArrNSlices) - 1;
  if(ParxRelsParHasValue("PVM_EchoPositionAIF") == 0)
    PVM_EchoPositionAIF = 50;
   AIF_FoV_factor=1;
//   strcpy(PVM_InfoString, "");
//   ParxRelsMakeNonEditable("PVM_InfoString");
  
  backbone();

  DB_MSG(("<--initMeth\n"));
}

/****************************************************************/
/*		E N D   O F   F I L E				*/
/****************************************************************/

