#ifndef _K_MEANS_H_
#define _K_MEANS_H_
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;
/******************  Type  ********************
 * 1: CPU intensive with network I/O          *
 * 2: CPU intensive without newtork I/O       *
 * 3: None CPU intensive with network I/O     *
 * 4: None CPU intensive without network I/O  *
 *                                            *
 ****************  Category  ******************
 * 1: Game                                    *
 * 2: Tool                                    *
 * ...                                        *
 *********************************************/
class k_means  {
	private:
		int k, type_num, category_num;
		int **type_pos;
		bool **prototype_flag;
		struct data  {
			data() : cpu(0), mem(0), neti(0), neto(0), sys(0), category(0), type(0) {}
			int cpu;
			int mem;
			int neti;
			int neto;
			int sys;
			unsigned short category;
			unsigned short type;
		}***prototype;
		vector< vector<data> > cat_v;
		vector<data> ts_v;

	public:
		friend ostream& operator<< (ostream&, const k_means::data&);
		friend bool operator< (k_means::data&, k_means::data&);
		k_means(const int);
		double distance(data, data);
		bool check_retrain(int, const string);
		bool read_tr_file(const string);
		void train();
		bool read_ts_file(const string);
		void test();
};
#endif 
