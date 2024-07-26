#include "tree.h"
#include "par.h"
#include "iostream"
#include "fstream"
#include "string.h"

bool SortDS(const Detector &a, const Detector &b)
{
		return a.raw > b.raw;
}

tree::tree()
{
		Init();
		std::cout << "No input tree " << std::endl;
		return;
}

tree::tree(TTree *ipt_)
{
		Init();
		ipt = ipt_;
		if(ipt == NULL)
		{
				std::cout << "Don't have the iuput tree!" << std::endl;
				return ;
		}
		GetBranch();

        std::string stmp;
        Int_t channel, det, id, cid;
        Double_t a0, a1, a2, thre;
        std::ifstream inf("./map.txt");
        if(inf.is_open())
        {
            getline(inf, stmp);
            while(inf >> channel >> det >> id >> cid >> a0 >> a1 >> a2 >> thre)
            {
                std::cout << channel << "   " << det << "   " << id << "   " << cid << "   " << a0 << "  " << a1 << "   " << thre << std::endl;
                DetInf detinf;
                detinf.det = det;
                detinf.id = id;
                detinf.cid = cid;
                detinf.a0 = a0;
                detinf.a1 = a1;
                detinf.a2 = a2;
                detinf.thre = thre;
                mapdet[channel] = detinf;
            }
        }
}

tree::~tree()
{}

Double_t tree::BaseLine(const std::vector<unsigned short> data, unsigned short TrigeTs)
{
		Double_t bar = 0.0;
		Int_t npoint = 0;
		if(TrigeTs > 0)
		{
				for(unsigned short trp=0; trp<TrigeTs; trp++)
				{
						bar = bar + Double_t(data.at(trp));
						npoint++;
				}
		}
		return bar/Double_t(npoint);
}

Double_t tree::Maxmium(const std::vector<unsigned short> data, unsigned short TrigeTs)
{
		Double_t Max = 0.0;
		if(TrigeTs > 0)
		{
				for(unsigned short trp=TrigeTs; trp<data.size(); trp++)
				{
						if(Double_t(data.at(trp)) > Max) 	Max = Double_t(data.at(trp));
				}
		}
		return Max;
}

void tree::StripCut(std::vector<Detector> &a)
{
		for(auto it = a.begin(); it != a.end(); )
		{
				if(TMath::Abs(it->id - a[0].id > 1))
						it = a.erase(it);
				else
						++it;
		}
}

void tree::StripDelete(std::vector<Detector> &a)
{
		if(a.size() > 1)
		{
				auto itt = a.begin();
				itt++;
				if(TMath::Abs(itt->id - a[0].id) > 1)
				{
						itt = a.erase(itt);
				}
				else
						itt++;
				for(auto it = itt; it != a.end(); )
						it = a.erase(it);
		}
}

void tree::Init()
{
		br_data = NULL;

		for(int i=0; i<kMaxdata_XIA; ++i) data_XIA_ESum[i] = 0;
		for(int i=0; i<kMaxdata_XIA; ++i) data_XIA_QDC[i] = 0;
		for(int i=0; i<kMaxdata_XIA; ++i) data_XIA_trace[i] = 0;

		ipt = NULL;
		opt = NULL;
		run = -1;

        mapdet.clear();
        for(int i =0; i<nYSO; i++)
            for(int j=0; j<4; j++)
		        YSOtmp[i][j].clear();
        vGe.clear();
        vYSOdata.clear();
        vp.clear();
}

void tree::InitDetector(Detector* detector)
{
		detector->id = -1;
		detector->raw = -1;
		detector->ts = -1;
		detector->e = -1.0;
#ifdef WAVE
		detector->ltra = 0;
		detector->data.clear();
		detector->ttr.clear();
		detector->rt = 0;
		detector->rits = 0;
		detector->rfts = 0;
#endif
}


