/* parse.cpp */
/*****************************************************************
 *   1. Parse cpu.txt, mem.txt, net.txt, sys.txt -> app.txt  	 *
 *   2. normalized mem(%), net(pkt/5secs)			 *
 *****************************************************************/
#include "parse.h"

int Parse::operator()  (const string app_name, int exec, int category)  {

	//Open file
	fstream f_cpu, f_mem, f_net, f_sys, f_out, f_applist;
	f_cpu.open("database/" + app_name + "/cpu.txt"); 
	if( !f_cpu.is_open() ) { cerr<<"Open cpu.txt err"<<endl; return -2; }
	f_mem.open("database/" + app_name + "/mem.txt"); 
	if( !f_mem.is_open() ) { cerr<<"Open mem.txt err"<<endl; return -2; }
	f_net.open("database/" + app_name + "/net.txt"); 
	if( !f_net.is_open() ) { cerr<<"Open net.txt err"<<endl; return -2; }
	f_sys.open("database/" + app_name + "/sys.txt"); 
	if( !f_sys.is_open() ) { cerr<<"Open sys.txt err"<<endl; return -2; }

	if( exec==1 )	f_out.open("train_app/" + app_name + ".txt", ios::out);	
	else		f_out.open("test_app/" + app_name + ".txt", ios::out);

	//max_mem init val & sys call number & net I/O init val
	int net_count = 1, neti_count = 3, neto_count = 11;
	string s_cpu, s_mem, s_net, s_neti, s_neto, s_sys, s_out;
	int neti_tmp, neto_tmp;

	getline(f_mem, s_mem);
	double max_mem=stod(s_mem);

	getline(f_sys, s_sys);

	getline(f_net, s_net);
	s_net = s_net.substr(s_net.find(":")+1);
        net_count++;
        while(1)  {

                if( s_net.find(" ")==0 )  {
                        s_net = s_net.substr(s_net.find(" ")+1);
                        continue;
                }
                else  {
                        s_net = s_net.substr(s_net.find(" "));
                        net_count++;
                }

                if( net_count==neti_count )  {
                        neti_tmp = stoi(s_net.substr(0, 8));
                        s_net = s_net.substr(8);
                        net_count++;
                }

                if( net_count==neto_count )  {
                        neto_tmp = stoi(s_net.substr(0, 8));
                        break;
                }
        }

	//Normalized mem, net I/0, output app.txt
	while(1)  {
		//cpu
		if( !getline(f_cpu, s_cpu) )  break;
		s_cpu = to_string(stoi(s_cpu.substr(9, 3)));

		//mem
		if( !getline(f_mem, s_mem) )  break;
		s_mem = to_string((int)round(stod(s_mem)/max_mem*100));

		//net
		net_count = 1, neti_count = 3, neto_count = 11;
		if( !getline(f_net, s_net) )  break;
		s_net = s_net.substr(s_net.find(":")+1);
                net_count++;
                while(1)  {

                        if( s_net.find(" ")==0 )  {
                                s_net = s_net.substr(s_net.find(" ")+1);
                                continue;
                        }
                        else  {
                                s_net = s_net.substr(s_net.find(" "));
                                net_count++;
                        }

                        if( net_count==neti_count )  {
                                s_neti = to_string(stoi(s_net.substr(0, 8)));
                                s_net = s_net.substr(8);
                                net_count++;
                        }

                        if( net_count==neto_count )  {
                                s_neto = to_string(stoi(s_net.substr(0, 8)));
                                break;
                        }
                }//end of inner while(1) loop
		string tmpi, tmpo;
                tmpi = s_neti; tmpo = s_neto;
                s_neti = to_string(stoi(s_neti)-neti_tmp);
                s_neto = to_string(stoi(s_neto)-neto_tmp);
                neti_tmp = stoi(tmpi); neto_tmp = stoi(tmpo);

		//Write to app.txt
		s_out = s_cpu + " " + s_mem + " " + s_neti + " " +
			s_neto + " " + s_sys + " " + to_string(category);
		f_out<<s_out<<endl;	
	}//end of outer while(1) loop

	//Add to app_list (Only train need)
	if( exec==1 )  {
		f_applist.open("train_app/app_list", ios::out | ios::app);
		f_applist<<app_name<<endl;
		f_applist.close();
	}

	//Done
	f_cpu.close();
	f_mem.close();
	f_net.close();
	f_sys.close();
	f_out.close();
	return 0;
}/* end of operator() */
/* end of parse.cpp */
