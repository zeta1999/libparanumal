#include "ins2D.h"

// complete a time step using LSERK4
void insUpdateStep2D(solver_t *ins, iint tstep, iint haloBytes,
				       dfloat * sendBuffer, dfloat * recvBuffer, 
				        char   * options){

mesh2D *mesh = ins->mesh; 

dfloat t = tstep*ins->dt + ins->dt;

//Exctract Halo On Device

	if(mesh->totalHaloPairs>0){
	 
    ins->updateHaloExtractKernel(mesh->Nelements,
                           mesh->totalHaloPairs,
                           mesh->o_haloElementList,
                           ins->o_PrI,
                           mesh->o_haloBuffer);

    // copy extracted halo to HOST 
    mesh->o_haloBuffer.copyTo(sendBuffer);            
   
    // start halo exchange
    meshHaloExchangeStart(mesh,
                          mesh->Np*sizeof(dfloat), 
                          sendBuffer,
                          recvBuffer);
  	}



   // computes div u^(n+1) volume term
   ins->updateVolumeKernel(mesh->Nelements,
                                 mesh->o_vgeo,
                                 mesh->o_DrT,
                                 mesh->o_DsT,
                                 ins->o_PrI,  
                                 ins->o_rhsU);


    // COMPLETE HALO EXCHANGE
  if(mesh->totalHaloPairs>0){
  // wait for halo data to arrive
    meshHaloExchangeFinish(mesh);

    mesh->o_haloBuffer.copyFrom(recvBuffer); 

    ins->updateHaloScatterKernel(mesh->Nelements,
                                  mesh->totalHaloPairs,
                                  mesh->o_haloElementList,
                                  ins->o_PrI,
                                  mesh->o_haloBuffer);
  }


   //computes div u^(n+1) surface term
  ins->updateSurfaceKernel(mesh->Nelements,
                              mesh->o_sgeo,
                              mesh->o_LIFTT,
                              mesh->o_vmapM,
                              mesh->o_vmapP,
                              mesh->o_EToB,
                              t,
                              mesh->o_x,
                              mesh->o_y,
                              ins->o_PrI,
                              ins->o_rhsU);

   //computes div u^(n+1) surface term
  ins->updateUpdateKernel(mesh->Nelements,
                              ins->dt,  
                              ins->g0,
                              ins->o_U,
                              ins->o_Pr,
                              ins->o_rhsU);






// Solve for o_PrI


   
}
