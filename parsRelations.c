/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/FC2D_ANGIO/parsRelations.c,v $
 *
 * Copyright (c) 2002
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 * $Id: parsRelations.c,v 1.13.2.7 2006/01/10 10:03:49 dwe Exp $
 *
 ****************************************************************/

static const char resid[] = "$Id: parsRelations.c,v 1.13.2.7 2006/01/10 10:03:49 dwe Exp $ (C) 2002 Bruker BioSpin MRI GmbH";

#define DEBUG		1
#define DB_MODULE	1
#define DB_LINE_NR	1
#include "method.h"


/* ------------------------------------------------------------ 
 *  backbone 
 * The main part of method code. The consitency of all parameters is checked,
 * relations between them are resolved, and functions setting the base level
 * parameters are called.
 * --------------------------------------------------------------
 */

void backbone( void )
{
  YesNo refAttIsAviable=No;

  double referenceAttenuation=0,
         minFov[3] = {1e-3, 1e-3, 1e-3},
         minThickness;
  //int	 nslices
  //YesNo RetVal; 

  DB_MSG(("====================================\n -->backbone\n"));

  STB_DefaultUserTypeHandler(); /* Controls param visibility for expert/routine */

  /* Nucleus and PVM_GradCalConst are handled by this funtion: */
  STB_UpdateNuclei(Yes);
 
  /* Handle method of Angle Incrementing in AIF */
  if (AIF_Increment_Choice == GoldenAngle)
  {
    NAngles = 233;
    AIF_Angle_Increment = 111.244635;
    ParxRelsMakeNonEditable("NAngles");
    ParxRelsMakeNonEditable("AIF_Angle_Increment");
    DB_MSG(("Golden Angle Scheme"));
  }
  else
  {
    ParxRelsMakeEditable("NAngles");
    ParxRelsMakeEditable("AIF_Angle_Increment");
    DB_MSG(("UserDef Angle Scheme"));
  }
 
  /* handle RF pulse */   
  if(PVM_DeriveGains == Yes)
    refAttIsAviable =
      STB_GetRefAtt(PVM_Nucleus1,&referenceAttenuation);
  else
    refAttIsAviable = No;

  STB_UpdateRFPulse("ExcPulse",
		    &ExcPulse,
		    refAttIsAviable,
		    referenceAttenuation);

  STB_UpdateExcPulseEnum("ExcPulseEnum",
			 &ExcPulseEnum,
			 ExcPulse.Filename,
			 ExcPulse.Classification);

  PVM_ExcPulseAngle = ExcPulse.FlipAngle;


  /* sequence atoms: */
  PVM_ReadDephaseTime = PVM_ExSliceRephaseTime;
  ParxRelsMakeNonEditable("PVM_ReadDephaseTime");
  PVM_2dPhaseGradientTime = PVM_ReadDephaseTime;
  //PVM_3dPhaseGradientTime = PVM_2dPhaseGradientTime;

  /* excitation pulse */
  PVM_ExSlicePulseLength   = ExcPulse.Length;
  PVM_ExSliceBandWidth     = ExcPulse.Bandwidth;
  PVM_ExSliceRephaseFactor = ExcPulse.RephaseFactor *
                             ExcPulse.TrimRephase / 100.0;

  /*------------------------------------------------------------
   * Begin the Flow Compensated specific part of the method   
   *------------------------------------------------------------
   */
    switch (PVM_ObjOrderScheme)
    {
    case Sequential:
    break;
    case Reverse_sequential:
    break;
    case Interlaced:
    PVM_ObjOrderScheme = Sequential;
    break;
    case Reverse_interlaced:
    PVM_ObjOrderScheme = Reverse_sequential;
    break;
    case Angiography:
    case User_defined_slice_scheme:
    default:
    PVM_ObjOrderScheme = Sequential;
    break;
    }

  /*PVM_ObjOrderScheme = Sequential;
  PARX_noedit(TRUE,"PVM_ObjOrderScheme");*/


  /* begin Update Geometry: */
  
  /* 1: in-plane geometry:
   * The STB_StandardInplaneGeoParHandler is called twice:
   * first, with a dummy value of minFov, to make size constraints;
   * then, after the true minFov is found, to do the rest.
   * (because the sizes must be set before we find minFov)
   */
  STB_StandardInplaneGeoParHandler(minFov,2.0);
  
  /* update bandwidth acquisition time, set read anti alias factor to its final value*/
  STB_UpdateDigPars(&PVM_EffSWh,
		    PVM_Matrix[0],
		    PVM_AntiAlias,
		    &PVM_AcquisitionTime);

 /* once the image size is decided (after first update of inpl geo), we
     can update the Encoding parameters, to get the acquisition size (PVM_EncMatrix)*/
  PVM_RareFactor = 1;

  STB_UpdateEncoding(PTB_GetSpatDim(),  /* total dimensions */
		     PVM_Matrix,        /* image size */ 
		     PVM_AntiAlias,     /* a-alias */
		     &PVM_RareFactor,   /* segment size */
                     SEG_SEQUENTIAL,   /* segmenting mode */
		     Yes,               /* ppi in 2nd dim allowed */
		     Yes,               /* ppi ref lines in 2nd dim allowed */
		     No);              /* partial ft in 2nd dim allowed */ 

  flowCompTimeRels();  

  LocalGeometryMinimaRels(minFov, &minThickness);

  STB_StandardInplaneGeoParHandler(minFov,2.0); 
 
  /*
   * 2: slice geometry 
   *   The maximum slice number per packages is set to 1 for 3D
   *   and free for 2D . The total maximum number of slices is 
   *   defined by the return value of CFG_MaxSlices() called by
   *   STB_UpdateSliceGeoPars if no constrain is given by the
   *   arguments (value 0)
   */
   STB_UpdateSliceGeoPars(0,0,0,minThickness);

  /* end Update Geometry */ 

  LocalGradientStrengthRels();

  /* The flow compensated gradient echo will imcrement the echo time
   * the gradient values for a given set  of geometry parameters as 
   * long they are greater than the limit values  
   */
  while(LocalFlowCompGradientConstrain() && (PVM_FCtau < 100) )
  {
    DB_MSG(("-->PVM_FCTau incremented"));
    PVM_FCtau = PVM_FCtau  + 0.01;
  }

  LocalFrequencyOffsetRels();
 
  PVM_NRepetitions = MAX_OF(1,PVM_NRepetitions);

  PVM_NEchoImages = 1;

  Local_NAveragesRange();

  echoTimeRelsAIF();

  echoTimeRelsDCE();

  RFSpoilDelayRange();

  /* spoilers */
  ReadSpoilerStrength = MIN_OF(ReadSpoilerStrength, 100.0);
  ReadSpoilerStrength = MAX_OF(ReadSpoilerStrength, -100.0);
  SliceSpoilerStrength = MIN_OF(SliceSpoilerStrength, 100.0);
  SliceSpoilerStrength = MAX_OF(SliceSpoilerStrength, -100.0);
  SliceSpoilerDuration = MAX_OF(SliceSpoilerDuration, 2*PVM_RiseTime);

  /* handling of modules */
  STB_UpdateSatSlicesModule(PVM_Nucleus1);
  STB_UpdateFlowSaturationModule(PVM_Nucleus1);
  Local_RFSpoilingRelation();

  /* AIF_FoV_factor*PVM_Fov[0] > PVM_MinFov[0] */
  AIF_FoV_factor = MAX_OF(PVM_MinFov[0]/PVM_Fov[0],AIF_FoV_factor);

  /* set values for the sin and cos arrays */
  double pi = 3.14159265359;

  PARX_change_dims("PEsinArray",NAngles);
  int i;
  for(i=0;i<NAngles;i++)
    {
      PEsinArray[i] = sin(i*AIF_Angle_Increment*pi/180);
    }

  PARX_change_dims("PEcosArray",NAngles);
  for(i=0;i<NAngles;i++)
    {
      PEcosArray[i] = cos(i*AIF_Angle_Increment*pi/180);
    }

  DB_MSG(("<--AIF gradients have been set\n"));

  repetitionTimeRels();

  //fixedTime = SliceSpoilerDuration +CFG_AmplifierEnable() + PVM_ExSlicePulseLength/2.0 + PVM_TaggingModuleTime;

  /* set baselevel acquisition parameter */
  SetBaseLevelParam();
  DB_MSG(("select gradients: %lf %lf %lf %lf", PVM_ExSliceGradient, \
          PVM_ExSliceFC1Gradient,PVM_ExSliceFC2Gradient,PVM_ReadDephaseGradient));

  /* set baselevel reconstruction parameter */
  SetRecoParam();
  
//   sprintf(PVM_InfoString, "Te(AIF/DCE)=%.2f/%.2f, phi=%.1f(#%i)",
// 	  PVM_EchoTimeAIF, PVM_EchoTime, AIF_Angle_Increment, NAngles);
//   ParxRelsMakeNonEditable("PVM_InfoString");


  DB_MSG(("<--backbone\n"));
}

