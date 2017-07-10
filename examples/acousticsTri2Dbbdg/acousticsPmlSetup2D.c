#include "acoustics2D.h"

void acousticsPmlSetup2D(mesh2D *mesh){

  //constant pml absorption coefficient
  dfloat xsigma = 40, ysigma = 40;

  //count the pml elements
  mesh->pmlNelements=0;
  for (iint e=0;e<mesh->Nelements;e++) {
    int type = mesh->elementInfo[e];
    if ((type==100)||(type==200)||(type==300))
      mesh->pmlNelements++;
  }

  //construct element and halo lists
  mesh->MRABpmlNelements = (iint *) calloc(mesh->MRABNlevels,sizeof(iint));
  mesh->MRABpmlElementIds = (iint **) calloc(mesh->MRABNlevels,sizeof(iint*));
  mesh->MRABpmlIds = (iint **) calloc(mesh->MRABNlevels, sizeof(iint*));

  //set up the pml
  if (mesh->pmlNelements) {
    mesh->pmlSigmaX = (dfloat *) calloc(mesh->pmlNelements*mesh->cubNp,sizeof(dfloat));
    mesh->pmlSigmaY = (dfloat *) calloc(mesh->pmlNelements*mesh->cubNp,sizeof(dfloat));

    //find the bounding box of the whole domain and interior domain
    dfloat xmin = 1e9, xmax =-1e9;
    dfloat ymin = 1e9, ymax =-1e9;
    dfloat pmlxmin = 1e9, pmlxmax =-1e9;
    dfloat pmlymin = 1e9, pmlymax =-1e9;
    for (iint e=0;e<mesh->Nelements;e++) {
      for (int n=0;n<mesh->Nverts;n++) {
        dfloat x = mesh->EX[e*mesh->Nverts+n];
        dfloat y = mesh->EY[e*mesh->Nverts+n];
        
        pmlxmin = (pmlxmin > x) ? x : pmlxmin;
        pmlymin = (pmlymin > y) ? y : pmlymin;
        pmlxmax = (pmlxmax < x) ? x : pmlxmax;
        pmlymax = (pmlymax < y) ? y : pmlymax;
      }

      //skip pml elements
      int type = mesh->elementInfo[e];
      if ((type==100)||(type==200)||(type==300)) continue;

      for (int n=0;n<mesh->Nverts;n++) {
        dfloat x = mesh->EX[e*mesh->Nverts+n];
        dfloat y = mesh->EY[e*mesh->Nverts+n];
        
        xmin = (xmin > x) ? x : xmin;
        ymin = (ymin > y) ? y : ymin;
        xmax = (xmax < x) ? x : xmax;
        ymax = (ymax < y) ? y : ymax;
      }
    }

    dfloat xmaxScale = pow(pmlxmax-xmax,2);
    dfloat xminScale = pow(pmlxmin-xmin,2);
    dfloat ymaxScale = pow(pmlymax-ymax,2);
    dfloat yminScale = pow(pmlymin-ymin,2);

    //set up the damping factor
    iint cnt = 0;
    for (iint lev =0;lev<mesh->MRABNlevels;lev++){
      for (iint e=0;e<mesh->Nelements;e++) {
        int type = mesh->elementInfo[e];
        if ((type==100)||(type==200)||(type==300)) {
          if (mesh->MRABlevel[e] == lev) {
            mesh->MRABpmlNelements[lev]++; //count the pml elemtents in this level
            
            iint id = e*mesh->Nverts;

            dfloat xe1 = mesh->EX[id+0]; /* x-coordinates of vertices */
            dfloat xe2 = mesh->EX[id+1];
            dfloat xe3 = mesh->EX[id+2];

            dfloat ye1 = mesh->EY[id+0]; /* y-coordinates of vertices */
            dfloat ye2 = mesh->EY[id+1];
            dfloat ye3 = mesh->EY[id+2];
          
            for(iint n=0;n<mesh->cubNp;++n){ /* for each node */
              // cubature node coordinates
              dfloat rn = mesh->cubr[n];
              dfloat sn = mesh->cubs[n];

              /* physical coordinate of interpolation node */
              dfloat x = -0.5*(rn+sn)*xe1 + 0.5*(1+rn)*xe2 + 0.5*(1+sn)*xe3;
              dfloat y = -0.5*(rn+sn)*ye1 + 0.5*(1+rn)*ye2 + 0.5*(1+sn)*ye3;

              if (type==100) { //X Pml
                if(x>xmax)
                  mesh->pmlSigmaX[mesh->cubNp*cnt + n] = xsigma*pow(x-xmax,2)/xmaxScale;
                if(x<xmin)
                  mesh->pmlSigmaX[mesh->cubNp*cnt + n] = xsigma*pow(x-xmin,2)/xminScale;
              } else if (type==200) { //Y Pml
                if(y>ymax)
                  mesh->pmlSigmaY[mesh->cubNp*cnt + n] = ysigma*pow(y-ymax,2)/ymaxScale;
                if(y<ymin)
                  mesh->pmlSigmaY[mesh->cubNp*cnt + n] = ysigma*pow(y-ymin,2)/yminScale;
              } else if (type==300) { //XY Pml
                if(x>xmax)
                  mesh->pmlSigmaX[mesh->cubNp*cnt + n] = xsigma*pow(x-xmax,2)/xmaxScale;
                if(x<xmin)
                  mesh->pmlSigmaX[mesh->cubNp*cnt + n] = xsigma*pow(x-xmin,2)/xminScale;
                if(y>ymax)
                  mesh->pmlSigmaY[mesh->cubNp*cnt + n] = ysigma*pow(y-ymax,2)/ymaxScale;
                if(y<ymin)
                  mesh->pmlSigmaY[mesh->cubNp*cnt + n] = ysigma*pow(y-ymin,2)/yminScale;
              }
            }
            cnt++;
          }
        }
      }
    }

    cnt = 0;
    for (iint lev =0;lev<mesh->MRABNlevels;lev++){
      mesh->MRABpmlElementIds[lev] = (iint *) calloc(mesh->MRABpmlNelements[lev],sizeof(iint));
      mesh->MRABpmlIds[lev] = (iint *) calloc(mesh->MRABpmlNelements[lev],sizeof(iint));
      iint cnt2  =0;
      for (iint e=0;e<mesh->Nelements;e++){
        int type = mesh->elementInfo[e];
        if ((type==100)||(type==200)||(type==300)) {
          if (mesh->MRABlevel[e] == lev) {
            if (type==100) { //X Pml
              mesh->MRABpmlIds[lev][cnt2] = cnt++;
              mesh->MRABpmlElementIds[lev][cnt2++] = e;
            } else if (type==200) { //Y Pml
              mesh->MRABpmlIds[lev][cnt2] = cnt++;
              mesh->MRABpmlElementIds[lev][cnt2++] = e;
            } else if (type==300) { //XY Pml
              mesh->MRABpmlIds[lev][cnt2] = cnt++;
              mesh->MRABpmlElementIds[lev][cnt2++] = e;
            }
          }
        }   
      }
    }

    printf("PML: found %d elements inside absorbing layers and %d elements outside\n",
    mesh->pmlNelements, mesh->Nelements-mesh->pmlNelements);

    // assume quiescent pml
    mesh->pmlNfields = 4;
    mesh->pmlq    = (dfloat*) calloc(mesh->pmlNelements*mesh->Np*mesh->pmlNfields, sizeof(dfloat));
    mesh->pmlrhsq = (dfloat*) calloc(3*mesh->pmlNelements*mesh->Np*mesh->pmlNfields, sizeof(dfloat));

    // set up PML on DEVICE
    mesh->o_pmlq      = mesh->device.malloc(mesh->pmlNelements*mesh->Np*mesh->pmlNfields*sizeof(dfloat), mesh->pmlq);
    mesh->o_pmlrhsq   = mesh->device.malloc(3*mesh->pmlNelements*mesh->Np*mesh->pmlNfields*sizeof(dfloat), mesh->pmlrhsq);
    mesh->o_pmlSigmaX = mesh->device.malloc(mesh->pmlNelements*mesh->cubNp*sizeof(dfloat),mesh->pmlSigmaX);
    mesh->o_pmlSigmaY = mesh->device.malloc(mesh->pmlNelements*mesh->cubNp*sizeof(dfloat),mesh->pmlSigmaY);

    mesh->o_MRABpmlElementIds = (occa::memory *) malloc(mesh->MRABNlevels*sizeof(occa::memory));
    mesh->o_MRABpmlIds = (occa::memory *) malloc(mesh->MRABNlevels*sizeof(occa::memory));
    for (iint lev=0;lev<mesh->MRABNlevels;lev++) {
      if (mesh->MRABpmlNelements[lev]) {
        mesh->o_MRABpmlElementIds[lev] = mesh->device.malloc(mesh->MRABpmlNelements[lev]*sizeof(iint),
           mesh->MRABpmlElementIds[lev]);
        mesh->o_MRABpmlIds[lev] = mesh->device.malloc(mesh->MRABpmlNelements[lev]*sizeof(iint),
           mesh->MRABpmlIds[lev]);
      }
    }
  }
}