/*
 *      .o.       ooo        ooooo ooooooooo.             ooooooo  ooooo 
 *     .888.      `88.       .888' `888   `Y88.            `8888    d8'  
 *    .8"888.      888b     d'888   888   .d88'  .ooooo.     Y888..8P    
 *   .8' `888.     8 Y88. .P  888   888ooo88P'  d88' `88b     `8888'     
 *  .88ooo8888.    8  `888'   888   888`88b.    888ooo888    .8PY888.    
 * .8'     `888.   8    Y     888   888  `88b.  888    .o   d8'  `888b   
 *o88o     o8888o o8o        o888o o888o  o888o `Y8bod8P' o888o  o88888o 
 *
 */

#include <cmath>

#include "AMReX_GeometryShop.H"
#include "AMReX_BoxIterator.H"
#include "AMReX_ParmParse.H"
#include "AMReX_RealVect.H"
#include "AMReX_SphereIF.H"
#include "AMReX_EBGraph.H"
namespace amrex
{
////////////
  int checkGraph(const EBGraph& a_ebgraph)
  {
    const Box& region = a_ebgraph.getDomain();
    ///account for regular and covered cells
    for (BoxIterator bit(region); bit.ok(); ++bit)
    {
      IntVect iv = bit();
      //check to see that there is never a regular cell next
      //to a covered cell
      for (int idir = 0; idir < SpaceDim; idir++)
      {
        for (SideIterator sit; sit.ok(); ++sit)
        {
          IntVect ivshift = BASISV(idir);
          ivshift[idir] *= sign(sit());
          IntVect ivneigh = iv + ivshift;
          if (a_domain.contains(ivneigh))
          {
            if((isRegular(iv) && isCovered(ivneigh) )  ||
               (isCovered(iv) && isRegular(ivneigh)))
            {
              return -2;
            }

            if((isRegular(iv)     && isMultiValued(ivneigh)) ||
               (isRegular(ivneigh) && isMultiValued(iv)     ))
            {
              return -3;
            }

            //check to see if the vof matches the faces
            std::vector<FaceIndex> faces = a_ebgraph.getFaces(iv, idir, sit());
            for(int iface = 0; iface < faces.size(); iface++)
            {
              const FaceIndex& face = faces[iface];
              VolIndex neighVoF = face.getVoF(sit());
              if(neighVoF.gridIndex() != ivshift)
              {
                return -4;
              }
              if(neighVoF.cellIndex() < 0)
              {
                return -5;
              }
            }
          }
        }
      }
    }
    return 0;
  }
////////////
  int checkGraph()
  {
    Real radius = 0.5;
    Real domlen = 1;
    std::vector<Real> centervec(SpaceDim);
    std::vector<int>  ncellsvec(SpaceDim);

    ParmParse pp;
    pp.getarr(  "n_cell"       , ncellsvec, 0, SpaceDim);
    pp.get(   "sphere_radius", radius);
    pp.getarr("sphere_center", centervec, 0, SpaceDim);
    pp.get("domain_length", domlen);                     // 

    SphereIF sphere(a_radius, a_center, insideRegular);
    int verbosity = 0;
    ParmParse pp;
    pp.get("verbosity", verbosity);
    GeometryShop gshop(sphere, verbosity);
    BaseFab<int> regIrregCovered;
    std::vector<IrregNode> nodes;

    IntVect ivlo = IntVect::TheZeroVector();
    IntVect ivhi;
    for(int idir = 0; idir < SpaceDim; idir++)
    {
      ivhi[idir] = ncellsvec[idir] - 1;
    }

    RealVect center = 0.5*RealVect::Unit;
    Box domain(ivlo, ivhi);
    Box validRegion = domain;
    Box ghostRegion = domain; //whole domain so ghosting does not matter
    RealVect origin = RealVect::Zero;
    gshop.fillGraph(regIrregCovered, nodes, validRegion, ghostRegion, domain, origin, a_dx);
  

    EBGraph ebgraph(domain, 1);
    ebgraph.buildGraph(regIrregCovered, nodes, domain, domain);
    int eekflag = checkGraph(ebgraph);
    return eekflag;
  }

}
int
main(int argc,char **argv)
{
#ifdef CH_MPI
  MPI_Init(&argc, &argv);
#endif

  const char* in_file = "sphere.inputs";
  //parse input file
  ParmParse pp;
  pp.Initialize(0, NULL, in_file);

  // check volume and surface area of approximate sphere
  int eekflag = amrex::checkGraph();

  if (eekflag != 0)
  {
    cout << "non zero eek detected = " << eekflag << endl;
    cout << "sphere test failed" << endl;
  }
  else
  {
    cout << "sphere test passed" << endl;
  }

  return 0;
}


