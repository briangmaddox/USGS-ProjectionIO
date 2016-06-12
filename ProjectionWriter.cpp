//Implementation for the ProjectionWriter class
//By Chris Bilderback


#ifndef PROJECTIONWRITER_CPP_
#define PROJECTIONWRITER_CPP_

#include <fstream>
#include "ProjectionWriter.h"

using namespace ProjIOLib;

//**********************************************************************
ProjectionWriter::ProjectionWriter() throw() : defaultimagetype(12)
{
}
  
//**********************************************************************
ProjectionWriter::~ProjectionWriter() throw()
{
  unsigned int counter;
  USGSImageLib::ImageOFile * temp;
  
  try
    {
      for (counter=0; counter < imagelist.size(); counter++)
        {
          temp = imagelist.front();
          imagelist.pop_front();
          delete temp;
        }
    }
  catch(...)
    {
      //nothing really we can do
    }
}

//This current will create a default image if it can (in some 
//nonstandard geotiff cases it will force a tif with a world file)
//TODO SOMEDAY: add functionality to have user defined geotiffs  
// ********************************************************************
USGSImageLib::ImageOFile * ProjectionWriter::create
 (Projection * proj, std::string filename, long int width, 
     long int height, int photo, double typoints[], double scale[])
        throw (ProjectionIOException, std::bad_alloc)
{
  USGSImageLib::ImageOFile * out = NULL;

  try
  {
    //check to see if the default projection is a geotiff
    if (defaultimagetype == 12)
    {
      //proccess the geotiff
      out = processGeoTiff(proj, filename, width, height, photo, typoints,
                      scale);
    }
    else
    {
      //only one other case now
      out = processTiff(proj, filename, width, height, photo, typoints,
                         scale);
    }

    if (out != NULL)
    {
      //put it in the list
      imagelist.push_back(out);
    }
    return out;
  }
  catch(...)
  {
    throw ProjectionIOException(PROJECTIONIO_OUTFILEBAD);
    return NULL;
  }
}

