/*
 * hello.cpp

 *
 *  Created on: Feb 16, 2016
 *      Author: anamika
 */

//#include <ID.h>
#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<cmath>
#include "dtree.h"
#include "ID.h"

using namespace std;
int sz=0;
unordered_set<string> tt;

int main(int argc,char *argv[])
{

  int file_size=0;
  vector<pair<pair<string,bool>,unordered_set<string> > > attr= attributes(argv[1],file_size);
  vector<vector<string> > input_data;
  vector<vector<string> > test_data;

  vector<vector<vector<string> > > train(10);
  vector<vector<vector<string> > > test(10);

  input_data=data(argv[2],file_size,test_data,attr);

  tenfold_data(input_data,train,test);

  // Tree

  Tree *root=NULL;

  vector<float> acc(10);

  for(int i=0;i<10;i++)
  {
	  if(train[i].size()==0) continue;
	  vector<bool> del_att(attr.size(),true);
	  root = id3(attr,train[i],del_att);
	  sz=root->class_type.size();

	  int a=0;

	  string v = get_majority(attr,input_data);


	  cout<<"Maj value is \t"<<v<<"\n";
	  for(pair<string,int> p : root->class_type)
	  {
		tt.insert(p.first);
	  }
	  vector<string> stack;
	  print_tree(root,stack);

	  float ac_w=test_accuracy(root,test[i],attr,a,v);
	  acc[i]=ac_w;
	  cout<<"==== WITHOUT PRUNNING ACCURACY =====\n";
	  cout<<"\n";
	  cout<<(ac_w/test[i].size())*100<<"\n";

	  cout<<"Majority Classifier (without pruning) is : \t";
	  cout<<((float)a/(float)test[i].size())*100.0<<"\n";

	  bool cont_prunning=true;
	  float  prev_val=INT_MIN,cur_val=0;

	  prune_tree(root);

	  cout<<"==== AFTER PRUNNING\n";
	  cout<<"\n";
	  vector<string> stack_pr;
	  print_tree(root,stack_pr);

	  a=0;
	  float ac=test_accuracy(root,test[i],attr,a,v);
	  cout<<"==== AFTER PRUNNING ACCURACY =====\n";
	  cout<<"\n";
	  cout<<(ac/test[i].size())*100<<"\n";

	  cout<<"Majority Classifier (with pruning) is : \t";
	 	  cout<<((float)a/(float)test[i].size())*100.0<<"\n";

	  cout<<"==========================================\n";
  }

  //Calculate mean accuracy
  float av_acc=0;
  for(int i=0;i<acc.size();i++)
  {
	  av_acc+=acc[i];
  }
  av_acc=(av_acc/1000);
  cout<<" MEAN ACCURACY : "<<av_acc<<"\n";

  //Calculate standard deviation
  float sd=0;
  for(int i=0;i<acc.size();i++)
  {
	  float v=(acc[i]/100.0)-av_acc;
	  sd+=pow(v,2);
  }
  sd=sd/10;
  sd=sqrt(sd);
  cout<<" Standard Deviation : "<<sd<<"\n";

  //Calculate standard error
  float se=0;
  se=sd/sqrt(10);
  cout<<" Standard Error : "<<se<<"\n";

  //Calculate confidence interval
  float one_ci=0,two_ci=0;
  one_ci=av_acc-(1.96*se);
  two_ci=av_acc+(1.96*se);
  cout<<"CI : ("<<one_ci<<"\t"<<two_ci<<")\n";

  return 0;
}

void check(vector<vector<string> > data)
{
	for(int i=0;i<data.size();i++)
	{
		for(string s : data[i])
		{
			cout<<s<<"\t";
		}
		cout<<"\n";
	}
}

float test_accuracy(Tree *root,vector<vector<string> >test_data,vector<pair<pair<string,bool>,unordered_set<string> > > attr,int& acc,string val)
{
	float pass=0;
	for(int i=0;i<test_data.size();i++)
	{
		pass+=check_each(root,test_data[i],attr,acc,val);
	}

	return pass;
}

float check_each(Tree *root,vector<string> data,vector<pair<pair<string,bool>,unordered_set<string> > > attr,int& acc,string v)
{
	if(!root) return 0;
	int index=get_attr_index(root->name,attr);
	int target_loc=attr.size();
	string val= data[index];

	if(root->is_leaf==true)
	{
		if(root->name.compare(v)==0)
			acc++;

			if(root->name.compare(data[target_loc])==0)
			{
				return 1;
			}
			else
			{
				return 0;
			}
	}
	if(attr[index].first.second==true)
	{
		string pp=root->child[1].second;
		pp.erase(pp.begin());

		if(stof(pp)>stof(val))
		{
			return check_each(root->child[0].first,data,attr,acc,v);
		}
		else
		{
			return check_each(root->child[1].first,data,attr,acc,v);
		}
	}
	else
	{
		int ip = get_val_index(root,val);
		return check_each(root->child[ip].first,data,attr,acc,v);
	}
}

