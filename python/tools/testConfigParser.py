import sys

import unittest
from ConfigParser import ConfigParser, ConfigError

"""

    A test class for the ConfigParser module.

"""
class testConfigParser(unittest.TestCase):   
    
    #called after constructor, sets the variables
    def setUp(self):
        self.testxml = 'test/Testconfig.xml'
        self.falsexml = 'test/FalseConfig.xml'
        self.parser = ConfigParser(self.testxml)
        self.falseParser = ConfigParser(self.falsexml)
        
    #test method
    def testInit(self):
        #check if the ConfigParser constructor throws an IOException 
        #if the file doesn't exist
        self.assertRaises(IOError, ConfigParser, 'nothere')

        
    def testGetRoot(self):
        self.assertEqual(self.parser.getRoot().localName, 'HistPlotter')
        
    def testGetNodeList(self):
        node = self.parser.getNodeList('include')
        self.assertEqual(node.length, 3)
        self.assertEqual(node[0].attributes.length, 1)
        self.assertEqual(node[1].attributes.length, 1)
        self.assertEqual(node[0].attributes.item(0).name, 'file')
        self.assertEqual(node[0].attributes.item(0).value, 'test/hist.xml')
        
    def testGetAttributeValue(self):
        nodes = self.parser.getNodeList('include')
        self.assertRaises(ConfigError,self.parser.getAttributeValue, nodes[0], 'color')
        self.assertEqual(self.parser.getAttributeValue(nodes[0], 'file'), 'test/hist.xml')
        
    def testReadIncludes(self):
        self.assertRaises(ConfigError, self.falseParser.readIncludes)
        includes = self.parser.readIncludes()
        self.assertTrue('test/hist.xml' in includes )
        self.assertEqual(len(includes), 3)
        
        
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
