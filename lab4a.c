#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 10

typedef struct Item {
    int key; // ключ элемента
    struct Info *info; // указатель на элемент
    struct Item *next; // указатель на след элемент в таблице
}Item;

typedef struct Info{
    int release; // версия элемента
    char *info; // указатель на информацию
    struct Info *next; // указатель на след элемент в списке
}Info;

Item *table[SIZE]; // объявляем массив указателей на элементы таблицы

int hash(int);
int dialog(int N);
int getint(int *a);
char *getstr();
void init(Item *table[]);
int add(Item *table[], int key, char* str);
int delete(Item *table[], int key, int rel);
void find_by_key(Item *table[], int key);
void find_by_key_release(Item *table[], int key, int rel);
void print_table(Item *table[]);
void clean(Item *table[]);
void d_add();
void d_delete();
void d_find_by_key();
void d_find_by_key_release();

const char *msgs[] = {"0. Quit",  //реализованные функции
                      "1. Add in table",
                      "2. Find by key",
                      "3. Find by key and release",
                      "4. Delete by key and release",
                      "5. Show table"
};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]); 

int main()
{
    init(&table); // инициализация таблицы
    int rc;
    while (rc = dialog(NMsgs)) // вызов пользовательского меню
    {
        if (rc == 1) // 1 - Добавление элемента 
            d_add(&table);
        if (rc == 2)
            d_find_by_key(&table); // 2 - Поиск элемента по ключу
        if (rc == 3)
            d_find_by_key_release(&table); // 3 - Поиск по ключу и версии
        if (rc == 4)
            d_delete(&table); // 4 - Удаление элемента из таблицы
        if (rc == 5)
            print_table(&table); // 5- Печать таблицы 

    }
    printf("That's all. Bye!\n"); // Завершение работы
    clean(&table);
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
    do{
        puts(errmsg);
        errmsg = "You are wrong. Repeat, please!";
        for(i = 0; i < N; ++i)
            puts(msgs[i]);
        printf("Make your choice: ");
        n = getint(&rc);
        if(n == 0)
            rc = 0;
    } while(rc < 0 || rc >= N);
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
    }while (n == 0 || a < 0);
    scanf("%*c");
    return n<0?0:1;
}

char *getstr() { //функция безопасного получения строки
    char *ptr = (char *) malloc(1);  //строка
    char buf[100]; //буфер для получения из входного потока
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
        if(n == 0)
            scanf("%*c");
        else {
            len += strlen(buf);
            ptr = (char *) realloc(ptr, len + 1); // добавляем память
            strcat(ptr, buf);
        }
    } while(n > 0);
    return ptr;
}

void init(Item *table[]) // инициализация таблицы 
{
    for(int i = 0; i < SIZE; ++i)
        table[i] = NULL;
}

int add(Item *table[], int key, char* str) // функция добавления элемента в таблицу
{
    int h = hash(key);
    Item *node, *tmpNode = NULL;
    if (table[h] == NULL)
    {
        node = malloc(sizeof(*node));
        node->info = (Info*)malloc(sizeof(Info));
        table[h] = node;
        node->key = key;
        node->info->release = 1;
        node->info->info = str;
        node->info->next = NULL;
        node->next = NULL;
    }
    else
    {     node = table[h];
          for (; node != NULL; node = node->next){
          if (node->key == key)
          {
            Info *tmp = (Info*)malloc(sizeof(Info));
            int rel = node->info->release;
            rel++;
            
            tmp->release = rel;
            tmp->info = str;
            tmp->next = node->info;
            node->info = tmp;
            return 0;
          }}
          if ((h == hash(table[h]->key)) && (key != table[h]->key))
          {
            Item *tmp = (Item*)malloc(sizeof(Item));
            tmp->info = (Info*)malloc(sizeof(Info));
            tmp->info->release = 1;
            tmp->info->info = str;
            tmp->key = key;
            tmp->info->next = NULL;
            tmp->next = table[h];
            table[h] = tmp;
          }
      }
      return 0;
}

