#include "dtree.h"

using namespace std;


vector<string> split(string line);
vector<pair<pair<string,bool>,unordered_set<string> > > attributes(string filename,int& file_size );
vector<vector<string> > data(string filename,int file_size,vector<vector<string> >& test_data,vector<pair<pair<string,bool>,unordered_set<string> > >& attr);
vector<vector<string> > Tenfold(vector<vector<string> > input_data, int file_size);

void tenfold_data(vector<vector<string> >& input_data, vector<vector<vector<string> > >& train, vector<vector<vector<string> > >& test);
