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
  vector<Cut*> cs;
  map<string,Cut*>::const_iterator cut=cuts_.begin();
  for(; cut!=cuts_.end(); ++cut){
    cs.push_back( cut->second );
  }
  sort( cs.begin(), cs.end(), CutComparator );

  unsigned int w1max = 1;
  for(vector<Cut*>::const_iterator c=cs.begin(); c!=cs.end(); ++c){
    unsigned int w1 = strlen( (*c)->name() );
    if(w1max < w1) w1max = w1;
  }

  for(unsigned int i = 0; i < w1max + 33; ++i) cout << "=";
  cout << "\n";
  cout << setw( w1max + 13 ) << std::left  << setfill( ' ' ) << name_;
  cout << setw( 21         ) << std::right << setfill( ' ' ) << "Number of events\n";
  for(unsigned int i = 0; i < w1max + 33; ++i) cout << "-";
  cout << "\n";

  for(vector<Cut*>::const_iterator c=cs.begin(); c!=cs.end(); ++c){
    (*c)->print( w1max, 10, 16 );
  }

  for(unsigned int i = 0; i < w1max + 33; ++i) cout << "=";
  cout << "\n";
}

void
CutMonitor::print(std::ofstream& file)
{
  vector<Cut*> cs;
  map<string,Cut*>::const_iterator cut=cuts_.begin();
  for(; cut!=cuts_.end(); ++cut){
    cs.push_back( cut->second );
  }
  sort( cs.begin(), cs.end(), CutComparator );

  unsigned int w1max = 1;
  for(vector<Cut*>::const_iterator c=cs.begin(); c!=cs.end(); ++c){
    unsigned int w1 = strlen( (*c)->name() );
    if(w1max < w1) w1max = w1;
  }

  for(unsigned int i = 0; i < w1max + 33; ++i) file << "=";
  file << "\n";
  file << setw( w1max + 13 ) << std::left  << setfill( ' ' ) << name_;
  file << setw( 21         ) << std::right << setfill( ' ' ) << "Number of events\n";
  for(unsigned int i = 0; i < w1max + 33; ++i) file << "-";
  file << "\n";

  vector<Cut*>::const_iterator c=cs.begin();
  for(; c!=cs.end(); ++c){
    (*c)->print( file, w1max, 10, 16 );
  }

  for(unsigned int i = 0; i < w1max + 33; ++i) file << "=";
  file << "\n";
}