int get_val_index(Tree *root,string val)
{
	int i=0;
	for(pair<Tree*,string> p : root->child)
	{
		// cout<<p.second<<"\t"<<val<<"\n";
		if(p.second.compare(val)==0)
			return i;

		i++;

	}
	return 0;
}

void do_act_prunning(Tree *root,Tree *target)
{
	if(!root ||  root->child.size()==0) return;

	if(root==target)
	{
		float mx=0;
		string mx_v;
		float total=get_total(root,mx,mx_v);

		while(root->child.size()>1)
		{
			root->child.pop_back();
		}

		Tree *new_node = new Tree(vector<string>(),mx_v,true);
		root->child[0].first=new_node;
	}
	else
	{
		for(int i=0;i<root->child.size();i++)
		{
			do_act_prunning(root->child[i].first,target);
		}
	}
}

Tree* get_node_to_prune(vector<pair<Tree*,float> > pt,float& cur_val)
{
	float min_v=INT_MAX;
	Tree *ret;
	for(pair<Tree*,float> p : pt)
	{
		//cout<<p.second<<"\n";
		if(p.second<min_v)
		{
			min_v=p.second;
			ret=p.first;
			cur_val=min_v;
		}
	}
	return ret;
}

void prune_tree(Tree *root)
{
	float mx=0;
	string mx_v;
	float total=get_total(root,mx,mx_v);

	float target=0;
	for(int i=0;i<root->child.size();i++)
	{
		target+=prune_each_child(root->child[i].first,total);
	}

}

float prune_each_child(Tree *root,float total_p)
{
	if(!root ||  root->child.size()==0) return 0;

	float with_prunning=0,mx=INT_MIN;
	float k=root->class_type.size();
	string mx_v;
	float total=get_total(root,mx,mx_v);

	//cout<<total<<"\t"<<mx<<"\t"<<k<<"\n";
	with_prunning=(total-mx+k-1)/(total+k);


	float without_prunning=0;

	int gg=rand()%15265;
	for(int i=0;i<root->child.size();i++)
	{
		without_prunning+=prune_each_child(root->child[i].first,total);
	}
	//cout<<with_prunning<<"\t"<<without_prunning<<"\n";
	if(with_prunning>without_prunning && gg%2==0)
	{
		//cout<<"PRUNNING CASE\n";

		float mx=0;
				string mx_v;
				float total=get_total(root,mx,mx_v);

				while(root->child.size()>1)
				{
					root->child.pop_back();
				}

				Tree *new_node = new Tree(vector<string>(),mx_v,true);
				root->child[0].first=new_node;

	}
	return (total/total_p)*with_prunning;
}

float get_total(Tree *root,float& mx,string& mx_v)
{
	float total=0;
	for(pair<string,int> p : root->class_type)
	{
		total+=p.second;
		if(mx < p.second)
		{
			mx=p.second;
			mx_v.assign(p.first);
		}
	}
	return total;
}

void print_tree(Tree *root,vector<string> stack)
{
	if(root==NULL) return;
	if(root->child.size()==0 && root->is_leaf==true)
	{
		stack.push_back(root->name);
		for(string s : stack)
			cout<<s<<"\t";
		cout<<"\n";

		return;
	}

	if(root->class_type.size()<sz)
	{
		unordered_set<string> loc;

		for(pair<string,int> p : root->class_type)
		{
			loc.insert(p.first);
		}

		for(string o : tt)
		{
			if(loc.find(o)==loc.end())
			{
				root->class_type.insert(make_pair(o,0));
			}
		}
	}
	stack.push_back(root->name);

	string pub=" [ ";

	for(pair<string,int> p : root->class_type)
	{
		//cout<<"got"<<p.second<<"\n";

		pub+=p.first+" "+to_string(p.second)+" ";
	}
	pub+=" ] ";

	stack.push_back(pub);
	for(int i=0;i<root->child.size();i++)
	{
		stack.push_back(" = "+root->child[i].second);
		print_tree(root->child[i].first,stack);
		stack.pop_back();
	}
}