//***************************************************************************
USGSImageLib::ImageOFile * ProjectionWriter::processGeoTiff
( Projection * proj, std::string filename, long int width, 
  long int height, int photo, double typoints[], double scale[])
  throw (ProjectionIOException, std::bad_alloc)
{
  USGSImageLib::ImageOFile * out;
  USGSImageLib::GeoTIFFImageOFile * outgeo;
  PROJSYS sys;
  short unsigned int datumcode, unitcode, projcode;
  DATUM datum;
  double StdParallel1 = 0.0;
  double StdParallel2 = 0.0;
  double NatOriginLong = 0.0;
  double NatOriginLat = 0.0;
  double FalseEasting = 0.0;
  double FalseNorthing = 0.0;
  double FalseOriginLong = 0.0;
  double FalseOriginLat = 0.0;
  double FalseOriginEasting = 0.0;
  double FalseOriginNorthing = 0.0;
  double CenterLong = 0.0;
  double CenterLat = 0.0;
  double CenterEasting = 0.0;
  double CenterNorthing = 0.0;
  double ScaleAtNatOrigin = 1.0;
  double AzimuthAngle = 0.0;
  double StraightVertPoleLong = 0.0;

  try
  {
    
    //first get the projsys and datum
    sys = proj->getProjectionSystem();   
    
    
    
    //go ahead and create the geotiff
    if (!(out = new (std::nothrow) USGSImageLib::GeoTIFFImageOFile
          (filename, width, height, photo)))
      throw std::bad_alloc();
    outgeo = dynamic_cast<USGSImageLib::GeoTIFFImageOFile*>(out);
    switch(sys)
    {
    case GEO:
      {
      
        datumcode = getGeoDatum(proj->getDatum());
        unitcode =  getGeoUnit(proj->getUnit());
        outgeo->setGeoKey(GTModelTypeGeoKey, static_cast<unsigned short int>
                          (ModelTypeGeographic));
        outgeo->setGeoKey(GTRasterTypeGeoKey, static_cast<unsigned short int>
                          (RasterPixelIsArea));
        outgeo->setGeoKey(GeographicTypeGeoKey, datumcode);
        outgeo->setGeoKey(GeogAngularUnitsGeoKey, unitcode);
        outgeo->setTiePoints(typoints, 6);
        outgeo->setPixelScale(scale);
        outgeo->setPlanarConfig(1);
        outgeo->setSubfileType(0);
        outgeo->setCompression(COMPRESSION_NONE);
        outgeo->setRowsPerStrip(1);
        return outgeo;
      }
    case UTM://the first two are a special case
      {
        datum = proj->getDatum();
        
        if (datum == NAD83)
          {
            outgeo->setUSGS_UTM(0, 0, 0, 0, 
                              dynamic_cast<UTMProjection *>(proj)->getZone(),
                              1983);
          }        
        else
        {
          if (datum == NAD27)
            outgeo->setUSGS_UTM(0, 0, 0, 0, 
                                dynamic_cast<UTMProjection *>(proj)->getZone(),
                                1927);
          else
            throw ProjectionIOException(PROJECTIONIO_UNKOWNDATUM);
        }
        outgeo->setTiePoints(typoints, 6);
        outgeo->setPixelScale(scale);
        outgeo->setPlanarConfig(1);
        outgeo->setSubfileType(0);
        outgeo->setCompression(COMPRESSION_NONE);
        outgeo->setRowsPerStrip(1);
        return outgeo;
      }
    case SPCS:
      {
        
        unitcode =  getGeoUnit(proj->getUnit());
		datum = proj->getDatum();
        if (datum == NAD83)
          outgeo->setUSGS_SPCS(0, 0, 0, 0, 
                  dynamic_cast<StatePlaneProjection *>(proj)->getZone(),
                               1983);
        else
        {
          if (datum == NAD27)
            outgeo->setUSGS_SPCS(0, 0, 0, 0, 
                       dynamic_cast<StatePlaneProjection *>(proj)->getZone(),
                                 1927);
          else
            throw ProjectionIOException(PROJECTIONIO_UNKOWNDATUM);
        }
        outgeo->setGeoKey(GeogLinearUnitsGeoKey, unitcode);
        outgeo->setTiePoints(typoints, 6);
        outgeo->setPixelScale(scale);
        outgeo->setPlanarConfig(1);
        outgeo->setSubfileType(0);
        outgeo->setCompression(COMPRESSION_NONE);
        outgeo->setRowsPerStrip(1);
        return outgeo;
      }
    case ALBERS:
      {
        AlbersConicProjection* albers = 
          dynamic_cast<AlbersConicProjection*>(proj);
        StdParallel1 = ConvertFromDMS(albers->getFirstStandardParallel());
        StdParallel2 = 
          ConvertFromDMS(albers->getSecondStandardParallel());
        CenterLong = ConvertFromDMS(albers->getCentralMeridian());
        NatOriginLat = ConvertFromDMS(albers->getOriginLatitude());
        FalseEasting = albers->getFalseEasting();
        FalseNorthing = albers->getFalseNorthing();
        break;
      }
    case AZMEQD:
    case GNOMON:
    case LAMAZ:
    case ORTHO:
    case STEREO:
      {
        AzimuthalProjection* azim =
          dynamic_cast<AzimuthalProjection *>(proj);
        CenterLong = ConvertFromDMS(azim->getCentralLongitude());
        CenterLat = ConvertFromDMS(azim->getCentralLatitude());
        FalseEasting = azim->getFalseEasting();
        FalseNorthing = azim->getFalseNorthing();
        break;
      }
    case MILLER:
    case ROBIN:
    case SNSOID:
      {
        PseudocylindricalProjection* pc = 
          dynamic_cast<PseudocylindricalProjection*>(proj);
        CenterLong = ConvertFromDMS(pc->getCentralMeridian());
        FalseEasting = pc->getFalseEasting();
        FalseNorthing = pc->getFalseNorthing();
        break;
      }
    case EQUIDC:
      {
        EquidistantConicProjection* edc =
          dynamic_cast<EquidistantConicProjection*>(proj);
        CenterLat = ConvertFromDMS(edc->getStandardParallel());
        CenterLong = ConvertFromDMS(edc->getCentralMeridian());
        NatOriginLat = ConvertFromDMS(edc->getOriginLatitude());
        FalseEasting = edc->getFalseEasting();
        FalseNorthing = edc->getFalseNorthing();
        StdParallel1 = ConvertFromDMS(edc->getFirstStandardParallel());
        StdParallel2 = ConvertFromDMS(edc->getSecondStandardParallel());
        break;
      }
    case EQRECT:
      {
        EquirectangularProjection* eqrect =
          dynamic_cast<EquirectangularProjection*>(proj);
        CenterLat = ConvertFromDMS(eqrect->getTrueScaleLatitude());
        CenterLong = ConvertFromDMS(eqrect->getCentralMeridian());
        FalseEasting = eqrect->getFalseEasting();
        FalseNorthing = eqrect->getFalseNorthing();
        break;
      }
    case HOM:
      {
        HotineObliqueMercatorProjection* homerc =
          dynamic_cast<HotineObliqueMercatorProjection*>(proj);
        
        // Make sure this is a type B HOM projection
        if ( 0.0 != homerc->getLatitude1()  
             || 0.0 != homerc->getLatitude2() 
             || 0.0 != homerc->getLongitude1() 
             || 0.0 != homerc->getLongitude2() 
             || 0.0 != homerc->getAzimuthPoint())
          throw ProjectionIOException(PROJECTIONIO_UNKOWNPROJECTION);
          
        ScaleAtNatOrigin = homerc->getProjectionCenterScaleFactor();
        AzimuthAngle = homerc->getAzimuthAngle();
        CenterLong = ConvertFromDMS(homerc->getCentralMeridian());
        CenterLat = ConvertFromDMS(homerc->getOriginLatitude());
        FalseEasting = homerc->getFalseEasting();
        FalseNorthing = homerc->getFalseNorthing();
        break;
      }
    case LAMCC:
      {
        LambertConformalConicProjection* lamcc =
            dynamic_cast<LambertConformalConicProjection*>(proj);
        StdParallel1 = ConvertFromDMS(lamcc->getFirstStandardParallel());
        StdParallel2 = ConvertFromDMS(lamcc->getSecondStandardParallel());
        NatOriginLong = ConvertFromDMS(lamcc->getCentralMeridian());
        FalseOriginLat = ConvertFromDMS(lamcc->getOriginLatitude());
        FalseEasting = lamcc->getFalseEasting();
        FalseNorthing = lamcc->getFalseNorthing();
        break;
      }
    case MERCAT:
      {
        MercatorProjection* merc =
          dynamic_cast<MercatorProjection*>(proj);
        NatOriginLong = ConvertFromDMS(merc->getCentralMeridian());
        NatOriginLat = ConvertFromDMS(merc->getOriginLatitude());
        CenterEasting = merc->getFalseEasting();
        CenterNorthing = merc->getFalseNorthing();
        break;
      }
    case POLYC:
      {
        PolyconicProjection* polycon =
          dynamic_cast<PolyconicProjection*>(proj);
        CenterLong = ConvertFromDMS(polycon->getCentralMeridian());
        CenterLat = ConvertFromDMS(polycon->getOriginLatitude());
        FalseEasting = polycon->getFalseEasting();
        FalseNorthing = polycon->getFalseNorthing();
        break;
      }
    case PS:
      {
        PolarStereographicProjection* polar = 
          dynamic_cast<PolarStereographicProjection*>(proj); 
        StraightVertPoleLong = ConvertFromDMS(polar->getLongitudePole());
        NatOriginLat = ConvertFromDMS(polar->getTrueScaleLatitude());
        FalseEasting = polar->getFalseEasting();
        FalseNorthing = polar->getFalseNorthing();
        break;
      }
    case ALASKA:
    case TM:
      {
        TransverseMercatorProjection* trans = 
          dynamic_cast<TransverseMercatorProjection*>(proj);
        ScaleAtNatOrigin = trans->getCentralMeridianScaleFactor();
        CenterLong = ConvertFromDMS(trans->getCentralMeridian());
        NatOriginLat = ConvertFromDMS(trans->getOriginLatitude());
        FalseEasting = trans->getFalseEasting();
        FalseNorthing = trans->getFalseNorthing();
        break;
      }
    case VGRINT:
      {
        VanDerGrintenProjection* vandg =
          dynamic_cast<VanDerGrintenProjection*>(proj);
        CenterLat = ConvertFromDMS(vandg->getOriginLatitude());
        CenterLong =ConvertFromDMS(vandg->getCentralMeridian());
        FalseEasting = vandg->getFalseEasting();
        FalseNorthing = vandg->getFalseNorthing();
        break;
      }
    case GOOD: //This begins cases which will be forced to be tif/worldfile
    case MOLL:
    case IMOLL:
      {
        delete outgeo;
        return processTiff(proj, filename, width, height, photo, typoints,
                         scale);
      }

      default:
        throw ProjectionIOException(PROJECTIONIO_UNKOWNPROJECTION);
    }
   
    projcode = getGeoProj(sys);
    datumcode = getGeoDatum(proj->getDatum());
    unitcode =  getGeoUnit(proj->getUnit());
    //now write out all parameters
    outgeo->setGeoKey(GTModelTypeGeoKey, static_cast<unsigned short>
                      (ModelProjected));
    outgeo->setGeoKey(GTRasterTypeGeoKey,static_cast<unsigned short>
                      (RasterPixelIsArea));
    outgeo->setGeoKey(GeographicTypeGeoKey, datumcode);
    outgeo->setGeoKey(ProjectedCSTypeGeoKey,static_cast<unsigned short>
                      (32767));
    outgeo->setGeoKey(ProjectionGeoKey, static_cast<unsigned short>
                      (32767));
    outgeo->setGeoKey(ProjCoordTransGeoKey, projcode);
    outgeo->setGeoKey(ProjLinearUnitsGeoKey, unitcode);
    outgeo->setGeoKey(ProjStdParallel1GeoKey, StdParallel1);
    outgeo->setGeoKey(ProjStdParallel2GeoKey, StdParallel2);
    outgeo->setGeoKey(ProjNatOriginLongGeoKey, NatOriginLong);
    outgeo->setGeoKey(ProjNatOriginLatGeoKey,  NatOriginLat);
    outgeo->setGeoKey(ProjFalseEastingGeoKey, FalseEasting);
    outgeo->setGeoKey(ProjFalseNorthingGeoKey, FalseNorthing);
    outgeo->setGeoKey(ProjFalseOriginLongGeoKey, FalseOriginLong);
    outgeo->setGeoKey(ProjFalseOriginLatGeoKey, FalseOriginLat);
    outgeo->setGeoKey(ProjFalseOriginEastingGeoKey,  FalseOriginEasting);
    outgeo->setGeoKey(ProjFalseOriginNorthingGeoKey, FalseOriginNorthing);
    outgeo->setGeoKey(ProjCenterLongGeoKey, CenterLong);
    outgeo->setGeoKey(ProjCenterLatGeoKey, CenterLat);
    outgeo->setGeoKey(ProjCenterEastingGeoKey, CenterEasting);
    outgeo->setGeoKey(ProjCenterNorthingGeoKey, CenterNorthing);
    outgeo->setGeoKey(ProjScaleAtNatOriginGeoKey, ScaleAtNatOrigin);
    outgeo->setGeoKey(ProjAzimuthAngleGeoKey, AzimuthAngle);
    outgeo->setGeoKey(ProjStraightVertPoleLongGeoKey, StraightVertPoleLong);
    
    outgeo->setTiePoints(typoints, 6);
    outgeo->setPixelScale(scale);
    outgeo->setPlanarConfig(1);
    outgeo->setSubfileType(0);
    outgeo->setCompression(COMPRESSION_NONE);
    outgeo->setRowsPerStrip(1);
    return outgeo;
  }
  catch(ProjectionIOException & e)
    {
      throw e;
    }
  catch (...)
  {
    delete outgeo;
    return NULL;
  }
}

