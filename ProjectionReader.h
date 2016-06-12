// ProjectionReader class by Chris Bilderback which generates projections 
// based on 
// ImageLib input files. Its main utitily is that it creates the projections
// and manages the memory for the client.  Much of this came from DLG viewer 32
// by Scott Childs 

#ifndef PROJECTIONREADER_H_
#define PROJECTIONREADER_H_


#include "ImageLib/GeoTIFFImageIFile.h"
#include "ImageLib/DOQImageIFile.h"
#include "ImageLib/SPCSMapper.h"
#include "ProjectionLib/UTMProjection.h"
#include "ProjectionLib/StatePlaneProjection.h"
#include "ProjectionLib/AlbersConicProjection.h"
#include "ProjectionLib/LambertConformalConicProjection.h"
#include "ProjectionLib/MercatorProjection.h"
#include "ProjectionLib/PolarStereographicProjection.h"
#include "ProjectionLib/PolyconicProjection.h"
#include "ProjectionLib/EquidistantConicProjection.h"
#include "ProjectionLib/TransverseMercatorProjection.h"
#include "ProjectionLib/StereographicProjection.h"
#include "ProjectionLib/LambertAzimuthalProjection.h"
#include "ProjectionLib/AzimuthalEquidistantProjection.h"
#include "ProjectionLib/GnomonicProjection.h"
#include "ProjectionLib/OrthographicProjection.h"
#include "ProjectionLib/SinusoidalProjection.h"
#include "ProjectionLib/EquirectangularProjection.h"
#include "ProjectionLib/MillerCylindricalProjection.h"
#include "ProjectionLib/VanDerGrintenProjection.h"
#include "ProjectionLib/RobinsonProjection.h"
#include "ProjectionLib/AlaskaConformalProjection.h"
#include "ProjectionLib/HotineObliqueMercatorProjection.h"
#include "ProjectionLib/GeographicProjection.h"
#include <list>
#include "ProjectionIOException.h"

extern "C"
{
#include "geokeys.h"
}


using namespace ProjLib;

namespace ProjIOLib
{

class ProjectionReader
{
public:
  //constuction
  ProjectionReader() throw();
  virtual ~ProjectionReader() throw();
  
  //creates a new projection, returns a reference to the projection
  //DO NOT delete memory returned/ it will be handled in the destruction
  Projection * createProjection( USGSImageLib::ImageIFile *  infile) 
    throw(ProjectionIOException);

  //Overladed createProjection for use until getImageType works in imagelib
  Projection * createProjection(USGSImageLib::GeoTIFFImageIFile * ingeo)
    throw(ProjectionIOException);
  Projection * createProjection(USGSImageLib::DOQImageIFile* indoq)
    throw(ProjectionIOException);


  //returns a list of all loaded projections
  std::list<Projection *> getProjections() throw();
  void getProjections(Projection* in[], unsigned int insize) 
    throw(ProjectionIOException);
  
  //returns a projection in a certain position in the list
  //warning this will be slow because of std::list
  Projection * operator[](unsigned int pos) throw(ProjectionIOException);

  //removes a projection in a certain position in the list
  bool removeProjection(unsigned int pos) throw();

  //removes all loaded projections
  void removeAllProjections() throw();
  
  //returns the size of the projectionlist
  unsigned int numberOfProjections();

  
 private:
  //geotiff parser
  Projection * parseGeotiff(USGSImageLib::GeoTIFFImageIFile * ingeo) 
    throw(ProjectionIOException);
  
  //Doq parser
  Projection * parseDOQ(USGSImageLib::DOQImageIFile * indoq)
    throw(ProjectionIOException);

  //UTM/State plane parser for geotiff
  Projection * parseUTMSPCS(USGSImageLib::GeoTIFFImageIFile * ingeo)
    throw(ProjectionIOException);

  //Get a ProjLib datumtype from Geotiff code
  DATUM getDatumFromGeo(unsigned short ingeocode) 
    throw(ProjectionIOException);
  
  //Get a ProjLib unittype from Geotiff code
  UNIT getUnitFromGeo(unsigned short ingeocode)
    throw(ProjectionIOException);
  
  double ConvertToDMS(double degrees) throw();


  std::list<Projection *> projectionlist;  //the list of projections created

};
}
#endif




