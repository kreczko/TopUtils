import sys

import unittest
from ConfigParser import ConfigParser, ConfigError

"""

    A test class for the ConfigParser module.

"""
class testConfigParser(unittest.TestCase):   
    
    def setUp(self):
        self.testxml = 'test/Testconfig.xml'
        self.falsexml = 'test/FalseConfig.xml'
        self.parser = ConfigParser(self.testxml)
        self.falseParser = ConfigParser(self.falsexml)
        
    def testInit(self):
        self.assertRaises(IOError, ConfigParser, 'nothere')

        
    def testGetRoot(self):
        self.assertEqual(self.parser.getRoot().localName, 'HistPlotter')
        
    def testGetNodeList(self):
        node = self.parser.getNodeList('include')
        self.assertEqual(node.length, 2)
        self.assertEqual(node[0].attributes.length, 1)
        self.assertEqual(node[1].attributes.length, 1)
        self.assertEqual(node[0].attributes.item(0).name, 'file')
        self.assertEqual(node[0].attributes.item(0).value, 'test/hist.xml')
        
    def testGetAttributeValue(self):
        nodes = self.parser.getNodeList('include')
        v1 = self.parser.getAttributeValue(nodes[0], 'file')
        self.assertRaises(ConfigError,self.parser.getAttributeValue, nodes[0], 'color')
        #v2 = self.parser.getAttributeValue(nodes[0], 'value')
        #self.assertEqual(v1, 'setup1')
        self.assertEqual(self.parser.getAttributeValue(nodes[0], 'file'), 'test/hist.xml')
        
    def testReadIncludes(self):
        self.assertRaises(ConfigError, self.falseParser.readIncludes)
        includes = self.parser.readIncludes()
        self.assertTrue('test/hist.xml' in includes )
        #self.assertEqual(includes['setup2'], 'test/hist1.xml')
        self.assertEqual(len(includes), 2)
        
        
"""
For personal Suite
"""      
def suite():

    suite = unittest.TestSuite()

    suite.addTest(unittest.makeSuite(testConfigParser))

    return suite

        


if __name__ == "__main__":

   unittest.main()