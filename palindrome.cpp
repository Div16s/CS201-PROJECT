#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<unordered_set>
#include<string.h>
using namespace std;
typedef struct node{
  struct node *child[256];
  struct node *suffixlink;
  //start and end denote the starting and ending character index of an edge
  int start;
  int *end;
  //suffixindex contains the starting index of asuffix that ends on leaf node,and for internal and root nodes it is -1
  int suffixindex;
  //unordered sets of each node that contain the forward and reverse indexes
  unordered_set <int> *forward;
  unordered_set <int> *reverse;
  } node;
/*Activelength and activeedge of the active point
Edge denotes the next node toward which we need to traverse from current node and length tells how much characters we need to traverse on activeedge*/
int leafend = -1,activelength = 0,activeEdge = -1,remainingsuffix=0,size=-1;
//leafend is the global variable that all leaves end index point to
//size denotes the size of text that we are processing
int *rootend=NULL;
//internal end is used to store end index of internal nodes
int *internalend=NULL;
//activenode of activepoint
node *activenode=NULL;
//lastnew stores the last newly created internal node that is waiting for its suffix link to point to proper node
node *lastnew=NULL;
//root of the tree
node*root=NULL;
char text[200];
int size1=0;
int reverseindex; 
//iterator for search
unordered_set<int>::iterator forwardindex;
//creates new node and returns it
node *newnode(int start, int *end)
{
    node *temp=(node*)malloc(sizeof(node));
    temp->start=start;
    temp->end =end;
    temp->suffixindex=-1;
    temp->suffixlink=root;
    for(int i=0;i<256;i++)
    {
        temp->child[i]=NULL;
    }
    temp->forward=new unordered_set<int>;
    temp->reverse=new unordered_set<int>;
    return temp;
}
//this function return the number of characters on an edge
int edgelength(node *n)
{
   if(n==root)
   return 0;

   else
   return *(n->end)-(n->start)+1;
 }
 //running a dfs on the nodes and setting their suffix indexes
 void setsuffixindexofleaves(node *n,int length)
 {
    if(n==NULL)
    return;

    int leaf=1;
    for(int i=0;i<256;i++)
    {
      if(n->child[i]!=NULL)
       {
         leaf=0;
        setsuffixindexofleaves(n->child[i],length+edgelength(n->child[i]));

        if(n != root)
            {
                //all suffix indexes of all children nodes are passed to parent node
                n->forward->insert(  n->child[i]->forward->begin(),n->child[i]->forward->end());
                n->reverse->insert( n->child[i]->reverse->begin(), n->child[i]->reverse->end());
            }
       }

    }
   //if current node is a leaf set the suffix index else it will remain -1
   if(leaf==1)
   {
    for(int i= n->start; i<= *(n->end); i++)
        {
            if(text[i]=='#')//removing characters that occur after '#'
            {
                n->end=(int*) malloc(sizeof(int));
                *(n->end)=i;
            }
        }
    n->suffixindex=size-length;

    if(n->suffixindex < size1)//then it means it is index of original string so insert this in forward set
           { n->forward->insert(n->suffixindex);}
        else//insert the reverse index in case current suffix index is more than size1 which means it belongs to reverse string
            {
              n->reverse->insert(n->suffixindex- size1);
              }
   }
 }
 //simple function that modifies active point if activelength is greater than the edge length of current node
 int walkdown(node *n)
 {
  if(activelength>=edgelength(n))
  {
    activeEdge=(int)text[activeEdge+edgelength(n)];
    activelength=activelength-edgelength(n);
    activenode=n;
    return 1;
  }
  return 0;
 }
 void extendtree(int phase)
 {
    remainingsuffix++;
    leafend=phase;
    ///first rule extension done by changind end of leaf nodes/////
    lastnew=NULL;
    ////in each new phase initially no node is waiting for a suffix link

    ///now addding suffix remaining using rule 2 or rule 3//////////
    while(remainingsuffix>0)
    {
       if(activelength==0)
       {
        ///rules of activepoint changes
        activeEdge=(int)text[phase];//change made
        }

        if(activenode->child[activeEdge]==NULL)
        {
            //Extension rule  2 will be applicable here as there is no outgoing edge
            activenode->child[activeEdge]=newnode(phase,&leafend);

          if(lastnew!=NULL)
          {
            lastnew->suffixlink=activenode;
            lastnew=NULL;
          }
        }

        else
        {
           node *next = activenode->child[activeEdge];
           if(walkdown(next))
           {
            continue;
           }
        ///rule 3 applicable if current phase character already  on the edge
          if(text[next->start + activelength]==text[phase])
          {
            if(lastnew!=NULL && activenode!=root)
            {
                lastnew->suffixlink=activenode;
                lastnew=NULL;
            }
            activelength++;
            break;
          }
          /////we reach here if the next character did not match the character we wanted so now we will create an internal node and a leaf node 
          //creating an internal node
          internalend=(int*)malloc(sizeof(int));
          *internalend = next->start + activelength - 1;;

          node *internal = newnode(next->start,internalend);
          activenode->child[activeEdge]= internal;
          next->start += activelength;
          //setting appropriate pointers to the leaf node and the next node
          internal->child[(int)text[phase]]=newnode(phase,&leafend);
          internal->child[text[next->start]] = next;
          ///internal node created so now we check for suffix link 
          if(lastnew!=NULL)
          {
            lastnew->suffixlink=internal;
          }
          lastnew=internal;
        

        }
        //as we are done with one suffix reduce count of suffix
        remainingsuffix--;
        if(activenode==root && activelength>0)
        {
          activeEdge=(int)text[phase -remainingsuffix + 1];
          activelength--; 
        }
        else if(activenode!=root)
        {
          activenode=activenode->suffixlink;
        }
        //update activepoint so that the next phase can us this activepoint to directly extend tree instead of starting from the node
        
    }

 }
 void buildsuffixtree()
 {
  size=strlen(text);
  rootend=(int*)malloc(sizeof(int));
  *rootend=-1;
  root=newnode(-1,rootend);
  activenode=root;

  for(int i=0;i<size;i++)
  {
    extendtree(i);
  }
  int length=0;
  setsuffixindexofleaves(root,length);
 }
 /*This function frees the suffix tree allocated memory
 at each recursion it  checks if that node has child or not and if node does 
 then it recurses on that child and frees the nodes also for internal nodes it also 
 frees the end variable.*/