/*==============================================================
 * relation of RFSpoilDelay (FLASHubc)
 *==============================================================*/
void RFSpoilDelayRels(void)
{
  RFSpoilDelayRange();
  backbone();
}

void RFSpoilDelayRange(void)
{
  if(!ParxRelsParHasValue("RFSpoilDelay"))
    {
      RFSpoilDelay = 1.0;
    }
  else
    {
      RFSpoilDelay = MAX_OF(RFSpoilDelay, 0.02);
      RFSpoilDelay = MIN_OF(RFSpoilDelay, 10.0);
    }
}


/*==============================================================
 * relation of RFSpoilDelayPhi (FLASHubc)
 *==============================================================*/
void RFSpoilPhiRels(void)
{
  RFSpoilPhiRange();
  backbone();
}

void RFSpoilPhiRange(void)
{
  if(!ParxRelsParHasValue("RFSpoilPhi"))
    {
      RFSpoilPhi = 117;
    }
  else
    {
      RFSpoilPhi = MAX_OF(RFSpoilPhi, 0);
      RFSpoilPhi = MIN_OF(RFSpoilPhi, 360);
    }
}


  /*--------------------------------------------------------------
    RF Spoiling Relations
    --------------------------------------------------------------*/
void Local_RFSpoilingRelation(void)
{
   if(RFSpoilerOnOff == Off)
   {
    RFSpoilerdelay = 0.1;
    PARX_change_dims("RFSpoilerlist",1);
    MRT_RFSpoilFreqList(0.0,1,RFSpoilerlist,RFSpoilerdelay);
   }
   if(RFSpoilerOnOff == On)
   {
    RFSpoilerdelay = 1.0;
    PARX_change_dims("RFSpoilerlist",512);
    MRT_RFSpoilFreqList(117,512,RFSpoilerlist,RFSpoilerdelay);
   }
}


  /*==============================================================
  * relation of NDummyScans
  *==============================================================*/
