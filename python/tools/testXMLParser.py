import sys
import copy
from xml.dom.minidom import Document
import unittest

from XMLConfigParser import *

##an how-it-should-be config
##both config types should be supported
testxml = 'test/NewConfig.xml'
testxml2 = 'test/TestConfig.xml'
#---------------------------------------------------------------- a false config
falsexml = 'test/FalseConfig.xml'
##default plot config
plotdefaults = "test/DefaultPlotConfig.xml"
#===============================================================================
# TestCase for testing the XMLConfigParser
#===============================================================================
class TestParser(unittest.TestCase):
    """TestCase for the XMLConfigParser"""
#===============================================================================
#    sets variables for every test
#===============================================================================
    def setUp(self):
        pass

    def testGetChildnodes(self):
        self.assertRaises(ConfigError, Parser.getAllChildNodes, None)

class TestConfigObject(unittest.TestCase):      
    #===============================================================================
#    sets variables for every test
#===============================================================================
    def setUp(self):
        self.object = ConfigObject("hist", ["var", "legend"])
        self.nodelist = Parser.getNodeList(testxml, "hist")
        self.object.parseFromNode(self.nodelist[0])
        
        
    def testOptions(self):
        #options should contain all attributes of the node
        for i in Parser.getListOfAttributes(self.nodelist[0]):
            self.assertEqual(self.object.getOption(i), Parser.getAttributeValue(self.nodelist[0], i))
        #and options should contain all childNodes, if they are no special objects themselves
        self.assertEqual(self.object.getOption("titleX"), "mva disc.")
        self.assertRaises(ConfigError, self.object.getOption, "Xsjfssdhfwhshdf")
        self.assertEqual(self.object.subobjects["var"][0].getOption("name"), "qcd")
        self.assertEqual(self.object.subobjects["legend"][0].getOption("name"), "plotname")
        
    def testDefaults(self):
        obj = ConfigObject("nothing")
        obj.readDefaults(plotdefaults)
        self.assertEqual(obj.getOption("create"), "png")
                         
    def testParse(self):
        self.assertRaises(ConfigError, self.object.parseFromNode, None)
            
class TestInput(unittest.TestCase):
    def setUp(self):
        self.input = Input()
        self.nodelist = Parser.getNodeList(testxml, "sources")
        self.input.parseFromNode(Parser.getChildNodes(self.nodelist[0], "input")[0])
#        self.input.parseFromNode(copy.deepcopy(Parser.getChildNodes(self.nodelist[0], "input")[0]))
#        filter2 = Parser.getChildNodes(Parser.getChildNodes(Parser.getChildNodes(self.nodelist[0], "input")[0], "folder")[0], "filter")
#        if filter2:
#            print "--filter2 pure `ok"
    
    def testInit(self):
        self.assertEqual(self.input.getOption("name"), "allMuonPt")
        f = self.input.subobjects["folder"]
        self.assertEqual(f[0].getOption("name"), "analyzeAllMuon")
        self.assertEqual(f[1].getOption("name"), "analyzeSelMuon")
           # self.assertEqual(i.getOption("name"), "analyzeAllMuon")
        
class TestFilter(unittest.TestCase):
    def setUp(self):
        self.nodelist = Parser.getChildNodes(Parser.getChildNodes(Parser.getNodeList(testxml, "sources")[0], "input")[0], "folder")
        self.filter = Filter()
        self.filter.parseFromNode(Parser.getChildNodes(self.nodelist[0], "filter")[0])

    def testInit(self):
        self.assertEqual(self.filter.getOption("type"), "contains")
        self.assertEqual(self.filter.getOption("value"), "pt")
        
