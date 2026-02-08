//Libraries
#include <stdio.h>
#include <stdlib.h>

//Structures definiton
typedef struct NodeStruc {
	int node;
	struct NodeStruc *point;
}C1;


typedef struct ClassStruc {
	int node;
	struct ClassStruc *point1;
	struct NodeStruc *point;
}C2;

//Global variables
int NNodes, ClassNum = 1, *ArrayActiveNodes, WNode, degree, WDegree = 0;
C1 **AdjacencyB, **AdjacencyE, *NodeInClassesPointer, *NotInThisClass, *CNotInThisClass, *CcNotInThisClass, *ActiveNodes, *CActiveNodes, *CcActiveNodes, *Aux1, *Aux2;
C2 *OriginClassesPointer, *ClassesPointer, *LastClassPointer;
	
	
//Functions
void Read(FILE *);
void Add2Adj(int, int, C1 **, C1 **);
void Step1(void);
void Step2(void);

void main(void) {
//Read file
	FILE *fp;
	LastClassPointer = NULL;
	ClassesPointer = NULL;
		fopen_s(&fp, "DSJC125.1.txt", "r");
		if (fp == NULL) {
			printf("\nError:	archivo de datos inv%clido\n", 160);
			exit(8);
		}
	Read(fp);

//Makes a list linking all playing nodes, in the moment all are included
	C1 *ConcreteNode;
	NotInThisClass = (C1 *)malloc(sizeof(C1));
	NotInThisClass->point = NULL;
	NotInThisClass->node = NULL;
	CNotInThisClass = NotInThisClass;
	ActiveNodes = (C1 *)malloc(sizeof(C1));
	CActiveNodes = ActiveNodes;
	CActiveNodes->node = NULL;
	for (int i = 1; i <= NNodes; i++) {
		ConcreteNode = (C1 *)malloc(sizeof(C1));
		ConcreteNode->node = i;
		ConcreteNode->point = NULL;
		ActiveNodes->point = ConcreteNode;
		ActiveNodes=ActiveNodes->point;
	}
	
//Runs function while there are still nodes to acommodate
	while (CActiveNodes->point!=NULL) {
		Step1();
	}
	//Prints the members of each of the final classes
	while (OriginClassesPointer != NULL) {
		printf_s("\n Nodes in class %d \n", OriginClassesPointer->node, " are: ");
		NodeInClassesPointer = OriginClassesPointer->point;
		while (NodeInClassesPointer != NULL) {
			printf_s("%d \n", NodeInClassesPointer->node);
			NodeInClassesPointer = NodeInClassesPointer->point;
		}
		OriginClassesPointer = OriginClassesPointer->point1;
	}
}

//Reads number of nodes first, then couple by couple nodes that have a connection and adds second to adjacency list of first and first to adjacency list of sencond
void Read(FILE *fp) {
	int i, j;
	fscanf_s(fp, "%d", &NNodes);

	AdjacencyB = (C1 **)calloc(NNodes + 1, sizeof(C1*));
	AdjacencyE = (C1 **)calloc(NNodes + 1, sizeof(C1*));

	while (!feof(fp)) {
		fscanf_s(fp, "%d", &i);
		fscanf_s(fp, "%d", &j);
		Add2Adj(i, j, AdjacencyB, AdjacencyE);
		Add2Adj(j, i, AdjacencyB, AdjacencyE);
	}
}

//Same function as in SEQ: Adds second element to adjacency list of first
void Add2Adj(int AdjOfThisNode, int Node2Adj, C1 **AdjacencyB, C1 **AdjacencyE) {
	C1 *ptr;
	ptr = (C1 *)malloc(sizeof(C1));
	ptr->node = Node2Adj;
	ptr->point = NULL;
	if (ptr == NULL) {
		printf("\nMemory error\n");
		exit(8);
	}
	if (AdjacencyB[AdjOfThisNode] == NULL) {
		AdjacencyB[AdjOfThisNode] = ptr;
		AdjacencyE[AdjOfThisNode] = ptr;
	}
	else {
		AdjacencyE[AdjOfThisNode]->point = ptr;
		AdjacencyE[AdjOfThisNode] = ptr;
	}
}

