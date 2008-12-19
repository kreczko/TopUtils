import sys
from xml.dom.minidom import Document
import unittest
from ConfigParser import ConfigParser, ConfigError

"""

    A test class for the ConfigParser module.

"""
class testConfigParser(unittest.TestCase):   
    
    """
    called after constructor, sets the variables
    """
    def setUp(self):
        #an how-it-should-be config
        self.testxml = 'test/Testconfig.xml'
        #a false config 
        self.falsexml = 'test/FalseConfig.xml'
        self.parser = ConfigParser(self.testxml)
        self.falseParser = ConfigParser(self.falsexml)
        
    """
    check if the constructor works properly
    """
    def testInit(self):
        #check if the ConfigParser constructor throws an IOException 
        #if the file doesn't exist
        self.assertRaises(IOError, ConfigParser, 'nothere')

    """
    tests ConfigParser.getRoot()
    """    
    def testGetRoot(self):
        #the root tag of the document should be HistPlotter
        self.assertEqual(self.parser.getRoot().localName, 'HistPlotter')
        
    def testGetNodeList(self):
        node = self.parser.getNodeList('include')
        self.assertEqual(node.length, 1)
        self.assertEqual(node[0].attributes.length, 1)
        self.assertEqual(node[0].attributes.item(0).name, 'file')
        self.assertEqual(node[0].attributes.item(0).value, 'test/HistConfig.xml')
        
    def testGetAttributeValue(self):
        nodes = self.parser.getNodeList('include')
        self.assertRaises(ConfigError,self.parser.getAttributeValue, nodes[0], 'color')
        self.assertEqual(self.parser.getAttributeValue(nodes[0], 'file'), 'test/HistConfig.xml')
        
    def testReadIncludes(self):
        self.assertRaises(ConfigError, self.falseParser.readIncludes)
        includes = self.parser.readIncludes()
        self.assertTrue('test/HistConfig.xml' in includes )
        self.assertEqual(len(includes), 1)
        
    def testGetInputFiles(self):
        self.assertRaises(ConfigError, self.falseParser.getInputFiles)
        inputfiles = self.parser.getInputFiles()
        self.assertEqual(inputfiles['qcd'], 'test/background.root')
        self.assertEqual(inputfiles['top'], 'test/signal.root')
        self.assertEqual(inputfiles['kin'], 'test/analyzeMuon.root')
        
    def testGetInputs(self):
        inputs = self.parser.getInputs()
        self.assertEqual(inputs['SemiLepAnalysis'].folderlist[0].name, 'trackmafter')
        self.assertTrue(inputs['SemiLepAnalysis'].folderlist[0].hasFilters())
        self.assertEqual(inputs['SemiLepAnalysis'].folderlist[0].filterlist[0].type, 'exact')
        self.assertEqual(inputs['SemiLepAnalysis'].folderlist[0].filterlist[0].value, 'mbg_nVSdisc')
        self.assertFalse(inputs['SelectedMuons'].folderlist[0].hasFilters())
        #self.assertFalse(inputs['strangeInput'].folderlist[0].hasFilters())
        
    def testGetChildNodes(self):
        #make a small XML document
        tree_data = Document()
        node = tree_data.createElement('li')
        text = tree_data.createTextNode('Loaded Python Modules')
        node.appendChild(text)
        node1 = tree_data.createElement('ul')
        node.appendChild(node1)
        #put the document together
        tree_data.appendChild(node)
        #test if the right child is given
        self.assertEqual(node1, self.parser.getChildNodes(node, 'ul')[0])
    
    def testGetPlots(self):
        plots = self.parser.getPlots()
        self.assertEqual(plots.fileOutputs, ['eps','pdf'])
        self.assertTrue(plots.makeSummary)
        self.assertEqual(plots.sumfile, 'inspect.pdf')
        self.assertEqual(plots.histlist[0].opt['name'], 'NeventsVSmvadisc')
        #self.assertEqual(plots.histlist)
    
    def testGetFilenameByID(self):
        self.assertEqual(self.parser.getFilenameByID('qcd'), 'test/background.root')
        
        
                
        
        
"""
For personal Suite
"""      
def suite():

    suite = unittest.TestSuite()

    suite.addTest(unittest.makeSuite(testConfigParser))

    return suite

        
"""
for running outside of eclipse (with 'python testConfigParser.py')
"""
if __name__ == "__main__":
    #do the whole suite
    #unittest.main()
    
    #create a testsuite with chosen test methods
    fewtest = unittest.TestSuite()
    #add function testInit to test suite
    fewtest.addTest(testConfigParser("testInit"))
    fewtest.addTest(testConfigParser("testReadIncludes"))
    #run the created suite
    unittest.TextTestRunner(verbosity=2).run(fewtest)
    
    #another way to run the whole suite with choosing the output level
    #unittest.TextTestRunner(verbosity=2).run(suite())
