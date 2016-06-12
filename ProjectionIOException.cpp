//Implementation for the ProjectionMapperExecption class

#ifndef PROJECTIONIOEXCEPTION_CPP_
#define PROJECTIONIOEXCEPTION_CPP_

#include "ProjectionIOException.h"

using namespace ProjIOLib;


// ******************************************************************
ProjectionIOException::ProjectionIOException() throw()
{
  exception = PROJECTIONIO_ERRORUNKOWN;
}

//******************************************************************
ProjectionIOException::
ProjectionIOException(short int inexception) throw()
{
  exception = inexception;
}

//*******************************************************************
ProjectionIOException::
~ProjectionIOException() throw()
{}
  
//********************************************************************
bool ProjectionIOException::
setException(short int inexception) throw()
{
  exception = inexception;
  return true;
}

//********************************************************************
void ProjectionIOException::
getException(short int& inexception)   const throw()
{
  inexception = exception;
}

//*******************************************************************
void ProjectionIOException::
getString(std::string& instring)       const throw()
{

  switch(exception)
    {
    case PROJECTIONIO_UNKOWNPROJECTION:
      instring = "Projection IO: Unkown Projection";
      break;
    case PROJECTIONIO_UNKOWNDATUM:
      instring = "Projection IO: Unkown Datum";
      break;
    case PROJECTIONIO_UNKOWNUNITS:
      instring = "Projection IO: Unkown Units";
      break;
    case PROJECTIONIO_UNKOWNIMAGETYPE:
      instring = "Projection IO: Unkown Image Types";
      break;
    case PROJECTIONIO_INFILEBAD:
      instring = "Projection IO: Can't read file";
      break;
    case PROJECTIONIO_OUTFILEBAD:
      instring = "Projection IO: Can't open output file";
      break;
    case PROJECTIONIO_CANNOT_WORDFILE:
      instring = "Projection IO: Can't write world file";
      break;
    case PROJECTIONIO_CANNOT_TIFF:
      instring = "Projection IO: Cannot proccess tiff";
      break;
    case PROJECTIONIO_OUTOFRANGE:
      instring = "Projection IO: Index of requested file is out of range";
      break;
    default:
      instring = "Projection IO: Unkown error";
    }
}

#endif