//****************************************************************************
void ProjectionWriter::setDefaultImageType(const int & inImageType) throw()
{
  defaultimagetype = inImageType;
}

//***************************************************************************
int ProjectionWriter::getDefaultImageType() throw()
{
  return defaultimagetype;
}


//****************************************************************************
USGSImageLib::ImageOFile * ProjectionWriter::processTiff
( Projection * proj, std::string filename, long int width, 
  long int height, int photo, double typoints[], double scale[])
  throw (ProjectionIOException, std::bad_alloc)
{
  USGSImageLib::ImageOFile * out = NULL;
  USGSImageLib::TIFFImageOFile * tifout = NULL;

  try
  {
                                                //first thing is to write out  
    if (!writeTFW(filename, typoints, scale))   //arc world file
      throw ProjectionIOException(PROJECTIONIO_CANNOT_WORDFILE);

                                                //now create the file
    if (!(out = new (std::nothrow) USGSImageLib::TIFFImageOFile
          (filename, width, height, photo)))
      throw std::bad_alloc();
    tifout = dynamic_cast<USGSImageLib::TIFFImageOFile*>(out);
    if (!tifout)
      return NULL;
    tifout->setPlanarConfig(1);
    tifout->setSubfileType(0);
    tifout->setCompression(COMPRESSION_NONE);
    return out;                                 //done
  }
  catch(ProjectionIOException & temp)
  {
    throw temp;                                 //just rethrow
  }
  catch(...)
  {
    delete out;
    throw ProjectionIOException(PROJECTIONIO_CANNOT_TIFF);
  }

}



