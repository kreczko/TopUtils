import sys
from xml.dom.minidom import Document
import unittest
from ConfigParser import ConfigParser, ConfigError, Histogramm

class testHist(unittest.TestCase): 
    
    def setUp(self):
        self.testfile = 'test/signal.root'
        self.histlist = ['mbg_nVSmet', 'mbg_nVSdisc', 'mbg_nVSdiscSimple', 'mbg_nVSmetSimple', 'mbg_selectedLayer1METs']
        self.dirlist = ['trackmafter', 'trackmbefore']
        self.hists = []
        for i in self.dirlist:
            for x in self.histlist:
                h = i + '/' + x
                self.hists.append(h) 
        print 'setup done'
        
    def testGetHistFromFile(self):
        list = Histogramm.getHistsFromFile(self.testfile)
        list.sort()
        self.hists.sort()
        self.assertEqual(list, self.hists)
        
        
        
        