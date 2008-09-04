from TopInspectWrapper import InspectWrapper
  
class Config(InspectWrapper):
    allowedTypes = "top, topbg, qcd, wjets, zjets"
    __isoHist = "Isolation.hist"
    __iso2Dhist = "Isolation2D.hist"
    __qcdHist = "QCDMatrix.hist"
    "constructor"
    def __init__(self, inspectType):
        InspectWrapper.__init__(self, inspectType)
        t = ""
        t = self.__readFromFile(self.__isoHist)
        print t.splitlines()
                 
    def __readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str