float get_mid(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> >& input_data,float& start,float& end,string atr)
{
	float res=0;
	float min_s=INT_MAX,max_e=INT_MIN;
	int target_loc=get_attr_index(atr,attr);

	for(int i=0;i<input_data.size();i++)
	{
		if(min_s>stof(input_data[i][target_loc]))
		{
			min_s=stof(input_data[i][target_loc]);
		}

		if(max_e<stof(input_data[i][target_loc]))
		{
			max_e=stof(input_data[i][target_loc]);
		}
	}
	res=(min_s+max_e)/2;
	start=min_s;
	end=max_e;
	return res;
}

void print_data(vector<vector<string> > data)
{
	for(int i=0;i<data.size();i++)
	{
		for(string s : data[i])
			cout<<s<<"\t";
		cout<<"\n";
	}
}

unordered_map<string,int> get_class_type(vector<pair<pair<string,bool>,unordered_set<string> > >& attr,vector<vector<string> > input_data,string split_attr)
{
	unordered_map<string,int> res;
	int index=attr.size();

	for(int i=0;i<input_data.size();i++)
	{
		string val=input_data[i][index];
		if(res.find(val)==res.end())
		{
			res.insert(make_pair(val,1));
		}
		else
		{
			res[val]++;
		}
	}
	return res;
}

Tree* id3(vector<pair<pair<string,bool>,unordered_set<string> > >& attr,vector<vector<string> > input_data,vector<bool>& del_att)
{

	string majority_target;
	//cout<<"input data"<<input_data.size()<<"\n";
	if(input_data.size()==0) return NULL;
	if(check_single_attr(attr,input_data,majority_target) || check_empty_attr(attr,input_data,majority_target,del_att))
	{
		Tree *node=new Tree(vector<string>(),majority_target,true);
		return node;
	}
	else
	{
		string split_attr=get_attr(attr,del_att,input_data);
		//cout<<split_attr<<"\n";

		int lc_index= get_attr_index(split_attr,attr);
		if(attr[lc_index].first.second==true)
		{
			// continous attribute
			//cout<<split_attr<<"continous\n";
			float start=0,end=0;
			float mid=get_mid(attr,input_data,start,end,split_attr);
			//cout<<"mid -- "<<start<<"\t"<<end<<"\t"<<mid<<"\n";

			string left="<="+to_string(mid);
			string right=">"+to_string(mid);

			vector<string> temp;

			temp.push_back(left);
			temp.push_back(right);

			Tree *node = new Tree(temp,split_attr,false);
			node->class_type=get_class_type(attr,input_data,node->name);

			//print_data(input_data);
			vector<vector<string> > lf=modify_cont(input_data,split_attr,start,mid,attr);
			//print_data(lf);
			node->child[0].first=id3(attr,lf,del_att);

			vector<vector<string> > rf=modify_cont(input_data,split_attr,mid,end,attr);
			node->child[1].first=id3(attr,rf,del_att);

			return node;
		}
		else
		{
			// discrete attribute
			// get split attribute index

			int index= get_attr_index(split_attr,attr);
			//cout<<split_attr<<"\t"<<index<<"\n";
			vector<string> temp=prep_child(attr[lc_index].second);

			Tree *node = new Tree(temp,split_attr,false);
			node->class_type=get_class_type(attr,input_data,node->name);

			del_att[lc_index]=false;
			for(int i=0;i<temp.size();i++)
			{
				vector<vector<string> > rf=modify(input_data,split_attr,node->child[i].second,attr);
				node->child[i].first=id3(attr,rf,del_att);
			}
			return node;
		}
	}
	return NULL;
}

vector<vector<string> > modify_cont(vector<vector<string> > input_data,string atr,float start, float end,vector<pair<pair<string,bool>,unordered_set<string> > > attr)
{
	vector<vector<string> > res;
	int index=get_attr_index(atr,attr);

	//cout<<"index --"<<index<<"\t"<<atr<<"\n";
	for(int i=0;i<input_data.size();i++)
	{
		string val=input_data[i][index];
		if(stof(val)>start && stof(val)<=end)
		{
			res.push_back(input_data[i]);
		}
	}
	return res;
}

vector<vector<string> > modify(vector<vector<string> > input_data,string atr,string val,vector<pair<pair<string,bool>,unordered_set<string> > > attr)
{

	vector<vector<string> > res;

	int index=get_attr_index(atr,attr);

	for(int i=0;i<input_data.size();i++)
	{
		if(input_data[i][index].compare(val)==0)
		{
			res.push_back(input_data[i]);
		}
	}
	return res;
}

