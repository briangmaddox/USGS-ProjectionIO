//Projection Writer class with writes projection to a geotiff
//By Chris Bilderback
//Rewritten July 20 to add support for tiff output files with a world file
//Changed so that the user could just output a world file if they wish...


#ifndef PROJECTIONWRITER_H_
#define PROJECTIONWRITER_H_

#include "ImageLib/GeoTIFFImageOFile.h"
#include "ImageLib/SPCSMapper.h"
#include "ImageLib/ImageCommon.h"
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
#include "ProjectionLib/GoodeHomolosineProjection.h"
#include <list>
#include <string>
#include "ProjectionIOException.h"

extern "C"
{
#include "geokeys.h"
}

using namespace ProjLib;

namespace ProjIOLib
{

class ProjectionWriter
{
public:
  //Construction / Destruction
  ProjectionWriter() throw();
  virtual ~ProjectionWriter() throw();
  
  //Returns a ImageOFile with either tags written or
  //a world file ouput
  USGSImageLib::ImageOFile * create
    (Projection * proj, std::string filename, long int width, 
     long int height, int photo, double typoints[], double scale[])
        throw (ProjectionIOException, std::bad_alloc);
  
  //Return a list of all the output images loaded
  std::list<USGSImageLib::ImageOFile *> getImages() throw();
  bool getImages(USGSImageLib::ImageOFile * in[])
     throw ();

  //Returns a reference to a specific ouput image
  USGSImageLib::ImageOFile * operator[](unsigned int pos)
    throw (ProjectionIOException);
  USGSImageLib::ImageOFile * get(const std::string& infilename) throw();

  
  //Delete a loaded ouput image
  bool removeImage(unsigned int pos) throw ();
  bool removeImage(const std::string& infilename) throw();
  
  //Deletes all loaded output images
  void removeAllImages() throw ();
  
  //returns the number of ouput images loaded
  unsigned int numberOfImages() throw();

  //Allows the user to set the default output image type
  //(currently only geotiff or tiff with a world file is supported)
  void setDefaultImageType(const int & inImageType) throw();

  //Allows the user to check default type
  int getDefaultImageType() throw();

  // writes a world file
  bool writeTFW( std::string filename, double typoints[], double scale[])
    throw ();

private:
  
  // handles geotiff
  USGSImageLib::ImageOFile * processGeoTiff
    ( Projection * proj, std::string filename, long int width, 
      long int height, int photo, double typoints[], double scale[])
    throw (ProjectionIOException, std::bad_alloc);

  //handles tiff
  USGSImageLib::ImageOFile * processTiff
    ( Projection * proj, std::string filename, long int width, 
      long int height, int photo, double typoints[], double scale[])
    throw (ProjectionIOException, std::bad_alloc);
  

  //gets the geotiff datum from projlib datum type
  short unsigned int getGeoDatum(DATUM indatum) throw(ProjectionIOException);
  
  //gets the geotiff unit code from the projlib unit type
  short unsigned int getGeoUnit(UNIT inunit) throw (ProjectionIOException);

  //gets the geotiff projection code from the projlib unit type
  short unsigned int getGeoProj(PROJSYS insys) throw(ProjectionIOException);

  //converts DMS 
  double ConvertFromDMS(double dms) throw();
  

  std::list<USGSImageLib::ImageOFile *> imagelist;
  int defaultimagetype;

};

}
#endif






