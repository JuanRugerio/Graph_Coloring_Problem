//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//Data Structures
typedef struct NodeStruc {
	int node;
	struct NodeStruc *point2node;
}C1;

typedef struct ClassStruc {
	int class;
	int CountN;
	int EdgesN;
	struct ClassStruc *point2class;
	struct NodeStruc *point2node;
}C2;

//Global Variables
int n, *InitialS, *VecNodesInClass,*VecNodesInAdj, Cmax, randomNodeInClass, Node2MoveVal, PickedClass2Move, Picked2ReceiveClass = 0;
int SizeFromClass, SizeToClass, EdgesFromClass, EdgesToClass;
C1 **Point2First, **Point2Last, *MovingNode;
C2 * StartClass, *AuxPointer2Class, *AuxPointer2Class0, *LastClass1, *FromSet, *ToSet;


//Functions
void Read(FILE *);
void Add(int, int, C1 **, C1 **);
int IRandomClass(int);
void DinamicStruc();
int Edges(C2 *);
int Node(void);
int Cost_Set(void);
void Neighbor(void);
void Link(int, C1 *);
void Dislink(int);
void UpdateClassStructure(void);
int Factibility(void);
void Best_Solution(void);
void Intersection(void);


void main(void) {
	//Variable declaration
	FILE *fp;
	C1 *ptr1;
	int L = 0, Cost = 0, Factible = 0, Best = 0, w = 0;
	int Delta1, Delta2, Delta3, Delta4, edges1_a, edges1_b, edges2_a, edges2_b;
	double T, Probability, r, Delta = 0, Random = 0;
	
	int Verify = 0;
	srand(time(NULL));
	
	//Open file and call function Read
	fopen_s(&fp, "Heuristica1_1.txt", "r");

	if (fp == NULL) {
		printf("\nError:	archivo de datos inv%clido\n", 160);
		exit(8);
	}
	Read(fp);

	Cmax = 100;

	LastClass1 = (C2 *)malloc(sizeof(C2));
	StartClass = (C2 *)malloc(sizeof(C2));
	StartClass = NULL;

	InitialS = (int *)calloc(n + 1, sizeof(int));
	VecNodesInClass = (int *)calloc(n + 1, sizeof(int));
	VecNodesInAdj = (int *)calloc(n + 1, sizeof(int));

	//Assign A random class to each node from 1 to 100 initially
	for (int i = 1; i <= n; i++) {
		InitialS[i] = 0;
		InitialS[i] = IRandomClass(Cmax);
	}

	//Print initial solution
	for (int i = 1; i <= n; i++) {
		printf("%3d\n", InitialS[i]);
	}

	ptr1 = (C1 *)malloc(sizeof(C1));

	//Parameters
	T = 10;
	r = 0.999;
	L = 0;
	randomNodeInClass = 0;
	DinamicStruc(); //Dinamic data structure for classes
	Cost = Cost_Set(); //Defines A cost value
	Factible = Factibility(); //Defines the number of edges and whether there are any for factibility
	w = RAND_MAX;

	//Run A process, as long as the temperature is above A certain threshold
	while (T>0.05) {
		L = 0;
		while (L <= 25) {	//Calls Neighbor, will randomly select A class, randomly pick A node and move it to another class or to A new one
			Neighbor();

			//Delta Calculation: Size relates to number of nodes in A class. Edges 1 or 2 relate to the ones created within A class or that disapear because of some move
			Intersection();
			Delta1 = 2 * SizeFromClass - 1;
			Delta2 = -2 * SizeToClass - 1;
			edges1_a = 2 * (SizeFromClass - 1)*(FromSet->EdgesN - EdgesFromClass);
			edges1_b = 2 * SizeFromClass*FromSet->EdgesN;
			edges2_a = 2 * (SizeToClass + 1)*(ToSet->EdgesN + EdgesToClass); 
			edges2_b = 2 * SizeToClass*ToSet->EdgesN;
			Delta3 = edges1_a - edges1_b;
			Delta4 = edges2_a - edges2_b;
			Delta = Delta1 + Delta2 + Delta3 + Delta4;
			//If the solution is better in Delta terms, moves the node with certainty and updates 
			if (Delta<0) {
				Dislink(PickedClass2Move);//Function to liberate A selected node to be moved from the current class it is located at
				Link(Picked2ReceiveClass, MovingNode); //Function to get it in the destiny class

				UpdateClassStructure();
				Cost = Cost + Delta; //Cost function update
				//Verifies whether the offered solution is A factible one
				Factible = Factibility();
				
				//In case it is and it is A better than the up to now best one, replaces, given it is factible, if the number of classes has been modified, it is updated
				//Verify AND W?
				if (Factible == 0 &&  Cmax<w) {
					Verify = Verify + 1;
					w = Cmax;
					Best_Solution();
				}

			}
			else {
				//If the solution is worse, it still is enabled to make the move with A certain probability
				Probability = exp(-Delta / T); //Probability of move acceptance: more chances to change with larger temperature, less chances to move with greater Delta
				Random = (double)rand() / (RAND_MAX + 1);

				if (Random<=Probability) {//If the probability passed the random number threshold, A change proceeds
					Dislink(PickedClass2Move);
					Link(Picked2ReceiveClass, MovingNode);
					UpdateClassStructure();
					Cost = Cost + Delta;
					Factible = Factibility();
				}
			}
			L = L + 1; //Updates number of iterations run
		}
		T = r*T; //Temperature update
	}

	if (Verify>=1) {
		Cmax = w;

	DinamicStruc();
	int H = 0;
	H = Factibility(); //Whether it is factible
	//Prints nodes within each class
	while (AuxPointer2Class0 != NULL) {
		printf_s("\n Nodes in class %d \n", AuxPointer2Class0->class, " are: ");
		ptr1 = AuxPointer2Class0->point2node;
		while (ptr1 != NULL) {
			printf_s("%d \n", ptr1->node);
			ptr1 = ptr1->point2node;
		}
		printf_s("\n Elements %d \n", AuxPointer2Class0->CountN, " are: "); //Prints number of elements in the class
		printf_s("\n Edges %d \n", AuxPointer2Class0->EdgesN, " are: "); //Prints number of edges in the class

		AuxPointer2Class0 = AuxPointer2Class0->point2class;
	}
	printf_s("\n Factible %d \n", H);
	printf_s("\n Classes %d \n", w);
	printf_s("\n Verify %d \n", Verify);
	}
	else {
		printf_s("\n Error \n");
	}

}

