#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <sys/stat.h>

#include <TCanvas.h>
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TF1.h>

#include <TASImage.h>
#include <TString.h>

#include <TApplication.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLorentzVector.h>
#include <TObjString.h>

#include <TH1.h>

#include <iostream>
#include <fstream>

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

using namespace std;

const int pol_nums = 8;

float boost = -8.06871530459878472e-01;
int part_mechanism = 0;
std::vector<int> leading_decay;

struct part_inline
{
	float e, px, py, pz;
	int pid, proc_id, par_id, decay_cnt;
	float weight;
};

bool converter(const std::string & file)
{
	cout << "Opening " << file;
	ifstream evt_file(file.c_str());
	if (!evt_file.is_open())
	{
		cout << " [ failed ]" << endl;
		return false;
	}
	cout << " [ done ]" << endl;

	string ang_filename = file;
	int pos = ang_filename.find_last_of(".");
	string ext = ang_filename.substr(pos, -1);
	if (ext == ".evt")
	{
		ang_filename.replace(pos, -1, ".ang.evt");
	}
	else
	{
		ang_filename += ".ang.evt";
	}

	cout << "Creating " << ang_filename;
	ofstream ang_file(ang_filename.c_str()/*, ios_base::trunc*/);
	if (!ang_file.is_open())
	{
		cout << " [ failed ]" << endl;
		return false;
	}
	cout << " [ done ]" << endl;

	int evt_num, part_num, flag;
	float eb, ev_weight;

	std::vector<part_inline> arr;
	arr.reserve(10);

//	TH1I * h = new TH1I("mass", "mass", 260, 1.100, 1.360);
//	TH1I * h2 = new TH1I("cth_lab", "costheta_lab", 100, -1, 1);
//	TH1I * h3 = new TH1I("cth_cms", "costheta_cms", 100, -1, 1);

	while (!evt_file.eof())
	{
		arr.clear();

		int proton_id = -1;
		float new_weight = 0.0;

		TLorentzVector vec;
		vec.SetPxPyPzE(0,0,0,0);

		evt_file >> evt_num >> part_num >> eb >> ev_weight >> flag;

		if (evt_file.eof())
			break;

		for (int i = 0; i < part_num; ++i)
		{
			part_inline pi;
//			evt_file >> pi.e >> pi.px >> pi.py >> pi.pz >> pi.pid >> pi.proc_id >> pi.par_id >> pi.decay_cnt >> pi.weight;
			evt_file >> pi.e >> pi.px >> pi.py >> pi.pz >> pi.pid >> pi.proc_id >> pi.par_id >> pi.weight;

			arr.push_back(pi);

			for (int k = 0; k < leading_decay.size(); ++k)
			{
				if (pi.par_id == part_mechanism and pi.pid == leading_decay[k])
				{
//					printf(" i = %d, k = %d\n", i, k);
					TLorentzVector child;
					child.SetPxPyPzE(pi.px, pi.py, pi.pz, pi.e);
//					PR(child.M());
					if (vec.M() == 0)
						vec = child;
					else
						vec += child;
//					PR(vec.M());
					break;
				}
			}
		}

//		h->Fill(vec.M());
//		h2->Fill(vec.CosTheta());

//		printf("costh_lab = %g", vec.CosTheta());
		if (boost)
		{
			vec.Boost(0, 0, boost);
//			PR(vec.M());
//			h3->Fill(vec.CosTheta());
//			printf(" costh_cms = %g", vec.CosTheta());
		}
//		printf("\n");

		ang_file << evt_num << " " << part_num << " " << eb << " ";

		if (vec.M())
		{
			ang_file << vec.CosTheta();
		}
		else
		{
			ang_file << ev_weight;
		}
		   
		ang_file << " " << flag << endl;
		for (int i = 0; i < part_num; ++i)
		{
			part_inline pi = arr[i];
			pi.weight = new_weight;
			ang_file << pi.e << " " << pi.px << " " << pi.py << " " << pi.pz << " "
				<< pi.pid << " " << pi.proc_id << " " << pi.par_id << " ";
//				<< pi.decay_cnt << " ";

			if (vec.M())
			{
				ang_file << vec.CosTheta() << endl;
			}
			else
			{
				ang_file << pi.weight << endl;
			}

		}//exit(0); // FIXME
	}

//	TFile * f = TFile::Open("hist.root", "RECREATE");
//	h->Write();
//	h2->Write();
//	h3->Write();
//	f->Close();
	return true;
}

int main(int argc, char ** argv)
{
//	TROOT AnalysisDST_Cal1("TreeAnalysis","compiled analysisDST macros");
//	TApplication app("treeanal", NULL, NULL, NULL, 0);
//	gROOT->SetBatch();

	struct option lopt[] =
		{
			{"dir",			required_argument,	0,		'd'},
			{"width",		required_argument,	0,		'w'},
			{"height",		required_argument,	0,		'h'},
			{"filter",		required_argument,	0,		'f'},
			{"boost",		required_argument,	0,		'b'},
			{"chain",		required_argument,	0,		'c'},

			{ 0, 0, 0, 0 }
		};


	std::string chain;
	Int_t c = 0;
	while (1) {
		int option_index = 0;

		c = getopt_long(argc, argv, "d:w:h:f:b:c:", lopt, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 0:
				if (lopt[option_index].flag != 0)
					break;
				printf ("option %s", lopt[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'd':
				break;
			case 'w':
				break;
			case 'h':
				break;
			case 'f':
				break;
			case 'b':
				boost = atof(optarg);
				break;
			case 'c':
				chain = optarg;
				break;
			case '?':
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}
	}

//	PR(chain);

	TString ccc = chain;
	TObjArray * ccc_arr = ccc.Tokenize(":");
	if (ccc_arr->GetEntries())
	{
		part_mechanism = ((TObjString *)ccc_arr->At(0))->String().Atoi();

		if (ccc_arr->GetEntries() > 1)
		{
			TString childs = ((TObjString *)ccc_arr->At(1))->String();
			TObjArray * childs_arr = childs.Tokenize("+");
			for (int i = 0; i < childs_arr->GetEntries(); ++i)
			{
				leading_decay.push_back(((TObjString *)childs_arr->At(i))->String().Atoi());
			}
		}
	}

//	PR(part_mechanism);
//	PR(leading_decay[0]);
//	PR(leading_decay[1]);
//	PR(boost);

	while (optind < argc)
	{
		std::cout << "Extracting from " << argv[optind] << std::endl;
		converter(argv[optind++]);
	}

	return 0;
}
