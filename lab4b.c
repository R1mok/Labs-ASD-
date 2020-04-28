//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 10

typedef struct Item {
	int key; // ключ элемента
	int setItem; // смещение элемента таблицы в файле относительно начала
	int next_setItem; // смещение следующего элемента таблицы в файле относительно начала
	struct Info *info; // указатель на элемент
	struct Item *next; // указатель на следующий элемет в таблице
}Item;

typedef struct Info {
	int len; // длина информации
	int offset; // смещение в файле
	int release; // версия элемента
	int setInfo; // смещение элемента Info в файле
	int next_setInfo; // смещение следующего элемента Info в файле
	struct Info *next; // указатель на следующий элемент в списке
}Info;

typedef struct Table
{
	FILE *fd;
	char *fName;
	struct Item *tab[SIZE];
}Table;

int hash(int);
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
	scanf("%*c");
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

int getint(int *a) // функция ввода числа
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

char *getstr() // функция безопасного получения строки 
{ 
	char *ptr = (char *)malloc(1);  // строка
	char buf[100]; // буфер для получения из выходного потока
	int n = 0;
	unsigned int len = 0; //len - длина строки
	*ptr = '\0';
	do {
		n = scanf("%99[^\n]", buf);
		if (n < 0) {
			free(ptr);
			ptr = NULL;
			continue;
		}
		if (n == 0)
			scanf("%*c");
		else {
			len += strlen(buf);
			ptr = (char *)realloc(ptr, len + 1); // добавляем память 
			strcat(ptr, buf);
		}
	} while (n > 0);
	return ptr;
}

void init(Table *table) // инициализация таблицы
{
	for (int i = 0; i < SIZE; ++i) {
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
		{
			rewind(table->fd);
			fwrite(&LinePos, sizeof(int), 2, table->fd);
		}
	}
	else
	{
		fread(&LinePos, sizeof(int), 1, table->fd);
		fread(&end, sizeof(int), 1, table->fd);
		fseek(table->fd, LinePos, SEEK_SET);

		int m = 0;
		Info *PrevInfo = NULL;
		Item* PrevItem = NULL, *tItem = NULL;
		Item tmpItem = { 0, 0, 0, NULL, NULL }; // вспомогательная переменная для считывания из файла элемента
		Info tmpInfo = { 0, 0, 0, 0, 0, NULL }; // для считывания Info
		Item* countItem = NULL;
		do
		{
			fread(&tmpItem, sizeof(Item), 1, table->fd);
			curItem = calloc(5, sizeof(Item*));
			*curItem = tmpItem;
			curItem->next = NULL;
			m = hash(curItem->key);
			if (table->tab[m] == NULL)
				table->tab[m] = curItem;
			else 
			if (hash(curItem->key) == hash(table->tab[m]->key) && (curItem->key != table->tab[m]->key))
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
				fread(&tmpInfo, sizeof(Info), 1, table->fd);
				curInfo = calloc(1, sizeof(Info));
				*curInfo = tmpInfo;
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
				if (curItem->next_setItem == posInfo)
				{
					free(curInfo);
					break;
				}
			} while (1);
			size_t posItem;
			posItem = ftell(table->fd);
			if (posItem == end)
			{
				free(curItem);
				break;
			}
			else{
				fseek(table->fd, posItem, SEEK_SET);
				free(curItem);
			}
		} while (1);
	}
	fclose(table->fd);
}
void Write_table_to_file(Table *table)
{
	Info *curInfo = NULL;
	Item* curItem = NULL;
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
					while (curItem)
					{
						if (setpos)
						fseek(table->fd, setpos, SEEK_SET);
						curItem->setItem = ftell(table->fd);
						fwrite(curItem, sizeof(Item), 1, table->fd);
						curInfo = curItem->info;
						while (curInfo)
						{
							pos = ftell(table->fd);
							curInfo->setInfo = pos;
							fwrite(curInfo, sizeof(Info), 1, table->fd);
							pos = ftell(table->fd);
							curInfo->next_setInfo = pos;
							fseek(table->fd, curInfo->setInfo, SEEK_SET);
							fwrite(curInfo, sizeof(Info), 1, table->fd);
							curInfo = curInfo->next;
						}
						pos = ftell(table->fd);
						curItem->next_setItem = pos;
						setpos = curItem->next_setItem;
						end = ftell(table->fd);
						fseek(table->fd, curItem->setItem, SEEK_SET);
						fwrite(curItem, sizeof(Item), 1, table->fd);
						curItem = curItem->next;
					}
				}
			}
		}
		rewind(table->fd);
		fwrite(&LinePos, sizeof(int), 1, table->fd);
		fwrite(&end, sizeof(int), 1, table->fd);
		printf("Str %d \n", LinePos);
		fclose(table->fd);
	}
}

