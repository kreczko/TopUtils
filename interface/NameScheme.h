#ifndef NameScheme_h
#define NameScheme_h

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include "TString.h"

using namespace std;

class NameScheme {
 public:
  explicit NameScheme();
  explicit NameScheme(const char*);
  explicit NameScheme(const char*, const char*);
  ~NameScheme();

  TString name(){TString namestr( name_ ); return namestr;};  
  TString name(const int);
  TString name(const char*);
  TString name(const char*, const int);
  TString name(const char*, const int, const int);
  TString name(ofstream&, const char*);
  TString name(ofstream&, const char*, const int);
  TString name(ofstream&, const char*, const int, const int);
  TString setLink(const char*);
  
 private:
  const char* name_;
  const char* link_;
};
#endif
