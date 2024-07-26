#include <TTree.h>
#include <TFile.h>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "par.h"
#pragma pack(8)

typedef struct DATAEVENT
{
		Short_t      det;//探测器类型
		Short_t      id;//探测器编号

		Short_t         sr;
		Long64_t        ts;
		UShort_t        raw;//原始能量道址
		double	  e;//刻度能量

		Bool_t          pileup;
		Bool_t          outofr;
		Short_t         cfd;
		Bool_t          cfdft;
		Short_t         cfds;
#ifdef WAVE
		unsigned short  ltra;
		std::vector<unsigned short>  data;
#endif

		// Long64_t        tsflag;
		// Short_t         sid;
		// Short_t         cid;
		// Short_t         ch;
		// UShort_t        evte;
}Dataevent;

typedef struct DETECTOR
{
		Short_t      id;//探测器编号
		Double_t     raw;
		Double_t	 e;//刻度能量
		Long64_t     ts;
#ifdef WAVE
		unsigned short  ltra;
		std::vector<unsigned short>  data;
		std::vector<unsigned short>  ttr;
		std::vector<short>  data_new1;
		unsigned short  rt;
		unsigned short  rits;
		unsigned short  rfts;
#endif  
		//  	Bool_t          pileup;
		//  	Bool_t          outofr;

}Detector;

typedef struct DataYSO_
{
		Int_t       id;//探测器编号
		Long64_t    ts;
        Double_t    x;
        Double_t    y;
		Double_t    raw;
		Double_t    e;//刻度能量

}DataYSO;

typedef struct DetInf_
{
    Int_t det;
    Int_t id;
    Int_t cid;
    Double_t a0;
    Double_t a1;
    Double_t a2;
    Double_t thre;
}DetInf;


class tree 
{
		public:

				tree();
				tree(TTree *ipt_);
				virtual ~tree();
				void GetBranch();   //SetBranchAddress
				void Init();  
				void InitDetector(Detector* detector);
				void StripCut(std::vector<Detector> &a);
				void StripDelete(std::vector<Detector> &a);	
				void BranchOpt();
				void GetEvent(TTree *opt_, Int_t run_);
				Double_t BaseLine(const std::vector<unsigned short> data, unsigned short TrigeTs);
				Double_t Maxmium(const std::vector<unsigned short> data, unsigned short TrigeTs);
                void FillEvent();


				//Origin branch
				//DAQ of H.Y. Wu
				std::vector<Dataevent>* br_data;

				//DAQ of J.G. Wang
				static constexpr Int_t kMaxdata_XIA = 500;
				Int_t           nXIA;
				Int_t           data_XIA_;
				UInt_t          data_XIA_fUniqueID[kMaxdata_XIA];   //[data_XIA_]
				UInt_t          data_XIA_fBits[kMaxdata_XIA];   //[data_XIA_]
				Long64_t        data_XIA_Event_ts[kMaxdata_XIA];   //[data_XIA_]
				Long64_t        data_XIA_Ext_ts[kMaxdata_XIA];   //[data_XIA_]
				UShort_t        data_XIA_finish_code[kMaxdata_XIA];   //[data_XIA_]
				UShort_t        data_XIA_crate_id[kMaxdata_XIA];   //[data_XIA_]
				UShort_t        data_XIA_channel[kMaxdata_XIA];   //[data_XIA_]
				UShort_t        data_XIA_Energy[kMaxdata_XIA];   //[data_XIA_]
				Double_t        data_XIA_CFD[kMaxdata_XIA];   //[data_XIA_]
				UShort_t        data_XIA_CFD_Bit[kMaxdata_XIA];   //[data_XIA_]
				UInt_t          data_XIA_nESum[kMaxdata_XIA];   //[data_XIA_]
				UInt_t         *data_XIA_ESum[kMaxdata_XIA];   //[data_XIA_nESum]
				UInt_t          data_XIA_nQDC[kMaxdata_XIA];   //[data_XIA_]
				UInt_t         *data_XIA_QDC[kMaxdata_XIA];   //[data_XIA_nQDC]
				UInt_t          data_XIA_trace_length[kMaxdata_XIA];   //[data_XIA_]
				UShort_t       *data_XIA_trace[kMaxdata_XIA];   //[data_XIA_trace_length]

                std::map<Int_t, DetInf> mapdet;

                static const Int_t nYSO = 6;

				std::vector<Detector> YSOtmp[nYSO][4];
				std::vector<DataYSO> vYSOdata;
				std::vector<Detector> vGe;
				std::vector<Detector> vp;

				TTree *ipt;
				TTree *opt;

				Int_t run;

};
