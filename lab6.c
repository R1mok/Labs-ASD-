#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>


const int INF = 100000000;
typedef struct AdjList
{
	struct node* node; // указатель на вершину
	struct AdjNode* adjnode; // указатель на смежные вершины
	struct AdjList* next; // указатель на следующую вершину
}AdjList;

typedef struct AdjNode
{
	struct node* node; // указатель на вершину
	int weight; // вес между вершинами
	struct AdjNode* next; // следующий и предыдущий элеметы в списке
}AdjNode;

typedef struct node
{
	int name; // имя вершины
	int x, y; // координаты вершины
	int offset; // смещение в файле
	int id; // id вершины
}node;

AdjList* add_node(AdjList* graph, int name, int x, int y);
AdjList* delete_node(AdjList* graph, int n);
AdjList* add_edge(AdjList* graph, int f, int s);
AdjList* random_graph(AdjList* graph, int n, int e);

const char* msgs[] = //реализованные функции
{
	"0. Quit",
	"1. Read graph from file",
	"2. Add node",
	"3. Add edge",
	"4. Remove node",
	"5. Find shortest way",
	"6. Print adjlist",
	"7. Create graph",
	"8. Timing",
	"9. Print graph"
};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int dialog(int N) {  // диалоговая функция
	char* errmsg = "";
	int rc;
	int i, n;
	do {
		puts(errmsg);
		errmsg = "You are wrong. Repeat, please!";
		for (i = 0; i < N; ++i)
			puts(msgs[i]);
		printf("Make your choice: ");
		n = getint(&rc);
		if (n == 0)
			rc = 0;
	} while (rc < 0 || rc >= N);
	return rc;
}

int getint(int* a) // функция ввода числа
{
	int n;
	do {
		n = scanf("%d", a);
		if (n == 0 || a < 0) {
			scanf("%*[^\n]");
			printf("Error! That's not number\n"); // если вводится не число
		}
	} while (n == 0 || a < 0);
	scanf("%*c");
	return n < 0 ? 0 : 1;
}

char* getstr() // функция безопасного получения строки 
{
	char* ptr = (char*)malloc(1);  // строка
	char buf[100]; // буфер для получения из выходного потока
	int n = 0;
	unsigned int len = 0; //len - длина строки
	*ptr = '\0';
	do
	{
		n = scanf("%99[^\n]", buf);
		if (n < 0)
		{
			free(ptr);
			ptr = NULL;
			continue;
		}
		if (n == 0)
			scanf("%*c");
		else
		{
			len += strlen(buf);
			ptr = (char*)realloc(ptr, len + 1); // добавляем память 
			strcat(ptr, buf);
		}
	} while (n > 0);
	return ptr;
}

AdjList* graph;
int dupl = 0;

AdjList* add_node(AdjList* graph, int name, int x, int y)
{
	AdjList* cur, * gr = graph;
	cur = malloc(sizeof(AdjList));
	cur->node = malloc(sizeof(node));
	while (gr)
	{
		if (gr->node->name == name)
			dupl = 1;
		gr = gr->next;
	}
	if (dupl == 0)
	{
		gr = graph;
		cur->node->name = name;
		cur->node->x = x;
		cur->node->y = y;
		cur->next = NULL;
		cur->adjnode = NULL;
		if (!gr)
			graph = cur;
		else
		{
			while (gr->next)
				gr = gr->next;
			gr->next = cur;
		}
	}
	else
	{
		free(cur->node);
		free(cur);
	}

	return graph;
}

int weight(int x1, int y1, int x2, int y2)
{
	int w;
	w = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) + 0.5;
	return w;
}

