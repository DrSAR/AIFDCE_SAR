/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/FC2D_ANGIO/parsLayout.h,v $
 *
 * Copyright (c) 1999-2002
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: parsLayout.h,v 1.5.2.2 2005/10/20 10:55:26 sako Exp $
 *
 ****************************************************************/

/****************************************************************/
/*	PARAMETER CLASSES				       	*/
/****************************************************************/

parclass
{
  PVM_EffSWh;
  PVM_EchoPosition;
  PVM_EchoPositionAIF;
  PVM_FCtau;
  PVM_ExSliceRephaseTime;
  ReadSpoilerDuration;
  ReadSpoilerStrength;
  SliceSpoilerDuration;
  SliceSpoilerStrength;
  DigitizerPars;
  RFSpoilDelay;
  RFSpoilPhi;
} Sequence_Details;


parclass
{
  ExcPulseEnum;
  ExcPulse;
} RF_Pulses;


parclass
{
  /*NDummyScans;*/
  RFSpoilerOnOff;
  PVM_FovSatOnOff;
  Sat_Slices_Parameters;
  PVM_InFlowSatOnOff;
  Flow_Sat_Parameters;
} Preparation;


parclass
{
 PVM_EchoTime1;       /* to  be stored in method file */
 PVM_NEchoImages;
}ScanEditorInterface; 


parclass
{
  PVM_EchoTimeAIF;
  AIF_Increment_Choice;
  AIF_Angle_Increment;
  NAngles;
  AIF_FoV_factor;
}
attributes
{
  display_name "AIF Block Details";
} AIFEditorInterface;


parclass
{
  PVM_EchoTime;
}
attributes
{
  display_name "DCE Block Details";
} DCEEditorInterface;


parclass
{
  Method;
//   PVM_InfoString;
  PVM_ScanTimeStr;
  AIFEditorInterface;
  DCEEditorInterface;
  PVM_RepetitionTime;
  PVM_NAverages;
  PVM_NRepetitions;
  PVM_UserType;
  PVM_DeriveGains;
  RF_Pulses;
  Nuclei;
  Encoding;
  Sequence_Details;
  StandardInplaneGeometry;
  StandardSliceGeometry;
  Preparation;
  ScanEditorInterface;
} MethodClass;

/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/

