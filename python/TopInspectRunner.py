import os
import sys
import getopt

class TopRunner():
    def __init__(self):
        self.config = ''
        
    def TopInspect(self, configfile):
        print 'Executing Inspect_top...'
        print "##################################################"
        print 'Inspect_top ' + configfile
        os.system('Inspect_top ' + configfile + ">& /dev/null&")
        
    def __main__(self):
        print 'hallo'
        
if __name__ == '__main__':
    runner = TopRunner()
    runner.main()