/*This algorithm creates class by class
Creates a class structure wether it is the first one or not, a List with nodes playing, for a playing node, takes its adjacency list and counts the number of 
elements from the playing ones with which this node has adjacency, (there is an Array structure created with 1s whenever a node is still playing and 0s whenever it is not)
decides which one has the largest number of adjacencies and declares a winner, finds it in the list of still playing nodes and unlinks it and links to the class where 
it now belongs, updates the array of 0s and 1s of still playing nodes to 0, then takes its adjacency list and all nodes still playing on this list are passed to AdjOfThisNode, 
the list of nodes no longer being able to participate in this class due to adjacency to some element, and the array of 0s and 1s of still playing nodes is also updated 
to 0*/
void Step1(void) { 
	int z = 0;
	ArrayActiveNodes = (int *)calloc(NNodes+1, sizeof(int));
	C1 * ListOfAdj,*FrontPositionPoint,*BackPositionPoint;
	ClassesPointer = (C2 *)malloc(sizeof(C2));
	ClassesPointer->node = ClassNum;
	ClassesPointer->point1 = NULL;
	ClassesPointer->point = NULL;
	WDegree = 0;
	if (LastClassPointer==NULL) {
		OriginClassesPointer = ClassesPointer;
		LastClassPointer = ClassesPointer;
		ClassNum = 2;
	}
	else {
		while (LastClassPointer->point1!=NULL) {
			LastClassPointer = LastClassPointer->point1;
		}
		LastClassPointer->point1 = ClassesPointer;
		ClassNum = ClassNum + 1;
	}
	for (int i = 1; i <= NNodes; i++) {
		ArrayActiveNodes[i] = 0;
	}
	CcActiveNodes = CActiveNodes;
	while (CcActiveNodes->point != NULL) {
		CcActiveNodes= CcActiveNodes->point;
		ArrayActiveNodes[CcActiveNodes->node] = 1;
	}
	CcActiveNodes = CActiveNodes;
	CcActiveNodes = CcActiveNodes->point;
	while (CcActiveNodes!=NULL) {	
		degree = 0;
		ListOfAdj = AdjacencyB[CcActiveNodes->node];
		while (ListOfAdj!=NULL) {
			if (ArrayActiveNodes[ListOfAdj->node] == 1) {
				degree = degree + 1;
			}
			ListOfAdj = ListOfAdj->point;
		}
		if (degree==0 && z==0) {
			WDegree = degree;
			WNode = CcActiveNodes->node;
			z = z + 1;
		}
		if (degree>WDegree) {
			WDegree = degree;
			WNode = CcActiveNodes->node;
		}
		CcActiveNodes = CcActiveNodes->point;
	}
	FrontPositionPoint = CActiveNodes->point;
	BackPositionPoint = CActiveNodes;
	while (FrontPositionPoint->node!=WNode) {
		FrontPositionPoint = FrontPositionPoint->point;
		BackPositionPoint = BackPositionPoint->point;
	}
	
	ClassesPointer->point = FrontPositionPoint;
	BackPositionPoint->point = FrontPositionPoint->point;
	FrontPositionPoint->point = NULL;
	ArrayActiveNodes[WNode] = 0;
	
	ListOfAdj = AdjacencyB[WNode];
	while (ListOfAdj!=NULL) {
		if (ArrayActiveNodes[ListOfAdj->node] == 1) {
			FrontPositionPoint = CActiveNodes->point;
			BackPositionPoint = CActiveNodes;
			while (FrontPositionPoint->node != ListOfAdj->node) {
				FrontPositionPoint = FrontPositionPoint->point;
				BackPositionPoint = BackPositionPoint->point;
			}
			NodeInClassesPointer = NotInThisClass;
			while (NodeInClassesPointer->point != NULL) {
				NodeInClassesPointer = NodeInClassesPointer->point;
			}
			NodeInClassesPointer->point = FrontPositionPoint;
			BackPositionPoint->point = FrontPositionPoint->point;
			FrontPositionPoint->point = NULL;
			ArrayActiveNodes[ListOfAdj->node] = 0;
		}
		ListOfAdj = ListOfAdj->point;
	}

	while (CActiveNodes->point!=NULL) {
		Step2();
	}
	Aux1 = CNotInThisClass;
	Aux2 = CActiveNodes;
	CActiveNodes = Aux1;
	CNotInThisClass = Aux2;
	ActiveNodes = CActiveNodes;
	NotInThisClass = CNotInThisClass;
}

