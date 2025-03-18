#include <stdio.h>
#include <stdlib.h>

//Data structures definition
typedef struct NodeStructure {
	int node;
	struct NodeStructure *pointerToNode;
}P;

typedef struct ClassStructure {
	int node;
	struct ClassStructure *pointerToClass;
	struct NodeStructure *pointerToNode2;
}Pq;

//Functions definition
void Read(FILE *);
void Add2Adj(int, int, P **, P **);
int ThroughAdj(int node, int nodeClass);
int ThroughClass(int node, int class);
void ThroughClassArray(int);

//Variable definition
int OriginalNode;
P **ArrayToFirst;
P **ArrayToLast;
Pq *P2ClassStruc;
Pq *P2fstClassStruc;
P *P2NodeStruc;
int ClassNumerator = 2;

//Main begins
void main(void) {
    //Variable declaration
	int i;
	P *ptr2Node;	
	FILE *fp;
	
    //Reads file
	fopen_s(&fp, "Heuristica1_5.txt", "r");
	if (fp == NULL) {
		printf("\nError: archivo de datos inv%clido\n", 160);
		exit(8);
	}
	Read(fp);
    //Prints final adjacency list of each node
	for (i = 0; i<=OriginalNode; i++) {
		ptr2Node = ArrayToFirst[i];
		printf("\nAdjacency list from node %d:", i);
		while (ptr2Node != NULL) {
			printf("%d ", ptr2Node->node);
			ptr2Node = ptr2Node->pointerToNode;
		}
		printf("\n");
	}
    //Creates skeleton with second structure for the first color, and includes the first node to it in a first structure kind
	P2ClassStruc = (Pq *)malloc(sizeof(Pq));
	P2NodeStruc = (P *)malloc(sizeof(P));
	P2NodeStruc->node = 1; 
	P2NodeStruc->pointerToNode = NULL;
	P2ClassStruc->pointerToClass = NULL;
	P2ClassStruc->pointerToNode2 = P2NodeStruc;
	P2ClassStruc->node = 1; 
	P2fstClassStruc = P2ClassStruc;
	
    //Passes all node values to function, calling it
	for (int ñ = 2; ñ <= OriginalNode; ñ++) {
		ThroughClassArray(ñ);
	}

    //Prints nodes pertaining to each color class
	while (P2fstClassStruc!=NULL)
	{
		printf_s("\n Nodes in class %d \n",P2fstClassStruc->node, " are: ");
		P2NodeStruc = P2fstClassStruc->pointerToNode2;
		while (P2NodeStruc!=NULL)
		{
			printf_s(  "%d \n",P2NodeStruc->node);
			P2NodeStruc = P2NodeStruc->pointerToNode;
		}
		P2fstClassStruc = P2fstClassStruc->pointerToClass;
	}
}

/*Reads the received file, reads pairs of values from the file, passing them in original and also reverse order, together with 2 pointers to arrays of pointers of 1st 
structure kind*/
void Read(FILE *fp) {
	int i, j;
	
	fscanf_s(fp, "%d", &OriginalNode);

	ArrayToFirst = (P **)calloc(OriginalNode+1, sizeof(P*));
	ArrayToLast = (P **)calloc(OriginalNode+1, sizeof(P*));

	while (!feof(fp)) {
		fscanf_s(fp, "%d", &i);
		fscanf_s(fp, "%d", &j);
		Add2Adj(i, j, ArrayToFirst, ArrayToLast);
		Add2Adj(j, i, ArrayToFirst, ArrayToLast);
	}
}

//Adds the structure kind 1 with the 2nd node value to the adjacency list of the 1st node, either if it is till now empty, or not
void Add2Adj(int AdjListOf, int AddNode2Adj, P **ArrayToFirst, P **ArrayToLast) {
	P *ptr;
	ptr = (P *)malloc(sizeof(P));
	ptr->node = AddNode2Adj;
	ptr->pointerToNode = NULL;
	if (ptr == NULL) {
		printf("\nMemory error\n");
		exit(8);
	}
	if (ArrayToFirst[AdjListOf] == NULL) {
		ArrayToFirst[AdjListOf] = ptr;
		ArrayToLast[AdjListOf] = ptr;
	}
	else {
		ArrayToLast[AdjListOf]->pointerToNode = ptr;
		ArrayToLast[AdjListOf] = ptr;
	}
}