void tree::GetBranch()
{
		if (ipt == NULL)
				return;
#ifdef PKDAQ 
		ipt->SetBranchAddress("event", &br_data);
#endif

#ifdef IMPDAQ
		ipt->SetMakeClass(1);
		ipt->SetBranchAddress("nXIA", &nXIA);
		ipt->SetBranchAddress("data_XIA", &data_XIA_);
		ipt->SetBranchAddress("data_XIA.fUniqueID", data_XIA_fUniqueID);
		ipt->SetBranchAddress("data_XIA.fBits", data_XIA_fBits);
		ipt->SetBranchAddress("data_XIA.Event_ts", data_XIA_Event_ts);
		ipt->SetBranchAddress("data_XIA.Ext_ts", data_XIA_Ext_ts);
		ipt->SetBranchAddress("data_XIA.finish_code", data_XIA_finish_code);
		ipt->SetBranchAddress("data_XIA.crate_id", data_XIA_crate_id);
		ipt->SetBranchAddress("data_XIA.channel", data_XIA_channel);
		ipt->SetBranchAddress("data_XIA.Energy", data_XIA_Energy);
		ipt->SetBranchAddress("data_XIA.CFD", data_XIA_CFD);
		ipt->SetBranchAddress("data_XIA.CFD_Bit", data_XIA_CFD_Bit);
		ipt->SetBranchAddress("data_XIA.nESum", data_XIA_nESum);
		ipt->SetBranchAddress("data_XIA.ESum", data_XIA_ESum);
		ipt->SetBranchAddress("data_XIA.nQDC", data_XIA_nQDC);
		ipt->SetBranchAddress("data_XIA.QDC", data_XIA_QDC);
		ipt->SetBranchAddress("data_XIA.trace_length", data_XIA_trace_length);
		ipt->SetBranchAddress("data_XIA.trace", data_XIA_trace);
#endif
}