int add(Table *table, int key, char* str) // функция добавления элемента в таблицу
{
	int h = hash(key);
	Item *node, *tmpNode = NULL;
	if (table->tab[h] == NULL)
	{
		node = malloc(sizeof(*node));
		node->info = (Info*)malloc(sizeof(Info));
		table->tab[h] = node;
		node->key = key;
		node->info->release = 1;
		node->info->next = NULL;
		node->next = NULL;
		if (table->fName)
		{
			table->fd = fopen(table->fName, "r+b");
			if (table->fd)
			{
				fseek(table->fd, 0, SEEK_END);
				if (LinePos)
					fseek(table->fd, LinePos, SEEK_SET);
				node->info->offset = ftell(table->fd);
				node->info->len = strlen(str) + 1;
				node->info->release = 1;
				fwrite(str, sizeof(char), node->info->len, table->fd);
				LinePos = ftell(table->fd);
				fclose(table->fd);
				if (tmpNode != NULL)
					tmpNode->next = node;
				else
					table->tab[h] = node;
				printf("Element added");
			}
			else printf("Open file error");
		}
		else printf("Name not found");

	}
	else
	{
		node = table->tab[h];
		for (; node; node = node->next)
		{
			if (node->key == key)
			{
				Info *tmp = (Info*)malloc(sizeof(Info));
				int rel = node->info->release;
				rel++;
				tmp->release = rel;
				if (table->fName)
				{
					table->fd = fopen(table->fName, "r+b");
					if (table->fd)
					{
						fseek(table->fd, 0, SEEK_END);
						if (LinePos)
							fseek(table->fd, LinePos, SEEK_SET);
						tmp->offset = ftell(table->fd);
						tmp->len = strlen(str) + 1;
						fwrite(str, sizeof(char), tmp->len, table->fd);
						LinePos = ftell(table->fd);
						fclose(table->fd);
						tmp->next = node->info;
						node->info = tmp;
						return 0;
					}
				}
			}
			if ((h == hash(table->tab[h]->key)) && (key != table->tab[h]->key))
			{
				Item *tmp = (Item*)malloc(sizeof(Item));
				tmp->info = (Info*)malloc(sizeof(Info));
				tmp->info->release = 1;

				if (table->fName)
				{
					table->fd = fopen(table->fName, "r+b");
					if (table->fd)
					{
						fseek(table->fd, 0, SEEK_END);
						if (LinePos)
							fseek(table->fd, LinePos, SEEK_SET);
						tmp->info->offset = ftell(table->fd);
						tmp->info->len = strlen(str) + 1;
						fwrite(str, sizeof(char), tmp->info->len, table->fd);
						LinePos = ftell(table->fd);
						fclose(table->fd);
						tmp->key = key;
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
	if (table->tab[h] == NULL)
		printf("The element to delete was not found\n");
	else {
		int k = 0;
		for (NodePrev = NULL; Node; Node = NodeNext)
		{
			NodeNext = Node->next;
			if (Node->key == key)
			{
				for (tmpPrev = NULL, tmp = Node->info; tmp; tmp = tmpNext)
				{
					tmpNext = tmp->next;
					if (tmp->release == rel)
					{
						k++;
						if (NodePrev == NULL)
						{
							if (tmpPrev == NULL)
							{
								if (tmp->next == NULL)
								{
									Node->info = Node->info->next;
									free(tmp);
									if (table->tab[h]->info == NULL && table->tab[h]->next == NULL)
									{
										free(NodePrev);
										table->tab[h] = NULL;
									}
									else
									{
										table->tab[h] = table->tab[h]->next;
										table->tab[h]->next = NULL;
									}
									return 0;
								}
								else
								{
									Node->info = tmp->next;
									free(tmp);
									return 0;
								}
							}
							else
							{
								Node->info->next = tmp->next;
								tmpPrev->next = tmpNext;
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
									free(Node->info);
									free(Node);
								}
								else
								{
									Node->info = tmp->next;
									tmp->next = tmpNext;
									free(tmp);
								}
							}
							else
							{
								Node->info->next = tmp->next;
								tmpPrev->next = tmpNext;
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
	return 0;
}

void find_by_key(Table *table, int key) // функция поиска элементов по ключу
{                                        // выводит все версии
	int h = hash(key);
	Item *Node;
	Node = table->tab[h];
	if (table->tab[h] == NULL) {
		printf("There is no such key in table \n"); // если хэша по такому ключу нет в таблице
		return 0;
	}
	else {
		int k = 0;
		while (Node)
		{
			if (Node->key == key) {
				Info *tmp = Node->info;
				while (tmp)
				{
					table->fd = fopen(table->fName, "r+b");
					char *info;
					fseek(table->fd, tmp->offset, SEEK_SET);
					long int  size = ftell(table->fd);
					printf("[%d] '", tmp->release);
					while (size = fread(&info, 1, tmp->len, table->fd))
					{
						fwrite(&info, 1, size - 1, stdout);
						printf("' ");
						break;
					}
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
	if (table->tab[h] == NULL) {
		printf("There is no such key in table\n");
		return 0;
	}
	else {
		int k = 0;
		while (Node)
		{
			if (Node->key == key) {
				Info *tmp = Node->info;
				while (tmp)
				{
					table->fd = fopen(table->fName, "r+b");
					if (tmp->release == rel) {
						k++;
						char *info;
						fseek(table->fd, tmp->offset, SEEK_SET);
						long int  size = ftell(table->fd);
						printf("'");
						while (size = fread(&info, 1, tmp->len, table->fd))
						{
							fwrite(&info, 1, size - 1, stdout);
							printf("' ");
							break;
						}
					}
					tmp = tmp->next;
				}
			}
			Node = Node->next;
		}
		if (k == 0)
			printf("There is no such release for this key\n"); // если найден ключ, но не найдена версия
	}

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
					table->fd = fopen(table->fName, "r+b");
					char *info;
					fseek(table->fd, tmpInfo->offset, SEEK_SET);
					long int  size = ftell(table->fd);
					printf("[%d] [%d] '", tmpItem->key, tmpInfo->release);
					while (size = fread(&info, 1, tmpInfo->len, table->fd))
					{
						fwrite(&info, 1, size - 1, stdout);
						printf("' ");
						break;
					}
					tmpInfo = tmpInfo->next;
				}
				printf("\n");
			}
		}
	}
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
					free(trashInfo);
				}
				trash = tmp;
				tmp = tmp->next;
				free(trash);
			}
		}
	}
}