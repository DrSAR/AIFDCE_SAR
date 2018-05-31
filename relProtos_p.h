/*
 *******************************************************************
 *
 * $Source: /bscl/CvsTree/bscl/gen/config/proto.head,v $
 *
 * Copyright (c) 1995
 * BRUKER ANALYTISCHE MESSTECHNIK GMBH
 * D-76287 Rheinstetten, Germany
 *
 * All Rights Reserved
 *
 *
 * $State: Exp $
 *
 *******************************************************************
 */

#ifndef _P_
#	if defined(HAS_PROTO) || defined(__STDC__) || defined(__cplusplus)
#		define _P_(s) s
#	else
#		define _P_(s) ()
#	endif
#endif

/* /opt/PV5.1/prog/parx/src/AIFDCE_SAR/initMeth.c */
void initMeth _P_((void));
/* /opt/PV5.1/prog/parx/src/AIFDCE_SAR/loadMeth.c */
void loadMeth _P_((const char *));
/* /opt/PV5.1/prog/parx/src/AIFDCE_SAR/parsRelations.c */
void backbone _P_((void));
void RFSpoilDelayRels _P_((void));
void RFSpoilDelayRange _P_((void));
void RFSpoilPhiRels _P_((void));
void RFSpoilPhiRange _P_((void));
void Local_RFSpoilingRelation _P_((void));
int LocalFlowCompGradientConstrain _P_((void));
void ExcPulseAngleRelation _P_((void));
void ExcPulseEnumRelation _P_((void));
void ExcPulseRelation _P_((void));
void ExcPulseRange _P_((void));
void flowCompTimeRels _P_((void));
void echoTimeRelsAIF _P_((void));
void echoTimeRelsDCE _P_((void));
double minLoopRepetitionTime _P_((void));
void repetitionTimeRels _P_((void));
void LocalGeometryMinimaRels _P_((double *, double *));
void LocalGradientStrengthRels _P_((void));
void LocalFrequencyOffsetRels _P_((void));
void Local_NAveragesRange _P_((void));
void Local_NAveragesHandler _P_((void));
void LocalEchoTimeAIF1Relation _P_((void));
void LocalEchoTimeDCE1Relation _P_((void));
void EffSWhRange _P_((void));
void EffSWhRel _P_((void));
void MyRgInitRel _P_((void));
/* /opt/PV5.1/prog/parx/src/AIFDCE_SAR/BaseLevelRelations.c */
void SetBaseLevelParam _P_((void));
void SetBasicParameters _P_((void));
void SetFrequencyParameters _P_((void));
void SetGradientParameters _P_((void));
void SetInfoParameters _P_((void));
void SetMachineParameters _P_((void));
void SetPpgParameters _P_((void));
/* /opt/PV5.1/prog/parx/src/AIFDCE_SAR/RecoRelations.c */
void SetRecoParam _P_((void));
int PowerOfTwo _P_((int));