//Read function which reads number of nodes as well as couples of them to, calling the AddAdj function, add members to the proper adjacency lists.
void Read(FILE *fp) {
	int i, j;
	fscanf_s(fp, "%d", &n);

	Point2First = (C1 **)calloc(n + 1, sizeof(C1*));
	Point2Last = (C1 **)calloc(n + 1, sizeof(C1*));

	while (!feof(fp)) {
		fscanf_s(fp, "%d", &i);
		fscanf_s(fp, "%d", &j);
		Add(i, j, Point2First, Point2Last);
		Add(j, i, Point2First, Point2Last);
	}
}

//Function to add first element to adjacency list of second and the other way around
void Add(int u, int v, C1 **Point2First, C1 **Point2Last) {
	C1 *ptr;
	ptr = (C1 *)malloc(sizeof(C1));
	ptr->node = v;
	ptr->point2node = NULL;
	if (ptr == NULL) {
		printf("\nError de memoria\n");
		exit(8);
	}
	if (Point2First[u] == NULL) {
		Point2First[u] = ptr;
		Point2Last[u] = ptr;
	}
	else {
		Point2Last[u]->point2node = ptr;
		Point2Last[u] = ptr;
	}
}


void DinamicStruc() {
	int i = 0;
	int x = 1, E = 1, Count_Edge = 0;

	AuxPointer2Class = (C2 *)malloc(sizeof(C2));
	AuxPointer2Class0 = (C2 *)malloc(sizeof(C2));
	AuxPointer2Class->class = NULL;
	AuxPointer2Class->CountN = NULL;
	AuxPointer2Class->point2node = NULL;
	AuxPointer2Class->point2class = NULL;

	LastClass1 = AuxPointer2Class->point2class;

	while (x <= Cmax) {
		C2 *last;
		last = (C2 *)malloc(sizeof(C2));

		last->class = NULL;
		last->CountN = NULL;
		last->point2node = NULL;
		last->point2class = NULL;

		C1 *aux1 = AuxPointer2Class->point2node;
		aux1 = (C1 *)malloc(sizeof(C1));

		for (i = 1; i <= n; i++) {
			if (InitialS[i] == x) {//If the node was assigned to the currently under construction class
				if (last->point2node != NULL) {//If there are added nodes
				//Adds the node to the class, keeps A pointer to the last element and counts number of elements of the class
					C1 *aux2;
					aux2 = (C1 *)malloc(sizeof(C1));
					aux2->node = i;
					aux1->point2node = aux2;
					aux2->point2node = NULL;
					aux1 = aux1->point2node;
					last->CountN = last->CountN + 1;
				}
				if (last->point2node == NULL) {//If there are no added nodes, simply links the only node in the class and counts 1
				
					C1 *ptr2;
					ptr2 = (C1 *)malloc(sizeof(C1));
					ptr2->node = i;
					ptr2->point2node = NULL;
					aux1 = ptr2;
					last->point2node = aux1;
					last->CountN = 1;
				}
			}

			if (i == n && last->point2node != NULL) {//If last node and there are elements in the class in the end of its creation, write the number of class and increments  
			
				last->class = E; //Increments the number of current classes, for A class to be, it must have at least 1 element
				E = E + 1;
				x = x + 1;
			}
			if (i == n && last->point2node == NULL) {//If last node and no elemnents in the class, just increments the counter to go around all suposedly existing classes
			
				x = x + 1;
			}
		}

		if (last->class == 1) {
			Count_Edge = Edges(last);
			last->EdgesN = Count_Edge;
			LastClass1 = last;
			AuxPointer2Class = LastClass1;
			AuxPointer2Class0 = AuxPointer2Class;
		}

		if (last->class >= 2) {
			Count_Edge = Edges(last);
			last->EdgesN = Count_Edge;
			LastClass1->point2class = last;
			last->point2class = NULL;
			LastClass1 = LastClass1->point2class;
		}
		Count_Edge = 0;
	}

	Cmax = E - 1; //Decrements E in 1, I believe this is since the count started at 1 and in programming typical index is 0?
	//WHY?
}

