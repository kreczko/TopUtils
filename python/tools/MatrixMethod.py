from math import sqrt

class MatrixMethod:
    def getNumbers(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight):
        nbg = ((Nloose * effsig) - Ntight) / (effsig - effqcd)
        nbga = nbg * effqcd
        nsig = (Ntight - (Nloose * effqcd)) / (effsig - effqcd)
        nsiga = nsig * effsig
        #error on N_s tight
        errNST = MatrixMethod.errorNSTight(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight)
        errQCDT = MatrixMethod.errorNQCDTight(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight)
        return {'NQL':int(nbg), 'NQT':int(nbga), 'NSL':int(nsig), 'NST':int(nsiga), 'NSTerr':errNST, 'NQTerr':errQCDT}
    getNumbers = staticmethod(getNumbers)
    
    def errorNSTight(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight):
        ########### dNsTight_deffsig
        tmp1 = effqcd * (effqcd * Nloose - Ntight)
        tmp2 = (effsig - effqcd) * (effsig - effqcd)
        d1 = tmp1 / tmp2
        ########### dNsLoose_deffqcd
        tmp1 = effsig * (Ntight - effsig * Nloose)
        d2 = tmp1 / tmp2
        ############ dNsTight_dN1
        d3 = (- effsig * effqcd) / (effsig - effqcd)
        ############ dNsTight_dN2
        d4 = effsig * (1 - effqcd) / (effsig - effqcd)
        err = sqrt(d1 * d1 * effsigerr * effsigerr + d2 * d2 * effqcderr * effqcderr + d3 * d3 * (Nloose - Ntight) + d4 * d4 * Ntight)
        return err
    errorNSTight = staticmethod(errorNSTight)
    
    def errorNS(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight):
        ############dNs_deffsig
        tmp1 = Ntight - (Nloose - Ntight) * effqcd
        tmp2 = (effsig - effqcd) * (effsig - effqcd)
        d1 = tmp1 / tmp2
        #############dNs_deffqcd
        tmp1 = Ntight + (Nloose - Ntight) * (effsig - 2 * effqcd)
        d2 = tmp1 / tmp2
        #############dNs_dN1
        d3 = effqcd / (effsig - effqcd)
        #############dNs_dN2
        d4 = (1 - effqcd) / (effsig - effqcd)
        err = sqrt(d1 * d1 * effsigerr * effsigerr + d2 * d2 * effqcderr * effqcderr + d3 * d3 * (Nloose - Ntight) + d4 * d4 * Ntight)
        return err
    errorNS = staticmethod(errorNS)
    
    def errorNQ(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight):
        ############dNs_deffsig
        tmp1 = Ntight - (Nloose - Ntight) * effqcd
        tmp2 = (effsig - effqcd) * (effsig - effqcd)
        d1 = tmp1 / tmp2
        #############dNs_deffqcd
        tmp1 = Ntight + (Nloose - Ntight) * (effsig - 2 * effqcd)
        d2 = tmp1 / tmp2
        #############dNs_dN1
        d3 = effqcd / (effsig - effqcd)
        #############dNs_dN2
        d4 = (1 - effqcd) / (effsig - effqcd)
        err = sqrt(d1 * d1 * effsigerr * effsigerr + d2 * d2 * effqcderr * effqcderr + d3 * d3 * (Nloose - Ntight) + d4 * d4 * Ntight)
        return err
    errorNQ = staticmethod(errorNQ)
        
    
    def errorNQCDTight(effqcd, effqcderr, effsig, effsigerr, Nloose, Ntight):
        ##########dNqcdTight_deffsig
        tmp1 = effqcd * (Ntight - effqcd * Nloose)
        tmp2 = (effsig - effqcd) * (effsig - effqcd)
        d1 = tmp1 / tmp2
        ####################dNqcdTight_deffqcd
        tmp1 = effsig * (effsig * Nloose - Ntight)
        d2 = tmp1 / tmp2
        ######################dNqcdTight_dNloose
        d3 = effsig * effqcd / (effsig - effqcd)
        ###################dNqcdTight_dNtight
        d4 = - effqcd / (effsig - effqcd)
        err = sqrt(d1 * d1 * effsigerr * effsigerr + d2 * d2 * effqcderr * effqcderr + d3 * d3 * Ntight + d4 * d4 * (Nloose - Ntight))
        return err
    errorNQCDTight = staticmethod(errorNQCDTight)