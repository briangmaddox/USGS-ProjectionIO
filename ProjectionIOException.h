//Execption class for ProjectionMapper class
//by Chris Bilderback

#ifndef PROJECTIONIOEXCEPTION_H_
#define PROJECTIONIOEXCEPTION_H_


#include <string>
#include <new>

#define PROJECTIONIO_UNKOWNPROJECTION  0
#define PROJECTIONIO_UNKOWNDATUM       1
#define PROJECTIONIO_UNKOWNUNITS       2
#define PROJECTIONIO_UNKOWNIMAGETYPE   3
#define PROJECTIONIO_INFILEBAD         4
#define PROJECTIONIO_OUTFILEBAD        5
#define PROJECTIONIO_CANNOT_WORDFILE   6
#define PROJECTIONIO_CANNOT_TIFF       7
#define PROJECTIONIO_OUTOFRANGE        8
#define PROJECTIONIO_ERRORUNKOWN       255


namespace ProjIOLib
{
class ProjectionIOException
{
 public:
  //Construction and Destruction
  ProjectionIOException() throw();
  ProjectionIOException(short int inexception) throw();
  virtual ~ProjectionIOException() throw();
  
  //member functions
  bool setException(short int inexception)          throw();
  void getException(short int& inexception)   const throw();
  void getString(std::string& instring)       const throw();
  
 protected:
  short int exception;    //the exception thrown
};
}
#endif