vector<string> prep_child(unordered_set<string> cl)
{
	vector<string> res;

	for(string s : cl)
	{
		res.push_back(s);
	}
	return res;
}

int get_attr_index(string split,vector<pair<pair<string,bool>,unordered_set<string> > > attr)
{
	int i=0;
	for(pair<pair<string,bool>,unordered_set<string> > p : attr)
	{
		if(p.first.first.compare(split)==0)
			return i;

		i++;
	}
	return 0;
}

string get_attr(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<bool> del_att,vector<vector<string> > input_data)
{

	float total_v=0;
	float total_e=get_total_entropy(attr,total_v,input_data);

	float max_entropy=INT_MIN;
	string split_att;
	//cout<<total_e<<"\n";
	int i=0;
	for(pair<pair<string,bool>,unordered_set<string> > p  : attr)
	{
		pair<string,bool> ec=p.first;
		if(del_att[i]==true)
		{
			float entropy_att=0;
			if(ec.second==false)
			{
				// discrete att
				for(string pp : p.second)
				{
					vector<vector<string> > mod_att=modify(input_data,ec.first,pp,attr);

					float sv=0;
					float entropy_at =get_total_entropy(attr,sv,mod_att);
					entropy_att+=((sv/total_v)*entropy_at);
				}
			}
			else
			{
				// cont att
				float start=0,end=0;
				float mid=get_mid(attr,input_data,start,end,ec.first);
				//cout<<"->"<<start<<"\t"<<end<<"\t"<<mid<<"\n";
				vector<vector<string> > left = modify_cont(input_data,ec.first,start,mid,attr);
				vector<vector<string> > right = modify_cont(input_data,ec.first,mid,end,attr);

				float sv_left=0;
				float e_left=get_total_entropy(attr,sv_left,left);
				entropy_att+=((sv_left/total_v)*e_left);

				float sv_right=0;
				float e_right=get_total_entropy(attr,sv_right,right);
				entropy_att+=((sv_right/total_v)*e_right);
			}

			float gain=total_e-entropy_att;
			if(gain>max_entropy)
			{
				max_entropy=gain;
				split_att.assign(ec.first);
			}
		}
		i++;
	}
	return split_att;
}

float get_total_entropy(vector<pair<pair<string,bool>,unordered_set<string> > > attr,float& total_v,vector<vector<string> > input_data)
{
	float total_e=0;
	unordered_map<string,float> nu_out;

	int target_loc=attr.size();
	for(int i=0;i<input_data.size();i++)
	{
		string val=input_data[i][target_loc];
		if(nu_out.find(val)==nu_out.end())
		{
			nu_out.insert(make_pair(val,1));
		}
		else
		{
			nu_out[val]++;
		}
		total_v+=nu_out[val];
	}
	for(pair<string,int> p : nu_out)
	{
		total_e+=-1.0*((p.second)/total_v)*log2(p.second/total_v);
	}
	return total_e;
}

string get_majority(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> > input_data)
{
	string major;
	int  max_value=INT_MIN;
	unordered_map<string,int> si_at;
	int target_loc=attr.size();
	for(int i=0;i<input_data.size();i++)
	{
			string val=input_data[i][target_loc];
			if(si_at.find(val)==si_at.end())
			{
				si_at.insert(make_pair(val,1));
			}
			else
			{
				si_at[val]++;
			}
			if(max_value<si_at[val])
			{
				max_value=si_at[val];
				major.assign(val);
			}
	}
	return major;
}

bool att_empty(vector<bool> del_att)
{
	for(bool p : del_att)
	{
		if(p==true)
			return false;
	}
	return true;
}

bool check_empty_attr(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> >& input_data,string&  target,vector<bool>& del_att)
{
	if(att_empty(del_att))
	{
		//cout<<"empty attr\n";
		target.assign(get_majority(attr,input_data));
		return true;
	}
	else
	return false;
}

bool check_single_attr(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> >& input_data,string& target)
{
	unordered_map<string,int> si_at;
	int target_loc=attr.size();
	for(int i=0;i<input_data.size();i++)
	{
		string val=input_data[i][target_loc];
		if(si_at.find(val)==si_at.end())
		{
			si_at.insert(make_pair(val,1));
		}
		else
		{
			si_at[val]++;
		}
	}
	if(si_at.size()==1)
	{
			//cout<<"single attr\n";
			for(pair<string,int> p : si_at)
			target.assign(p.first);
			return true;
	}
	else
	return false;
}
