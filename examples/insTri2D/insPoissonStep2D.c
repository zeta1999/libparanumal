#include "ins2D.h"

// complete a time step using LSERK4
void insPoissonStep2D(solver_t *ins, iint tstep, iint haloBytes,
				       dfloat * sendBuffer, dfloat * recvBuffer, 
				        char   * options){

mesh2D *mesh = ins->mesh; 

dfloat t = tstep*ins->dt + ins->dt;

//Exctract Halo On Device

	if(mesh->totalHaloPairs>0){
	 
    ins->poissonHaloExtractKernel(mesh->Nelements,
                           mesh->totalHaloPairs,
                           mesh->o_haloElementList,
                           ins->o_U,
                           mesh->o_haloBuffer);

    // copy extracted halo to HOST 
    mesh->o_haloBuffer.copyTo(sendBuffer);            
    // start halo exchange
    meshHaloExchangeStart(mesh,
                          mesh->Np*ins->NVfields*sizeof(dfloat), 
                          sendBuffer,
                          recvBuffer);
  	}



   // computes div u^(n+1) volume term
   ins->poissonRhsVolumeKernel(mesh->Nelements,
                                 mesh->o_vgeo,
                                 mesh->o_DrT,
                                 mesh->o_DsT,
                                 mesh->o_MM,
                                 ins->o_U,  
                                 ins->o_rhsPr);


    // COMPLETE HALO EXCHANGE
  if(mesh->totalHaloPairs>0){
  // wait for halo data to arrive
    meshHaloExchangeFinish(mesh);

    mesh->o_haloBuffer.copyFrom(recvBuffer); 

    ins->poissonHaloScatterKernel(mesh->Nelements,
                                  mesh->totalHaloPairs,
                                  mesh->o_haloElementList,
                                  ins->o_U,
                                  mesh->o_haloBuffer);
  }


   //computes div u^(n+1) surface term
  ins->poissonRhsSurfaceKernel(mesh->Nelements,
  	                          ins->dt,	
                              ins->g0,
                              mesh->o_sgeo,
                              mesh->o_FMMT,
                              mesh->o_vmapM,
                              mesh->o_vmapP,
                              mesh->o_EToB,
                              t,
                              mesh->o_x,
                              mesh->o_y,
                              ins->o_U,
                              ins->o_rhsPr);



  ins->poissonRhsIpdgBCKernel(mesh->Nelements,
                                mesh->o_sgeo,
                                mesh->o_vgeo,
                                mesh->o_DrT,
                                mesh->o_DsT,
                                mesh->o_FMMT,
                                mesh->o_vmapM,
                                mesh->o_vmapP,
                                mesh->o_EToB,
                                t,
                                ins->dt,
                                ins->tau,
                                mesh->o_x,
                                mesh->o_y,
                                ins->o_Pr,
                                ins->o_rhsPr
                                );



// Solve for prI


   
}
