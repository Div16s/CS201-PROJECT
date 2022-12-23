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
       { leaf=0;
        setsuffixindexofleaves(n->child[i], length + edgelength(n->child[i]));
       }
    }
  //if current node is a leaf set the suffix index else it will remain -1
   if(leaf==1)
   {
    n->suffixindex=size-length;
   }
 }
 //simple function that modifies active point if activelength is greater than the edge length of current node
 int walkdown(node *n)
 {
  if(activelength>=edgelength(n))
  {
    activeEdge=(int)text[activeEdge+edgelength(n)];//change made
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
//this function does a dfs on tree and returns the starting index and length of deepest rootes internal node
void traverse1(node *n,int height,int *maxheight,int *index)
{
  //Basic dfs on the tree
  if(n == NULL)
  return;
   
   if(n->suffixindex==-1)
   {
    for(int i=0;i<256;i++)
    {
      if(n->child[i] != NULL)
            {
                traverse1(n->child[i], height +  edgelength(n->child[i]), maxheight,index);
            }
    }
   }
   //update maxheight and starting index if we found a repeating substring with more length than current one
   else if(n->suffixindex > -1 &&
                (*maxheight < height - edgelength(n)))
    {
        *maxheight = height - edgelength(n);
        *index = n->suffixindex;
    }
}
void repeat()
{
    int maxheight = 0;
    //index stores the position from where the longest repeated substring starts.
    int index = 0;
    traverse1(root, 0, &maxheight, &index);
    //use traverse function to find the deepest internal nodeand then print the characters
    if(maxheight!=0)
    {
      printf("Longest repeated substring in <%s> is: ", text);
    for (int k=0; k<maxheight; k++)
        {printf("%c", text[k + index]);}
    }
    else
    printf("No repeated substrings found in text <%s>.",text);
    printf("\n");
}
//this function compares the characters on an edge label with the characters of the substring that we want to search
int labelcompare(int index,char *s,int start,int end)
{
  for(int i=start;i<=end && s[index]!='\0';i++,index++)
  {
    /*compare each character on edge lable to the string characters until either we reach null character or encounter a mismatch.*/
    if(text[i]!=s[index])
    {return -1;}
  }
  if(s[index]=='\0')
  return 1;

  else
  return 0;//until now all characters were found on edge but still
  // some characters are left to compare to denote that we return another integer than 1 or -1
}
//function that searches the characters of substring and runs comparison until mismatch is found or we find whole substring successfully
int sequencecheck(char *s,int begin,node *n)
{
  if(n==NULL)
  return 0;//as there is no further character to compare to

    //if n is not a root then run edge label comparison
    if(n->start!=-1)
    {
      int check=labelcompare(begin,s,n->start,*(n->end));
      
      if(check!=0)
      return check;
    }
    //update the index of current character that we are searching/comparing
    begin=begin+edgelength(n);

    if(n->child[s[begin]] != NULL)
        return sequencecheck(s,begin,n->child[s[begin]]);
    else
        return -1; 

}
//this does substring check on texta and prints the result
void subcheck(char *s)
{
   int check = sequencecheck(s,0,root);
   if(check==1)
   {
    printf("<%s> is a substring of text.\n",s);
   }
   else
   {
    printf("<%s> is not a substring of text.\n",s);
   }
}
int main()
{ 
  //some examples of Longest repeated substring application
    strcpy(text, "GEEKSFORGEEKS$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "AAAAAAAAAA$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "ABCDEFG$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "ABABABA$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "ATCGATCGA$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "banana$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "abcpqrabpqpq$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "pqrpqpqabab$");
    buildsuffixtree();   
    repeat();
    freetree(root);
    /************************************************************************************************/
    /*Example of substring check function in linear time*/
    strcpy(text,"this is testcase for substring$");
    buildsuffixtree();
    subcheck("test");
    subcheck("case for");
    subcheck("check");
    subcheck("ubstr");
}