AdjList* add_edge(AdjList* graph, int f, int s)
{
	if (f == s)
	{
		dupl = 1;
		return graph;
	}
	AdjList* gr = graph;
	AdjNode* ANode, * ANodeTmp;
	node* tmpNode = NULL;
	dupl = 3;
	if (gr)
	while (gr)
	{
		if (gr->node->name == s)
		{
			dupl = 2;
			tmpNode = gr->node;
		}
		gr = gr->next;
	}
	if (dupl == 3)
	{
		return graph;
	}
	gr = graph;
	dupl = 3;
	while (gr)
	{
		if (gr->node->name == f)
			dupl = 2;
		gr = gr->next;
	}
	if (dupl == 3)
	{
		return graph;
	}
	dupl = 0;
	gr = graph;
	while (gr)
	{
		if (gr->node->name == f)
		{
			ANode = gr->adjnode;
			while (ANode)
			{
				if (ANode->node->name == s)
					dupl = 1;
				ANode = ANode->next;
			}
			ANode = gr->adjnode;
			if (dupl == 0)
			{
				if (ANode)
				{
					ANodeTmp = malloc(sizeof(AdjNode));
					ANodeTmp->node = tmpNode;
					ANodeTmp->weight = weight(tmpNode->x, tmpNode->y, gr->node->x, gr->node->y);
					ANodeTmp->next = ANode;
					gr->adjnode = ANodeTmp;
				}
				else
				{
					ANode = malloc(sizeof(AdjNode));
					ANode->node = tmpNode;
					ANode->next = NULL;
					ANode->weight = weight(tmpNode->x, tmpNode->y, gr->node->x, gr->node->y);
					gr->adjnode = ANode;
				}
			}
		}
		gr = gr->next;
	}
	return graph;
}
AdjList* d_add_node(AdjList* graph)
{
	int x, y, name;
	printf("Enter name: ");
	getint(&name);
	printf("Enter x and y: ");
	getint(&x);
	getint(&y);
	graph = add_node(graph, name, x, y);
	if (dupl == 1)
	{
		printf("Node already exist\n");
		dupl = 0;
	}
	return graph;
}

AdjList* d_add_edge(AdjList* graph)
{
	int f, s;
	printf("Enter first node: ");
	getint(&f);
	printf("Enter second node: ");
	getint(&s);
	graph = add_edge(graph, f, s);
	if (dupl == 1)
	{
		printf("Edge already exist\n");
		dupl = 0;
	}
	if (dupl == 3)
	{
		printf("Node not found\n");
		dupl = 0;
	}
	return graph;
}
AdjList* d_random_graph(AdjList* graph)
{
	int n = 0, e = 0;
	printf("Enter number of nodes: ");
	getint(&n);
	printf("Enter number of edges: ");
	getint(&e);
	if (e > (n * (n - 1)))
	{
		printf("Error! Can't create graph");
		return graph;
	}
	graph = random_graph(graph, n, e);
	return graph;
}

AdjList* random_graph(AdjList* graph, int n, int e)
{
	srand(time(NULL));
	AdjList* gr;
	for (int i = 0; i < n; ++i)
	{
		int name, x, y;
		name = rand() % 10000;
		x = rand() % 1000;
		y = rand() % 1000;
		graph = add_node(graph, name, x, y);
		if (dupl == 1)
		{
			dupl = 0;
			--i;
		}
	}
	for (int i = 0; i < e; ++i)
	{
		gr = graph;
		int f, s, fname, sname;
		f = rand() % n;
		for (int i = 0; i < f; ++i)
			gr = gr->next;
		fname = gr->node->name;
		s = rand() % n;
		gr = graph;
		for (int i = 0; i < s; ++i)
			gr = gr->next;
		sname = gr->node->name;
		graph = add_edge(graph, fname, sname);
		if (dupl == 1)
		{
			dupl = 0;
			--i;
		}
	}
	return graph;
}

AdjList* print_adjlist(AdjList* graph)
{
	AdjList* gr = graph;
	AdjNode* tmpAdjNode;
	while (gr)
	{
		tmpAdjNode = gr->adjnode;
		int i = 0;
		printf("%d -> ", gr->node->name);
		if (tmpAdjNode)
		{
			while (tmpAdjNode)
			{
				i++;
				printf("%d) %d w:%d ", i, tmpAdjNode->node->name, tmpAdjNode->weight);
				tmpAdjNode = tmpAdjNode->next;
			}
			printf("\n");
		}
		else
			printf("0\n");
		gr = gr->next;
	}
	return graph;
}

