#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Node
{
	char* key;
	char* info;
	struct Node* left;
	struct Node* right;
	struct Node* par;
}Node;

int dialog(int N);
int getint(int* a);
char* getstr();
void d_add(Node* root);
int add(Node** root, char* key, char* info);
Node* addNode(char* key, char* info, Node* parent);
void d_from_file(Node* root);
char* getstr_from_file(FILE* file);
void d_find_by_key_N(Node* root);
Node* find_by_key(Node** root, char* key);
void find_by_key_N(Node** root, char* key, int n);
void delete(Node** root, char* key);
void d_delete(Node* root);
void freemem(Node* root);
void d_print(Node* root);
Node* print_for_key(Node** root, char* key);
void d_print_tree(Node* root, int p, int s);
void showLine(char* c, int p, int s);
void print(Node* root);

const char* msgs[] = //реализованные функции
{
	"0. Quit",
	"1. Add in tree",
	"2. Add tree from file",
	"3. Find by key",
	"4. Find by key and N",
	"5. Delete by key",
	"6. Timing",
	"7. Show table by key",
	"8. Show tree"
};
int m = 0;
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

char* getstr() { //функция безопасного получения строки
	char* ptr = (char*)malloc(1);  //строка
	char buf[100]; //буфер для получения из входного потока
	int n = 0;
	unsigned int len = 0; //len - длина строки
	*ptr = '\0';
	do {
		n = scanf("%99[^\n]", buf);
		if (n < 0) 
		{
			free(ptr);
			ptr = NULL;
			continue;
		}
		if (n == 0)
			scanf("%*c");
		else {
			len += strlen(buf);
			ptr = (char*)realloc(ptr, len + 1); // добавляем память
			strcat(ptr, buf);
		}
	} while (n > 0);
	return ptr;
}


void d_add(Node* root) // диалоговая функция добавления элемента в дерево
{
	char* key;
	char* info;
	printf("Enter key: ");
	key = getstr();
	printf("Enter info: ");
	info = getstr();
	if (add(root, key, info) == 2)
		printf("Node already exist\n");
	return;
}

Node* addNode(char* key, char* info, Node* parent) // функция создания вершины
{
	Node* tmp = (Node*)malloc(sizeof(Node));
	tmp->left = tmp->right = NULL;
	tmp->info = info;
	tmp->key = key;
	tmp->par = parent;
	return tmp;
}
int add(Node** root, char* key, char* info) // функция добавления элемента в дерево
{
	int k = 0;
	Node* tmp = NULL, * ins = NULL;
	if (*root == NULL) // если дерево пустое, создаём корень
	{
		*root = addNode(key, info, NULL);
		return 0;
	}
	else
	{
		tmp = *root;
		while (tmp)
		{
			if ((k = strcmp(tmp->key, key)) < 0) // если ключ больше ключа текущей вершины 
			{
				if (tmp->right)
				{
					tmp = tmp->right;
					continue;
				}
				else
				{
					tmp->right = addNode(key, info, tmp);
					return 0;
				}
			}
			else if ((k = strcmp(tmp->key, key)) > 0) // если ключ меньше ключа текущей вершины
			{
				if (tmp->left)
				{
					tmp = tmp->left;
					continue;
				}
				else
				{
					tmp->left = addNode(key, info, tmp);
					return 0;
				}
			}
			else
			{
				return 2; // если ключ нашёлся такой же ключ
			}
		}

	}
}
char* getstr_from_file(FILE* file)
{
	char* ptr = (char*)malloc(1);  //строка
	char buf[100]; //буфер для получения из входного потока
	int n = 0, len = 0; //len - длина строки
	*ptr = '\0';
	do {
		n = fscanf(file, "%99[^\n]", buf);
		if (n < 0) {
			free(ptr);
			ptr = NULL;
			continue;
		}
		if (n == 0)
			fscanf(file, "%*c");
		else {
			len += strlen(buf);
			ptr = (char*)realloc(ptr, len + 1); // добавляем память
			strcat(ptr, buf);
		}
	} while (n > 0);
	return ptr;
}
void d_from_file(Node* root) // диалоговая функция добавления из файла
{
	FILE* file;
	char* filename;
	char* key = NULL, * info;
	char c = NULL;
	printf("Enter filename: ");
	filename = getstr();
	file = fopen(filename, "r");
	free(filename);
	if (!file)
	{
		printf("Unable to open file\n");
		return;
	}
	do
	{
		key = getstr_from_file(file);
		info = getstr_from_file(file);
		if (!key)
		{
			free(key);
			break;
		}
		else
		{
			add(root, key, info);
		}

	} while (1);
	fclose(file);
}

Node* find_by_key(Node** root, char* key) // функция поиска элемента по ключу
{
	int k;
	Node* tmp;
	tmp = *root;
	while (tmp)
	{
		if ((k = strcmp(tmp->key, key)) < 0)
		{
			tmp = tmp->right;
			continue;
		}
		else if ((k = strcmp(tmp->key, key)) > 0)
		{
			tmp = tmp->left;
			continue;
		}
		else if ((k = strcmp(tmp->key, key)) == 0)
			return tmp;
	}
	return NULL;

}

