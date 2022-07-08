/* k-means.cpp */
/************************************************
 *	1.Given k, number of prototype		*
 *	3.Read training data file		*
 *	4.Read testing data file		*
 *	5.Training prototypes			*
 *	6.Testing, alert anomaly or classify	*
 ************************************************/
#include "k-means.h"

bool operator< (k_means::data& d1, k_means::data& d2)  {
	return d1.cpu<d2.cpu;
}/* end of operator< */

ostream& operator<< (ostream& out, const k_means::data& d)  {
	return out<<d.cpu<<" "<<d.mem<<" "<<d.neti<<" "<<d.neto<<" "<<d.sys<<" "<<d.category<<" "<<d.type;
}/* end of operator << */

k_means::k_means(const int ktmp)  {

	k = ktmp;
	type_num = 4;//4 type TODO dynamic
	category_num =2;//2 category TODO dynamic 

	prototype_flag = new bool* [category_num];
	for( int i=0; i<category_num; i++ )  {
		prototype_flag[i] = new bool [type_num];
		for( int j=0; j<type_num; j++ )
			prototype_flag[i][j] = true;
	}

	prototype = new data** [category_num];
	for( int i=0; i<category_num; i++ )  {
		prototype[i] = new data*  [type_num];
		for( int j=0; j<type_num; j++ )  
			prototype[i][j] = new data[k];
	}

	type_pos = new int *[category_num];
	for( int i=0; i<category_num; i++ )
		type_pos[i] = new int [type_num];
	for( int i=0; i<category_num; i++ )
		for( int j=0; j<type_num; j++ )
			type_pos[i][j]=0;

	cout<<"*******************************"<<endl;
	cout<<"**        k-means, k="<<k<<"       **"<<endl;
	cout<<"*******************************"<<endl;
}/* end of k_means() */

double k_means::distance(data d1, data d2)  {
	return sqrt(pow(d1.cpu-d2.cpu, 2)+pow(d1.mem-d2.mem, 2)
			+pow(d1.neti-d2.neti, 2)+pow(d2.neto-d2.neto, 2)
			+pow(d1.sys-d2.sys, 2));
}/* end of distance() */

bool k_means::check_retrain(int k, const string pr_file)  {

	//TODO More perfect, also check train.txt
	//Check if prototype.txt exist
	fstream f_check;
	f_check.open("/home/ghoo/CRAT/" + pr_file);

	vector<data> data_check;
	data tmp;
	int total = category_num*k*type_num;
	//Compare if k satisfied
	if(f_check.is_open())  {
		
		//Read data
		while( f_check>>tmp.cpu )  {
			f_check>>tmp.mem>>tmp.neti>>tmp.neto
				>>tmp.sys>>tmp.category>>tmp.type;
			data_check.push_back(tmp);
		}
		cout<<"data_check size = "<<data_check.size()<<endl;

		//Clean it or do nothing
		if( total==data_check.size() )  {
			f_check.close();
			for( int cidx=0; cidx<category_num; cidx++ )
				for( int i=0; i<type_num; i++ )
					for( int j=0; j<k; j++ )  {
						prototype[cidx][i][j]=
						data_check[cidx*type_num*k+i*k+j];
					}
			//Output to terminal
			for( int cidx=0; cidx<category_num; cidx++ )  {
				cout<<"=======   Category "<<cidx+1<<"    ======="<<endl;
				cout<<"       === Prototype ==="<<endl;
				for( int i=0; i<type_num; i++ )  {
					cout<<"	  ==Type  "<<i+1<<"==   "<<endl;
					if( prototype[cidx][i][0].cpu==0 && prototype[cidx][i][0].mem==0
						&& prototype[cidx][i][0].neti==0 && prototype[cidx][i][0].neto==0
						&& prototype[cidx][i][0].sys==0 )  {
						cout<<"\n	 No Prototypes...\n\n";
						continue;
					}
					for( int j=0; j<k; j++ )  { 
						cout<<"	 "<<prototype[cidx][i][j].cpu<<" "
							<<prototype[cidx][i][j].mem<<" "
							<<prototype[cidx][i][j].neti<<" "
							<<prototype[cidx][i][j].neto<<" "
							<<prototype[cidx][i][j].sys<<endl;
					}
				}
			}
			cout<<"===============================\n\n";
			return false;
		}
		else  {
			string cmd;
			cmd = "rm /home/ghoo/CRAT/" + pr_file;
			cout<<cmd<<endl;
			system(cmd.c_str());
		}			
	}// end if(f_check.is_open())

	f_check.close();
	return true; 

}/* end of check_retrain() */

