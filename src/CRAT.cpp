/* CRAT.cpp */
/********************************************************
 *	Monitor: Fetch->Parse				*
 *	Detech : Fetch->Parse->Preprocess->Detect	*
 ********************************************************/
#include <iostream>
#include <fstream>
#include <chrono>
#include "/home/ghoo/CRAT/parse/parse.h"
#include "/home/ghoo/CRAT/preprocess/preprocess.h"
#include "/home/ghoo/CRAT/k-means/k-means.h"
using namespace std;

/*TODO List
  1. Type automation
  2. Category automation
  3. System call automation
 */

//argv[1]: app_name, argv[2]: exec, argv[3]: Category(1 Game, 2 NonGame, ...), argv[4]: k(for k-means)
int main(int argc, char **argv)  {

	/***** Parsing args *****/
	int k;
	if( argc==4 && atoi(argv[2])==1 && atoi(argv[3])>0 );
	else if( argc==5 && atoi(argv[2])==2 && atoi(argv[3])>0 && atoi(argv[4])>0)
		k = atoi(argv[4]);
	else  {
		cerr<<"Input as format[\"Application_name\", 1(Preparation Mode), Category ] OR\n";
		cerr<<"Input as format[\"Application_name\", 2(Detection Mode), Category, k>0 ]\n";
		return -1;
	}
	string app_name = argv[1];
	app_name = app_name.substr(0, 15);//due to cmd "b2g-ps" will cut app_name

	//Calculate space num, due to grep
	int space_num = 0;
	int exec = atoi(argv[2]), category = atoi(argv[3]);
	if( exec==1 )	cout<<"Monitoring application : "<<app_name<<endl;
	else		cout<<"Testing application : "<<app_name<<endl;

	int found = app_name.find(" ");
	while( found!=string::npos )  {
		space_num++;
		found = app_name.find(" ", found+1);
	}

	/****** Monitoring ******/
	//Check if file exists
	fstream f_check;
	if( exec==1 )	f_check.open("train_app/" + app_name + ".txt");
	else		f_check.open("test_app/" + app_name + ".txt");

	if( f_check.is_open() )  {
		if( exec==1 )  {
			cout<<"Application info already exist. Done Monitoring."<<endl;
			return 0;
		}
		cout<<"Application info already exist. Detect directly."<<endl;
	}
	else  {
		//Fetch & create app_dir
		string cmd;
		cmd = "sh /home/ghoo/CRAT/fetch/fetch.sh " + to_string(space_num) + " " + "\"" + app_name + "\"";
		cout<<"cmd = "<<cmd<<endl;
		system(cmd.c_str()); 

		//Wait for sys.txt, TODO automation
		cout<<"Waiting for sys.txt, press ENTER to continue..."<<endl;
		cin.get();
		cout<<"Done Fetch!"<<endl;

		//Parse, remove file if parse fail
		class Parse parse;
		int ret = parse(app_name, exec, category);
		if( ret==-2 )  {
			cerr<<"Parse error!"<<endl;
			return -1;
		}
		else if( ret==-1 ) {
			cout<<"Result: Network I/O Anomaly!!"<<endl; 
			string cmd;
			if( exec==1 )	cmd = "rm /home/ghoo/CRAT/train_app/\"" + app_name + ".txt" + "\"";
			else		cmd = "rm /home/ghoo/CRAT/test_app/\""  + app_name + ".txt" + "\"";
			cout<<"cmd = "<<cmd<<endl;
			//system(cmd.c_str());
			return -1; 
		}
		else
			cout<<"Done Parse!"<<endl;

		cmd = "rm -r /home/ghoo/CRAT/database/\"" + app_name + "\"";
		cout<<"cmd = "<<cmd<<endl;
		//system(cmd.c_str());
	}
	f_check.close();

	/*** Anomaly_Detection ***/
	class Preprocess preprocess;
	if( exec==2 )  {

		//Preprocess
		if( !preprocess(app_name) )  { cout<<"Done Preproces!"<<endl; }
		else { cerr<<"preprocess error!"<<endl; return -1;}

		//Record time t1
		chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

		//Detect
		srand(time(0));
		k_means km(k);
		if( km.check_retrain(k, "prototype.txt") )  {
			if( !km.read_tr_file("train.txt") ) return -1;
			km.train();
			cout<<"Done training!"<<endl;
		}
		else
			cout<<"Prototypes exist, do not retrain. Dectect directly.\n";
		if( !km.read_ts_file("test.txt") ) return -1;
		km.test();
		cout<<"Done Detection!"<<endl;

		//Record time t2
		chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
		cout<<"Detection time: "<<chrono::duration_cast< chrono::microseconds >(t2-t1).count()<<" us\n";
	}
	else
		cout<<"Done Preparation!"<<endl;

	/*** Done ***/
	return 0;
}
/* end of CRAT.cpp */