/*
void dsRelations(void)
{
  DB_MSG(("-->dsRelations\n"));
  dsRange();
  backbone();
  DB_MSG(("<--dsRelations\n"));
}

void dsRange(void)
{
  if(ParxRelsParHasValue("NDummyScans") == No)
    NDummyScans = 0;
  else
    NDummyScans = MAX_OF(0, NDummyScans);
}
*/


  /*=====================================================================
   * Constraints of the gradient points values for the flow compensation. 
   * The relation  calculates the flow compensation gradient amplitudes
   * for the given durations (PVM_FCtau * 2 = PVM_ReadFCTime, PVM_SliceFCTime etc)
   * and returns constrain_violated=1 if one of the gradient values is
   * greater than the limits otherwise it returns zero.
   *======================================================================*/
int LocalFlowCompGradientConstrain(void)
{
  DB_MSG(("-->LocalFlowCompGradientConstrain"));

  int i;
  int constrain_violated = 0;
  double slicetrim[2];
  double c_0, c_1, c_2, c_3;
  double readgrad,slicegrad;
  double ramp;
  ramp = PVM_RampTime+PVM_InterGradientWaitTime; // d30 in the pulse program
  double tau = PVM_FCtau;
  // define constants that will be used to calculate gradient strengths for flow comp
  /*c_0 = (PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0) + PVM_InterGradientWaitTime + PVM_RampTime/2.0) / (tau - ramp);
  c_1 = -1.0 * ( (PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0) * PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0) )/2.0 +
				2.0*PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0)*tau + ramp*(tau - 2.0*ramp) + (11.0/12.0)*ramp*ramp);
  c_2 = -1.0 * (2.0*ramp*ramp + (7.0/2.0)*ramp*(tau - 2.0*ramp) + (3.0/2.0)*(tau - 2.0*ramp)*(tau - 2.0*ramp));
  c_3 = -1.0 * (ramp*(tau - 2.0*ramp) + (tau - 2.0*ramp)*(tau - 2.0*ramp)) / 2.0; */
  
  // These constants account for the inter-gradient wait time (assuming it starts before the start of any ramp)
  c_0 = (PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0) + PVM_InterGradientWaitTime + PVM_RampTime/2.0) / (tau - ramp);
  c_1 = (-1.0* (PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0)+PVM_InterGradientWaitTime) * (PVM_ExSlicePulseLength*(1.0-PVM_ExSliceRephaseFactor/100.0) + PVM_InterGradientWaitTime + 4*tau)) / 2.0
				+ PVM_RampTime*PVM_RampTime/6.0 - tau*PVM_RampTime;
  c_2 = PVM_RampTime*PVM_RampTime + (PVM_RampTime*PVM_InterGradientWaitTime - 3.0*PVM_RampTime*tau) / 2.0
									 + ((tau - PVM_InterGradientWaitTime - 2.0*PVM_RampTime)*(-3.0*tau + PVM_InterGradientWaitTime + 2.0*PVM_RampTime)) / 2.0;
  c_3 = -1.0 * ( tau*tau + PVM_InterGradientWaitTime*PVM_InterGradientWaitTime + 2.0*PVM_RampTime*PVM_RampTime
				- 2*tau*PVM_InterGradientWaitTime - 3*tau*PVM_RampTime + 3*PVM_InterGradientWaitTime*PVM_RampTime) / 2.0;

  
  // combine the constants above to calculate the flow comp grandient strengths
  slicetrim[0] = -1 * PVM_ExSliceGradient * (c_0 - c_1/c_3) / (c_2/c_3 - 1.);
  slicetrim[1] = PVM_ExSliceGradient * (c_0 - c_1/c_2) / (c_3/c_2 - 1.);
  
  for(i=0;i<2;i++)
   {
    if(slicetrim[i] > 57.0)
    {
      constrain_violated =1;
      DB_MSG(("-->slicetrim too large (%i)", i));
    }
   }
  slicegrad = 100.0*PVM_MinSliceThick/PVM_SliceThick;
  DB_MSG(("slicegrad ( %f )", slicegrad));
  PVM_ExSliceGradient= slicegrad;
  DB_MSG(("slicetrim ( %f , %f)", slicetrim[0], slicetrim[1]));
  PVM_ExSliceFC1Gradient = slicetrim[0];
  PVM_ExSliceFC2Gradient = slicetrim[1];

  readgrad = 100.0*PVM_MinFov[0]/(PVM_Fov[0]*AIF_FoV_factor);
  PVM_ReadGradient = readgrad;
  PVM_ReadDephaseGradient = MRT_ReadDephaseGrad( 
  			ReadGradRatio,
			PVM_ReadGradient );
  
  DB_MSG(("<--LocalFlowCompGradientConstrain\n"));

  return constrain_violated;
}


