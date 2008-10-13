import os
import sys
import getopt
from makeQCDAnalysisPlots import Config

class TopRunner:
    macro = 'TopAnalysis/TopAnalyzer/plugins/Macro.cc'
    def __init__(self):
        self.__cfg = Config('simple')
        
    def TopInspect(self, configfile):
        print 'Executing Inspect_top...'
        print "##################################################"
        print 'Inspect_top ' + configfile
        #os.system('Inspect_top ' + configfile + ">& /dev/null&")
        
    def main(self):
       # print 'hallo'
        self.__cfg.replaceOption('outputFileName', "test.ps")
        self.TopInspect(self.__cfg.returnTempCfg())
        
    def prepare(self):
        os.system('root -q ' + self.macro)
        
if __name__ == '__main__':
    runner = TopRunner()
    runner.main()