// *************************************************************************
// This function writes an ARC World file to [outputfilename].TFW 
bool ProjectionWriter::writeTFW( std::string filename, double typoints[], 
                                 double scale[])
  throw ()
{
  std::string tfwname;                              // name for the world file
 
  std::ofstream outfile;
  int dotpos;


  try
  {
    dotpos = filename.find_first_of(".");           //find the . in the file
    
    if (dotpos == -1)
      return false;
                                                    //append the tfw extension
    tfwname = filename.substr(0, dotpos) + std::string(".tfw");         
                                                    //open the file
    outfile.open(tfwname.c_str(), std::ios::out | std::ios::trunc);   
    if (outfile.fail())                             //handle error
    {
      outfile.close();
      return false;
    }

    outfile.setf(std::ios::fixed);                       //fixed point
    outfile.precision(5);                                        
    outfile << scale[0] << std::endl;                    //xscale
    outfile << "0.00000" << std::endl;                   //no rotation
    outfile << "0.00000" << std::endl;                   //(prewarped)
    outfile << ( -1.0 * scale[1]) << std::endl;          //-yscale
    outfile << typoints[3] << std::endl;                 //upper left x
    outfile << typoints[4] << std::endl;                 //upper left y
    
    outfile.close();
    return true;
  }
  catch (...)
  {
    outfile.close();
    return false;    
  }
}



