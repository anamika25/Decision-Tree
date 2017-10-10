/*
 * dtree.h
 *
 *  Created on: Feb 16, 2016
 *      Author: anamika
 */
#ifndef __DTREE_H__
#define __DTREE_H__

#include<iostream>
#include<vector>
#include<unordered_map>
#include<sstream>
#include<fstream>
#include<map>
#include<math.h>
#include<algorithm>
#include<unordered_set>

using namespace std;

class Tree
{
public:

	string name;
	vector<pair<Tree*,string> > child;
	bool is_leaf;
	unordered_map<string,int> class_type;

	Tree(vector<string> val,string s,bool lf)
	{
		name.assign(s);
		is_leaf=lf;

		for(string s : val)
		{
			pair<Tree*,string> p;
			p.first=NULL;
			p.second.assign(s);
			child.push_back(p);
		}
	}

};


void print_tree(Tree *root,vector<string> stack);
float get_mid(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> >& input_data,float& start,float& end,string atr);
Tree* id3(vector<pair<pair<string,bool>,unordered_set<string> > >& attr,vector<vector<string> > input_data,vector<bool>& del_att);
vector<vector<string> > modify_cont(vector<vector<string> > input_data,string atr,float start, float end,vector<pair<pair<string,bool>,unordered_set<string> > > attr);
vector<vector<string> > modify(vector<vector<string> > input_data,string atr,string val,vector<pair<pair<string,bool>,unordered_set<string> > > attr);
vector<string> prep_child(unordered_set<string> cl);
int get_attr_index(string split,vector<pair<pair<string,bool>,unordered_set<string> > > attr);
string get_attr(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<bool> del_att,vector<vector<string> > input_data);
float get_total_entropy(vector<pair<pair<string,bool>,unordered_set<string> > > attr,float& total_v,vector<vector<string> > input_data);
string get_majority(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> > input_data);

bool att_empty(vector<bool> del_att);
bool check_empty_attr(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> >& input_data,string&  target,vector<bool>& del_att);
bool check_single_attr(vector<pair<pair<string,bool>,unordered_set<string> > > attr,vector<vector<string> >& input_data,string& target);

unordered_map<string,int> get_class_type(vector<pair<pair<string,bool>,unordered_set<string> > >& attr,vector<vector<string> > input_data,string split_attr);


void do_act_prunning(Tree *root,Tree *target);
Tree* get_node_to_prune(vector<pair<Tree*,float> > pt,float& cur_val);
void prune_tree(Tree *root);
float prune_each_child(Tree *root,float total_p);
float get_total(Tree *root,float& mx,string& mx_v);

int get_val_index(Tree *root,string val);
float check_each(Tree *root,vector<string> data,vector<pair<pair<string,bool>,unordered_set<string> > > attr,int& acc,string v);
float test_accuracy(Tree *root,vector<vector<string> >test_data,vector<pair<pair<string,bool>,unordered_set<string> > > attr,int& acc,string val);

void check(vector<vector<string> > data);
#endif