int delete(Item *table[], int key, int rel)
{
  Item *Node, *tmpItem;
  Info *tmpPrev, *tmpNext, *tmp;
  int h = hash(key);
  Node = table[h];
  if (table[h] == NULL)
  printf("The element to delete was not found\n");
  else {
    int k = 0;
    while (Node)
    {
      if (Node->key == key)
      {
        for (tmpPrev = NULL, tmp = Node->info; tmp; tmp = tmpNext)
        {
            tmpNext = tmp->next;
            if (tmp->release == rel)
            {
              if (tmpPrev == NULL)
                {

                  Node->info = tmpNext;
                  tmpItem = tmp;
                  free(tmpItem->info);
                  free(tmpItem);
                  if (Node->info == NULL)
                  {
                    free(table[h]);
                    table[h] = NULL;
                  }
                  return 0;
                  k++;
                }
              else
                {
                  tmpPrev->next = tmpNext;
                  tmpItem = tmp;
                  free(tmpItem->info);
                  free(tmpItem);
                  k++;
                  return 0;
                }
            }
            tmpPrev = tmp;
        }    
      }
      Node = Node->next;
    }
    if (k == 0)
    printf("The element to delete was not found\n");
  }
  return 0;
}

void find_by_key(Item *table[], int key) // функция поиска элементов по ключу
{                                        // выводит все версии
  int h = hash(key);
  Item *tmp, *Node;
  Node = table[h];
  if (table[h] == NULL){
  printf("There is no such key in table \n"); // если хэша по такому ключу нет в таблице
  return 0;
  }
  else {
    int k = 0;
    while(Node)
    {
      if (Node->key == key){
      Info *tmp = Node->info;
      while (tmp)
      {
        k++;
        printf("Release: %d Info: %s\n", tmp->release, tmp->info);
        tmp = tmp->next;
      }
      }
      Node = Node->next;
    }
    if (k == 0)
    printf("There is no such key in table\n"); // если такого ключа нет в таблице
  }
  
}

void find_by_key_release(Item *table[], int key, int rel) // функиця поиска элемента в таблицы по ключу и версии
{
  int h = hash(key);
  Item *tmp, *Node;
  Node = table[h];
  if (table[h] == NULL){ 
  printf("There is no such key in table\n"); 
  return 0;
  }
  else {
    int k = 0;
    while(Node)
    {
      if (Node->key == key){
      Info *tmp = Node->info;
      while (tmp)
      {
        if (tmp->release == rel){
        k++;
        printf("Info: %s\n", tmp->info);
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

void d_add() // диалоговая функция добавления элемента в таблицу
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

void print_table(Item *table[]) // функция печати таблицы
{
    Item *tmpItem;
    for (int i = 0; i < SIZE; ++i)
    {
        tmpItem = table[i];
        if (tmpItem)
        {
            for (; tmpItem != NULL; tmpItem = tmpItem->next)
            {
              Info *tmpInfo = tmpItem->info;
              while(tmpInfo != NULL)
              {
                printf("[%d] [%d] '%s' ", tmpItem->key, tmpInfo->release, tmpInfo->info);
                tmpInfo = tmpInfo->next;
              }
              printf("\n");
        }   }
    }
}

void clean(Item *table[]) // функция очистки таблицы
{
    Info *tmpInfo, *trashInfo;
    Item *tmpItem, *tmp, *prevItem = NULL, *trash;
    for (int i = 0; i < SIZE; ++i)
    {
        tmp = table[i];
        if (tmp)
        {
            while (tmp)
            {
                tmpInfo = tmp->info;
                while (tmpInfo)
                {
                    trashInfo = tmpInfo;
                    tmpInfo = tmpInfo->next;
                    free(trashInfo->info);
                    free(trashInfo);
                }
                trash = tmp;
                tmp = tmp->next;
                free(trash);
            }
        }
    }
}