/*=============================================================
  ExcPulseAngleRelation
  Redirected relation of PVM_ExcPulseAngle
  =============================================================*/
void ExcPulseAngleRelation(void)
{
  DB_MSG(("-->ExcPulseAngleRelation\n"));
  ExcPulse.FlipAngle = PVM_ExcPulseAngle;
  ExcPulseRange();
  backbone();
  DB_MSG(("<--ExcPulseAngleRelation\n"));
}


/*===========================================================
 *
 *  examples for relations concearning special pulses and 
 *  pulselists
 *
 *==========================================================*/

/* =================================================================
   ExcPulseEnumRelation
   Relation of ExcPulseEnum (a dynamic enmueration parameter which
   allows to select one of the existing library exc. pulses)
   Sets the name and the clasification  of the pulse perameter ExcPulse 
   according to the selected enum value.
   ==================================================================*/
void ExcPulseEnumRelation(void)
{
  YesNo status;
  DB_MSG(("-->ExcPulsesEnumRelation\n"));
  
  /* set the name and clasification of ExcPulse: */
  status = STB_UpdateExcPulseName("ExcPulseEnum",
				  &ExcPulseEnum,
				  ExcPulse.Filename,
				  &ExcPulse.Classification);

  /* call the method relations */
  backbone();

  DB_MSG(("<--ExcPulseEnumRelation \n"));
}


/* ===================================================================
 * Relation of ExcPulse
 *
 * All pulses of type PVM_RF_PULSE_TYPE must have relations like this.
 * However, if you clone this funtion for a different pulse parameter
 * remember to replace the param name in the call to UT_SetRequest!
 *
 * IMPORTANT: this function should not be invoked in the backbone!
 ====================================================================*/
void ExcPulseRelation(void)
{
  DB_MSG(("-->ExcPulseRelation\n"));

  /*
   * Tell the request handling system that the parameter
   * ExcPulse has been edited
   */
  UT_SetRequest("ExcPulse");

  /* Check the values of ExcPulse */
  ExcPulseRange();

  /*
   * call the backbone; further handling will take place there
   * (by means of STB_UpdateRFPulse)
   */
  backbone();

  DB_MSG(("-->ExcPulseRelation\n"));
}


/*================================================================
 *	         L O C A L   F U N C T I O N S			
 *==============================================================*/

void ExcPulseRange(void)
{
  DB_MSG(("-->ExcPulseRange\n"));
  
  /* allowed clasification */
  switch(ExcPulse.Classification)
  {
    default:
   
      ExcPulse.Classification = LIB_EXCITATION;
      break;
    case LIB_EXCITATION:
    case PVM_EXCITATION:
    case USER_PULSE:
      break;
  }

  /* allowed angle for this pulse */
  ExcPulse.FlipAngle = MIN_OF(90.0,ExcPulse.FlipAngle);

  /* general verifiation of all pulse atributes  */
  STB_CheckRFPulse(&ExcPulse);

  DB_MSG(("<--ExcPulseRange\n"));
}


/* Calculate flow compensation parameters */
void flowCompTimeRels(void)
{
  DB_MSG(("-->flowCompTimeRels\n"));

  double ramp;
  ramp = PVM_RampTime+PVM_InterGradientWaitTime; // d30 in the pulse program

  // calculate the time for one slice FC gradient lobe (tau)
  PVM_FCtau = (PVM_EchoTimeAIF -
	 (PVM_ExSlicePulseLength * (1.0 - PVM_ExSliceRephaseFactor/100.0) + ramp/2.0) -
	 (PVM_AcquisitionTime * PVM_EchoPositionAIF / 100.0 + ramp/2.0) ) / 2.0 ; 
  ParxRelsMakeNonEditable("PVM_FCtau");
  //PVM_ReadFCTime = PVM_FCtau * 2;
    
  if(PVM_FCtau < PVM_MinReadFCTime/2.)
  {
    PVM_FCtau = PVM_MinReadFCTime/2.;
    echoTimeRelsAIF();
  }
  
  DB_MSG(("<--flowCompTimeRels\n"));
} 