bool k_means::read_tr_file(const string tr_file)  {

	//Open file & Read train data
	ifstream fp;
	fp.open(tr_file);
	if(!fp.is_open())  {
		cout<<"Open "<<tr_file<<" err!\n";
		return false;
	}
	vector<data> tr_v1, tr_v2;//TODO dynamic
	data tr_data;
	while( fp>>tr_data.cpu )  {
		fp>>tr_data.mem>>tr_data.neti>>tr_data.neto>>tr_data.sys>>tr_data.category;
		if( tr_data.category==1 )
			tr_v1.push_back(tr_data);
		else if( tr_data.category==2 )
			tr_v2.push_back(tr_data);
		else  {
			cerr<<"Category err!!"<<endl;
			exit(-1);
		}
	}
	cat_v.push_back(tr_v1); cat_v.push_back(tr_v2);
	fp.close();
	return true;
}/* end of read_tr_file() */

void k_means::train()  {

	//Sort by cpu
	for( int i=0; i<cat_v.size(); i++ )
		sort( cat_v[i].begin(), cat_v[i].end(), [](data d1, data d2)->bool{return d2<d1;} );

	//Assign init type, record pos
	//Type: CPU+Net 1, CPU+nonNet 2, nonCPU+Net 3, nonCPU+nonNet 4
	int noproto_count = 0;
	for( int cidx=0; cidx<cat_v.size(); cidx++ )  { 
		int max_cpu_net    = -1;
		int min_cpu_net    = 99999;
		int max_cpu_nonnet = -1;
		int min_cpu_nonnet = 99999;
		for( int i=0; i<cat_v[cidx].size(); i++ )  {
			//Type 1
			if( (cat_v[cidx][i].neti+cat_v[cidx][i].neto)!=0 
					&& cat_v[cidx][i].cpu>max_cpu_net )  {
				max_cpu_net  = cat_v[cidx][i].cpu;
				type_pos[cidx][0]  = i;
				cat_v[cidx][i].type = 1;
			}
			//Type 2
			else if( (cat_v[cidx][i].neti+cat_v[cidx][i].neto)==0
					&& cat_v[cidx][i].cpu>max_cpu_nonnet )  {
				max_cpu_nonnet  = cat_v[cidx][i].cpu;
				type_pos[cidx][1]  = i;
				cat_v[cidx][i].type = 2;
			}
			//Type 3
			else if( (cat_v[cidx][i].neti+cat_v[cidx][i].neto)!=0
					&& cat_v[cidx][i].cpu<min_cpu_net )  {
				min_cpu_net  = cat_v[cidx][i].cpu;
				type_pos[cidx][2]  = i;
				cat_v[cidx][i].type = 3;
			}
			//Type 4
			else if( (cat_v[cidx][i].neti+cat_v[cidx][i].neto)==0
					&& cat_v[cidx][i].cpu<min_cpu_nonnet )  {
				min_cpu_nonnet  = cat_v[cidx][i].cpu;
				type_pos[cidx][3]  = i;
				cat_v[cidx][i].type = 4;
			}
			else;
		}
		//		cout<<"Type_pos: "<<type_pos[cidx][0]<<" "<<type_pos[cidx][1]
		//			<<" "<<type_pos[cidx][2]<<" "<<type_pos[cidx][3]<<endl;	

		//Assign type to rest of the data
		int type_count[type_num];
		for( int i=0; i<type_num; i++ )
			type_count[i] = 0;
		for( int i=0; i<cat_v[cidx].size(); i++ )  {
			int min = 9999999, pos;
			//Type 1, 3
			if( (cat_v[cidx][i].neti+cat_v[cidx][i].neto)!=0 )  {
				int dis1 = cat_v[cidx][type_pos[cidx][0]].cpu - cat_v[cidx][i].cpu;
				int dis2 = cat_v[cidx][i].cpu - cat_v[cidx][type_pos[cidx][2]].cpu;
				if( dis1 < dis2 )
					pos = 0;
				else
					pos = 2;	
			}
			//Type 2, 4
			else  {
				int dis1 = cat_v[cidx][type_pos[cidx][1]].cpu - cat_v[cidx][i].cpu;
				int dis2 = cat_v[cidx][i].cpu - cat_v[cidx][type_pos[cidx][3]].cpu;
				if( dis1 < dis2 )
					pos = 1;
				else
					pos = 3;	
			}
			cat_v[cidx][i].type = cat_v[cidx][type_pos[cidx][pos]].type;
			type_count[pos]++;
		}
		cout<<"Type_count: "<<type_count[0]<<" "<<type_count[1]
			<<" "<<type_count[2]<<" "<<type_count[3]<<endl;

		//Check if k < data_num in each type, if yes, there's no prototype in that type
		int pf_count = 0;
		for( int i=0; i<type_num; i++ )
			if( type_count[i]<k )  {
				prototype_flag[cidx][i] = false;
				pf_count++;
			}
		noproto_count+=pf_count;

		//Init the prototypes
		int c, choose[cat_v[cidx].size()];
		int* k_count;//Each type choose k init prototypes
		k_count = new int [type_num];
		for( int i=0; i<type_num; i++ )
			k_count[i]=0;
		for( int i=0; i<cat_v[cidx].size(); i++ )
			choose[i]=0;
		for( int i=0; i<(type_num-pf_count)*k; i++ )  {
			c = rand()%cat_v[cidx].size();
			if( choose[c]==1 || (k_count[cat_v[cidx][c].type-1]==k)
				|| prototype_flag[cidx][cat_v[cidx][c].type-1]==false )  {
				i--;
				continue;
			}
			else
				choose[c]=1;
			prototype[cidx][cat_v[cidx][c].type-1][k_count[cat_v[cidx][c].type-1]++]=cat_v[cidx][c];
		}

		//Calculate k prototypes in *EACH TYPES*
		bool flag=true;
		data tmp[type_num][k];//Sum all the features belongs to the prototype
		int count[type_num][k];
		for(int i=0; i<type_num; i++)  
			for(int j=0; j<k; j++)  {
				tmp[i][j].type=i+1;
				tmp[i][j].category=cidx+1;
				count[i][j]=0;
			}	

		int converge_times = 0;
		while(flag)  {
			for(int i=0; i<cat_v[cidx].size(); i++)  {
				int miny=0;
				double min=100000;
				if( prototype_flag[cidx][cat_v[cidx][i].type-1]==false )
					continue;
				for(int j=0; j<k; j++)  {
					if( distance(cat_v[cidx][i],
						prototype[cidx][cat_v[cidx][i].type-1][j])<=min )  {
						miny=j;
						min=distance(cat_v[cidx][i],
								prototype[cidx][cat_v[cidx][i].type-1][j]);
					}
				}
				tmp[cat_v[cidx][i].type-1][miny].cpu+=cat_v[cidx][i].cpu;
				tmp[cat_v[cidx][i].type-1][miny].mem+=cat_v[cidx][i].mem;
				tmp[cat_v[cidx][i].type-1][miny].neti+=cat_v[cidx][i].neti;
				tmp[cat_v[cidx][i].type-1][miny].neto+=cat_v[cidx][i].neto;
				tmp[cat_v[cidx][i].type-1][miny].sys+=cat_v[cidx][i].sys;
				count[cat_v[cidx][i].type-1][miny]++;
			}
			for(int i=0; i<type_num; i++)  {
				for(int j=0; j<k; j++)  {
					//Cause data might totally the same,
					//no data classify to one prototype, assign origin prototype to it
					if( count[i][j]==0 )  {
						tmp[i][j].cpu =prototype[cidx][i][j].cpu;
						tmp[i][j].mem =prototype[cidx][i][j].mem;
						tmp[i][j].neti=prototype[cidx][i][j].neti;
						tmp[i][j].neto=prototype[cidx][i][j].neto;
						tmp[i][j].sys =prototype[cidx][i][j].sys;
						continue;
					}
					tmp[i][j].cpu = (int)round((double)tmp[i][j].cpu/count[i][j]); 
					tmp[i][j].mem = (int)round((double)tmp[i][j].mem/count[i][j]); 
					tmp[i][j].neti= (int)round((double)tmp[i][j].neti/count[i][j]); 
					tmp[i][j].neto= (int)round((double)tmp[i][j].neto/count[i][j]); 
					tmp[i][j].sys = (int)round((double)tmp[i][j].sys/count[i][j]); 
				}
			}
			//End if prototypes converge
			int f_count=0;
			const double converge=10e-6;
			for(int i=0; i<type_num; i++)  
				for(int j=0; j<k; j++)
					if( distance(tmp[i][j], prototype[cidx][i][j])<converge )  
						f_count++;
			if(f_count==type_num*k)
				flag=false;
			else  
				f_count=0;

			//Assign & init
			for(int i=0; i<type_num; i++)  {
				for(int j=0; j<k; j++)  {
					if( count[i][j]!=0 )//Problem same as above, if 0 don't assign
						prototype[cidx][i][j]=tmp[i][j];
					tmp[i][j].cpu=0;
					tmp[i][j].mem=0;
					tmp[i][j].neti=0;
					tmp[i][j].neto=0;
					tmp[i][j].sys=0;
					count[i][j]=0;
				}
			}
		}//end of while loop

		//Output to prototype.txt
		fstream f_prototype;
		f_prototype.open("/home/ghoo/CRAT/prototype.txt", ios::out | ios::app);
		for(int i=0; i<type_num; i++)
			for(int j=0; j<k; j++) 
				f_prototype<<prototype[cidx][i][j].cpu<<" "<<prototype[cidx][i][j].mem<<" "
					<<prototype[cidx][i][j].neti<<" "<<prototype[cidx][i][j].neto<<" "
					<<prototype[cidx][i][j].sys<<" "<<prototype[cidx][i][j].category<<" "
					<<prototype[cidx][i][j].type<<endl;
		f_prototype.close();

		//Output to terminal
		cout<<"=======   Category "<<cidx+1<<"    ======="<<endl;
		cout<<"       === Prototype ==="<<endl;
		for(int i=0; i<type_num; i++)  {
			cout<<"	  ==Type  "<<i+1<<"==   "<<endl;
			if( prototype[cidx][i][0].cpu==0 && prototype[cidx][i][0].mem==0
				&& prototype[cidx][i][0].neti==0 && prototype[cidx][i][0].neto==0
				&& prototype[cidx][i][0].sys==0 )  {
				cout<<"\n	 No Prototypes...\n\n";
				continue;
			}
			for(int j=0; j<k; j++)  { 
				cout<<"	 "<<prototype[cidx][i][j].cpu<<" "<<prototype[cidx][i][j].mem<<" "
					<<prototype[cidx][i][j].neti<<" "<<prototype[cidx][i][j].neto<<" "
					<<prototype[cidx][i][j].sys<<endl;
			}
		}
		cout<<"===============================\n\n";
	}//outer for loop cat_v

	if( noproto_count==category_num*type_num )  {
		cerr<<"There is no prototype in each type for k = "<<k<<"."<<endl;
		exit(-1);
	}
	
}/* end of train() */