/*Similar process than the previous function, though the degree of each still participating node is calculated counting with how many of the nodes in AdjOfThisNode (the list of
no longer being able to participate in the current under construction class due to some adjacency list)*/
void Step2(void) {
	int *ArrayNotInThisClass;
	ArrayNotInThisClass = (int *)calloc(NNodes+1, sizeof(int));
	C1 * ListOfAdj, *FrontPositionPoint, *BackPositionPoint;
	WDegree = 0;
	for (int i = 1; i <= NNodes; i++) {
		ArrayNotInThisClass[i] = 0;
	}
	CcNotInThisClass = CNotInThisClass;
	while (CcNotInThisClass->point != NULL) {
		CcNotInThisClass = CcNotInThisClass->point;
		ArrayNotInThisClass[CcNotInThisClass->node] = 1;
	}
	CcActiveNodes = CActiveNodes;
	CcActiveNodes = CcActiveNodes->point;
	while (CcActiveNodes != NULL) {
		degree = 0;
		ListOfAdj = AdjacencyB[CcActiveNodes->node];
		while (ListOfAdj != NULL) {
			if (ArrayNotInThisClass[ListOfAdj->node] == 1) {
				degree = degree + 1;
			}
			ListOfAdj = ListOfAdj->point;
		}
		
		if (degree>WDegree) {
			WDegree = degree;
			WNode = CcActiveNodes->node;
		}
		CcActiveNodes = CcActiveNodes->point;
	}
	if (WDegree == 0) {
		CcActiveNodes = CActiveNodes;
		CcActiveNodes = CcActiveNodes->point;
		WNode = CcActiveNodes->node;
	}
	FrontPositionPoint = CActiveNodes->point;
	BackPositionPoint = CActiveNodes;
	while (FrontPositionPoint->node != WNode) {
		FrontPositionPoint = FrontPositionPoint->point;
		BackPositionPoint = BackPositionPoint->point;
	}
	NodeInClassesPointer = ClassesPointer->point;

	while (NodeInClassesPointer->point != NULL) {
		NodeInClassesPointer = NodeInClassesPointer->point;
	}
	NodeInClassesPointer->point = FrontPositionPoint;
	BackPositionPoint->point = FrontPositionPoint->point;
	FrontPositionPoint->point = NULL;
	ArrayActiveNodes[WNode] = 0;

	ListOfAdj = AdjacencyB[WNode];
	while (ListOfAdj != NULL) {
		if (ArrayActiveNodes[ListOfAdj->node] == 1) {
			FrontPositionPoint = CActiveNodes->point;
			BackPositionPoint = CActiveNodes;
			while (FrontPositionPoint->node != ListOfAdj->node) {
				FrontPositionPoint = FrontPositionPoint->point;
				BackPositionPoint = BackPositionPoint->point;
			}
			NodeInClassesPointer = NotInThisClass;
			while (NodeInClassesPointer->point != NULL) {
				NodeInClassesPointer = NodeInClassesPointer->point;
			}
			NodeInClassesPointer->point = FrontPositionPoint;
			BackPositionPoint->point = FrontPositionPoint->point;
			FrontPositionPoint->point = NULL;
			ArrayActiveNodes[ListOfAdj->node] = 0;
			ArrayNotInThisClass[ListOfAdj->node] = 1;
		}
		ListOfAdj = ListOfAdj->point;
	}
}


	
