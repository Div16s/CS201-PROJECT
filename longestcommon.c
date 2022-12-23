#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct node{
  struct node *child[256];
  struct node *suffixlink;
  //start and end denote the starting and ending character index of an edge
  int start;
  int *end;
  //suffixindex contains the starting index of asuffix that ends on leaf node,and for internal and root nodes it is -1
  int suffixindex;} node;

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
        setsuffixindexofleaves(n->child[i], length + edgelength(n->child[i]));
       }
    }
   //if current node is a leaf set the suffix index else it will remain -1
   if(leaf==1)
   {
    for(int i= n->start; i<= *(n->end); i++)
        {
            if(text[i]=='#')//remove characters that occur after '#'
            {
                n->end=(int*) malloc(sizeof(int));
                *(n->end)=i;
            }
        }
    n->suffixindex=size-length;
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
    ///first rule extension done by changing end of leaf nodes/////
    lastnew=NULL;
    ////in each new phase initially no node is waiting for a suffix link

    ///now addding suffix remaining using rule 2 or rule 3//////////
    while(remainingsuffix>0)
    {
       if(activelength==0)
       {
        ///rules of activepoint changes
        activeEdge=(int)text[phase];
        }

        if(activenode->child[activeEdge]==NULL)
        {
            //Extension rule  2 will be applicable here as there is no outgoing edge
            activenode->child[activeEdge]=newnode(phase,&leafend);
          //set suffix link if a node is waiting for it to be set
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
          ///internal node created so now we check for suffix link and set if found
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
int doTraversal(node *n, int labelHeight, int* maxHeight,
int* substringStartIndex)
{
    if(n == NULL)
    {
        return -1;
    }
    int i=0;
    int ret = -1;
    if(n->suffixindex < 0) //if it is internal node
    {
        for (i = 0; i < 256; i++)
        {
            if(n->child[i] != NULL)
            {
                ret = doTraversal(n->child[i], labelHeight + edgelength(n->child[i]),  maxHeight, substringStartIndex);
                 
                if(n->suffixindex == -1)//setting initial suffix index after just one of the leaf is traversed
                    {n->suffixindex = ret;}
                else if((n->suffixindex == -2 && ret == -3) || (n->suffixindex == -3 && ret == -2) || n->suffixindex == -4)
                {
                    n->suffixindex = -4;//Mark node as xy as ther are leaves from this node that belong to first and second string
                    //update info of deepest node
                    if(*maxHeight < labelHeight)
                    {
                        *maxHeight = labelHeight;
                        *substringStartIndex = *(n->end)-labelHeight + 1;
                    }
                }
            }
        }
    }
    else if(n->suffixindex > -1 && n->suffixindex < size1)//suffix of x
        return -2;//Mark node as x

    else if(n->suffixindex >= size1)//suffix of Y
        return -3;//Mark node as y

    return n->suffixindex;
}
 
void getLongestCommonSubstring()
{
    int maxHeight = 0;
    int substringStartIndex = 0;
    doTraversal(root, 0, &maxHeight, &substringStartIndex);
     
    int k;
    for (k=0; k<maxHeight; k++)
        printf("%c", text[k + substringStartIndex]);
    if(k == 0)
        printf("No common substring");
    else
        printf(", of length: %d",maxHeight);
    printf("\n");
}
int main()
{
  //an example of this application of finding longest common substring in a text
  //size1 is the length of string+1
  //+1 is to account for special character '#' 
    size1 = 9;
    printf("Longest Common Substring in xabxac and abcabxabcd is: ");
    strcpy(text, "xabxac#abcabxabcd$");
    buildsuffixtree();
    getLongestCommonSubstring();
}