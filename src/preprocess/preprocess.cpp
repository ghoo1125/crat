/* preprocess.cpp */
/************************************************
 *	1. getting average			*
 *	2. normalized system call number	*
 *	3. generate train.txt			*
 *	4. generate test.txt			*
 ************************************************/
#include "preprocess.h"

int Preprocess::operator() (const string test_app)  {

	fstream f_applist, f_app, f_train, f_test;
	string app_name, app_vec;
	int cpu, mem, neti, neto, sys, category, d_num;
	int total_c = 0, total_m = 0, total_i = 0, total_o = 0;
	vector<int> v_cpu, v_mem, v_neti, v_neto, v_sys, v_category;

	//Getting vector & average
	f_applist.open("train_app/app_list");
	if( !f_applist.is_open() ) { cerr<<"Open applist err"<<endl; return -1; }
	while( getline(f_applist, app_name) )  {
		f_app.open("train_app/" + app_name + ".txt");
		if( !f_app.is_open() )  { cerr<<"Open " + app_name + ".txt err"<<endl; return -1; }

		d_num = 0;
		while( getline(f_app, app_vec) )  {

			//parse by ' '
			cpu = stoi(app_vec.substr(0, app_vec.find(" ")));
			app_vec = app_vec.substr(app_vec.find(" ")+1);
			mem = stoi(app_vec.substr(0, app_vec.find(" ")));
			app_vec = app_vec.substr(app_vec.find(" ")+1);
			neti = stoi(app_vec.substr(0, app_vec.find(" ")));
			app_vec = app_vec.substr(app_vec.find(" ")+1);
			neto = stoi(app_vec.substr(0, app_vec.find(" ")));
			app_vec = app_vec.substr(app_vec.find(" ")+1);
			sys = stoi(app_vec.substr(0, app_vec.find(" ")));
			app_vec = app_vec.substr(app_vec.find(" ")+1);
			category = stoi(app_vec.substr(0, app_vec.find(" ")));

			d_num++;
			total_c+=cpu;
			total_m+=mem;
			total_i+=neti;
			total_o+=neto;
		}
		total_c = round((double)total_c/d_num); v_cpu.push_back(total_c);  total_c = 0;
		total_m = round((double)total_m/d_num); v_mem.push_back(total_m);  total_m = 0;
		total_i = round((double)total_i/d_num); v_neti.push_back(total_i); total_i = 0;
		total_o = round((double)total_o/d_num); v_neto.push_back(total_o); total_o = 0;
		v_sys.push_back(sys); v_category.push_back(category);
		f_app.close();	
	}

	//Normalized system call number
	int sys_max=-1;
	vector<int>::iterator it;
	for( it=v_sys.begin(); it<v_sys.end(); it++ )  
		if( *it>sys_max )
			sys_max=*it;
	for( it=v_sys.begin(); it<v_sys.end(); it++ )  
		*it=round((double)((*it))/sys_max*100);

	//train.txt
	f_train.open("train.txt", ios::out);
	for( int i=0; i<v_cpu.size(); i++ )  {
		f_train<<v_cpu[i]<<" "<<v_mem[i]<<" "<<v_neti[i]<<" "<<v_neto[i]
			<<" "<<v_sys[i]<<" "<<v_category[i]<<endl;
	}

	//test.txt
	f_app.open("test_app/" + test_app + ".txt");
	if( !f_app.is_open() )  { cerr<<"Open test_app/"<<test_app<<".txt err"<<endl; return -1; }
	d_num = 0;
	while( getline(f_app, app_vec) )  {
		cpu = stoi(app_vec.substr(0, app_vec.find(" ")));
		app_vec = app_vec.substr(app_vec.find(" ")+1);
		mem = stoi(app_vec.substr(0, app_vec.find(" ")));
		app_vec = app_vec.substr(app_vec.find(" ")+1);
		neti = stoi(app_vec.substr(0, app_vec.find(" ")));
		app_vec = app_vec.substr(app_vec.find(" ")+1);
		neto = stoi(app_vec.substr(0, app_vec.find(" ")));
		app_vec = app_vec.substr(app_vec.find(" ")+1);
		sys = stoi(app_vec.substr(0, app_vec.find(" ")));
		app_vec = app_vec.substr(app_vec.find(" ")+1);
		category = stoi(app_vec.substr(0, app_vec.find(" ")));
	
		d_num++;
		total_c+=cpu;
		total_m+=mem;
		total_i+=neti;
		total_o+=neto;
	}
	total_c = (int)round((double)total_c/d_num);
	total_m = (int)round((double)total_m/d_num);
	total_i = (int)round((double)total_i/d_num);
	total_o = (int)round((double)total_o/d_num);
	sys = (int)round((double)(sys)/sys_max*100);

	f_test.open("test.txt", ios::out);
	f_test<<total_c<<" "<<total_m<<" "<<total_i<<" "<<total_o<<" "<<sys<<" "<<category<<endl;

	//Done
	f_app.close();		
	f_applist.close();
	f_train.close();
	return 0;
}/* end of operator() */
/* end of preprocess.cpp */