void d_find_by_key(Node* root) // диалоговая функция поиска по ключу
{
	char* key;
	printf("Enter key: ");
	key = getstr();
	Node* cur = find_by_key(root, key);
	if (!cur)
		printf("Key not found\n");
	else
	{
		printf("Key: %s Info: %s", cur->key, cur->info);
		free(cur->info);
		free(cur->key);
		free(cur);
	}
	free(key);
}

void d_find_by_key_N(Node* root) // диалоговая функция поиска по N символам
{
	int k = 0, n = 0;
	char* key;
	printf("Enter number of first symbols: ");
	k = getint(&n);
	printf("Enter first n symbols of key: ");
	key = getstr();
	if (strlen(key) != n)
	{
		printf("Key doesnt match with length\n");
		free(key);
		return;
	}
	find_by_key_N(root, key, n);
	free(key);
}

int findDetected = 0;

void find_by_key_N(Node** root, char* key, int n) // функция поиска по N символам
{
	Node* tmp;
	int k = 0;
	tmp = *root;
	if (tmp)
	{
		char* erease_key = calloc(n, (n + 1) * sizeof(char));
		strncat(erease_key, tmp->key, n);
		if ((k = strcmp(erease_key, key)) == 0)
		{
			printf("Key: %s Info: %s\n", tmp->key, tmp->info);
			findDetected++;
		}
		find_by_key_N(&tmp->left, key, n);
		find_by_key_N(&tmp->right, key, n);
		free(erease_key);
	}
	return;
}
Node* minNode(Node* root) // функция поиска минимального элемента в поддереве
{
	while (root->left)
		root = root->left;
	return root;
}
Node* maxNode(Node* root) // функция поиска максимального элемента в поддереве
{
	while (root->right)
		root = root->right;
	return root;
}

Node* nextElem(Node** root) // функция поиска следующего элемента
{
	Node* cur = root, * ppar = NULL;
	if (cur->right)
		return minNode(cur->right);
	ppar = cur->par;
	while (ppar && (cur == ppar->right))
	{
		cur = ppar;
		ppar = ppar->par;
	}
	return ppar;
}

void del(Node** root, char* key) // функция удаления элемента
{
	Node* cur = NULL, * pred = NULL;
	cur = find_by_key(root, key); // поиск удаляемого элемента
	if (!cur->par && !cur->right && !cur->left)
	{
		free(cur->info);
		free(cur->key);
		free(cur);
		cur = NULL;
		*root = NULL;
		return;
	}
	if (!cur)
		return;
	if (!cur->par) // если удаляется корень 
	{
		if (cur->right) // если есть правое поддерево
		{
			pred = minNode(cur->right);
			Node* tmp = pred->par;
			free(cur->info);
			free(cur->key);
			cur->info = pred->info;
			cur->key = pred->key;
			tmp->left = pred->right;
			if (pred->right)
				pred->right->par = tmp;
			free(pred);
		}
		else // если есть только левое
		{
			pred = maxNode(cur->left);
			Node* tmp = pred->par;
			free(cur->info);
			free(cur->key);
			cur->info = pred->info;
			cur->key = pred->key;
			tmp->right = pred->left;
			if (pred->left)
				pred->left->par = tmp;
			if (!cur->left->left && !cur->left->right)
				cur->left = NULL;
			free(pred);
		}
	}
	else // если удаляется не корень
	{
		if (!cur->left && !cur->right) // если у элемента нет потомков
		{
			pred = cur->par;
			if (cur == pred->right)
				pred->right = NULL;
			else
				pred->left = NULL;
			free(cur->key);
			free(cur->info);
			free(cur);
		}
		else if (!cur->left && cur->right) // если у элемента потомок справа
		{
			pred = cur->par;
			if (cur == pred->right)
			{
				pred->right = cur->right;
				pred->right->par = cur->par;
			}
			else
			{
				pred->left = cur->right;
				pred->left->par = cur->par;
			}
			free(cur->info);
			free(cur->key);
			free(cur);

		}
		else if (cur->left && !(cur->right)) // если у элемента потомок слева
		{
			pred = cur->par;
			if (cur == pred->right)
			{
				pred->right = cur->left;
				pred->right->par = cur->par;
			}
			else
			{
				pred->left = cur->left;
				pred->left->par = cur->par;
			}
			free(cur->info);
			free(cur->key);
			free(cur);
		}
		else if (cur->left && cur->right) // если у элемента оба потомка
		{
			if (!cur->right->left)
			{
				free(cur->info);
				free(cur->key);
				cur->info = cur->right->info;
				cur->key = cur->right->key;
				if (cur->right->right)
				{
					pred = cur->right->right;
					pred->par = cur;
				}
				free(cur->right);
				cur->right = pred;
			}
			else
			{
				pred = minNode(cur->right);
				free(cur->info);
				free(cur->key);
				cur->info = pred->info;
				cur->key = pred->key;
				Node* tmp = pred->par;
				free(pred);
				tmp->left = NULL;
			}
		}
	}
	return;
}

