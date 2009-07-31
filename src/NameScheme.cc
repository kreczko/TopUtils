#include "TopAnalysis/TopUtils/interface/NameScheme.h"

NameScheme::NameScheme():name_("hist"), link_("_")
{
}

NameScheme::NameScheme(const char* name):name_(name), link_("_")
{
}

NameScheme::NameScheme(const char* name, const char* link):name_(name), link_(link)
{
}

TString
NameScheme::removeAll(TString& str, const char c)
{
  if(str.Contains(c)){
    TString trunc = str(0,  str.First(c));
    for(int idx=str.First(c); idx<str.Length(); ++idx){
      if(str[idx]!=c){
	trunc+=str[idx];
      }
    }
    return trunc;
  }
  return str;
}

TString
NameScheme::name(ofstream& file, const char* name)
{
  TString namestr( name_ );
  namestr += link_;
  namestr += name;

  file << namestr << "\n";
  return namestr;
}

TString
NameScheme::name(ofstream& file, const char* name, const int i)
{
  TString namestr( name_ );
  namestr += link_;
  namestr += name;
  namestr += "_";
  namestr += i;

  file << namestr << "\n";
  return namestr;
}

TString
NameScheme::name(ofstream& file, const char* name, const int i, const int j)
{
  TString namestr( name_ );
  namestr += link_;
  namestr += name;
  namestr += "_";
  namestr += i;
  namestr += "_";
  namestr += j;

  file << namestr << "\n";
  return namestr;
}

TString
NameScheme::name(ofstream& file, const char* name, const double i, const double j)
{
  TString namestr( name_ );
  namestr += link_;
  namestr += name;
  namestr += "_";
  namestr += i;
  namestr += "_";
  namestr += j;

  file << namestr << "\n";
  return namestr;
}