/*There is the node to accomodate in a class, and one node currently studied in the currently studied class: Determines whether there is connection with this last node, 
or not, 1 means, no problem by currently studied node in the class, for the new node to be integrate, 2 means there is a problem*/
int  ThroughAdj(int node, int nodeClass) {

	int i=0,CurrentNode;
	P *NodeInAdjList;

	if (ArrayToFirst[node]==NULL)
	{
		return i = 1;
	}

	NodeInAdjList = ArrayToFirst[node];
	CurrentNode = ArrayToFirst[node]->node;

	while (i == 0)
	{
		if (nodeClass < CurrentNode)
		{
			i = 1;
		}
		if (nodeClass == CurrentNode)
		{
			i = 2;
		}

		if (nodeClass > CurrentNode)
		{
			if (NodeInAdjList->pointerToNode == NULL)
			{
				i = 1;
				break;
			}

			NodeInAdjList = NodeInAdjList->pointerToNode;
			CurrentNode = NodeInAdjList->node;
			
		}

	}
    
    return i;
}

/*Iterates over the member nodes of a studied class, using RecorreA to determine whether there is a problem with any of them, resultThroughClass 2 means node cannot be accomodated in 
current class, and resultThroughClass 1 means it can be*/
int  ThroughClass(int node, int class) {
	int j = 0;
	int resultThroughClass = 0; 
	int resultThroughAdj; 
	P *CurrentNodeInClass;

	CurrentNodeInClass = P2ClassStruc->pointerToNode2;

	while (resultThroughClass==0){
		resultThroughAdj=ThroughAdj(node, CurrentNodeInClass->node);
		if (resultThroughAdj==2){
			resultThroughClass = 2; 
		} else if (resultThroughAdj == 1) {
			if (CurrentNodeInClass->pointerToNode==NULL) {
				resultThroughClass = 1;
			} else {
				CurrentNodeInClass = CurrentNodeInClass->pointerToNode;
			}
		}
    }

	return resultThroughClass;
}

/*Accomodate a node in a class, there are cases: Creates a new class and includes if the node could not be included in any to now created class, if it can be included
in one of the classes, it acommodates it in*/
void ThroughClassArray(int i) {
	P2ClassStruc = P2fstClassStruc;
	int RThroughClass=0,ResultAcommodateNode=0;
	P *AuxPoint2Node = P2ClassStruc->pointerToNode2;

	while (ResultAcommodateNode==0) {
		RThroughClass = ThroughClass(i, P2ClassStruc->node);

		if (RThroughClass == 2) {
			if (P2ClassStruc->pointerToClass==NULL) {
				Pq *ClassStruc; 
				ClassStruc = (Pq *)malloc(sizeof(Pq));
				P *NodeStruc; 
				NodeStruc = (P *)malloc(sizeof(P));
				NodeStruc->node = i; 
				NodeStruc->pointerToNode = NULL;
				ClassStruc->pointerToNode2 = NodeStruc;
				ClassStruc->node = ClassNumerator; 
				ClassStruc->pointerToClass = NULL;
				P2ClassStruc->pointerToClass = ClassStruc;
				ClassNumerator = ClassNumerator + 1;
				ResultAcommodateNode = 1;
			} else 
                P2ClassStruc = P2ClassStruc->pointerToClass;
		} else if (RThroughClass==1) {
			P *NodeStruc; 
			NodeStruc = (P *)malloc(sizeof(P));
			AuxPoint2Node = P2ClassStruc->pointerToNode2;
			while (AuxPoint2Node->pointerToNode!=NULL) {
				AuxPoint2Node = AuxPoint2Node->pointerToNode;
			}
			NodeStruc->node = i;
			AuxPoint2Node->pointerToNode = NodeStruc;
			NodeStruc->pointerToNode = NULL;
			ResultAcommodateNode = 1;
		}
	}
}



