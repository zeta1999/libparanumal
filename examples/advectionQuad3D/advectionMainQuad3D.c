#include "advectionQuad3D.h"

int main(int argc, char **argv){

  // start up MPI
  MPI_Init(&argc, &argv);

  //changes solver mode.
  //options are: DOPRI MRSAAB
  char *mode = "MRSAAB";
  
  // int specify polynomial degree 
  int N = atoi(argv[2]);
  dfloat alpha;
  
  if (argc > 3) 
    alpha = atoi(argv[3])/10.;
  else
    alpha = 1./N;

  // set up mesh stuff
  dfloat sphereRadius = 1;
  mesh_t *mesh = meshSetupQuad3D(atoi(argv[1]), N, sphereRadius,mode);

  // set up boltzmann stuff
  solver_t *solver = advectionSetupPhysicsQuad3D(mesh);
  if (strstr(mode,"DOPRI")) {
    advectionSetupDOPRIQuad3D(solver);
  }
  else if (strstr(mode,"LSERK") || strstr(mode,"RK_SPECTRUM")) {
    advectionSetupLSERKQuad3D(solver);
  }
  else if (strstr(mode,"MRSAAB")) {
    advectionSetupMRSAABQuad3D(solver);
  }
  
  // time step Boltzmann equations
  if (strstr(mode,"DOPRI")) {
    advectionRunDOPRIQuad3D(solver);
  }
  else if (strstr(mode,"LSERK")) {
    advectionRunLSERKbasicQuad3D(solver,alpha);
  }
  else if (strstr(mode,"RK_SPECTRUM")) {
    advectionSpectrumLSERKQuad3D(solver,alpha);
  }
  else if (strstr(mode,"MRSAAB")) {
    advectionRunMRSAABQuad3D(solver);
  }
  
  solver->o_qpre.copyTo(solver->q);
  advectionErrorNormQuad3D(solver,solver->finalTime,"end",0);
  
  /*    mesh->o_q.copyTo(mesh->q);
  dfloat l2 = 0;
  for (iint e = 0; e < mesh->Nelements; ++e) {
    for (iint n = 0; n < mesh->Np; ++n) {
      dfloat x = mesh->x[e*mesh->Np + n];
      dfloat y = mesh->y[e*mesh->Np + n];
      dfloat z = mesh->z[e*mesh->Np + n];
      dfloat t = mesh->finalTime;

      //rotate reference frame back to original
      dfloat xrot = x*cos(t) + y*sin(t);
      dfloat yrot = -1*x*sin(t) + y*cos(t);
      dfloat zrot = z;

      //current q0 is a gaussian pulse
      dfloat qref = 1 + .1*exp(-20*((xrot-1)*(xrot-1)+yrot*yrot+zrot*zrot));

      dfloat J = mesh->vgeo[mesh->Nvgeo*mesh->Np*e + JWID*mesh->Np + n];
      
      l2 += J*(qref - mesh->q[e*mesh->Np*mesh->Nfields + n])*(qref - mesh->q[e*mesh->Np*mesh->Nfields + n]);
    }
  }
  

  printf("norm %.5e\n",sqrt(l2));
  */
  // close down MPI
  MPI_Finalize();

  exit(0);
  return 0;
}