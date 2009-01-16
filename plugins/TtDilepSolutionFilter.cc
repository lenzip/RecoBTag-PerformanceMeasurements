#include "RecoBTag/PerformanceMeasurements/plugins/TtDilepSolutionFilter.h"


using namespace std;
using namespace reco;
using namespace math;
using namespace edm;

//
// constructors and destructor
//
TtDilepSolutionFilter::TtDilepSolutionFilter(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed
  debug = iConfig.getParameter<bool> ("debug");
  jetPtCut   = iConfig.getParameter<double> ("jetPtCut");
  jetEtCut   = iConfig.getParameter<double> ("jetEtCut");
  leptonPtCut   = iConfig.getParameter<double> ("leptonPtCut");
  leptonTriggerPtCut   = iConfig.getParameter<double> ("leptonTriggerPtCut");

  evtsols    = iConfig.getParameter<edm::InputTag> ("EvtSolution");

  TString rootFileName      = iConfig.getParameter<string> ("eventFileName");
  theFile = new TFile (rootFileName , "RECREATE" ) ;
  theFile->cd();
  if (debug) cout << "Opend file: " << rootFileName<<endl;
  procB = new TH1F("ProcessesBefore", "Processes before filter", 75, -1., 74.);
  procA = new TH1F("ProcessesAfter", "Processes after filter", 75, -1., 74.);
  procBee = new TH1F("ProcessesBeforeEE", "Processes before filter e-e", 75, -1., 74.);
  procBem = new TH1F("ProcessesBeforeEM", "Processes before filter e-mu", 75, -1., 74.);
  procBmm = new TH1F("ProcessesBeforeMM", "Processes before filter mu-mu", 75, -1., 74.);

  weight = iConfig.getParameter< double > ("weight");

  allSolution = 0;
  goodSolution = 0;
  B=0;nonB=0;tau=0;exception=0;
}


TtDilepSolutionFilter::~TtDilepSolutionFilter()
{
}

void TtDilepSolutionFilter::endJob()
{
   cout << "TtDilepSolutionFilter: Events per fb-1 " << allSolution <<" , "<< goodSolution <<" emu - "
   		   << B << " "<<nonB<<" ; tau "<<tau
		   <<" , "<< exception<<endl;

  if (debug) cout << "TtDilepSolutionFilter: Store files" << endl;
  theFile->cd();
    procA->GetXaxis()->SetBinLabel(1,"Other");
    procB->GetXaxis()->SetBinLabel(1,"Other");
    procBee->GetXaxis()->SetBinLabel(1,"Other");
    procBem->GetXaxis()->SetBinLabel(1,"Other");
    procBmm->GetXaxis()->SetBinLabel(1,"Other");
  procA->Write();
  procB->Write();
  procBee->Write();
  procBem->Write();
  procBmm->Write();
  cout << "After    : " << procA->Integral(1,71)<<endl;
  cout << "Before   : " << procB->Integral(1,71)<<endl;
  cout << "Before ee: " << procBee->Integral(1,71)<<endl;
  cout << "Before em: " << procBem->Integral(1,71)<<endl;
  cout << "Before mm: " << procBmm->Integral(1,71)<<endl;
  theFile->Close();
}

bool
TtDilepSolutionFilter::filter(edm::Event& iEvent, edm::EventSetup const & iSetup)
{
   // get the event solution
  edm::Handle< std::vector<TtDilepEvtSolution> > eSols; 
  iEvent.getByLabel(evtsols, eSols);
  std::vector<TtDilepEvtSolution> sols;
  sols = *eSols;

  if (sols.size()!= 2)  return false;
  TtDilepEvtSolution & sol = sols[0];

  if (debug) cout << "TtDilepSolutionFilter: Found "<< sols.size()  << " dilepton soutions.\n";
  int procID = -1;
//cout << "a\n";
  procB->Fill(procID, weight);
  if ((sol.getWpDecay()=="electron")&&(sol.getWmDecay()=="electron"))
    procBee->Fill(procID, weight);
  if ((sol.getWpDecay()=="muon")&&(sol.getWmDecay()=="muon"))
    procBmm->Fill(procID, weight);
  if ( ((sol.getWpDecay()=="muon")||(sol.getWmDecay()=="muon")) &&
	((sol.getWpDecay()=="electron")||(sol.getWmDecay()=="electron")) )
    procBem->Fill(procID, weight);

  allSolution += weight;
  
  //cout << "mass in filter " << sol.getLRSignalEvtObsVal(13)<< endl;
  if ( (sol.getLRSignalEvtObsVal(13) < 100.)&&(sol.getLRSignalEvtObsVal(13) > 83.)) return false;

  if ((sol.getCalJetB().pt()<jetPtCut) || (sol.getCalJetBbar().pt()<jetPtCut))
      return false;

  if ((sol.getCalJetB().et()<jetEtCut) || (sol.getCalJetBbar().et()<jetEtCut))
      return false;

  if (debug) cout << sol.getWpDecay() << " "<<sol.getWmDecay()<<endl;

  if ((sol.getLeptNeg().p4().pt()<leptonTriggerPtCut) && (sol.getLeptPos().p4().pt()<leptonTriggerPtCut))
	return false;

  if ((sol.getLeptNeg().p4().pt()<leptonPtCut) || (sol.getLeptPos().p4().pt()<leptonPtCut))
	return false;


  // Check that we have an e-mu event

//   if ( ((sol.getWpDecay()!="muon")&&(sol.getWmDecay()!="muon")) ||
// 	((sol.getWpDecay()!="electron")&& (sol.getWmDecay()!="electron")) )
// 	return false;

//   if ( (sol.getWpDecay()=="electron")&&( !checkElectron(sol.getElectronp()) ) )
//      return false;
// 
//   if ( (sol.getWmDecay()=="electron")&&( !checkElectron(sol.getElectronm()) ) )
//      return false;

    goodSolution += weight;

    try {if ( (abs(sols[0].getGenEvent()->lepton()->pdgId())==15) || 
  (abs(sols[0].getGenEvent()->leptonBar()->pdgId())==15) ) tau+= weight;
    } catch (...){exception++;}


    if (sols[0].getJetB().partonFlavour()==5) B+= weight;
      else nonB+= weight;
    if (sols[0].getJetBbar().partonFlavour()==5) B+= weight;
      else nonB+= weight;

  if (debug) cout <<" ============================ End TtDilepSolutionFilter ============================" <<endl;

  procA->Fill(procID, weight);

  return true;
}


bool TtDilepSolutionFilter::checkElectron (const pat::Electron & electron) const
{
  if ( (electron.hadronicOverEm() > 0.1) ||
  	(electron.eSuperClusterOverP() > 3.) ||
	(electron.eSuperClusterOverP() < 0.8) ) return false;
  return true;

}
