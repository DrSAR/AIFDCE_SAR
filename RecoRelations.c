/****************************************************************
 *
 * $Source: /pv/CvsTree/pv/gen/src/prg/methods/FC2D_ANGIO/RecoRelations.c,v $
 *
 * Copyright (c) 2002
 * Bruker BioSpin MRI GmbH
 * D-76275 Ettlingen, Germany
 *
 * All Rights Reserved
 *
 *
 * $SId$
 *
 ****************************************************************/

static const char resid[] = "$Id: RecoRelations.c,v 1.4.2.4 2005/11/21 18:01:12 pull Exp $ (C) 2002 Bruker BioSpin GmbH";

#define DEBUG           0
#define DB_MODULE       1
#define DB_LINE_NR      1



#include "method.h"

void SetRecoParam( void )

{

  int dim,i,size,ftSize[3];

  DB_MSG(("-->SetRecoParam\n"));

  /* set baselevel reconstruction parameter */
  /* default initialization of reco based on acqp pars allready set */

  ATB_InitDefaultReco();

  for(i=0; i<PTB_GetSpatDim(); i++)
    ftSize[i] = PVM_Matrix[i]*PVM_AntiAlias[i];
  DB_MSG(("ftSize = ( %i ), (%i )\n", ftSize[0], ftSize[1])); 

  if(PVM_EncUseMultiRec == Yes)
  {
   /* select method specific reconstruction method */
   RECO_mode = USER_MODE;
   ParxRelsParRelations("RECO_mode",Yes);
   ATB_InitUserModeReco(ACQ_dim, PVM_EncMatrix, ftSize, 
		     PVM_EncSteps1, PVM_EncSteps2,
		     PVM_EncNReceivers, PVM_EncPpiAccel1, PVM_EncPpiRefLines1,
		     NI, ACQ_obj_order, ACQ_phase_factor, PVM_EchoPosition);
  }

  /* set reco rotate according to phase offsets     */

  dim = PTB_GetSpatDim();

  /* set reco sizes and ft_mode (dim 2&3) */
  /* (dim 1 is kept as it was set by ATB_InitDefaultReco) */
  for(i=1; i<dim; i++)
  {
    size = PVM_Matrix[i]*PVM_AntiAlias[i];
    RECO_ft_mode[i] = (size == PowerOfTwo(size)) ?  COMPLEX_FFT:COMPLEX_FT;
    RECO_ft_size[i] = size;
    RECO_size[i] = PVM_Matrix[i];
  }  


  ATB_SetRecoRotate(PVM_EffPhase1Offset,
                    PVM_Fov[1]*PVM_AntiAlias[1],
                    NSLICES,    
                    1,           /* 1 echo image    */
                    1) ;         /* phase1 direction*/

  if(dim==3)
  {

    ATB_SetRecoRotate(PVM_EffPhase2Offset,      
		      PVM_Fov[2]*PVM_AntiAlias[2],
		      NSLICES,     
		      1,           /* 1 echo image    */
		      2) ;         /* phase2 direction*/

  }


  /* set reco offset */

  ATB_SetRecoOffset(RECO_ft_size,
                    PVM_AntiAlias,
	            NI,              
	            dim);
  for(i=0;i<dim;i++)
    RECO_fov[i]= PVM_FovCm[i];

  ParxRelsParRelations("RECO_fov",Yes);

  ATB_SetRecoTransposition(PtrType3x3 ACQ_grad_matrix[0],
                           PVM_NSPacks,
                           NSLICES,
                           ACQ_ns_list_size,
                           ACQ_obj_order) ;

  RECO_map_mode = ABSOLUTE_MAPPING;
  ParxRelsParRelations("RECO_map_mode",Yes);

  DB_MSG(("<--SetRecoParam\n"));

}


int PowerOfTwo(int x)
/* returns next power of two */
{

 return (1<<(int)ceil(log((double)x)/log(2.0)));
}
