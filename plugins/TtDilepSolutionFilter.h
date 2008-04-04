#ifndef TtDilepSolutionFilter_H
#define TtDilepSolutionFilter_H


#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "AnalysisDataFormats/TopObjects/interface/TopLepton.h" 
#include "AnalysisDataFormats/TopObjects/interface/TopObject.h" 
#include "AnalysisDataFormats/TopObjects/interface/TopParticle.h" 
#include "AnalysisDataFormats/TopObjects/interface/TopMET.h" 
#include "AnalysisDataFormats/TopObjects/interface/TopJet.h" 
#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h" 
#include "AnalysisDataFormats/TopObjects/interface/TtDilepEvtSolution.h" 

#include <vector>
#include <map>
#include <string>
#include <utility> 


class TtDilepSolutionFilter : public edm::EDFilter {

  public:

    /// Constructor
    TtDilepSolutionFilter(const edm::ParameterSet& pset);

    /// Destructor
    virtual ~TtDilepSolutionFilter();
    virtual void endJob();

    /// Perform the real analysis
    virtual bool filter(edm::Event & iEvent, edm::EventSetup const& iSetup);


  private: 

    bool checkElectron (const TopElectron & electron) const;

    // Switch for debug output
    bool debug;

    std::string rootFileName;
    double jetPtCut, leptonPtCut, leptonTriggerPtCut, weight, jetEtCut;
    int signal, background, goodSolution, allSolution, B, nonB, tau;
    int bestSol,exception;
    edm::InputTag evtsols;
};


#endif