int IRandomClass(int r) {//Random number within the number of classes

	int random = 0;
	random = rand() % r + 1;
	return random;
}

int Edges(C2 *C) {//Counts the number of edges in the classes initially created 

	C1 *Point2BaseElementInClass, *Point2Next2Base, *AdjOfNodeInClass;
	int i = 0, ToEdges = 0;
	for (i = 1; i <= n; i++) {
		VecNodesInClass[i] = 0;
	}
	Point2BaseElementInClass = C->point2node;

	if (Point2BaseElementInClass == NULL) {
		ToEdges = 0;
	}
	else {
		if (Point2BaseElementInClass->point2node == NULL) {
			ToEdges = 0;
		}
		else {
			while (Point2BaseElementInClass->point2node != NULL) {
				Point2Next2Base = Point2BaseElementInClass->point2node;
				AdjOfNodeInClass = Point2First[Point2BaseElementInClass->node];
				while (Point2Next2Base != NULL) {
					while (AdjOfNodeInClass != NULL) {
						if (AdjOfNodeInClass->node == Point2Next2Base->node) {
							VecNodesInClass[Point2BaseElementInClass->node] = VecNodesInClass[Point2BaseElementInClass->node] + 1;
							break;
						}
						else {
							AdjOfNodeInClass = AdjOfNodeInClass->point2node;
						}
					}
					AdjOfNodeInClass = Point2First[Point2BaseElementInClass->node];
					Point2Next2Base = Point2Next2Base->point2node;
				}
				Point2BaseElementInClass = Point2BaseElementInClass->point2node;
			}
			for (i = 1; i <= n; i++) {
				ToEdges = ToEdges + VecNodesInClass[i];
			}
		}
	}

	return ToEdges;
}

int Node(void) {//Picks A random node number whithin the selected class to be changed, and collects the value

	int count = 1, ValueOfPickedNode = 0;
	StartClass = AuxPointer2Class0;
	C1 *Aux = StartClass->point2node;
	Aux = (C1 *)malloc(sizeof(C1));
	randomNodeInClass = 0;
	while (StartClass != NULL) {
		if (StartClass->class == PickedClass2Move) {
			randomNodeInClass = IRandomClass(StartClass->CountN);
			Aux = StartClass->point2node;
			while (Aux != NULL) {
				if (count == randomNodeInClass) {
					ValueOfPickedNode = Aux->node;
					break;
				}
				else {
					Aux = Aux->point2node;
					count = count + 1;
				}
			}
			break;
		}
		else
		{
			StartClass = StartClass->point2class;
		}
	}
	return(ValueOfPickedNode);
}

