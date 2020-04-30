#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define SIZE 10

typedef struct Item 
{
	struct ItemFile *ItemFile;
	struct Info *info; // указатель на элемент
	struct Item *next; // указатель на следующий элемет в таблице
}Item;

typedef struct ItemFile
{
	int key; // ключ элемента
	int setItem; // смещение элемента таблицы в файле относительно начала
	int next_setItem; // смещение следующего элемента таблицы в файле относительно начала
}ItemFile;

typedef struct Info 
{
	struct InfoFile *InfoFile;
	struct Info *next; // указатель на следующий элемент в списке
}Info;

typedef struct InfoFile
{
	int len; // длина информации
	int offset; // смещение в файле
	int release; // версия элемента
	int setInfo; // смещение элемента Info в файле
}InfoFile;

typedef struct Table
{
	FILE *fd;
	char *fName;
	struct Item *tab[SIZE];
}Table;

int hash(int);
int ftruncate(int fildes, off_t length);
int dialog(int N);
int getint(int *a);
char *getstr();
void init(Table *table);
int add(Table *table, int key, char* str);
int delete(Table *table, int key, int rel);
void find_by_key(Table *table, int key);
void find_by_key_release(Table *table, int key, int rel);
void print_table(Table *table);
void clean(Table *table);
void d_add();
void d_delete();
void d_find_by_key();
void d_find_by_key_release();
void Read_from_file(Table *table);
void Write_table_to_file(Table *table);
const char *msgs[] = { "0. Quit",  // объявляем массив указателй на элементы таблицы
					  "1. Add in table",
					  "2. Find by key",
					  "3. Find by key and release",
					  "4. Delete by key and release",
					  "5. Show table"
};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

size_t LinePos = 0, end = 0;

Table *table;

int main()
{
	init(&table); // инициализация таблицы
	int rc;
	Read_from_file(&table);
	while (rc = dialog(NMsgs)) // вызов пользовательского меню
	{
		if (rc == 1) // 1 - Добавление элмента
			d_add(&table);
		if (rc == 2)
			d_find_by_key(&table); // 2 - Поиск элементов по ключу
		if (rc == 3)
			d_find_by_key_release(&table); // 3 - Поиск элемента по ключу и версии
		if (rc == 4)
			d_delete(&table); // 4 - Удаление элемента из таблицы
		if (rc == 5)
			print_table(&table); // 5- Печать таблицы
	}
	Write_table_to_file(&table);
	clean(&table);
	printf("That's all. Bye!\n"); // Завершение работы
	return 0;
}

int hash(int x) // хэш-функция
{
	return x % SIZE;
}