void freetree(node *n)
{
	if(n==NULL)
		{return;}

	for (int i=0;i<256; i++)
	{
		if (n->child[i] != NULL)
		{
			freetree(n->child[i]);
		}
	}
	if(n->suffixindex==-1)
	{free(n->end);}

	free(n);
}
//this function finds the deepest internal node such that the length of commom substring is largest and the starting and end index of that match 
//in original as well as reverse string
void doTraversal(node *n, int labelHeight, int* maxHeight,int* substringStartIndex)
{
    if(n == NULL)
    {
        return;
    }
    int i=0;
    int check = -1;
    if(n->suffixindex < 0) //If it is internal node
    {
        for (i = 0; i <256; i++)
        {
            if(n->child[i] != NULL)
            {
                doTraversal(n->child[i], labelHeight +edgelength(n->child[i]),maxHeight, substringStartIndex);
                 //check if current labelheight is more than current max palindrome we found
                if(*maxHeight < labelHeight && n->forward->size() > 0 && n->reverse->size() > 0)
                {
                  //checking for forward and reverse index that satisfy the relation necessary
                    for (forwardindex=n->forward->begin();  forwardindex!=n->forward->end();++forwardindex)
                    {
                        reverseindex = (size1-2)-(*forwardindex+labelHeight-1);
                        //If we find a corresponding index in reverse such that both forward and reverse index are at same position
                        //then update the deepest node to current node and change substingstart index and length appropriately
                        if(n->reverse->find(reverseindex) !=n->reverse->end())
                        {
                            *maxHeight = labelHeight;
                            *substringStartIndex = *(n->end) -labelHeight + 1;
                            break;
                        }
                    }
                }
            }
        }
    }
}
//checks if we found a palindrome and prints it if found
void  palindrome()
{
    int maxheight = 0;
    int start = 0;
    doTraversal(root, 0, &maxheight, &start);
    int i;
    for (i=0; i<maxheight;i++)
       { printf("%c",text[i+start]);}
    if(i == 0)
        printf("No palindromic substring found.\n");
    else
        printf(",of length:%d\n",maxheight);
}
int main()
{
  //An example of finding longest possible palindrome substring in text "xababayz"
  //size1 is the length of substring+1
  //+1 happens to account for special character '#'
    size1 = 9;
    printf("Longest Palindromic Substring in xababayz is: ");
    //appending text and its reverse string as shown below
    //here zyababax is the reverse string
    strcpy(text, "xababayz#zyababax$"); buildsuffixtree();
    palindrome();
    freetree(root);
}