void echoTimeRelsAIF( void )
{
  DB_MSG(("-->echoTimeRelsAIF"));
  
  PVM_MinEchoTimeAIF = 
    PVM_ExSlicePulseLength * (1.0 - PVM_ExSliceRephaseFactor/100.0) +
    PVM_RampTime                  +
    PVM_InterGradientWaitTime     +			// PVM_FCtau is defined as two x 'half ramp' + FC gradient
    PVM_FCtau * 2                 +
    PVM_AcquisitionTime * PVM_EchoPositionAIF / 100.0;

    PVM_EchoTimeAIF = PVM_EchoTimeAIF < PVM_MinEchoTimeAIF   ?
      PVM_MinEchoTimeAIF : PVM_EchoTimeAIF;
  DB_MSG((" Te(AIF) = %lf", PVM_MinEchoTimeAIF));
  /* Set Echo Parameters for Scan Editor  */
  ParxRelsMakeNonEditable("PVM_NEchoImages");
 
  DB_MSG(("<--echoTimeRelsAIF\n"));
}


/* From FLASHubc */
void echoTimeRelsDCE( void )
{
  DB_MSG(("-->echoTimeRelsDCE"));
  
  PVM_MinEchoTime = 
    PVM_ExSlicePulseLength * (1.0-PVM_ExSliceRephaseFactor/100.0) +
	PVM_InterGradientWaitTime    +
    PVM_RampTime                 +
    PVM_ExSliceRephaseTime       +
    PVM_InterGradientWaitTime    +
    PVM_RampTime                 +
	//PVM_InterGradientWaitTime    +  // accounted for in D2
    //PVM_RampTime                 +  // We have a delay between the rephase gradient and the acquisition window, so two ramps?  Accounted for in D14D2?
    PVM_AcquisitionTime * PVM_EchoPosition / 100.0;

  /* if(EchoTimeMode == Long_TE) 
     PVM_MinEchoTime += (PVM_ReadDephaseTime+PVM_InterGradientWaitTime); */
  
  PVM_EchoTime = PVM_EchoTime < PVM_MinEchoTime   ?
    PVM_MinEchoTime : PVM_EchoTime;
 
  /* Set Echo Parameters for Scan Editor  */  
  PVM_EchoTime1 = PVM_EchoTime;

  /* set ppg flag related to echo time mode */
  // PVM_ppgFlag1 = (EchoTimeMode == Long_TE)? Yes:No;

  DB_MSG(("<--echoTimeRelsDCE\n"));
}


/* Function from FLASHubc */
double minLoopRepetitionTime(void)
/* ---------------------------------------------------------
this function returns the minimum duration of the innermost
pulse program loop
----------------------------------------------------------*/
{
  double minTr;
// From a complicated spreadsheet
  minTr = (
	//PVM_FatSupModuleTime         +
	//PVM_SatPulseModuleTime       +
	//PVM_MagTransModuleTime       +
	//PVM_FovSatModuleTime         +
	//PVM_InFlowSatModuleTime      +
	SliceSpoilerDuration			+
	-PVM_InterGradientWaitTime		+
	PVM_RiseTime * 2				+
	CFG_AmplifierEnable()			+
	PVM_ExSlicePulseLength 			+
	0.0125							+
	PVM_EchoTime					+
	(-PVM_ExSlicePulseLength * (1-PVM_ExSliceRephaseFactor/100.0)) +
	(-PVM_AcquisitionTime * PVM_EchoPosition/100.0) +
	-PVM_RampTime					+
	-PVM_InterGradientWaitTime		+
	PVM_AcquisitionTime				+
	ReadSpoilerDuration          	);

  return minTr;
}