int dialog(int N) {  // диалоговая функция
	char *errmsg = "";
	int rc;
	int i, n;
	do 
	{
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

int getint(int *a) // функция ввода числа
{
	int n;
	do 
	{
		n = scanf("%d", a);
		if (n == 0 || a < 0) 
		{
			scanf("%*[^\n]");
			printf("Error! That's not number\n"); // если вводится не число
		}
	} while (n == 0 || a < 0);
	scanf("%*c");
	return n < 0 ? 0 : 1;
}

char *getstr() // функция безопасного получения строки 
{ 
	char *ptr = (char *)malloc(1);  // строка
	char buf[100]; // буфер для получения из выходного потока
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
		else 
		{
			len += strlen(buf);
			ptr = (char *)realloc(ptr, len + 1); // добавляем память 
			strcat(ptr, buf);
		}
	} while (n > 0);
	return ptr;
}

void init(Table *table) // инициализация таблицы
{
	for (int i = 0; i < SIZE; ++i) 
	{
		table->tab[i] = NULL;
	}
}

void Read_from_file(Table *table)
{
	int i;
	Item *curItem = NULL;
	Info *curInfo = NULL;
	printf("Enter file name: ");
	table->fName = getstr();
	table->fd = fopen(table->fName, "r+b");
	if (table->fd == NULL)
	{
		table->fd = fopen(table->fName, "w+b");
		if (table->fd)
			fwrite(&LinePos, sizeof(int), 2, table->fd);
	}
	else
	{
		fread(&LinePos, sizeof(int), 1, table->fd);
		fread(&end, sizeof(int), 1, table->fd);
		fseek(table->fd, LinePos, SEEK_SET);
		int m = 0;
		Info *PrevInfo = NULL;
		Item* PrevItem = NULL, *tItem = NULL;
		ItemFile tmpItemFile = {NULL, NULL, NULL};
		ItemFile* curItemFile;
		Item tmpItem = { 0, 0, 0 }; // вспомогательная переменная для считывания из файла элемента
		InfoFile tmpInfoFile = {0, 0, 0, 0};
		Info tmpInfo = { NULL, NULL }; // для считывания Info
		Item* countItem = NULL;
		do
		{
			fread(&tmpItemFile, sizeof(ItemFile), 1, table->fd);
			curItem = (Item*)malloc(sizeof(Item));
			curItem->next = NULL;
			curItem->ItemFile = calloc(3, sizeof(ItemFile));
			*curItem->ItemFile = tmpItemFile;
			m = hash(curItem->ItemFile->key);
			if (table->tab[m] == NULL)
				table->tab[m] = curItem;
			else 
			if (hash(curItem->ItemFile->key) == hash(table->tab[m]->ItemFile->key) && (curItem->ItemFile->key != table->tab[m]->ItemFile->key))
			{
				countItem = table->tab[m];
				while (countItem->next)
					countItem = countItem->next;
				countItem->next = curItem;
				curItem->next = NULL;
			}
			tItem = table->tab[m];
			if (tItem->next)
				tItem = tItem->next;
			Info* count = NULL;
			do
			{
				fread(&tmpInfoFile, sizeof(InfoFile), 1, table->fd);
				curInfo = (Info*)malloc(sizeof(Info));
				curInfo->InfoFile = calloc(3, sizeof(InfoFile));
				*curInfo->InfoFile = tmpInfoFile;
				curInfo->next = NULL;
				if (count)
				{
					count = tItem->info;
					while (count->next)
						count = count->next;
					count->next = curInfo;
					curInfo->next = NULL;
				}
				else
				{
					count = curInfo;
					tItem->info = count;
				}
				size_t posInfo = ftell(table->fd);
				PrevInfo = table->tab[m]->info;
				if (curItem->ItemFile->next_setItem == posInfo)
					break;
			} while (1);
			size_t posItem;
			posItem = ftell(table->fd);
			if (posItem == end)
				break;
			else
				fseek(table->fd, posItem, SEEK_SET);
		} while (1);
	}
	fclose(table->fd);
}

void Write_table_to_file(Table *table)
{
	Info *curInfo = NULL;
	Item *curItem = NULL;
	int checkend = 0;
	size_t pos = 0, setpos = 0;
	if (table->fName)
	{
		table->fd = fopen(table->fName, "r+b");
		if (table->fd)
		{
			fseek(table->fd, LinePos, SEEK_SET);
			for (int i = 0; i < SIZE; ++i)
			{
				curItem = table->tab[i];
				if (curItem)
				{
					checkend++;
					while (curItem)
					{
						if (setpos)
							fseek(table->fd, setpos, SEEK_SET);
						curItem->ItemFile->setItem = ftell(table->fd);
						curItem->ItemFile->next_setItem = NULL;
						fwrite(curItem->ItemFile, sizeof(ItemFile), 1, table->fd);
						curInfo = curItem->info;
						while (curInfo)
						{
							pos = ftell(table->fd);
							curInfo->InfoFile->setInfo = pos;
							fwrite(curInfo->InfoFile, sizeof(InfoFile), 1, table->fd);
							curInfo = curInfo->next;
						}
						pos = ftell(table->fd);
						curItem->ItemFile->next_setItem = setpos = pos;
						end = ftell(table->fd);
						fseek(table->fd, curItem->ItemFile->setItem, SEEK_SET);
						fwrite(curItem->ItemFile, sizeof(ItemFile), 1, table->fd);
						curItem = curItem->next;
					}
				}
			}
		}
		rewind(table->fd);
		fwrite(&LinePos, sizeof(int), 1, table->fd);
		fwrite(&end, sizeof(int), 1, table->fd);
		if (end)
			ftruncate(fileno(table->fd), end);
		if (!checkend)
		{
			int k = remove(table->fName);
			printf("%d", k);
		}
		printf("Str %d \n", LinePos);
	}
	fclose(table->fd);
}

int add(Table *table, int key, char* str) // функция добавления элемента в таблицу
{
	int h = hash(key);
	Item *node, *tmpNode = NULL;
	table->fd = fopen(table->fName, "r+b");
	if (table->tab[h] == NULL)
	{
		node = malloc(sizeof(Item));
		node->ItemFile = (ItemFile*)malloc(sizeof(ItemFile));
		node->info = (Info*)malloc(sizeof(Info));
		node->info->InfoFile = (InfoFile*)malloc(sizeof(InfoFile));
		table->tab[h] = node;
		node->ItemFile->key = key;
		node->info->InfoFile->release = 1;
		node->info->next = NULL;
		node->next = NULL;
		if (table->fName)
		{
			if (table->fd)
			{
				fseek(table->fd, 0, SEEK_END);
				if (LinePos)
					fseek(table->fd, LinePos, SEEK_SET);
				node->info->InfoFile->offset = ftell(table->fd);
				node->info->InfoFile->len = strlen(str) + 1;
				node->info->InfoFile->release = 1;
				fwrite(str, sizeof(char), strlen(str) + 1, table->fd);
				LinePos = ftell(table->fd);
				if (tmpNode != NULL)
					tmpNode->next = node;
				else
					table->tab[h] = node;
				printf("Element added");
			}
			else 
				printf("Open file error");
		}
		else 
			printf("Name not found");
	fclose(table->fd);
	}
	else
	{
		node = table->tab[h];
		for (; node; node = node->next)
		{
			if (node->ItemFile->key == key)
			{
				Info *tmp = (Info*)malloc(sizeof(Info));
				tmp->InfoFile = (InfoFile*)malloc(sizeof(InfoFile));
				int rel = node->info->InfoFile->release;
				rel++;
				tmp->InfoFile->release = rel;
				if (table->fName)
				{
					if (table->fd)
					{
						fseek(table->fd, 0, SEEK_END);
						if (LinePos)
							fseek(table->fd, LinePos, SEEK_SET);
						tmp->InfoFile->offset = ftell(table->fd);
						tmp->InfoFile->len = strlen(str) + 1;
						fwrite(str, sizeof(char), tmp->InfoFile->len, table->fd);
						LinePos = ftell(table->fd);
						fclose(table->fd);
						tmp->next = node->info;
						node->info = tmp;
						return 0;
					}
				}
			}
			if ((h == hash(table->tab[h]->ItemFile->key)) && (key != table->tab[h]->ItemFile->key))
			{
				Item *tmp = (Item*)malloc(sizeof(Item));
				tmp->ItemFile = (ItemFile*)malloc(sizeof(ItemFile));
				tmp->info = (Info*)malloc(sizeof(Info));
				tmp->info->InfoFile = (InfoFile*)malloc(sizeof(InfoFile));
				tmp->info->InfoFile->release = 1;

				if (table->fName)
				{
					if (table->fd)
					{
						fseek(table->fd, 0, SEEK_END);
						if (LinePos)
							fseek(table->fd, LinePos, SEEK_SET);
						tmp->info->InfoFile->offset = ftell(table->fd);
						tmp->info->InfoFile->len = strlen(str) + 1;
						fwrite(str, sizeof(char), tmp->info->InfoFile->len, table->fd);
						LinePos = ftell(table->fd);
						fclose(table->fd);
						tmp->ItemFile->key = key;
						tmp->info->next = NULL;
						tmp->next = table->tab[h];
						table->tab[h] = tmp;
						return 0;
					}
				}
			}
		}
	}
}

int delete(Table *table, int key, int rel)
{
	Item *Node, *tmpItem, *NodePrev, *NodeNext;
	Info *tmpPrev, *tmpNext, *tmp, *tmpInfo;
	int h = hash(key);
	Node = table->tab[h];
	table->fd = fopen(table->fName, "r+b");
	if (table->tab[h] == NULL)
		printf("The element to delete was not found\n");
	else 
	{
		int k = 0;
		for (NodePrev = NULL; Node; Node = NodeNext)
		{
			NodeNext = Node->next;
			if (Node->ItemFile->key == key)
			{
				for (tmpPrev = NULL, tmp = Node->info; tmp; tmp = tmpNext)
				{
					tmpNext = tmp->next;
					if (tmp->InfoFile->release == rel)
					{
						k++;
						if (NodePrev == NULL)
						{
							if (tmpPrev == NULL)
							{
								if (tmp->next == NULL)
								{
									Node->info = Node->info->next;
									free(tmp->InfoFile);
									free(tmp);
									if (table->tab[h]->info == NULL && table->tab[h]->next == NULL)
									{
										free(Node->ItemFile);
										free(Node);
										table->tab[h] = NULL;
									}
									else
									{
										table->tab[h] = table->tab[h]->next;
										free(Node->ItemFile);
										free(Node);
									}
									return 0;
								}
								else
								{
									Node->info = tmp->next;
									free(tmp->InfoFile);
									free(tmp);
									return 0;
								}
							}
							else
							{
								Node->info->next = tmp->next;
								tmpPrev->next = tmpNext;
								free(tmp->InfoFile);
								free(tmp);
								return 0;
							}
						}
						else
						{
							if (tmpPrev == NULL)
							{
								if (tmp->next == NULL)
								{
									NodePrev->next = Node->next;
									free(Node->info->InfoFile);
									free(Node->info);
									free(Node->ItemFile);
									free(Node);
								}
								else
								{
									Node->info = tmp->next;
									tmp->next = tmpNext;
									free(tmp->InfoFile);
									free(tmp);
								}
							}
							else
							{
								Node->info->next = tmp->next;
								tmpPrev->next = tmpNext;
								free(tmp->InfoFile);
								free(tmp);
								return 0;
							}
						}
					}
					tmpPrev = tmp;
				}
			}
			NodePrev = Node;
		}
		if (k == 0)
			printf("The element to delete was not found\n");
	}
	fclose(table->fd);
	return 0;
}

void find_by_key(Table *table, int key) // функция поиска элементов по ключу
{                                        // выводит все версии
	int h = hash(key);
	Item *Node;
	Node = table->tab[h];
	table->fd = fopen(table->fName, "r+b");
	if (table->tab[h] == NULL) 
	{
		printf("There is no such key in table \n"); // если хэша по такому ключу нет в таблице
		return 0;
	}
	else 
	{
		int k = 0;
		while (Node)
		{
			if (Node->ItemFile->key == key) 
			{
				Info *tmp = Node->info;
				while (tmp)
				{
					fseek(table->fd, tmp->InfoFile->offset, SEEK_SET);
					int  size = tmp->InfoFile->len;
					printf("[%d] '", tmp->InfoFile->release);
					while (size--)
					{
						char*c = getc(table->fd);
						printf("%c", c);
					}
					printf("'");
					k++;
					tmp = tmp->next;
				}
			}
			Node = Node->next;
		}
		if (k == 0)
			printf("There is no such key in table\n"); // если такого ключа нет в таблице
	}
	fclose(table->fd);
	return 0;
}

void find_by_key_release(Table *table, int key, int rel) // функция поиска элемента в таблице по ключу и версии
{
	int h = hash(key);
	Item *tmp, *Node;
	Node = table->tab[h];
	table->fd = fopen(table->fName, "r+b");
	if (table->tab[h] == NULL) 
	{
		printf("There is no such key in table\n");
		return 0;
	}
	else 
	{
		int k = 0;
		while (Node)
		{
			if (Node->ItemFile->key == key) 
			{
				Info *tmp = Node->info;
				while (tmp)
				{
					if (tmp->InfoFile->release == rel) 
					{
						printf("+\n");
						k++;
						fseek(table->fd, tmp->InfoFile->offset, SEEK_SET);
						int  size = tmp->InfoFile->len;
						printf("'");
						while (size--)
						{
							char*c = getc(table->fd);
							printf("%c", c);
						}
						printf("'");
					}
					tmp = tmp->next;
				}
			}
			Node = Node->next;
		}
		if (k == 0)
			printf("There is no such release for this key\n"); // если найден ключ, но не найдена версия
	}
	fclose(table->fd);
}

void d_add() // диалоговая функция добавления элемента
{
	int k, n;
	char *info;
	printf("Enter key: ");
	n = getint(&k);
	printf("Enter info: ");
	info = getstr();
	add(&table, k, info);
	free(info);
}

void d_delete() // диалоговая функция удаления элемента
{
	int n, k;
	int r, rel;
	printf("Enter key: ");
	n = getint(&k);
	printf("Enter release: ");
	r = getint(&rel);
	delete(&table, k, rel);
}
void d_find_by_key() // диалоговая функция поиска элемента по ключу
{
	int n, k;
	printf("Enter key: ");
	n = getint(&k);
	find_by_key(&table, k);
}

void d_find_by_key_release() // диалоговая функция поиска по ключу и версии
{
	int n, k;
	int r, rel;
	printf("Enter key: ");
	n = getint(&k);
	printf("Enter release: ");
	r = getint(&rel);
	find_by_key_release(&table, k, rel);
}

void print_table(Table *table) // функция печати таблицы
{
	Item *tmpItem;
	table->fd = fopen(table->fName, "r+b");
	for (int i = 0; i < SIZE; ++i)
	{
		tmpItem = table->tab[i];
		if (tmpItem)
		{
			for (; tmpItem != NULL; tmpItem = tmpItem->next)
			{
				Info *tmpInfo = tmpItem->info;
				while (tmpInfo != NULL)
				{
					fseek(table->fd, tmpInfo->InfoFile->offset, SEEK_SET);
					int size = tmpInfo->InfoFile->len;
					printf("[%d] [%d] '", tmpItem->ItemFile->key, tmpInfo->InfoFile->release);
					while (size--)
					{
						char *c = getc(table->fd);
						printf("%c", c);
					}
					printf("' ");
					tmpInfo = tmpInfo->next;
				}
				printf("\n");
			}
		}
	}
	fclose(table->fd);
}

void clean(Table *table) // функция очистки таблицы
{
	Info *tmpInfo, *trashInfo;
	Item *tmpItem, *tmp, *prevItem = NULL, *trash;
	for (int i = 0; i < SIZE; ++i)
	{
		tmp = table->tab[i];
		if (tmp)
		{
			while (tmp)
			{
				tmpInfo = tmp->info;
				while (tmpInfo)
				{
					trashInfo = tmpInfo;
					tmpInfo = tmpInfo->next;
					free(trashInfo->InfoFile);
					free(trashInfo);
				}
				trash = tmp;
				tmp = tmp->next;
				free(trash->ItemFile);
				free(trash);
			}
		}
	}
	free(table->fName);
}