//************************************************************************
std::list<USGSImageLib::ImageOFile *> ProjectionWriter::getImages()
throw()
{
  return imagelist;
}
 
//***********************************************************************
bool ProjectionWriter::getImages
(USGSImageLib::ImageOFile * in[])
  throw()
{
  unsigned int counter;
  std::list<USGSImageLib::ImageOFile*>::iterator it;
  
  it = imagelist.begin();
  for (counter = 0; counter < imagelist.size(); counter++)
  {
    in[counter] = (*it);
    it++;
  }
  return true;
}


//*********************************************************************
USGSImageLib::ImageOFile * ProjectionWriter::operator[]
(unsigned int pos)  throw (ProjectionIOException)
{
  std::list<USGSImageLib::ImageOFile*>::iterator it;
  unsigned int counter;

  if (pos >= imagelist.size()-1 || pos < 0)
    throw ProjectionIOException(PROJECTIONIO_OUTOFRANGE);
  
  it = imagelist.begin();
  for (counter = 0; counter != pos; counter++)
    it++;
  return (*it);
}

//*******************************************************************
USGSImageLib::ImageOFile * ProjectionWriter::
get(const std::string& infilename) throw()
{
  std::list<USGSImageLib::ImageOFile*>::iterator it;
  std::string tempfile;

  for (it = imagelist.begin(); it != imagelist.end(); it++)
  {
    (*it)->getFileName(tempfile);                  //get the file name
    if (tempfile == infilename)
      return *it;
  }

  return NULL; //made it this far which is bad
}
  

//********************************************************************
bool ProjectionWriter::removeImage(unsigned int pos) throw ()
{
  std::list<USGSImageLib::ImageOFile*>::iterator it;
  unsigned int counter;

  if (pos >= imagelist.size()-1 || pos < 0)
    return false;
  
  it = imagelist.begin();
  for (counter = 0; counter != pos; counter++)
    it++;
  delete (*it);
  imagelist.erase(it);
  return true;
}

//*******************************************************************
bool ProjectionWriter::removeImage(const std::string& infilename) throw()
{
  std::list<USGSImageLib::ImageOFile*>::iterator it;
  std::string tempfilename;

  for (it = imagelist.begin(); it != imagelist.end(); it++)
  {
    (*it)->getFileName(tempfilename);
    
    if (tempfilename == infilename)
    {
      delete (*it);
      imagelist.erase(it);
      return true;
    }
  }
  return false;
}


