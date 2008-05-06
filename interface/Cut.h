#ifndef Cut_h
#define Cut_h

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>

class Cut {

 public:

  enum Type
    {
      Lower,
      LowerEqual,
      Equal,
      GreaterEqual,
      Greater,
      Boolean
    };
  
  explicit Cut(const char*, Type, double);
  ~Cut();
  
  bool is(int);
  bool is(bool);
  bool is(double);
  void print(std::string, const unsigned int w1=10, const unsigned int w2=10, const unsigned int w3=10);
  void print(std::ofstream&, std::string, const unsigned int w1=10, const unsigned int w2=10, const unsigned int w3=10);
  const char* name() const {return name_;}
  char* type() const {
    switch(type_){
    case Lower        : return " < ";
    case LowerEqual   : return " <=";
    case Equal        : return " ==";
    case GreaterEqual : return " >=";
    case Greater      : return " > ";
    case Boolean      : return "   ";
    default           : return "???";
    }
  }
  double value() const {return value_;}
  int occur() const {return occur_;}
  int increment() {return ++occur_;}

 private:

  const char* name_;
  Type type_;
  double value_;
  int occur_;
};

#endif
