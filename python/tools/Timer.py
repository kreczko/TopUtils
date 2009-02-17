import time
import datetime
import os

class Timer:
    __name = 'nothing'
    __start = 0.0
    __stop = 0.0
    def __init__(self):
       self.__name = 'timer'
        
    "Return a formated date string"
    def getDate():
        return time.strftime("%d%m%y", time.gmtime())
    
    "Return a formated date and time string" 
    def getTime():
        return time.mktime(datetime.datetime.utcnow().timetuple()).__str__()
    
    def start(self):
        #print 'start'
        t0 = os.times()
        self.__start = t0[3] + t0[4]
        
    def stop(self):
        #print 'stop'
        t0 = os.times()
        self.__stop = t0[3] + t0[4]
        
    "returns a formated time string: h m s"
    def getMeasuredTime(self):
        t = self.__stop - self.__start
        h = 0
        m = 0
        s = 0
        ft = ""
        if t >= 3600:
            h = (t - t%3600)/3600
            t = t -h*3600
        if t >= 60:
            m = (t - t%60)/60
            t = t-m*60
            
        s = t
        if h > 0:
            ft = round(h,0).__str__() + "h "
        if m > 0:
            ft = ft + round(m, 0).__str__() + "m "
        ft =  ft + round(s,2).__str__() + "s"  
        return ft
    
    def timePassed(self, osTime):
        t = osTime[3] + osTime[4]
        p = t - self.__start
        return p
    
    def sleep(sleeptime):
        time.sleep(sleeptime)
        
    def formatSFnumber(num, upto=100):
        out = 0
        tens = 0
        for i in range(1,100):
            
            if (num > 1 and num > 0) or  tens >= upto:
                out = num
                break
            else:
                num = num *10
                tens += 1
        str = '%1.3fd%d' % (out, -tens)
        return str, tens
    
    def formatNumPair(num1, num2):
        out = Timer.formatSFnumber(num1)
        out1 = out[0]
        out2 = Timer.formatSFnumber(num2, out[1])[0]
        return out1, out2
    
    def formatErrorPair(num1, num2):
        out = Timer.formatNumPair(num1, num2)
        num1 = out[0].split('d')[0]
        num2 = out[1].split('d')[0]
        exp = out[0].split('d')[1]
        str = '($%s \\pm %s$)\\num{d%s}' % (num1, num2, exp)
        return str
    
    formatErrorPair = staticmethod(formatErrorPair)
    formatNumPair = staticmethod(formatNumPair)
    formatSFnumber = staticmethod(formatSFnumber)
    getDate = staticmethod(getDate)
    getTime = staticmethod(getTime)
    sleep = staticmethod(sleep)
    