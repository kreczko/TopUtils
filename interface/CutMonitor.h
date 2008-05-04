#ifndef CutMonitor_h
#define CutMonitor_h

#include <memory>
#include <map>

#include <string>
#include <fstream>
#include <iostream>
#include <ios>

#include "TopAnalysis/TopUtils/interface/Cut.h"

class CutMonitor {
 public:
  CutMonitor();
  CutMonitor(char*);
  ~CutMonitor();

  void name(const char* name) {name_=name; return;}
  int size() const {return cuts_.size();}
  const char* name() {return name_;}

  int add(char*, Cut::Type, int);
  int add(char*, Cut::Type, bool);
  int add(char*, Cut::Type, double);
  int add(char*, int, Cut::Type, int);
  int add(char*, int, Cut::Type, bool);
  int add(char*, int, Cut::Type, double);

  bool select(const char*, const int);
  bool select(const char*, const bool);
  bool select(const char*, const double);
  bool select(const char*, int, const int);
  bool select(const char*, int, const bool);
  bool select(const char*, int, const double);
  void print();
  void print(std::ofstream&);

 private:

  struct CompareCut {
    bool operator()( Cut* cut1, Cut* cut2 ) const
    {
      if( !strcmp(cut1->name(), "sample") )
	return true;
      return cut1->occur() > cut2->occur();
    }
  };
  CompareCut CutComparator;
  
 private:
  const char* name_;
  std::map<std::string,Cut*> cuts_;
};

#endif