int Cost_Set(void) {//Cost calculation

	int CostCalc = 0, CA = 0, ED = 0;
	StartClass = AuxPointer2Class0;
	while (StartClass != NULL) {
		CA = CA + (StartClass->CountN)*(StartClass->CountN);
		ED = ED + (StartClass->CountN)*(StartClass->EdgesN);
		StartClass = StartClass->point2class;
	}
	CostCalc = -CA + 2 * ED;

	return CostCalc;
}

void Neighbor(void) {//Figures the selected node, randomly picks A class between the existing plus one, with the chance to create A new one. And provides both classes
//structures with their node count

	PickedClass2Move = 0;
	Node2MoveVal = 0;
	MovingNode = (C1 *)malloc(sizeof(C1));
	MovingNode->node = NULL;
	MovingNode->point2node = NULL;
	PickedClass2Move = IRandomClass(Cmax);
	Node2MoveVal = Node();
	MovingNode->node = Node2MoveVal;
	MovingNode->point2node = NULL;
	FromSet = (C2 *)malloc(sizeof(C2));
	ToSet = (C2 *)malloc(sizeof(C2));

	Picked2ReceiveClass = IRandomClass(Cmax + 1);

	while (Picked2ReceiveClass == PickedClass2Move) {
		Picked2ReceiveClass = IRandomClass(Cmax + 1);
	}

	StartClass = AuxPointer2Class0;
	while (StartClass!=NULL) {
		if (StartClass->class == PickedClass2Move) {
			FromSet = StartClass;
			SizeFromClass = StartClass->CountN;
			break;
		}
		else {
			StartClass = StartClass->point2class;
		}
	}
	
	if (Picked2ReceiveClass<=Cmax) {
		StartClass = AuxPointer2Class0;
		while (StartClass != NULL) {
			if (StartClass->class == Picked2ReceiveClass) {
				ToSet = StartClass;
				SizeToClass = StartClass->CountN;
				break;
			}
			else {
				StartClass = StartClass->point2class;
			}
		}
	}
	else {
		ToSet->CountN =0 ; 
		ToSet->EdgesN = 0;
		ToSet->point2node = NULL;
		ToSet->point2class = NULL;
		SizeToClass = 0;
	}
	
}

void Dislink(int ValueClass) {//Searches node to take out of its current class. Finds the class, and the dislinks it from it, updates count, and EdgesN number

	C1 *Aux1, *Aux2;

	Aux1 = (C1 *)malloc(sizeof(C1));
	Aux2 = (C1 *)malloc(sizeof(C1));
	StartClass = AuxPointer2Class0;

	while (StartClass != NULL) {
		if (StartClass->class == ValueClass) {
			Aux1 = StartClass->point2node;
			if (Aux1->point2node != NULL) {
				Aux2 = Aux1->point2node;
				while (Aux2 != NULL) {
					if (Aux1->node == Node2MoveVal)  {
						StartClass->point2node = Aux2;
						break;
					}
					if (Aux2->node == Node2MoveVal) {
						Aux1->point2node = Aux2->point2node;
						break;
					}
					else {
						Aux1 = Aux1->point2node;
						Aux2 = Aux2->point2node;
					}
				}
			}
			else {
				StartClass->point2node = NULL;
			}
			///Actualizaciones
			StartClass->CountN = StartClass->CountN - 1;
			StartClass->EdgesN = Edges(StartClass);
			break;
		}
		else {
			StartClass = StartClass->point2class;
		}
	}
}