void d_delete(Node* root) // диалоговая функция удаления элемента
{
	char* key;
	printf("Enter key: ");
	key = getstr();
	del(root, key);
	free(key);
}

void freemem(Node* root) // очистка всего дерева
{
	if (root)
	{
		freemem(root->left);
		freemem(root->right);
		free(root->key);
		free(root->info);
		free(root);
	}
}

void d_print(Node* root) // диалоговая функция вывода в порядке ключей
{
	char* key;
	printf("Enter key: ");
	key = getstr();
	print_for_key(root, key);
	free(key);
}

Node* print_for_key(Node** root, char* key) // функция вывода в порядке ключей 
{
	Node* cur = NULL;
	cur = minNode(*root);
	if (strlen(key) == 0)
	{
		cur = minNode(*root);
		//cur = nextElem(cur);
	}
	else
		cur = find_by_key(root, key);
	while (cur && (*root != maxNode(cur)))
	{
		printf("Key: %s Info: %s\n", cur->key, cur->info);
		cur = nextElem(cur);
	}
	return;

}

int d_timing() // функция таймирования 
{
	Node* root = NULL;
	int n = 10, cnt = 1000000, i, m;
	clock_t first, last;
	srand(time(NULL));
	char* k;
	char* info = '\0';
	char* key[10000];
	while (n-- > 0)
	{
		for (i = 0; i < 10000; ++i) // создаём массив ключей 
		{
			int len = rand() % 20 + 1;
			key[i] = (char*)malloc((len + 1) * sizeof(char));
			for (int j = 0; j < len; ++j)
			{
				key[i][j] = rand() % 56 + 65;
			}
			key[i][len] = '\0';
		}
		for (i = 0; i < cnt;) // создаём ключ и добавляем его и информацию в дерево
		{
			int len = rand() % 20 + 1;
			k = (char*)malloc((len + 1) * sizeof(char));
			for (int j = 0; j < len; ++j)
			{
				k[j] = rand() % 56 + 65;
			}
			k[len] = '\0';
			int count = 0;
			count = add(&root, k, info);
			if (!count)
				++i;
			if (count == 2) // если ключ не вставляется в дерево, нужно очистить память
			{
				free(k);
				free(info);
			}
		}
		m = 0;
		first = clock();
		for (i = 0; i < 10000; ++i)
			if (find_by_key(&root, key[i])) // поиск элементов
				++m;
		last = clock();
		for (i = 0; i < 10000; ++i) // очистка памяти массива ключей
			free(key[i]);
		printf("%d items was found\n", m);
		printf("test #%d, number of nodes = %d, time = %d\n", 10 - n, (10 - n) * cnt, last - first);
	}
	freemem(root);

	return 1;
}
void d_print_tree(Node* root, int p, int s) // функция печати дерева в виде дерева
{
	int i;
	if (root == NULL)
		return;
	printf("%s %s\n", root->key, root->info);
	if (root->left)
	{
		showLine("|\n", p, s);
		showLine("L: ", p, s);
		d_print_tree(root->left, p + 1, s + ((root->right == NULL ? 0 : 1) << p));
	}
	if (root->right)
	{
		showLine("|\n", p, s);
		showLine("R: ", p, s);
		d_print_tree(root->right, p + 1, s);
	}
}

void print(Node* root) // функция печати элементов дерева
{
	if (root != NULL)
	{
		print(root->left);
		printf("%10d %10d", root->info, root->key);
		print(root->right);
	}
}

void showLine(char* c, int p, int s) // функция печати вспомогательных линий 
{
	int t = s, i; 
	for (i = 0; i < p; i++) 
	{ 
		printf(t & 1 ? "|  " : "   "); t /= 2; 
	} 
	printf(c);
}

int main(void)
{
	Node* root = NULL;
	int rc, p = 0, s = 0;
	while (rc = dialog(NMsgs)) // вызов пользовательского меню
	{
		if (rc == 1) // добавление элемента
			d_add(&root);
		if (rc == 2)
			d_from_file(&root); // добавление дерева из файла
		if (rc == 3)
			d_find_by_key(&root); // поиск по ключу
		if (rc == 4)
		{
			d_find_by_key_N(&root); // поиск по первым N элементам
			if (!findDetected)
				printf("No elements with this paremetrs\n");
		}
		if (rc == 5)
			d_delete(&root); // удаление элемента из дерева
		if (rc == 7)
			d_print(&root); // вывод таблицы в порядке кючей
		if (rc == 6)
			d_timing(); // таймирования
		if (rc == 8) 
		{
			printf("\nTree: \n");
			d_print_tree(root, p, s); // вывод дерева в виде дерева
		}
	}
	freemem(root); // очистка дерева
	printf("That's all. Bye!\n");
	return 0;
}