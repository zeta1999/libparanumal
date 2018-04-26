#include <math.h>
#include "mesh3D.h"

void cnsCavitySolution3D(dfloat x, dfloat y, dfloat z, dfloat t,
			 dfloat *u, dfloat *v, dfloat *w, dfloat *p){

  // dudt = -dpdx
  // dvdt = -dpdy
  // dpdt = -dudx -dvdy

  // boundary conditions
  // dpdn = 0
  // u.n = 0

  dfloat pi = M_PI;
  
  *u = sin(pi*t/sqrt(2.))*cos(pi*x/2.)*sin(pi*y/2.)*sin(pi*z/2.);
  *v = sin(pi*t/sqrt(2.))*sin(pi*x/2.)*cos(pi*y/2.)*sin(pi*z/2.);
  *w = sin(pi*t/sqrt(2.))*sin(pi*x/2.)*sin(pi*y/2.)*cos(pi*z/2.);
  *p = (-sqrt(2.))*cos(pi*t/sqrt(2.))*sin(pi*x/2.)*sin(pi*y/2.)*sin(pi*z/2.);

}