int no_found = 0;

AdjList* d_delete_node(AdjList* graph)
{
	int n;
	printf("Enter removing node: ");
	getint(&n);
	graph = delete_node(graph, n);
	if (!no_found)
		printf("Removable node not found\n");
	no_found = 0;
	return graph;
}

AdjList* delete_node(AdjList* graph, int n)
{
	int k = 0;
	AdjList* gr = graph, * prevList, * nextList, * tmpList;
	AdjNode* ANode, * nextAdjNode, * prevAdjNode;
	while (gr)
	{
		AdjNode* tmpAdjNode = gr->adjnode;
		for (prevAdjNode = NULL; tmpAdjNode; tmpAdjNode = nextAdjNode)
		{
			nextAdjNode = tmpAdjNode->next;
			if (tmpAdjNode->node->name == n)
			{
				if (!prevAdjNode)
				{
					gr->adjnode = nextAdjNode;
					free(tmpAdjNode);
				}
				else
				{
					prevAdjNode->next = nextAdjNode;
					free(tmpAdjNode);
				}
				break;
			}
			prevAdjNode = tmpAdjNode;
		}
		gr = gr->next;
	}
	tmpList = graph;
	for (prevList = NULL; tmpList; tmpList = nextList)
	{
		AdjNode* tmpAdjNode = tmpList->adjnode, * curAdjNode, * prevAdjNode, * nextAdjNode;
		nextList = tmpList->next;
		if (tmpList->node->name == n)
		{
			no_found++;
			for (; tmpAdjNode; tmpAdjNode = nextAdjNode)
			{
				nextAdjNode = tmpAdjNode->next;
				free(tmpAdjNode);
				tmpList->adjnode = nextAdjNode;
			}
			if (!prevList)
			{
				graph = nextList;
				free(tmpList->node);
				free(tmpList);
			}
			else
			{
				prevList->next = nextList;
				free(tmpList->node);
				free(tmpList);
			}
			break;
		}
		prevList = tmpList;
	}
	return graph;
}

void delete_graph(AdjList* graph)
{
	AdjList* gr = graph;
	AdjNode* tmpAdjNode, * nextAdjNode;
	while (gr)
	{
		tmpAdjNode = gr->adjnode;
		for (; tmpAdjNode; tmpAdjNode = nextAdjNode)
		{
			nextAdjNode = tmpAdjNode->next;
			free(tmpAdjNode);
			gr->adjnode = nextAdjNode;
		}
		gr = gr->next;
	}
	AdjList* tmp, * grNext;
	gr = graph;
	for (; gr; gr = grNext)
	{
		grNext = gr->next;
		free(gr->node);
		free(gr);
	}
}
FILE* fd = NULL;
char* fName = "file1";
int ncount = 0;

void print_to_file(AdjList* graph)
{
	AdjList* gr = graph;
	fd = fopen(fName, "r+b");
	if (!fd)
	{
		fd = fopen(fName, "w+b");
	}
	fwrite(&ncount, sizeof(int), 1, fd);
	while (gr)
	{
		ncount++;
		gr->node->offset = 0;
		fwrite(gr->node, sizeof(int), 4, fd);
		gr = gr->next;
	}
	gr = graph;
	while (gr)
	{
		AdjNode* ANode = gr->adjnode;
		while (ANode)
		{
			int name = ANode->node->name;
			fwrite(&name, sizeof(int), 1, fd);
			ANode = ANode->next;
		}

		if (gr->adjnode)
			gr->node->offset = ftell(fd);

		gr = gr->next;
	}
	int endOffset = ftell(fd);
	fseek(fd, sizeof(int), SEEK_SET);
	gr = graph;
	while (gr)
	{
		fwrite(gr->node, sizeof(int), 4, fd);
		gr = gr->next;
	}
	ftruncate(fileno(fd), endOffset); // обрезание файла
	rewind(fd);
	fwrite(&ncount, sizeof(int), 1, fd);
	fclose(fd);
	return;
}

