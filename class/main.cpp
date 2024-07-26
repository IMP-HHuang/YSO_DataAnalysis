
#include "par.h"
#include "tree.h"
#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TString.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

//using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		std::cout << "eg. ./xxx  [run number]" << std::endl;
		exit(1);
	}

	int run = atoi(argv[1]);
	TString InputFilePath = INPUTFILEPATH;
	TString InputFileName1 = INPUTFILENAME1;
	TString InputFileName2 = INPUTFILENAME2;
	std::cout << Form("Run %04d", run) << std::endl;
    auto ipf = new TFile(Form("%s%s%05d%s.root", InputFilePath.Data(), InputFileName1.Data(), run, InputFileName2.Data()), "READ");
	if(ipf==0)
		std::cout << "Can not open the input file :"<< Form("%s%s%04d%s.root", InputFilePath.Data(), InputFileName1.Data(), run, InputFileName2.Data())  << std::endl;
	TString InputTreeName = INPUTTREENAME; 
	auto ipt = (TTree*)ipf->Get(InputTreeName.Data());
	TString OutputFilePath = OUTPUTFILEPATH;
	TString OutputFileName = OUTPUTFILENAME;

	auto opf = new TFile(Form("%s%s%04d.root", OutputFilePath.Data(), OutputFileName.Data(), run), "RECREATE");

	TString OutputTreeName = OUTPUTTREENAME;
	auto opt = new TTree(OutputTreeName.Data(), OutputTreeName.Data());
	
	auto it = new tree(ipt);
	it->GetEvent(opt,run);
	opt->Write();
	opf->Close();
	return 0;
}