void repetitionTimeRels( void )
{
  int nSlices,dim;
  double TotalTime, minD0;

  DB_MSG(("-->repetitionTimeRels"));

  TotalTime = 0.0;
  nSlices = GTB_NumberOfSlices( PVM_NSPacks, PVM_SPackArrNSlices );
  minD0 = 0.04  /* ADC_END_4ch */ + PVM_InterGradientWaitTime;

  if( PVM_ErrorDetected == Yes )
    {
      UT_ReportError("AIFDCE_SAR-backbone: In function call!");
      return;
    }

  /* count all the delays during AIF projection acq */
  PVM_MinRepetitionTimeAIF =
    (
	0.07						 +
	SliceSpoilerDuration         +
	CFG_AmplifierEnable()        +
	PVM_ExSlicePulseLength       +
	PVM_FCtau * 2                +
	PVM_RampTime                 +
	PVM_InterGradientWaitTime    +  
	PVM_AcquisitionTime	     	 + // denab and ADC_init take no time!
	PVM_FCtau                    +
	PVM_RampTime                 +
	PVM_InterGradientWaitTime    +
	CFG_AmplifierEnable()        + // d8 is being re-used. should be ok...
	2*0.005
    );
  PVM_MinRepetitionTimeDCE = (
	SliceSpoilerDuration			+
	-PVM_InterGradientWaitTime		+
	PVM_RiseTime * 2				+
	CFG_AmplifierEnable()			+
	PVM_ExSlicePulseLength 			+
	0.0325							+
	PVM_EchoTime					+
	(-PVM_ExSlicePulseLength * (1-PVM_ExSliceRephaseFactor/100.0)) +
	-PVM_RampTime * 2				+
	(-PVM_AcquisitionTime * PVM_EchoPosition/100.0) +
	PVM_AcquisitionTime				+
	ReadSpoilerDuration          	);

  DB_MSG(("min Tr AIF/DCE ( %f / %f )", PVM_MinRepetitionTimeAIF, PVM_MinRepetitionTimeDCE));

  /* min TR in DCE section: */
  PVM_MinRepetitionTime = PVM_MinRepetitionTimeAIF + (nSlices-1) * PVM_MinRepetitionTimeDCE; 
  /* add RF Spoiling delay */
  PVM_MinRepetitionTime += PVM_InterGradientWaitTime + 0.0455 + RFSpoilDelay;


      /* if there is no movie, TR also includes some modules: */
      /* if(PVM_MovieOnOff == Off || PVM_NMovieFrames == 1)
	{
	  PVM_MinRepetitionTime += 
	    PVM_BlBloodModuleTime +
	    PVM_SelIrModuleTime +
	    PVM_TaggingModuleTime;
	} 
	}  */

  PVM_RepetitionTime = ( PVM_RepetitionTime < PVM_MinRepetitionTime ? 
			 PVM_MinRepetitionTime : PVM_RepetitionTime );

  /* Calculate Total Scan Time and Set for Scan Editor */ 
  dim = PTB_GetSpatDim();
  
  if( dim >1 )
    TotalTime = //nSlices *
                PVM_RepetitionTime *
                PVM_EncMatrix[1]*
                PVM_NAverages;
    /* add time for dummy scans */
    /*TotalTime = TotalTime + (nSlices * PVM_RepetitionTime * PVM_NAverages * NDummyScans);*/

  TotalTime = TotalTime * PVM_NRepetitions;
  UT_ScanTimeStr(PVM_ScanTimeStr,TotalTime);

  ParxRelsShowInEditor("PVM_ScanTimeStr");
  ParxRelsMakeNonEditable("PVM_ScanTimeStr");

  DB_MSG(("<-- repetitionTimeRels\n"));
}


