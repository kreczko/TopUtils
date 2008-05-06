#include "TString.h"
#include "TopAnalysis/TopUtils/interface/Cut.h"
#include "TopAnalysis/TopUtils/interface/NameScheme.h"

using namespace std;


Cut::Cut(const char* name, Type type, double value):
  name_(name),
  type_(type),
  value_(value),
  occur_(0)
{
}

Cut::~Cut()
{
}

bool
Cut::is(const bool value)
{
  if(value == value_){
    ++occur_;
    return true;
  }
  return false;
}

bool
Cut::is(const double value)
{
  if( (type_==Lower        && value< value_)||
      (type_==LowerEqual   && value<=value_)||
      (type_==Equal        && value==value_)||
      (type_==GreaterEqual && value>=value_)||
      (type_==Greater      && value> value_) ){
    ++occur_;
    return true;
  }
  return false;
}

bool
Cut::is(const int value)
{
  if( (type_==Lower        && value< value_)||
      (type_==LowerEqual   && value<=value_)||
      (type_==Equal        && value==value_)||
      (type_==GreaterEqual && value>=value_)||
      (type_==Greater      && value> value_) ){
    ++occur_;
    return true;
  }
  return false;
}

void
Cut::print(std::string name, const unsigned int w1, const unsigned int w2, const unsigned int w3)
{
  std::cout << ::std::setw( w1 ) << ::std::right << ::std::setfill( ' ' ) 
	    << name 
	    << type()
	    << ::std::setw( w2 ) << ::std::left  << ::std::setfill( '.' ) 
	    << value_ 
	    << " :: "  
	    << ::std::setw( w3 ) << ::std::right << ::std::setfill( '.' ) 
	    << occur_ 
	    << "\n";
}

void
Cut::print(ofstream& file, std::string name, const unsigned int w1, const unsigned int w2, const unsigned int w3)
{
  file << ::std::setw( w1 ) << ::std::right << ::std::setfill( ' ' ) 
       << name 
       << type() 
       << ::std::setw( w2 ) << ::std::left  << ::std::setfill( '.' ) 
       << value_ 
       << " :: "  
       << ::std::setw( w3 ) << ::std::right << ::std::setfill( '.' ) 
       << occur_ 
       << "\n"; 
}
