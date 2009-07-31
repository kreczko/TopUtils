#ifndef NameScheme_h
#define NameScheme_h

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include "TString.h"

class NameScheme {

 public:
  ///default constructor with name='hist' und link='_'
  explicit NameScheme();
  ///default constructor with und link='_'
  explicit NameScheme(const char* name);
  ///default constructor; name and link can be specified
  explicit NameScheme(const char* name, const char* link);
  /// default destructor
  ~NameScheme(){};

  /// return name w/o additional parameter
  TString operator() ()
  {
    TString namestr( name_ ); 
    return namestr;
  }  
  /// return name with one additional parameter
  template<class T>
  TString operator() (T i)
  {
    TString namestr( name_ );
    namestr += link_;
    namestr += i;
    return namestr;
  }
  /// return name with two additional parameters
  template<class T>
    TString operator () (const char* name, T i)
  {
    TString namestr( name_ );
    namestr += link_;
    namestr += name;
    namestr += link_;
    namestr += i;
    return namestr;
  }
  /// return name with three additional parameters   
  template<class T, class S>
    TString operator () (const char* name, T i, S j)
  {
    TString namestr( name_ );
    namestr += link_;
    namestr += name;
    namestr += link_;
    namestr += i;
    namestr += link_;
    namestr += j;
    return removeAll(namestr, ' ');
  }


  ///
  /// kept for legacy for the moment
  ///

  /// return name with one additional parameter
  template<class T> 
    TString name(T i)
  {
    TString namestr( name_ );
    namestr += link_;
    namestr += i;
    return namestr;
  }

  /// return name with two additional parameters
  template<class T>
  TString name(const char* name, T i)
  {
    TString namestr( name_ );
    namestr += link_;
    namestr += name;
    namestr += link_;
    namestr += i;
    return namestr;
  }

  /// return name with three additional parameters   
  template<class T, class S>
  TString name(const char* name, T i, S j)
  {
    TString namestr( name_ );
    namestr += link_;
    namestr += name;
    namestr += link_;
    namestr += i;
    namestr += link_;
    namestr += j;
    return namestr;
  }
  TString name(){TString namestr( name_ ); return namestr;};  
  TString name(ofstream&, const char*);
  TString name(ofstream&, const char*, const int);
  TString name(ofstream&, const char*, const int, const int);
  TString name(ofstream&, const char*, const double, const double);


 private:
  // remove all chars of type c from str
  TString removeAll(TString& str, const char c);
  
 private:
  /// name prefix
  const char* name_;
  /// link between different name sections
  const char* link_;
};
#endif