bool k_means::read_ts_file(const string ts_file)  {
	//Open file & Read test data
	ifstream fp;
	fp.open(ts_file);
	if(!fp.is_open())  {
		cout<<"Open "<<ts_file<<" err!\n";
		return false;
	};
	data ts_data;
	while( fp>>ts_data.cpu )  {
		fp>>ts_data.mem>>ts_data.neti>>ts_data.neto>>ts_data.sys>>ts_data.category;
		ts_v.push_back(ts_data);
	}
	fp.close();
	return true;
}/* end of read_ts_file() */

void k_means::test()  {
	//Classify through every testing data
	for(int i=0; i<ts_v.size(); i++)  {
		//Find the nearest prototypes(Nearest) in belonging category
		int minx=0, miny=0, cidx;
		double min=100000;

		if( ts_v[i].category==1 )
			cidx=0;
		else if( ts_v[i].category==2 )
			cidx=1;
		else;

		for(int j=0; j<type_num; j++)  {
			for(int l=0; l<k; l++)  {
				if( distance(ts_v[i], prototype[cidx][j][l])<min )  {
					minx=j;
					miny=l;
					min=distance(ts_v[i], prototype[cidx][j][l]);
				}
			}
		}

		//Prototype Average distance(Average) 			
		double avg=0;
		for(int j=0; j<k; j++)
			for(int l=0; l<k; l++)
				avg+=distance(prototype[cidx][minx][j], prototype[cidx][minx][l]);
		avg=avg/2/(k*(k-1)/2);
		cout<<"Avg  = "<<avg<<endl<<"Near = "<<min<<endl;

		//Check if Anomaly, Nearest<Average
		if( min<avg )
			ts_v[i].type = minx+1;
		else
			ts_v[i].type = 0;//Anomaly
		cout<<"Test data "<<i+1<<endl;
		cout<<"	Category : "<<ts_v[i].category<<endl;
		cout<<"	Vector   : "<<ts_v[i]<<endl;
		cout<<"	Result   : ";
		if( ts_v[i].type )
			cout<<"Type "<<minx+1<<"("<<miny+1<<")"<<endl;
		else
			cout<<"Anomaly!!"<<"("<<minx+1<<", "<<miny+1<<")"<<endl;
	}//end of for loop
}/* end of test() */
/* end of k_means.cpp */
