//Implementation for the ProjectionReader class
//By Chris Bilderback


#ifndef PROJECTIONREADER_CPP_
#define PROJECTIONREADER_CPP_

#include "ProjectionReader.h"

using namespace ProjIOLib;

//**************************************************************************
ProjectionReader::ProjectionReader() throw()
{}

//**************************************************************************
ProjectionReader::~ProjectionReader() throw()
{
  Projection * temp;
  unsigned int counter;
  try
    {
      for (counter = 0; counter < projectionlist.size(); counter++)
        {
          temp = projectionlist.front();
          projectionlist.pop_front();
          delete temp; //delete the projection
        }
    }
  catch(...)
    {
      //do nothing since this is the constructor
    }
}

//**************************************************************************
Projection * ProjectionReader::
createProjection( USGSImageLib::ImageIFile * infile) 
  throw(ProjectionIOException)
{
  int imagetype;
  Projection * proj; //temp projection
  try
    {
      if (!infile->good())
        throw ProjectionIOException(PROJECTIONIO_INFILEBAD);

      infile->getImageType(imagetype);

      switch(imagetype)
        {
        case 5: //doq case
          proj = parseDOQ(dynamic_cast<USGSImageLib::DOQImageIFile *>
                          (infile));
          //put the projection in the list
          projectionlist.push_back(proj);
          break;
        case 12: //geotiff case
          proj = parseGeotiff(dynamic_cast
                <USGSImageLib::GeoTIFFImageIFile *>(infile));
          //put the projection in the list
          projectionlist.push_back(proj);
          break;
        default:
          throw ProjectionIOException(PROJECTIONIO_UNKOWNIMAGETYPE);
        }
      return proj;
    }
  catch(ProjectionIOException& temp)
    {
      //forward error
      throw temp;
    }
  catch(...)
    {
      return NULL; //about the only thing we can do at this point is return
                   //NULL
    }
}

//************************************************************************
Projection * ProjectionReader::createProjection
(USGSImageLib::GeoTIFFImageIFile * ingeo) throw(ProjectionIOException)
{
  Projection * proj = NULL;
  
  try
    {
      proj = parseGeotiff(ingeo);
      //put the projection in the list
      projectionlist.push_back(proj);
      return proj;
    }
  catch(ProjectionIOException& temp)
    {
      throw temp;
    }
  catch(...)
    {
      return NULL;
    }     
}

//**************************************************************************
Projection * ProjectionReader::createProjection
(USGSImageLib::DOQImageIFile* indoq)  throw(ProjectionIOException)
{
   Projection * proj = NULL;
  
  try
    {
      proj = parseDOQ(indoq);
      //put the projection in the list
      projectionlist.push_back(proj);
      return proj;
    }
  catch(ProjectionIOException& temp)
    {
      throw temp;
    }
  catch(...)
    {
      return NULL;
    }     
}


//***************************************************************************
std::list<Projection *> ProjectionReader::getProjections() throw ()
{
  return projectionlist;
}

//***************************************************************************
void ProjectionReader::getProjections(Projection* in[], unsigned int insize) 
  throw (ProjectionIOException)
{
  unsigned int counter;
  std::list<Projection *>::iterator it;

  if (insize < projectionlist.size())
    throw ProjectionIOException();
  
  it = projectionlist.begin();
  for (counter = 0; counter < projectionlist.size(); counter++)
    {
      in[counter] = (*it);
      it++;
    }
}

// **********************************************************************
Projection * ProjectionReader::operator[](unsigned int pos) 
  throw(ProjectionIOException)
{
  std::list<Projection*>::iterator it;
  unsigned int counter;
  if (pos >= projectionlist.size())
    throw ProjectionIOException();
  
  it = projectionlist.begin();
  for (counter = 0; counter != pos; counter++)
    it++;
  return (*it);
}
   
//***********************************************************************
bool ProjectionReader::removeProjection(unsigned int pos) throw()
{
  std::list<Projection *>::iterator it;
  unsigned int counter;
  if (pos >= projectionlist.size())
    return false;
  
  it = projectionlist.begin();
  for (counter = 0; counter!=pos; counter++)
    it++;
  delete (*it);
  projectionlist.erase(it);
  return true;
}