class TestFolder(unittest.TestCase):
    
    def setUp(self):
        self.paths = ["analyzeAllMuon/muonpt", "analyzeAllMuon/electronpt", "analyzeAllMuon/muoneta", "analyzeAllMuon/ptres", "analyzeSelectedMuons/muonpt"]
        self.nodelist = Parser.getChildNodes(Parser.getNodeList(testxml, "sources")[0], "input")
        self.folder = Folder()
        self.folder.parseFromNode(Parser.getChildNodes(self.nodelist[0], "folder")[0])
        self.folder2 = Folder()
        self.folder2.parseFromNode(Parser.getChildNodes(self.nodelist[1], "folder")[0])
        self.folder.setContent(self.paths)
        self.folder2.setContent(self.paths)
    
    def testInit(self):
        self.assertEqual(len(self.folder.subobjects["filter"]), 2)
        self.assertEqual(self.folder.getOption("name"), "analyzeAllMuon")
        
    def testContent(self):
        self.assertEqual(self.folder.getContent(), self.paths)
        
    def testFilteredContent(self):
        #test contain and !contain filter in a row
        con = self.folder.getFilteredContent()
        filtered = ["analyzeAllMuon/muonpt", "analyzeAllMuon/electronpt"]
        self.assertEqual(con, filtered)
        #test exact filter
        con = self.folder2.getFilteredContent()
        filtered = ["analyzeAllMuon/muonpt"]
        self.assertEqual(con, filtered)
        
class TestConfiguration(unittest.TestCase):
    
    def testloadConfiguration(self):
        obj = Configuration(verbose=True)
        obj.loadConfiguration(testxml)
        self.assertEqual(obj.getFilenameByID("top"), "test/signal.root")
        self.assertEqual(obj.getInputByName("allMuonPt").subobjects["folder"][0].getOption("name"), "analyzeAllMuon")
        self.assertEqual(obj.getPlots().getOption("makeSummary"), "true")
        self.assertEqual(obj.getPlots().getOption("summaryFile"), "inspect.pdf")
        self.assertEqual(obj.getPlots().subobjects["hist"][0].getOption("name"), "mvadisc")
        self.assertEqual(obj.getPlots().subobjects["hist"][0].getVariable("qcd").getOption("type"), "line")
        self.assertEqual(obj.getPlots().subobjects["hist"][0].getVariable("qcd").getOption("source"), ["test/background.root", "trackmafter/mbg_nVSdisc"])
        
