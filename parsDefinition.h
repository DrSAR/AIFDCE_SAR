/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/FC2D_ANGIO/parsDefinition.h,v $
 *
 * Copyright (c) 2001
 * BRUKER MEDICAL GMBH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: parsDefinition.h,v 1.2 2002/06/20 12:22:31 dwe Exp $
 *
 ****************************************************************/


/****************************************************************/
/* INCLUDE FILES						*/
/****************************************************************/
double parameter ReadGradRatio;
double parameter SliceGradRatio;
double parameter Phase3dInteg;
double parameter Phase2dInteg;
double parameter trimread[2];
double parameter trimslice[2];
double parameter trimphase[2];

// char parameter
// {
//   display_name "Sequence Info";
//   relations backbone;
// } PVM_InfoString[];

OnOff parameter
{
  display_name  "RF Spoiler ";
  relations backbone;
} RFSpoilerOnOff;

double parameter
{
  display_name "Read spoiler duration";
  format "%.2f";
  units "ms";
  relations backbone;
} ReadSpoilerDuration;

double parameter
{
  display_name "Read spoiler strength";
  format "%.1f";
  units "%";
  relations backbone;
} ReadSpoilerStrength;

double parameter
{
  display_name "Slice spoiler duration";
  format "%.2f";
  units "ms";
  relations backbone;
} SliceSpoilerDuration;

double parameter
{
  display_name "Slice spoiler strength";
  format "%.1f";
  units "%";
  relations backbone;
} SliceSpoilerStrength;

/* From FLASHubc */
double parameter
{
  display_name "RF Spoil Delay";
  units "ms";
  format "%.3f";
  relations RFSpoilDelayRels;
} RFSpoilDelay;

double parameter
{
  display_name "RF Spoil Phase Increment";
  units "degrees";
  format "%.3f";
  relations RFSpoilPhiRels;
} RFSpoilPhi;

PV_PULSE_LIST parameter
{
  display_name "Excitation Pulse Choice";
  relations    ExcPulseEnumRelation;
}ExcPulseEnum;

PVM_RF_PULSE_TYPE parameter
{
  display_name "Excitation Pulse";
  relations    ExcPulseRelation;
}ExcPulse;

/*int parameter
{
  display_name "Number of dummy scans";
  relations dsRelations;
} NDummyScans; */

double parameter
{
  relations backbone;
} PEsinArray[];

double parameter
{
  relations backbone;
} PEcosArray[];

double parameter
{
  display_name "Echo Time AIF";
  units "ms";
  format "%.3f";
  relations backbone;
} PVM_EchoTimeAIF;

double parameter
{
  display_name "FC single lobe";
  units "ms";
  relations backbone;
} PVM_FCtau;

double parameter
{
  relations backbone;
} PVM_MinEchoTimeAIF;

double parameter
{
  display_name "Echo Position (AIF)";
  units "%";
  relations backbone;
} PVM_EchoPositionAIF;

double parameter PVM_MinRepetitionTimeAIF;
double parameter PVM_MinRepetitionTimeDCE;
double parameter PVM_AcquisitionTimeAIF;
double parameter RFSpoilerlist[];
double parameter RFSpoilerdelay;
double parameter readDephIntegFLASH;
double parameter ReadGradRatioFLASH;
//double parameter PVM_ReadGradientFLASH;
double parameter PVM_ReadGradientAIF;
//double parameter PVM_ReadDephaseGradientFLASH;
double parameter PVM_ReadDephaseGradientAIF;
double parameter PVM_2dPhaseGradientFLASH;
int parameter NDCESlices;

AIF_Angle_Increment_Type parameter
{
  display_name "AIF Angle Increment Type";
  relations backbone;
} AIF_Increment_Choice;

int parameter
{
  display_name "# of angles";
  relations backbone;
} NAngles;

double parameter
{
  display_name "AIF angle increment";
  relations backbone;
} AIF_Angle_Increment;

double parameter
{
  display_name "AIF FoV factor";
  relations backbone;
} AIF_FoV_factor;
/****************************************************************/
/*	E N D   O F   F I L E					*/
/****************************************************************/
