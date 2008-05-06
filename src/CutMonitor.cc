#include <math.h>
#include <vector>
#include <algorithm>

#include "TopAnalysis/TopUtils/interface/CutMonitor.h"
#include "TopAnalysis/TopUtils/interface/NameScheme.h"

using namespace std;


CutMonitor::CutMonitor()
{
}

CutMonitor::CutMonitor(char* name):
  name_(name)
{
}

CutMonitor::~CutMonitor()
{
}

int
CutMonitor::add(char* name, Cut::Type type, double value)
{
  NameScheme key(name);
  cuts_[(const char*)(key.name())]=new Cut((const char*)key.name(), type, value);
  return cuts_.size();
}

int
CutMonitor::add(char* name, Cut::Type type, bool value)
{
  NameScheme key(name);
  cuts_[(const char*)key.name()]=new Cut((const char*)key.name(), type, value);
  return cuts_.size();
}

int
CutMonitor::add(char* name, Cut::Type type, int value)
{
  NameScheme key(name);
  cuts_[(const char*)key.name()]=new Cut((const char*)key.name(), type, value);
  return cuts_.size();
}

int
CutMonitor::add(char* name, int idx, Cut::Type type, double value)
{
  NameScheme key(name);
  cuts_[(const char*)key.name(idx)]=new Cut((const char*)key.name(idx), type, value);
  return cuts_.size();
}

int
CutMonitor::add(char* name, int idx, Cut::Type type, bool value)
{
  NameScheme key(name);
  cuts_[(const char*)key.name(idx)]=new Cut((const char*)key.name(idx), type, value);
  return cuts_.size();
}

int
CutMonitor::add(char* name, int idx, Cut::Type type, int value)
{
  NameScheme key(name);
  cuts_[(const char*)key.name(idx)]=new Cut((const char*)key.name(idx), type, value);
  return cuts_.size();
}

bool
CutMonitor::select(const char* name, const int value)
{
  NameScheme key(name);
  if( cuts_.count((const char*)key.name()) ){
    return (*cuts_[(const char*)key.name()]).is(value);
  }
  return true;
}

bool
CutMonitor::select(const char* name, const bool value)
{
  NameScheme key(name);
  if( cuts_.count((const char*)key.name()) ){
    return (*cuts_[(const char*)key.name()]).is(value);
  }
  return true;
}

bool
CutMonitor::select(const char* name, const double value)
{
  NameScheme key(name);
  if( cuts_.count((const char*)key.name()) ){
    return (*cuts_[(const char*)key.name()]).is(value);
  }
  return true;
}

bool
CutMonitor::select(const char* name, int idx, const int value)
{
  NameScheme key(name);
  if( cuts_.count((const char*)key.name(idx)) ){
    return (*cuts_[(const char*)key.name(idx)]).is(value);
  }
  return true;
}

bool
CutMonitor::select(const char* name, int idx, const bool value)
{
  NameScheme key(name);
  if( cuts_.count((const char*)key.name(idx)) ){
    return (*cuts_[(const char*)key.name(idx)]).is(value);
  }
  return true;
}

bool
CutMonitor::select(const char* name, int idx, const double value)
{
  NameScheme key(name);
  if( cuts_.count((const char*)key.name(idx)) ){
    return (*cuts_[(const char*)key.name(idx)]).is(value);
  }
  return true;
}

void
CutMonitor::print()
{
  // determine maximal length of all cut names
  unsigned int w1max = 1;
  for(map<string,Cut*>::const_iterator cut=cuts_.begin(); 
      cut!=cuts_.end(); ++cut){
    unsigned int w1 = (cut->first).size();
    if(w1max < w1) w1max = w1;
  }

  // print head line
  for(unsigned int i=0; i<60; ++i) cout << "=";
  cout << "\n";
  cout << setw( w1max+25 ) << std::left  << setfill( ' ' ) << name_;
  cout << setw( 22       ) << std::right << setfill( ' ' ) << "Number of events\n";
  for(unsigned int i=0; i<60; ++i) cout << "-";
  cout << "\n";

  // print cut values
  for(map<string,Cut*>::const_iterator cut=cuts_.begin(); 
      cut!=cuts_.end(); ++cut){
    (cut->second)->print( cut->first, w1max+3, 15, 15 );
  }
  
  // print bottom line 
  for(unsigned int i=0; i<60; ++i) cout << "=";
  cout << "\n";
}

void
CutMonitor::print(std::ofstream& file)
{
  // determine maximal length of all cut names
  unsigned int w1max = 1;
  for(map<string,Cut*>::const_iterator cut=cuts_.begin(); 
      cut!=cuts_.end(); ++cut){
    unsigned int w1 = (cut->first).size();
    if(w1max < w1) w1max = w1;
  }

  // print head line
  for(unsigned int i=0; i<60; ++i) cout << "=";
  cout << "\n";
  cout << setw( w1max+25 ) << std::left  << setfill( ' ' ) << name_;
  cout << setw( 22       ) << std::right << setfill( ' ' ) << "Number of events\n";
  for(unsigned int i=0; i<60; ++i) cout << "-";
  cout << "\n";

  // print cut values
  for(map<string,Cut*>::const_iterator cut=cuts_.begin(); 
      cut!=cuts_.end(); ++cut){
    (cut->second)->print( file, cut->first, w1max+3, 15, 15 );
  }
  
  // print bottom line 
  for(unsigned int i=0; i<60; ++i) cout << "=";
  cout << "\n";
}