void tree::GetEvent(TTree *opt_, Int_t run_)
{
		if (opt_ == NULL)
				return;
		opt = opt_;
		run = run_;
		BranchOpt();
		Long64_t nentries = ipt->GetEntries();
//		nentries = 2000000;
		for (Long64_t jentry = 0; jentry < nentries; jentry++)
		{
				ipt->GetEntry(jentry);
#ifdef PKDAQ
                /*
				for (int i = 0; i < int(br_data->size()); i++)
				{
						//  if((*br_data)[i].cfd==0)
						//    continue;
						if((*br_data)[i].det == Det_A && (*br_data)[i].id ==1)
						{
								Detector detector;
								InitDetector(&detector);
								if((*br_data)[i].raw < Det_A_Th)   continue;
								detector.id = (*br_data)[i].id;
								detector.ts = (*br_data)[i].ts*ATime;
								detector.raw = (*br_data)[i].raw;
#ifdef WAVE
								detector.ltra = (*br_data)[i].ltra;
#ifdef WAVEOUT
								detector.data.assign((*br_data)[i].data.begin(), (*br_data)[i].data.end());
								for(unsigned short j=0; j<(*br_data)[i].ltra; j++)
										detector.ttr.push_back(j);
#endif
#endif
								A_vtmp.push_back(detector);
						}
				}
                */
#endif
#ifdef IMPDAQ
				for (int i = 0; i < nXIA; i++)
				{
						//  if((*br_data)[i].cfd==0)
						//    continue;
                        auto it = mapdet.find(data_XIA_channel[i]);
                        if(it == mapdet.end()) continue;


						Detector detector;
						InitDetector(&detector);

                        detector.id = it->second.cid + 100*it->second.id;
						detector.ts = data_XIA_Event_ts[i] + data_XIA_CFD[i];
						detector.raw = data_XIA_Energy[i];
                        detector.e = it->second.a0 + it->second.a1 * detector.raw + it->second.a2 * pow(detector.raw, 2.0);

                        //trace analyse
#ifdef WAVE
						detector.ltra = data_XIA_trace_length[i];
						for(unsigned short j=0; j<data_XIA_trace_length[i]; j++)
						{
								detector.data.push_back(data_XIA_trace[i][j]);
								detector.ttr.push_back(j);
						}
#ifdef RAWMAXMIUM
						detector.raw = 0.0;
						if(data_XIA_trace_length[i] >0)
								detector.raw = Maxmium(detector.data, 40) - BaseLine(detector.data, 40);
#endif

#ifndef WAVEOUT
						detector.data.clear();
						detector.ttr.clear();
#endif
#endif

                        if(it->second.det == 0) // YSO
                        {
#ifdef YSOQDC
                            if(data_XIA_nQDC[i] > 0)
                                detector.raw = data_XIA_QDC[i][2] + data_XIA_QDC[i][3] - data_XIA_QDC[i][0] - data_XIA_QDC[i][1];
                            else 
                                continue;
#endif
                        if(detector.raw < it->second.thre) continue;
                            YSOtmp[it->second.id][it->second.cid].push_back(detector);
                        }
                        if(detector.raw < it->second.thre) continue;
                        if(it->second.det == 1) //Ge
                            vGe.push_back(detector);
                        if(it->second.det == 2) //pulse
                            vp.push_back(detector);
				}
#endif

                for(int i =0; i<nYSO; i++)
                {
                    bool gd = true;
                    for(int j=0; j<4; j++)
                    {
                        if(YSOtmp[i][j].size() > 0)
				            sort(YSOtmp[i][j].begin(), YSOtmp[i][j].end(), SortDS);
                        else
                        {
                            gd = false;
				            Detector detectorTmp;
				            InitDetector(&detectorTmp);
				            detectorTmp.raw = 0.0;
                            YSOtmp[i][j].push_back(detectorTmp);
                        }
                    }
                    if(!gd)     continue;
                    DataYSO dYSO;
                    dYSO.id = i;
                    dYSO.ts = YSOtmp[i][0].at(0).ts;
                    Double_t RawA = YSOtmp[i][0].at(0).raw;
                    Double_t RawMax= RawA;
                    Double_t RawB = YSOtmp[i][1].at(0).raw;
                    if(RawB > RawMax)
                    {
                        dYSO.ts = YSOtmp[i][1].at(0).ts;
                        RawMax = RawB;
                    }
                    Double_t RawC = YSOtmp[i][2].at(0).raw;
                    if(RawC > RawMax)
                    {
                        dYSO.ts = YSOtmp[i][2].at(0).ts;
                        RawMax = RawC;
                    }
                    Double_t RawD = YSOtmp[i][3].at(0).raw;
                    if(RawD > RawMax)
                    {
                        dYSO.ts = YSOtmp[i][3].at(0).ts;
                        RawMax = RawD;
                    }
                    Double_t Sum = RawA + RawB + RawC + RawD;
                    dYSO.x = (RawA + RawB) / Sum;
                    dYSO.y = (RawA + RawD) / Sum;
                    vYSOdata.push_back(dYSO);
                }

                FillEvent();

				if (jentry % 1000 == 0)
				{
						std::cout << TString::Format("Process %.2f %%, %lldk / %lldk\r", Double_t(jentry) / nentries * 100.0,
										jentry / 1000, nentries / 1000);
						std::cout << std::flush;
				}
		}
		std::cout << std::endl;
}

void tree::FillEvent()
{
	opt->Fill();
    for(int i =0; i<nYSO; i++)
        for(int j=0; j<4; j++)
	        YSOtmp[i][j].clear();
    vGe.clear();
    vYSOdata.clear();
    vp.clear();
}
void tree::BranchOpt()
{
		opt->Branch("run", &run, "run/I");  //file
		opt->Branch("pulse", &vp);
		opt->Branch("Ge", &vGe);
		opt->Branch("YSO", &vYSOdata);
#ifdef YSORaw
        for(int i=0; i<nYSO; i++)
        {
		    opt->Branch(Form("YSO%dA", i), &YSOtmp[i][0]);
		    opt->Branch(Form("YSO%dB", i), &YSOtmp[i][0]);
		    opt->Branch(Form("YSO%dC", i), &YSOtmp[i][0]);
		    opt->Branch(Form("YSO%dD", i), &YSOtmp[i][0]);
        }
#endif
}
