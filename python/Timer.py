import time
import datetime
import os

class Timer:
    __name = 'nothing'
    __start = 0.0
    __stop = 0.0
    def __init__(self):
       self.__name = 'timer'
        
    def getDate():
        return time.strftime("%d%m%y", time.gmtime())
    
    def getTime():
        return time.mktime(datetime.datetime.utcnow().timetuple()).__str__()
    
    def start(self):
        t0 = os.times()
        self.__start = t0[3] + t0[4]
        
    def stop(self):
        t0 = os.times()
        self.__stop = t0[3] + t0[4]
        
    def getMeasuredTime(self):
        return self.__stop - self.__start
    
    def sleep(sleeptime):
        time.sleep(sleeptime)
    
    getDate = staticmethod(getDate)
    getTime = staticmethod(getTime)
    sleep = staticmethod(sleep)
    