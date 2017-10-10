
//#include <ID.h>
#include "dtree.h"
#include "ID.h"

using namespace std;

vector<string> split(string line)
{
	vector<string> res;
	stringstream s(line);
	string i;
	while( s.good() )
	{
	    if(getline(s, i, ',' ))
	    res.push_back(i);
	}

	return res;
}

// function to random data values
void tenfold_data(vector<vector<string> >& input_data, vector<vector<vector<string> > >& train, vector<vector<vector<string> > >& test)
{

	random_shuffle(input_data.begin(),input_data.end());

	int k=0,j=0;
	float items=input_data.size()/10.0;

	for(int i=0;i<input_data.size();i+=items)
	{
		for(int u=0;u<i;u++)
		{
			train[k].push_back(input_data[u]);
		}
		for(j=i;j<i+items && k<10;j++)
		{
			test[k].push_back(input_data[j]);
		}
		for(int u=j;u<input_data.size();u++)
		{
			train[k].push_back(input_data[u]);
		}
		k++;
	}

}
// function to fill attribute name
vector<pair<pair<string,bool>,unordered_set<string> > > attributes(string filename,int& file_size )
{
	vector<string> full;
	vector<pair<pair<string,bool>,unordered_set<string> > > attr;
	ifstream infile(filename);
	string line;
	bool attr_type;
	int i=0;
	unordered_set<string> temp;
	//cout<<"called\n";
	while (getline(infile, line))
	{
		//cout<<"FILE : "<<line<<"\n";
	     full=split(line);
	     if(full[1]=="continuous") attr_type=true;
	     if(full[1]=="discrete") attr_type=false;
	    // if(full[0]=="target_attr") break;
	     attr.push_back(make_pair(make_pair(full[0],attr_type),temp));
	     i++;
	}

	file_size=i;
	//cout<<"filesize="<<file_size;
	  //  get value from control file
	return attr;
}

// function to fill att data
vector<vector<string> > data(string filename,int file_size,vector<vector<string> >& test_data,vector<pair<pair<string,bool>,unordered_set<string> > >& attr)
{
	vector<vector<string> > rt_data;
	int limit=file_size*0.3;

	ifstream infile(filename);
	string line;
	vector<string> full;
	while (getline(infile, line))
	{
		full=split(line);

		int target_loc=attr.size();

		for(int i=0;i<attr.size();i++)
		{
			//cout<<"each\t";
			//cout<<attr[i].first.first<<"\t";

			if(attr[i].second.find(full[i])==attr[i].second.end())
			{
				attr[i].second.insert(full[i]);
			}

		}

			rt_data.push_back(full);

	}
	//rt_data= Tenfold(rt_data, file_size);
	return rt_data;
}