void LocalGeometryMinimaRels(double *min_fov, double *min_thickness )
{
  /*
   * This function calculates the minima for the fields-of-view in all
   * three directions as well as the minimum slice thickness. It is always
   * assumed that all slices have the same thickness
   * 
   * The results is returned in min_fov[0..2] and min_thickness.
   */

  double readRampInteg;  /* normalised integral rising read gradient ramp   */
  double readDephInteg;  /* normalised integral read dephase gradient       */
  double sliceRampInteg; /* normalised integral falling slice gradient ramp */
  double sliceRephInteg; /* normalised integral slice reph.c# */
  DB_MSG(("-->LocalGeometryMinimaRels"));

  /* This function calculates all the gradient strengths */
  int dim = PTB_GetSpatDim();

   // copied from standard FLASH
       /* FREQUENCY ENCODING GRADIENT
       *
       * Step #1:
       * Calculate the normalised integral of the read dephase gradient
       */	      

   readDephInteg = MRT_NormGradPulseTime( PVM_FCtau-PVM_InterGradientWaitTime,
					  PVM_ReadDephaseRampUpTime,
					  PVM_ReadDephaseRampUpIntegral,
					  PVM_ReadDephaseRampDownTime,
					  PVM_ReadDephaseRampDownIntegral );

   readDephIntegFLASH = MRT_NormGradPulseTime( PVM_ReadDephaseTime,
					  PVM_ReadDephaseRampUpTime,
					  PVM_ReadDephaseRampUpIntegral,
					  PVM_ReadDephaseRampDownTime,
					  PVM_ReadDephaseRampDownIntegral );

   /*
    * Step #2:
    * Calculate the normalised integral of the rising ramp of the read
    * gradient preceding data collection */
   readRampInteg = MRT_NormGradRampTime( PVM_ReadRampUpTime,
					 PVM_ReadRampUpIntegral );
						
   /*
    * Step #3
    * Calculate the ratio of the strength of the read gradient to the
    * strength of the read dephase gradient
    *
    * The variable ReadGradRatio is a parameter defined in the file:
    * parsDefinition.h but it is NOT included in the definition of
    * MethodClass that appears in parsLayout.h, The value of ReadGradRatio
    * determined here is used later in "LocalGradientStrengthRels()" */
   ReadGradRatio = MRT_ReadGradRatio( PVM_AcquisitionTime,
				      PVM_EchoPositionAIF, PVM_AcqStartWaitTime,
				      readDephInteg, readRampInteg );
   DB_MSG(("ReadGradRatio = %f",ReadGradRatio));

   ReadGradRatioFLASH = MRT_ReadGradRatio( PVM_AcquisitionTime,
				      PVM_EchoPosition, PVM_AcqStartWaitTime,
				      readDephIntegFLASH, readRampInteg );
   DB_MSG(("ReadGradRatioFLASH = %f",ReadGradRatioFLASH));

   /* Step #4
    * Calculate the minimum field of view in the read direction */
   min_fov[0] =  MRT_MinReadFov( PVM_EffSWh,
			   1.0, PVM_LimReadGradient, PVM_LimReadGradient,
			   PVM_GradCalConst );


     /* PHASE ENCODING GRADIENT - 2nd DIM
     *
     * The variable Phase2dInteg is a parameter defined in the file:
     * parsDefinition.h but it is NOT included in the definition of
     * MethodClass that appears in parsLayout.h, The value of
     * Phase2dInteg determined here is used later in
     * "LocalGradientStrengthRels()" */
    PVM_2dPhaseGradientTime = PVM_ReadDephaseTime;
    Phase2dInteg = MRT_NormGradPulseTime (PVM_2dPhaseGradientTime,
					  PVM_2dPhaseRampUpTime,
					  PVM_2dPhaseRampUpIntegral,
					  PVM_2dPhaseRampDownTime,
					  PVM_2dPhaseRampDownIntegral );

    /* Set the minimum field of view in the phase direction */
     /* min_fov[1] = MRT_PhaseFov( Phase2dInteg,
			      PVM_Matrix[1], PVM_Lim2dPhaseGradient,
			      PVM_GradCalConst ); */
     min_fov[1] = min_fov[0];


   /* SLICE SELECTION GRADIENT
    *
    * Calculate the normalised integral of the descending gradient
    * ramp after the RF pulse */
   sliceRampInteg = MRT_NormGradRampTime( PVM_ExSliceRampDownTime,
					  PVM_ExSliceRampDownIntegral );

   /*
    * Calculate the normalised integral of the slice selection rephasing
    * gradient */
   sliceRephInteg =
     MRT_NormGradPulseTime( PVM_ExSliceRephaseTime,
			    PVM_ExSliceRephaseRampUpTime,
			    PVM_ExSliceRephaseRampUpIntegral,
			    PVM_ExSliceRephaseRampDownTime,
			    PVM_ExSliceRephaseRampDownIntegral );

   /*
    * Calculate the ratio of the strength of the slice selection
    * gradient to the strength of the slice selection rephase
    * gradient
    *
    * The variable SliceGradRatio is a parameter defined in the file:
    * parsDefinition.h but it is NOT included in the definition of
    * MethodClass that appears in parsLayout.h.
    * The value of SliceGradRatio determined here is used later in
    * "LocalGradientStrengthRels()" */
   SliceGradRatio =
     MRT_SliceGradRatio( PVM_ExSlicePulseLength,
			 PVM_ExSliceRephaseFactor, PVM_ExSliceRampDownWaitTime,
			 sliceRampInteg, sliceRephInteg );


   /* Calculate the minimum slice thickness */
    *min_thickness = MRT_MinSliceThickness( PVM_ExSliceBandWidth,
				      SliceGradRatio, PVM_LimExSliceGradient,
				      PVM_LimExSliceFCGradient, PVM_GradCalConst
				      );

  /*
   * PHASE ENCODING GRADIENT - 3nd DIM 
   */
  if(dim > 2)
  {
      /*PVM_3dPhaseGradient = 
	MRT_PhaseGrad( Phase3dInteg,
		       PVM_Matrix[2],
		       PVM_Fov[2],
		       PVM_GradCalConst );
       */
	PVM_3dPhaseGradient = 0.0;
	DB_MSG(("WARNING!! 3D not possible, reprogram: PVM_3dPhaseGradient\n"));
  }  
  else
    PVM_3dPhaseGradient = 0.0;

  DB_MSG(("<--LocalGeometryMinimaRels\n"));
}


/* From FLASHubc */
void LocalGradientStrengthRels( void )
{
  int dim;
  DB_MSG(("-->LocalGradientStrengthRels"));
  
  /* This function calculates all the gradient strengths */
  dim = PTB_GetSpatDim();

  PVM_2dPhaseGradient = 0;
  PVM_2dPhaseFC1Gradient = 0;
  PVM_2dPhaseFC2Gradient = 0;

  /* PHASE ENCODING GRADIENT - 2nd DIM */
  if(dim > 1)
  {     
      PVM_2dPhaseGradientFLASH = 
	MRT_PhaseGrad( Phase2dInteg,
		       PVM_Matrix[1],
		       PVM_Fov[1],
		       PVM_GradCalConst );
  }
  else
    PVM_2dPhaseGradientFLASH = 0.0;
      
  /* FREQUENCY ENCODING GRADIENT */
  if(dim >0)
  {
  PVM_ReadGradientFLASH = 
    MRT_ReadGrad( PVM_EffSWh,
		  PVM_Fov[0],
		  PVM_GradCalConst );
  PVM_ReadDephaseGradientFLASH = 
    MRT_ReadDephaseGrad( ReadGradRatioFLASH,
			 PVM_ReadGradientFLASH );
  }
  else
    PVM_ReadGradientFLASH = PVM_ReadDephaseGradientFLASH = 0.0;


  /* SLICE SELECTION GRADIENT */ 
 /* if(dim > 2)
    {
      ParxRelsShowInEditor("SlabSelect");
    }
  else
    {
    ParxRelsHideInEditor("SlabSelect");
    SlabSelect = On;
    }
  
  if (SlabSelect == Off)
    { 
      PVM_ExSliceGradient = 0;
      PVM_ExSliceRephaseGradient = 0;
    }
  else
    { */
    PVM_ExSliceGradient = 
  	  MRT_SliceGrad( PVM_ExSliceBandWidth,
  			 PVM_SliceThick,
 			 PVM_GradCalConst );
    PVM_ExSliceRephaseGradient = 
	  MRT_SliceRephaseGrad( SliceGradRatio,
			      PVM_ExSliceGradient );
    //}

  DB_MSG(("<--LocalGradientStrengthRels\n"));
}