AdjList* read_from_file()
{
	if (graph != NULL)
		return graph;
	AdjList* graph = NULL, * tmpNode, * gr = NULL;
	node tmpAList = { 0, 0, 0 };

	int count = 0;
	fd = fopen(fName, "r+b");
	if (!fd)
	{
		fd = fopen(fName, "w+b");
	}
	fread(&count, sizeof(int), 1, fd);
	while (count--)
	{
		fread(&tmpAList, sizeof(int), 4, fd);
		tmpNode = malloc(sizeof(AdjList));
		tmpNode->next = NULL;
		tmpNode->node = malloc(sizeof(node));
		*tmpNode->node = tmpAList;
		if (!graph)
		{
			gr = tmpNode;
			gr->adjnode = NULL;
			graph = gr;
		}
		else
		{
			gr->next = tmpNode;
			gr = gr->next;
			gr->adjnode = NULL;
		}
	}
	gr = graph;
	AdjList* tmpGr = graph;
	AdjNode* ANode;
	node tmpANode = { 0, 0, 0, 0 };
	int name = 0, curOffset = 0;
	while (gr)
	{
		ANode = gr->adjnode;
		int offset = gr->node->offset;
		if (offset)
		{
			do
			{
				fread(&name, sizeof(int), 1, fd);
				add_edge(graph, gr->node->name, name);
				if (ANode)
					ANode = ANode->next;
				curOffset = ftell(fd);
			} while (offset != curOffset);
			fseek(fd, offset, SEEK_SET);
		}
		gr = gr->next;
	}
	fclose(fd);
	return graph;
}

int flag = 0, pathFlag = 0, minPath = 0;

AdjList* Dijkstra(AdjList* graph, int f, int s)
{
	AdjList* gr = graph;
	int n = 0, i = 0, minindex, min = 0, tmp, fi = -1, e = -1, w = 0, j = 0;
	int* d, * u, * prev;
	while (gr)
	{
		gr->node->id = i++;
		if (gr->node->name == f)
			fi = gr->node->id;
		if (gr->node->name == s)
			e = gr->node->id;
		++n;
		gr = gr->next;
	}
	if (fi == -1 || e == -1)
	{
		flag = 1;
		return graph;
	}
	gr = graph;
	d = malloc(sizeof(int) * n);
	u = malloc(sizeof(int) * n);
	prev = malloc(sizeof(int) * n);
	for (int i = 0; gr; gr = gr->next, i++)
	{
		d[i] = INF;
		u[i] = 0;
	}
	for (int i = 0; i < n; ++i)
		prev[i] = -1;
	d[fi] = 0;
	minindex = fi;
	while (min < INF)
	{
		i = minindex;
		u[i] = 1;
		gr = graph;
		while (gr)
		{
			if (gr->node->id == i)
			{
				j = 0;
				AdjNode* ANode = gr->adjnode;
				while (ANode)
				{
					w = ANode->weight;
					if (d[i] + w < d[ANode->node->id])
					{
						d[ANode->node->id] = d[i] + w;
						prev[ANode->node->id] = i;
					}
					ANode = ANode->next;
				}
			}
			gr = gr->next;
		}
		min = INF;
		for (int i = 0; i < n; ++i)
		{
			if (!u[i] && d[i] < min)
			{
				min = d[i];
				minindex = i;
			}
		}
	}
	int si = 0;


	if (d[e] == INF)
		pathFlag = 1;
	else
		minPath = d[e];

	//---------------------------------------------------
	if (d[e] != INF)
	{
		int k = 0;
		int* path = malloc(sizeof(int) * n);
		for (int i = 0; i < n; ++i)
			path[i] = -1;
		j = e;
		while (j != -1)
		{
			path[k] = j;
			j = prev[j];
			k++;
		}
		gr = graph;
		for (int i = n-1; i >= 0; --i)
		{
			if (path[i] >= 0)
			{
				while (gr)
				{
					if (path[i] == gr->node->id)
						printf("%d ", gr->node->name);
					gr = gr->next;
				}
			}
			gr = graph;
		}
		printf("\n");
		free(path);
	}
	// ---------------------------------------------------


	free(prev);
	free(d);
	free(u);

	return graph;
}