void Link(int ValueClass, C1 * MovingNode) {//Gets node within the destinity class, whether it already exists or not

	C1 *Aux1, *Aux2;

	Aux1 = (C1 *)malloc(sizeof(C1));
	Aux2 = (C1 *)malloc(sizeof(C1));
	StartClass = AuxPointer2Class0;

	if (ValueClass == Cmax + 1) {
		while (StartClass->point2class != NULL) {
			StartClass = StartClass->point2class;
		}
		C2 *last;
		last = (C2 *)malloc(sizeof(C2));
		last->point2node = MovingNode;
		last->class = Cmax + 1;
		last->CountN = 1;
		last->EdgesN = 0;
		last->point2class = NULL;
		StartClass->point2class = last;
		Cmax = Cmax + 1;
	}
	else {
		while (StartClass != NULL) {
			if (StartClass->class == ValueClass) {
				Aux1 = StartClass->point2node;
				if (Aux1 == NULL) {
					StartClass->point2node = MovingNode; 
					StartClass->CountN = 1;
					StartClass->EdgesN = 0;
					break;
				}
				else {
					MovingNode->point2node = Aux1;
					StartClass->point2node = MovingNode;
					StartClass->CountN = StartClass->CountN + 1;
					StartClass->EdgesN = Edges(StartClass);
					break;
				}
			}
			else {
				StartClass = StartClass->point2class;
			}
		}
	}

}

void UpdateClassStructure(void) {//Given the case that there was A class without nodes it eliminates it and updates the numeration of classes properly

	int E = 1;
	C2 * Aux3;
	Aux3 = (C2 *)malloc(sizeof(C2));

	StartClass = AuxPointer2Class0;

	while (StartClass != NULL) {
		if (StartClass->point2class != NULL) {
			Aux3 = StartClass->point2class;
		}
		if (StartClass->point2node == NULL && E == 1) {
			AuxPointer2Class0 = StartClass->point2class;
		}

		if (Aux3->point2node == NULL) {
			StartClass->point2class = Aux3->point2class;
			Aux3 = StartClass->point2class;
		}

		if (StartClass->point2node != NULL) {
			StartClass->class = E;
			E = E + 1;
		}

		StartClass = StartClass->point2class;
	}

	Cmax = E - 1;
}

int Factibility(void) {//Number of connected nodes within existing classes

	int SumOfEdges = 0;
	StartClass = AuxPointer2Class0;
	while (StartClass != NULL) {
		SumOfEdges = SumOfEdges + StartClass->EdgesN; 
		StartClass = StartClass->point2class;
	}
	return (SumOfEdges);
}

void Best_Solution(void) {//Remember best found up to now solution, to which class each node is currently assigned
	C1 *Aux;

	Aux = (C1 *)malloc(sizeof(C1));
	StartClass = AuxPointer2Class0;

	while (StartClass != NULL) {
		Aux = StartClass->point2node;
		while (Aux != NULL) {
			InitialS[Aux->node] = StartClass->class;
			Aux = Aux->point2node;
		}
		StartClass = StartClass->point2class;
	}
}

void Intersection(void) {//For the node which will be moved, counts how many connections it has in the current class and in the one to which it is to be moved
//VecNodesInClass has all nodes which are present in the current class of the node to be moved, Aux1 all the nodes whith which it has adjacency. There is an EdgesN in the class
//when both are 1, then it updates the first array to be about the class to which it is to be moved and those same trick to count
	C1 *Aux;

	EdgesFromClass = 0;
	EdgesToClass = 0;
	Aux = (C1 *)malloc(sizeof(C1));

	for (int i = 1; i <=n; i++) {
		VecNodesInClass[i] = 0;
		VecNodesInAdj[i] = 0;
	}
	
	Aux = FromSet->point2node;

	while (Aux!=NULL) {
		VecNodesInClass[Aux->node] = 1;
		Aux = Aux->point2node;
	}

	Aux = Point2First[Node2MoveVal]; 

	while (Aux!=NULL) {
		VecNodesInAdj[Aux->node] = 1;
		Aux = Aux->point2node;
	}

	for (int i = 1; i <=n; i++) {
		if (VecNodesInClass[i] ==1 && VecNodesInAdj[i]==1) {
			EdgesFromClass = EdgesFromClass + 1;
		}
	}

	if (Picked2ReceiveClass<=Cmax) {
		for (int i = 1; i <= n; i++) {
			VecNodesInClass[i] = 0;
		}

		Aux = ToSet->point2node;

		while (Aux != NULL) {
			VecNodesInClass[Aux->node] = 1;
			Aux = Aux->point2node;
		}

		for (int i = 1; i <= n; i++) {
			if (VecNodesInClass[i] ==1 && VecNodesInAdj[i]==1) {
				EdgesToClass = EdgesToClass + 1;
			}
		}
	}
	else {
		EdgesToClass = 0;
		ToSet->EdgesN = 0;
	}
}