void LocalFrequencyOffsetRels( void )
{
  int spatDim;
  int i,nslices;
  DB_MSG(("-->LocalFrequencyOffsetRels"));

  spatDim = PTB_GetSpatDim();

  nslices = GTB_NumberOfSlices(PVM_NSPacks,PVM_SPackArrNSlices);
  NDCESlices = nslices - 1; /* one slice for AIF acquisiton */
  DB_MSG(("nslices = ( %i )", nslices));  
  DB_MSG(("NDCESlices = ( %i )", NDCESlices));

  /* Calculate offset in read direction */
  MRT_FrequencyOffsetList(nslices,
			  PVM_EffReadOffset,
			  PVM_ReadGradient,
			  PVM_GradCalConst,
			  PVM_ReadOffsetHz );

  /* Calculate slice offset */
  MRT_FrequencyOffsetList(nslices,
			  PVM_EffSliceOffset,
			  PVM_ExSliceGradient,
			  PVM_GradCalConst,
			  PVM_SliceOffsetHz );

  if(spatDim == 3)
  {
    for(i=0;i<nslices;i++)
      PVM_EffPhase2Offset[i] -= PVM_SliceOffset[i]/PVM_AntiAlias[2];
  }

  DB_MSG(("<--LocalFrequencyOffsetRels\n"));
}


/*===============================================================
 *
 * Range checking routine for parameter PVM_NAverages
 *
 *==============================================================*/
void Local_NAveragesRange(void)
{
  int ival;
  DB_MSG(("-->Local_NAveragesRange"));
  
  /* Range check of PVM_NAverages: prevent it to be negative or 0 */
  if(ParxRelsParHasValue("PVM_NAverages") == No)
    {
      PVM_NAverages = 1;
    }

  ival = PVM_NAverages;
  PVM_NAverages = MAX_OF(ival,1);

  DB_MSG(("<--Local_NAveragesRange\n"));
}


void Local_NAveragesHandler(void)
{
  DB_MSG(("-->Local_NAveragesRange\n"));
  Local_NAveragesRange();

  /*
   *   Averages range check is finished, handle the request by
   *   the method:
   */
  backbone();

  DB_MSG(("<--Local_NAveragesRange\n"));
  return;
}


/*==================================================
 *     relation to update Echotime from scan editor
 *==================================================*/
void LocalEchoTimeAIF1Relation(void)
{
  DB_MSG(("-->LocalEchoTimeAIF1Relation"));

  PVM_EchoTime = PVM_EchoTime1;//AIF = PVM_EchoTimeAIF1;
  backbone();

  DB_MSG(("<--LocalEchoTimeAIF1Relation\n"));
}


void LocalEchoTimeDCE1Relation(void)
{
  DB_MSG(("-->LocalEchoTimeDCE1Relation"));

  PVM_EchoTime = PVM_EchoTime1;
  backbone();

  DB_MSG(("<--LocalEchoTimeDCE1Relation\n"));
}


/* rangechecking and redirected relations of PVM_EffSWh */
void EffSWhRange(void)
{
  DB_MSG(("-->EffSWhRange"));

  if(!ParxRelsParHasValue("PVM_EffSWh"))
  {
    PVM_EffSWh = 50000;
  }
  else
  {
    PVM_EffSWh = MIN_OF(MAX_OF(PVM_EffSWh,2000.0),1000000);
  }

  DB_MSG(("-->EffSWhRange"));
  return;
}


void EffSWhRel(void)
{
  DB_MSG(("-->EffSWhRel"));

  EffSWhRange();
  backbone();

  DB_MSG(("-->EffSWhRel"));
  return;
}


void MyRgInitRel(void)
{
  DB_MSG(("-->MyRgInitRel"));
  STB_UpdateSliceGeoPars(1,1,1,1e-3);
  backbone();
  ParxRelsParRelations("PVM_AutoRgInitHandler",Yes);
  DB_MSG(("<--MyRgInitRel"));
}

/****************************************************************/
/*		E N D   O F   F I L E				*/
/****************************************************************/