class TestFileService(unittest.TestCase):
    
    def testGetFileContent(self):
        c = FileService.getFileContent("test/analyzeMuon.root")
        content = ['analyzeAllMuon/id_muComp', 'analyzeAllMuon/id_muEm',
                   'analyzeAllMuon/id_muEmS9', 'analyzeAllMuon/id_muHad',
                   'analyzeAllMuon/id_muHadS9', 'analyzeAllMuon/id_muHo',
                   'analyzeAllMuon/id_muHoS9', 'analyzeAllMuon/kin_en',
                   'analyzeAllMuon/kin_pt', 'analyzeAllMuon/kin_eta',
                   'analyzeAllMuon/kin_phi', 'analyzeAllMuon/iso_isoJet',
                   'analyzeAllMuon/iso_isoJet5', 'analyzeAllMuon/iso_isoJet10',
                   'analyzeAllMuon/iso_isoJet15', 'analyzeAllMuon/iso_isoJet20',
                   'analyzeAllMuon/iso_isoJet25', 'analyzeAllMuon/iso_isoJet30',
                   'analyzeAllMuon/iso_isoJet35', 'analyzeAllMuon/iso_isoJet40',
                   'analyzeAllMuon/iso_isoTrkPt', 'analyzeAllMuon/iso_isoCalPt',
                   'analyzeAllMuon/iso_isoTrkN', 'analyzeAllMuon/iso_isoCalN',
                   'analyzeAllMuon/iso_dRTrkPt', 'analyzeAllMuon/iso_dRTrkN',
                   'analyzeAllMuon/iso_dRCalPt', 'analyzeAllMuon/iso_dRCalN', 
                   'analyzeAllMuon/iso_ptVsTrkIso', 'analyzeAllMuon/iso_ptVsCalIso', 
                   'analyzeAllMuon/fit_relPt_0', 'analyzeAllMuon/fit_relPt_1', 
                   'analyzeAllMuon/fit_relPt_2', 'analyzeAllMuon/fit_relPt_3', 
                   'analyzeAllMuon/fit_relPt_4', 'analyzeAllMuon/fit_relPt_5', 
                   'analyzeAllMuon/cal_relPt', 'analyzeAllMuon/res_relPt', 
                   'analyzeAllMuon/fit_Eta_0', 'analyzeAllMuon/fit_Eta_1', 
                   'analyzeAllMuon/fit_Eta_2', 'analyzeAllMuon/fit_Eta_3', 
                   'analyzeAllMuon/fit_Eta_4', 'analyzeAllMuon/fit_Eta_5', 
                   'analyzeAllMuon/cal_Eta', 'analyzeAllMuon/res_Eta', 
                   'analyzeAllMuon/fit_Phi_0', 'analyzeAllMuon/fit_Phi_1', 
                   'analyzeAllMuon/fit_Phi_2', 'analyzeAllMuon/fit_Phi_3', 
                   'analyzeAllMuon/cal_Phi', 'analyzeAllMuon/res_Phi', 
                   'analyzeSelMuon/id_muComp', 'analyzeSelMuon/id_muEm', 
                   'analyzeSelMuon/id_muEmS9', 'analyzeSelMuon/id_muHad', 
                   'analyzeSelMuon/id_muHadS9', 'analyzeSelMuon/id_muHo', 
                   'analyzeSelMuon/id_muHoS9', 'analyzeSelMuon/kin_en', 
                   'analyzeSelMuon/kin_pt', 'analyzeSelMuon/kin_eta', 
                   'analyzeSelMuon/kin_phi', 'analyzeSelMuon/iso_isoJet', 
                   'analyzeSelMuon/iso_isoJet5', 'analyzeSelMuon/iso_isoJet10', 
                   'analyzeSelMuon/iso_isoJet15', 'analyzeSelMuon/iso_isoJet20', 
                   'analyzeSelMuon/iso_isoJet25', 'analyzeSelMuon/iso_isoJet30', 
                   'analyzeSelMuon/iso_isoJet35', 'analyzeSelMuon/iso_isoJet40', 
                   'analyzeSelMuon/iso_isoTrkPt', 'analyzeSelMuon/iso_isoCalPt', 
                   'analyzeSelMuon/iso_isoTrkN', 'analyzeSelMuon/iso_isoCalN', 
                   'analyzeSelMuon/iso_dRTrkPt', 'analyzeSelMuon/iso_dRTrkN', 
                   'analyzeSelMuon/iso_dRCalPt', 'analyzeSelMuon/iso_dRCalN', 
                   'analyzeSelMuon/iso_ptVsTrkIso', 'analyzeSelMuon/iso_ptVsCalIso', 
                   'analyzeSelMuon/fit_relPt_0', 'analyzeSelMuon/fit_relPt_1', 
                   'analyzeSelMuon/fit_relPt_2', 'analyzeSelMuon/fit_relPt_3', 
                   'analyzeSelMuon/fit_relPt_4', 'analyzeSelMuon/fit_relPt_5', 
                   'analyzeSelMuon/cal_relPt', 'analyzeSelMuon/res_relPt', 
                   'analyzeSelMuon/fit_Eta_0', 'analyzeSelMuon/fit_Eta_1', 
                   'analyzeSelMuon/fit_Eta_2', 'analyzeSelMuon/fit_Eta_3', 
                   'analyzeSelMuon/fit_Eta_4', 'analyzeSelMuon/fit_Eta_5', 
                   'analyzeSelMuon/cal_Eta', 'analyzeSelMuon/res_Eta', 
                   'analyzeSelMuon/fit_Phi_0', 'analyzeSelMuon/fit_Phi_1', 
                   'analyzeSelMuon/fit_Phi_2', 'analyzeSelMuon/fit_Phi_3', 
                   'analyzeSelMuon/cal_Phi', 'analyzeSelMuon/res_Phi']
        self.assertEqual(c, content)
        
        
        
def suite():
    suite = unittest.TestSuite()

    suite.addTest(unittest.makeSuite(TestParser))
    suite.addTest(unittest.makeSuite(TestConfigObject))
    suite.addTest(unittest.makeSuite(TestInput))
    suite.addTest(unittest.makeSuite(TestFilter))
    suite.addTest(unittest.makeSuite(TestConfiguration))
    suite.addTest(unittest.makeSuite(TestFileService))

    return suite

if __name__ == "__main__":
    unittest.TextTestRunner(verbosity=2).run(suite())
#    obj = ConfigObject("hist", ["var", "legend"])
##    inp.parseFromNode(None)
#    root = Parser.getDocumentRoot("test/NewConfig.xml")
#    nodelist = Parser.getNodeList("test/NewConfig.xml", "hist")
#    print Parser.getListOfAttributes(nodelist[0])
#    obj.parseFromNode(nodelist[0])
#    print obj.getOption("setup")
    