AdjList* d_Dijkstra(AdjList* graph)
{
	int f, s;
	printf("Enter first node: ");
	getint(&f);
	printf("Enter second node: ");
	getint(&s);
	graph = Dijkstra(graph, f, s);
	if (flag)
	{
		printf("Node not found\n");
		flag = 0;
	}
	else
		if (pathFlag)
		{
			printf("Path not exist\n");
			pathFlag = 0;
		}
		else
		{
			printf("%d\n", minPath);
			minPath = 0;
		}

	return graph;
}
void d_timing()
{
	AdjList* graph = NULL;
	srand(time(NULL));
	long long  n = 10, nodes = 100, edges = 2000, m = 2, first, last;
	while (n--)
	{
		graph = random_graph(graph, nodes, edges);
		first = clock();
		graph = Dijkstra(graph, graph->node->name, graph->next->node->name);
		last = clock();
		printf("test #%lld, number of nodes = %lld, number of edges = %lld, time = %lld\n", 10 - n, nodes, edges, last - first);
		nodes = 100 * (11-n);
		//edges = 1000 * (11-n) * (11-n);
		delete_graph(graph);
		graph = NULL;
	}
	return;
}

#define P_LEN 40
void draw_graph(AdjList* graph)
{
	AdjList* g = graph;
	Agraph_t *pic;
	GVC_t* gvc;
	gvc = gvContext();
	int k = 0;
	pic = agopen("G", Agstrictdirected, NULL);
	Agsym_t  *atr;
	while (g)
	{
		g->node->id = k;
		g = g->next;
		k++;
	}
	g = graph;
	Agnode_t **agn;
	agn = (Agnode_t **)malloc(sizeof(Agnode_t) * k);
	AdjList* tmp;
	Agedge_t *edge;
	char s[P_LEN];
	int i = 0;
	while (g)
	{
		sprintf(s, "%d: %d,%d", g->node->name, g->node->x, g->node->y);
		agn[g->node->id] = agnode(pic, s, TRUE);
		sprintf(s, "%d,%d", g->node->x, g->node->y);
		g = g->next;
	}
	g = graph;
	while (g)
	{
		AdjNode* ANode = g->adjnode;
		while (ANode)
		{
			sprintf(s, "%d", ANode->weight);
			edge = agedge(pic, agn[g->node->id], agn[ANode->node->id], NULL, TRUE);
			agattr(pic, 2, (char*)"label", (char*)"0");
			agset(edge, (char*)"label", s);
			ANode = ANode->next;
		}
		g = g->next;
	}
	gvLayout(gvc, pic, "dot");
	gvRenderFilename(gvc, pic, "png", "out.png");
	gvFreeLayout(gvc, pic);
	agclose(pic);
}

int main(void)
{
	int rc;
	while (rc = dialog(NMsgs))
	{
		if (rc == 1)
			graph = read_from_file();
		if (rc == 2)
			graph = d_add_node(graph);
		if (rc == 3)
			graph = d_add_edge(graph);
		if (rc == 4)
			graph = d_delete_node(graph);
		if (rc == 5)
			graph = d_Dijkstra(graph);
		if (rc == 7)
			graph = d_random_graph(graph);
		if (rc == 6)
			graph = print_adjlist(graph);
		if (rc == 8)
			d_timing();
		if (rc == 9)
			draw_graph(graph);
	}
	print_to_file(graph);
	delete_graph(graph);
	return 0;
}