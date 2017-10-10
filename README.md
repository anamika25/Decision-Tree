# Decision-Tree
My main file is dtree.cpp. I have experimented my code on 5 dataset which are included in my code.I have given control file
(information about columns) as first input and dataset file(all the data) as second input.
I am collecting attribute information from control file, its name and if it is continuous or discrete then all its 
possible values. If it is discrete then to split the tree, I calculated Information Gain to select the best classifier in all
available attributes and make that root node and then make its possible values as its children. To proceed in this way, I 
called id3 function to make the decision tree. It is a recursive function which will be called for each attribute according to 
the modified data. For discrete attribute, we can directly implement this algorithm but for continuous attributes, I took mean 
of the range and then split the values in binary order by comparing them with mean.
We stop splitting when we do not have any other possible data to split and we make that node as leaf.

After building the decision tree, I am checking if it is possible to prune the nodes in the tree using Minimum Error Pruning 
technique. It gives a tree which has minimum error rate when classifying independent sets of data.If pruning the node leads to 
a greater expected error rate, then keep the sub-tree otherwise prune it.
I have used ten fold cross validation here.
