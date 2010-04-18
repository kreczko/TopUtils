#ifndef CommonObjectSelector_h
#define CommonObjectSelector_h

#include <vector>
#include "DataFormats/Common/interface/RefVector.h"
#include "CommonTools/UtilAlgos/interface/ObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SingleElementCollectionSelector.h"

#include "DataFormats/JetReco/interface/GenJet.h"

typedef SingleObjectSelector<std::vector<Electron>,
              StringCutObjectSelector<Electron>
          > PATElectronSelector;