//**********************************************************************
void ProjectionReader::removeAllProjections() throw()
{
  unsigned int counter;
  Projection * temp;
  
  for (counter = 0; counter < projectionlist.size(); counter++)
    {
      temp = projectionlist.front();
      projectionlist.pop_front();
      delete temp;
    }
}

//********************************************************************
unsigned int ProjectionReader::numberOfProjections()
{
  return projectionlist.size();
}

  

//*********************************************************************
Projection * ProjectionReader::
parseGeotiff(USGSImageLib::GeoTIFFImageIFile * ingeo) 
  throw(ProjectionIOException)
{
  unsigned short int modeltype, rastertype, projCStype,
    projtype, linunits, idatum;
  DATUM datum;
  UNIT units;
  
  //input parameters
  double StdParallel1;
  double StdParallel2;
  double NatOriginLong;
  double NatOriginLat;
  double FalseEasting;
  double FalseNorthing;
  double FalseOriginLong;
  double FalseOriginLat;
  double FalseOriginEasting;
  double FalseOriginNorthing;
  double CenterLong;
  double CenterLat;
  double CenterEasting;
  double CenterNorthing;
  double ScaleAtNatOrigin;
  double AzimuthAngle;
  double StraightVertPoleLong;
  Projection * proj = NULL;
  try
    {
       if (!ingeo->getModelType(modeltype))
         throw ProjectionIOException();
          
     
       
       if (modeltype != ModelTypeProjected) //try to treat as geo
         {
             if (!ingeo->getGeoKey(GeographicTypeGeoKey,idatum))
               throw ProjectionIOException();
             
             datum = getDatumFromGeo(idatum);
       
             if (!ingeo->getAngularUnits(linunits))
               return NULL;
             
             units = getUnitFromGeo(linunits);

           if(!(proj = new GeographicProjection(datum, units)))
			 throw std::bad_alloc();
		   return proj;
         }
                                                
       if (!ingeo->getRasterType(rastertype))
         throw ProjectionIOException();
       
       if (rastertype != RasterPixelIsArea) //must have raster as area
         throw ProjectionIOException();
       
       
       if (!ingeo->getProjectedCSType(projCStype))
         return NULL;
       
       
       if (projCStype == 32767)
         {
           if (!ingeo->getGeoKey(ProjCoordTransGeoKey, projtype))
             return NULL;
          
           if (!ingeo->getGeoKey(GeographicTypeGeoKey,idatum))
             throw ProjectionIOException();
           
           datum = getDatumFromGeo(idatum);
           
           if (!ingeo->getLinearUnits(linunits))
             return NULL;
       
           units = getUnitFromGeo(linunits);
           
           //get all the possible params
           ingeo->getGeoKey(ProjStdParallel1GeoKey, StdParallel1);
           ingeo->getGeoKey(ProjStdParallel2GeoKey, StdParallel2);
           ingeo->getGeoKey(ProjNatOriginLongGeoKey,NatOriginLong);
           ingeo->getGeoKey(ProjNatOriginLatGeoKey, NatOriginLat);
           ingeo->getGeoKey(ProjFalseNorthingGeoKey,FalseNorthing);
           ingeo->getGeoKey(ProjFalseEastingGeoKey, FalseEasting);
           ingeo->getGeoKey(ProjFalseOriginLongGeoKey, 
                            FalseOriginLong);
           ingeo->getGeoKey(ProjFalseOriginLatGeoKey,
                            FalseOriginLat);
           ingeo->getGeoKey(ProjFalseOriginEastingGeoKey, 
                               FalseOriginEasting);
           ingeo->getGeoKey(ProjFalseOriginNorthingGeoKey, 
                            FalseOriginNorthing);
           ingeo->getGeoKey(ProjCenterLongGeoKey, CenterLong);
           ingeo->getGeoKey(ProjCenterLatGeoKey, CenterLat);
           ingeo->getGeoKey(ProjCenterEastingGeoKey, 
                            CenterEasting);
           ingeo->getGeoKey(ProjCenterNorthingGeoKey, 
                            CenterNorthing);
           ingeo->getGeoKey(ProjScaleAtNatOriginGeoKey, 
                            ScaleAtNatOrigin);
           ingeo->getGeoKey(ProjAzimuthAngleGeoKey, AzimuthAngle);
           ingeo->getGeoKey(ProjStraightVertPoleLongGeoKey, 
                            StraightVertPoleLong);
           
           switch(projtype)
             {
             case CT_AlaskaConformal:
               if(!(proj =  new(std::nothrow) AlaskaConformalProjection
                     (0.0, 0.0, FalseEasting, FalseNorthing,
                      datum, units )))
                 throw std::bad_alloc();
               break;
             case CT_AlbersEqualArea:
               if(!(proj =  new (std::nothrow) AlbersConicProjection
                    ( ConvertToDMS(StdParallel1), 
                      ConvertToDMS(StdParallel2),
                      0.0, 0.0, ConvertToDMS(CenterLong),
                      ConvertToDMS(NatOriginLat), 
                      FalseEasting, FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
               
             case CT_AzimuthalEquidistant:
               if(!(proj = new (std::nothrow) AzimuthalEquidistantProjection
                    ( ConvertToDMS(CenterLong),
                      ConvertToDMS(CenterLat),
                      FalseEasting, FalseNorthing, 0.0, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_EquidistantConic:
               if ( StdParallel1 == StdParallel2 )
                 {
                   if(!(proj =  new(std::nothrow) EquidistantConicProjection
                        ( ConvertToDMS(CenterLat), 0.0, 0.0,
                          ConvertToDMS(CenterLong),
                          ConvertToDMS(NatOriginLat),
                          FalseEasting,
                          FalseNorthing,
                          datum, units)))
                     throw std::bad_alloc();
                   
                 }
               else
                 {      
                   if(!(proj = new(std::nothrow) EquidistantConicProjection
                        ( ConvertToDMS(StdParallel1),
                          ConvertToDMS(StdParallel2),
                          0.0, 0.0,
                          ConvertToDMS(CenterLong),
                          ConvertToDMS(NatOriginLat),
                          FalseEasting,
                          FalseNorthing,
                          datum, units)))
                     throw std::bad_alloc();
                 }
               break;
             case CT_Equirectangular:
               if(!(proj =  new(std::nothrow) EquirectangularProjection
                    ( ConvertToDMS(CenterLat), 0.0, 
                      ConvertToDMS(CenterLong),
                      FalseEasting,FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_Gnomonic:
               if(!(proj = new(std::nothrow) GnomonicProjection
                    ( ConvertToDMS(CenterLong),
                      ConvertToDMS(CenterLat),
                      FalseEasting,FalseNorthing, 
                      0.0, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_ObliqueMercator:
               if(!(proj = new (std::nothrow) HotineObliqueMercatorProjection
                    ( ScaleAtNatOrigin, AzimuthAngle,
                      0.0, 0.0, ConvertToDMS(CenterLong),
                      ConvertToDMS(CenterLat), FalseEasting,
                      FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_LambertAzimEqualArea:
               if(!(proj = new(std::nothrow) LambertAzimuthalProjection
                    ( ConvertToDMS(CenterLong), ConvertToDMS(CenterLat),
                      FalseEasting, FalseNorthing, 0.0, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_LambertConfConic_1SP:
             case CT_LambertConfConic_2SP:
               if(!(proj = new(std::nothrow) LambertConformalConicProjection
                    ( ConvertToDMS(StdParallel1), ConvertToDMS(StdParallel2),
                      0.0, 0.0, ConvertToDMS(NatOriginLong),
                      ConvertToDMS(FalseOriginLat),
                      FalseEasting,FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_Mercator:
               if(!(proj = new(std::nothrow) MercatorProjection
                    ( 0.0, 0.0, ConvertToDMS(NatOriginLong),
                      ConvertToDMS(NatOriginLat),
                      CenterEasting, CenterNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_MillerCylindrical:
               if(!(proj = new (std::nothrow) MillerCylindricalProjection
                    ( 0.0, ConvertToDMS(CenterLong),
                      FalseEasting, FalseNorthing,
                      datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_Orthographic:
               if(!(proj = new(std::nothrow) OrthographicProjection
                    ( ConvertToDMS(CenterLong),
                      ConvertToDMS(CenterLat),
                      FalseEasting,FalseNorthing, 0.0, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_Polyconic:
               if(!(proj = new(std::nothrow) PolyconicProjection
                    ( 0.0, 0.0, ConvertToDMS(CenterLong),
                      ConvertToDMS(CenterLat), FalseEasting,
                      FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_PolarStereographic:
               if(!(proj = new(std::nothrow) PolarStereographicProjection
                    (ConvertToDMS(StraightVertPoleLong),
                     ConvertToDMS(NatOriginLat), 0.0, 0.0,
                     FalseEasting,FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_Robinson:
               if(!(proj = new(std::nothrow) RobinsonProjection
                    ( 0.0, ConvertToDMS(CenterLong),
                     FalseEasting, FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break; 
             case CT_Sinusoidal:
               if(!(proj = new(std::nothrow) SinusoidalProjection
                    ( 0.0, ConvertToDMS(CenterLong),
                      FalseEasting,
                      FalseNorthing, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_Stereographic:
               if(!(proj = new(std::nothrow) StereographicProjection
                    (ConvertToDMS(CenterLong),
                     ConvertToDMS(CenterLat),
                     FalseEasting, FalseNorthing,
                     0.0, datum, units)))
                 throw std::bad_alloc();
               break;
             case CT_TransverseMercator:
               if(!(proj = new(std::nothrow) TransverseMercatorProjection
                    (ScaleAtNatOrigin, 0.0, 0.0,
                     ConvertToDMS(CenterLong),
                     ConvertToDMS(NatOriginLat), FalseEasting,
                     FalseNorthing, datum, units)))
               throw std::bad_alloc();
               break;
             case CT_VanDerGrinten:
               if(!(proj = new(std::nothrow) VanDerGrintenProjection
                    ( ConvertToDMS(CenterLat), 0.0, 
                      ConvertToDMS(CenterLong),
                      FalseEasting,FalseNorthing,
                      datum, units)))
                 throw std::bad_alloc();
               break;
             default:
               throw ProjectionIOException
                 (PROJECTIONIO_UNKOWNPROJECTION); 
  
             }
         }
       else
         {
           proj = parseUTMSPCS(ingeo);
         }
       return proj;
       
    }
  catch(ProjectionIOException & e)
    {
      throw e;
    }
  catch(...)
    {
      throw ProjectionIOException();
    }
}

//*************************************************************************
DATUM ProjectionReader::getDatumFromGeo(unsigned short ingeocode) 
  throw(ProjectionIOException)
{
  switch ( ingeocode )
    {
    case GCS_Adindan:
      return ADINDAN; 
    case GCS_Arc_1950: 
      return ARC1950;
    case GCS_Arc_1960:
      return ARC1960;
    case GCS_Cape:
      return CAPE;
    case GCS_ED50:
      return EUROPEAN_DATUM_1950;
    case GCS_GD49:
      return GEODETIC_DATUM_1949;
    case GCS_Hu_Tzu_Shan:
      return HU_TZU_SHAN;
    case GCS_Indian_1975:
      return INDIAN;
    case GCS_NAD27:
      return NAD27;
    case GCS_NAD83:
      return NAD83;
    case GCS_OSGB_1936:
      return ORDNANCE_SURVEY_1936;
    case GCS_Pulkovo_1942:
      return PULKOVO_1942;
    case GCS_PSAD56:
      return PROVISIONAL_S_AMERICAN_1956;
    case GCS_Tokyo:
      return TOKYO;
    case GCS_WGS_72:
      return WGS_72;
    case GCS_WGS_84:
      return WGS_84;
    default:
      throw ProjectionIOException(PROJECTIONIO_UNKOWNDATUM);
    
    }
}

UNIT ProjectionReader::getUnitFromGeo(unsigned short ingeocode)
  throw(ProjectionIOException)
{ 

  switch ( ingeocode )
    {
    case Linear_Meter:
      return METERS;
	case Angular_Degree:
	  return ARC_DEGREES;
    case Linear_Foot:
    case Linear_Foot_Modified_American:
    case Linear_Foot_Clarke:
    case Linear_Foot_Indian:
      return US_FEET;
    default:
      throw ProjectionIOException(PROJECTIONIO_UNKOWNUNITS);
    }
}

//*************************************************************************
Projection * ProjectionReader::parseDOQ(USGSImageLib::DOQImageIFile * indoq)
  throw(ProjectionIOException)
{
  DATUM datum;
  std::string sdatum, coordsystem;
  Projection * proj = NULL;
  int zone;
  indoq->getCoordinateZone(zone);
  indoq->getHorizontalDatum(sdatum);
  indoq->getHorizontalCoordinateSystem(coordsystem);
 
  try
    {
      
      //now check the datum 
      if ( (sdatum == std::string("NAD83")) || 
           (sdatum == std::string("nad83")))
        datum = NAD83;
      else
        datum = NAD27;
      
      if ( (coordsystem == std::string("UTM")) || 
           (coordsystem == std::string("utm")))
        {
         
          //It is probably a bad thing to assume that it is in meters
          if (!(proj = new (std::nothrow) UTMProjection(zone, 
                                                        datum, METERS)))
            throw std::bad_alloc();
        }
      else
        {
          //State plane case
          //set the param files
          StatePlaneProjection::setNAD83ParameterFilename
            (std::string("./nad83sp"));
          //create the state plane projection
          //here I am assuming that the zone will be 
          //a correct state plane zone
          if (!(proj = new (std::nothrow) StatePlaneProjection
                (zone, datum, US_FEET)))
            throw std::bad_alloc();
          
        }
      return proj;
    }
  catch(...)
    {
      throw ProjectionIOException();
    }
}


//*******************************************************************
Projection * ProjectionReader::parseUTMSPCS
(USGSImageLib::GeoTIFFImageIFile * ingeo) throw(ProjectionIOException)
{
  USGSImageLib::SPCSMapper stateplanemap;
  Projection * proj = NULL;
  PROJSYS projSys = UNKNOWN_PROJSYS;
  DATUM   datum = UNKNOWN_DATUM;
  UNIT    u = METERS;
  short unsigned int   zone = 0;
  short unsigned int pcsCode;
  try
    {
      if (!ingeo->getProjectedCSType(pcsCode))
        throw ProjectionIOException();

      if ( pcsCode < 20137 )
        throw ProjectionIOException
          (PROJECTIONIO_UNKOWNPROJECTION); 
    
      
      // Adindan UTM
      if ( pcsCode >= 20137 && pcsCode <= 20138 )
        {
          projSys = UTM;
          datum = ADINDAN;
          zone = pcsCode % 100;
        }
      // European Datum 1950 UTM
      else if ( pcsCode >= 23028 && pcsCode <=23038 )
        {
          projSys = UTM;
          datum = EUROPEAN_DATUM_1950;
          zone = pcsCode % 100;
        }
      // Indian 1954/1978 UTM
      else if ( pcsCode >= 23947 && pcsCode <= 24048 )
        {
          projSys = UTM;
          datum = INDIAN;
          zone = pcsCode % 100;
        }
      // Provisional South American 1956 UTM North
      else if ( pcsCode >= 24818 && pcsCode <= 24821 )
        {
          projSys = UTM;
          datum = PROVISIONAL_S_AMERICAN_1956;
          zone = pcsCode % 100;
        }
      // Provisional South American 1956 UTM South
      else if ( pcsCode >= 24818 && pcsCode <= 24821 )
        {
          projSys = UTM;
          datum = PROVISIONAL_S_AMERICAN_1956;
          zone = 60 - ( pcsCode % 100 );
        }
      // NAD27 UTM North
      else if ( pcsCode >= 26703 && pcsCode <= 26722 )
        {
          projSys = UTM;
          datum = NAD27;
          zone = pcsCode % 100;
        }
      // NAD27 State Plane
      else if ( pcsCode >= 26729 && pcsCode <= 26798 )
        {
          stateplanemap.initNAD27Mappings();
          if (stateplanemap.getUSGSCode( pcsCode, zone ) )
            {
              projSys = SPCS;
              datum = NAD27;
              u = US_FEET;
            }
        }
      // NAD83 UTM North
      else if ( pcsCode >= 26903 && pcsCode <= 26923 )
        {
          projSys = UTM;
          datum = NAD83;
          zone = pcsCode % 100;
        }
      // NAD83 State Plane
      else if ( pcsCode >= 26929 && pcsCode <= 26998 )
        {
          stateplanemap.initNAD83Mappings(); //init the mappings
          if (stateplanemap.getUSGSCode( pcsCode, zone ) )
            {
              projSys = SPCS;
              datum = NAD83;
              u = US_FEET;
            }
        }
      // NAD27 State Plane
      else if ( pcsCode >= 32001 && pcsCode <= 32060 )
        {
         
          stateplanemap.initNAD27Mappings();
          if (stateplanemap.getUSGSCode( pcsCode, zone ) )
            {
              projSys = SPCS;
              datum = NAD27;
              u = US_FEET;
            }
        }
      // NAD83 State Plane
      else if ( pcsCode >= 32100 && pcsCode <= 32161 )
        {
          stateplanemap.initNAD83Mappings();
          if (stateplanemap.getUSGSCode( pcsCode, zone ) )
            {
              projSys = SPCS;
              datum = NAD83;
              u = US_FEET;
            }
        }
      // WGS72 UTM North
      else if ( pcsCode >= 32201 && pcsCode <= 32260 )
        {
          projSys = UTM;
          datum = WGS_72;
          zone = pcsCode % 100;
        }
      // WGS72 UTM South
      else if ( pcsCode >= 32301 && pcsCode <= 32360 )
        {
          projSys = UTM;
          datum = WGS_72;
          zone = -( pcsCode % 100 );
        }
      // WGS72BE UTM North
      else if ( pcsCode >= 32401 && pcsCode <= 32460 )
        {
          projSys = UTM;
          datum = WGS_72;
          zone = pcsCode % 100;
        }
      // WGS72BE UTM South
      else if ( pcsCode >= 32501 && pcsCode <= 32560 )
        {
          projSys = UTM;
          datum = WGS_72;
          zone = -( pcsCode % 100 );
        }
      // WGS84 UTM North
      else if ( pcsCode >= 32601 && pcsCode <= 32660 )
        {
          projSys = UTM;
          datum = WGS_84;
          zone = pcsCode % 100;
        }
      // WGS84 UTM South
      else if ( pcsCode >= 32701 && pcsCode <= 32760 )
        {
          projSys = UTM;
          datum = WGS_84;
          zone = -( pcsCode % 100 );
        }

      // Create the projection if a valid projection system was found
      if ( UNKNOWN_PROJSYS != projSys )
        {
          switch ( projSys )
            {
            case UTM:
              if (!(proj = new UTMProjection( zone, datum, u) ))
                throw std::bad_alloc();
              break;
            case SPCS:
              if (!(proj = new StatePlaneProjection( zone, datum, u)))
                throw std::bad_alloc();
              break;
            default:
              return NULL;
            }
         
        }
      return proj;
    }
  catch(...)
    {
      throw ProjectionIOException();
    }
}

//**************************************************************************
double ProjectionReader::ConvertToDMS(double degrees) throw()
{
  double temp;
  int deg, min;
  
  int sign = 1;
  
  temp = degrees;
  if (degrees < 0.0)
  {
    sign = -1;
    temp = -temp;
  }
  //get the degrees
  deg = static_cast<int>(temp);
  temp -= deg;
  temp *= 60.0; //get minutes
  min = static_cast<int>(temp);
  temp -= min; //get seconds
  temp *= 60;

  temp = deg * 1000000 + min * 1000 + temp;
  return temp*sign;
}

#endif




