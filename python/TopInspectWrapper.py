class InspectWrapper:
    def __init__(self, cfg = "TopAnalysis/TopUtils/python/TopInspectTemplate.cfg"):
         self.config = self.readFromFile(cfg)
         self.outputConfig = ""
         
    def histInput(self, input):
        #replace histInput
        self.replace("histInput", input)
        
    def rootInput(self, input):
        #input is an array of files
        self.replace(rootInput, input)
        
    def replace(self, searchFor, replaceWith):
        #it has to handle both arrays and singles
        print "nothing"
        
    def replaceInFile(self, search, replace):
        print search, replace
        self.config = self.config.replace(search, replace)
        
    def readFromFile(self, filename):
        file = open(filename, 'r')
        str = file.read()
        file.close()
        return str
    
    #writes a string to a file (overwrites the file)
    def writeToFile(self, filename, str):
        file = open(filename, 'w')
        file.write(str)
        file.close()