//*******************************************************************
void ProjectionWriter::removeAllImages()
  throw ()
{
  unsigned int counter;
  USGSImageLib::ImageOFile * temp;
  
  for (counter=0; counter < imagelist.size(); counter++)
    {
      temp = imagelist.front();
      imagelist.pop_front();
      delete temp;
    }
}

//*********************************************************************
unsigned int ProjectionWriter::numberOfImages() throw()
{
  return imagelist.size();
}


  
//************************************************************************
short unsigned int ProjectionWriter::getGeoDatum(DATUM indatum) 
  throw(ProjectionIOException)
{
  switch(indatum)
    {
    case ADINDAN: 
      return GCS_Adindan;
    case ARC1950:
      return GCS_Arc_1950;
    case ARC1960:
      return GCS_Arc_1960;
    case CAPE:
      return GCS_Cape;
    case EUROPEAN_DATUM_1950:
      return GCS_ED50;
    case GEODETIC_DATUM_1949:
      return GCS_GD49;
    case HU_TZU_SHAN:
      return GCS_Hu_Tzu_Shan;
    case INDIAN:
      return GCS_Indian_1975;
    case NAD27:
      return GCS_NAD27;
    case NAD83:
      return GCS_NAD83;
    case ORDNANCE_SURVEY_1936:
      return GCS_OSGB_1936;
    case PULKOVO_1942:
      return GCS_Pulkovo_1942;
    case PROVISIONAL_S_AMERICAN_1956:
      return GCS_PSAD56;
    case TOKYO:
      return GCS_Tokyo;
    case WGS_72:
      return GCS_WGS_72;
    case WGS_84:
      return GCS_WGS_84;
    default:
      throw ProjectionIOException(PROJECTIONIO_UNKOWNDATUM);
    }
}
  
//************************************************************************
short unsigned int ProjectionWriter::getGeoUnit(UNIT inunit) 
  throw (ProjectionIOException)
{
   switch ( inunit )
    {
    case METERS:
      return Linear_Meter;
     
    case US_FEET:
      return Linear_Foot;
    case RADIANS:
      return Angular_Radian;
    case ARC_DEGREES:
      return Angular_Degree;
    case ARC_SECONDS:
      return Angular_Arc_Second;
  
    default:
      throw ProjectionIOException(PROJECTIONIO_UNKOWNUNITS);
    }
}

//********************************************************************
short unsigned int ProjectionWriter::getGeoProj(PROJSYS insys) 
  throw(ProjectionIOException)
{
  
  switch(insys)
    {
    case GEO:
    case UTM://the first two are a special case
    case SPCS:
      return 0;
    case ALBERS:
      return CT_AlbersEqualArea;
    case AZMEQD:
      return CT_AzimuthalEquidistant;
    case GNOMON:
      return CT_Gnomonic;
    case LAMAZ:
      return CT_LambertAzimEqualArea;
    case ORTHO:
      return CT_Orthographic;
    case STEREO:
      return CT_Stereographic;
    case MILLER:
      return CT_MillerCylindrical;
    case ROBIN:
      return CT_Robinson;
    case SNSOID:
      return CT_Sinusoidal;
    case EQUIDC:
      return CT_EquidistantConic;
    case EQRECT:
      return CT_Equirectangular;
    case HOM:
      return CT_ObliqueMercator;
    case LAMCC:
      return CT_LambertConfConic_1SP;
    case MERCAT:
      return CT_Mercator;
    case POLYC:
      return CT_Polyconic;
    case PS:
      return CT_PolarStereographic;
    case ALASKA:
      return CT_AlaskaConformal;
    case TM:
      return CT_TransverseMercator;
    case VGRINT:
      return CT_VanDerGrinten;
    default:
      throw ProjectionIOException(PROJECTIONIO_UNKOWNPROJECTION);
    }
}


//************************************************************************
double ProjectionWriter::ConvertFromDMS(double dms) throw()
{
  double sign = 1.0;
  double temp = dms;
  int deg;
  int min;
  if ( temp < 0.0 )
  {
    temp = -temp;
    sign = -1.0;
  }
  
  // Extract the degrees, minutes, and seconds
  deg = static_cast<int>(temp / 1000000);
  temp -= deg * 1000000;
  min = static_cast<int>(temp / 1000);
  temp -= min * 1000;
  temp /= 3600;
  temp += min / 60.0;
  temp += deg;
  return temp *sign;
}
#endif








