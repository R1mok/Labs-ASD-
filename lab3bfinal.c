#include <stdio.h>
#include <stdlib.h>

typedef struct item {
    char c;
    struct item *next;
} item;

int getList(item **pptr);
void printList(item *ptr);
item *freeList(item *head);
void deleteword(item *head, item **tail);
item *skipSpace(item *phead);
item *extractWord(item *head, int *len, item **lastletter);
item *reorg(item *phead);

int main() {
    item *first;
    int n;
    do {
        printf("Enter string-->");
        n = getList(&first);
        printf("Your string: ");
        printList(first);
        printf("\n");
        first = reorg(first);
        printf("New string: ");
        printList(first);
        printf("\n");
        freeList(first);
    } while (n > 0);
    return 0;
}
item *freeList(item *head) {
    item *tmp = NULL;
    while(head){
        tmp = head;
        head = head->next;
        free(tmp);
    }
    return head;
}

int getList(item **pptr) {
    char buf[101], *str;
    item head = {'*', NULL};
    item *tail = &head;
    int n, rc = 1;
    do {
        n = scanf("%100[^\n]", buf);
        if (n < 0) {
            freeList(head.next);
            head.next = NULL;
            rc = 0;
            continue;
        }
        if (n > 0) {
            for (str = buf; *str != '\0'; ++str) {
                tail->next = (item *)malloc(sizeof(item));
                tail = tail->next;
                tail->c = *str;
            }
            tail->next = NULL;
        } else
            scanf("%*c");
    } while (n > 0);
    *pptr = head.next;
    return rc;
}

void printList(item *ptr) {
    //printf("\'");
    for(; ptr != NULL; ptr = ptr->next)
        printf("%c", ptr->c);
    //printf("\'");
}

void deleteword(item *head, item **tail) {
    item *temp = head->next;
    item *t = temp;
    while(temp && temp != *tail && temp->next != *tail && temp->next && head != *tail)
    {
        t = temp->next;
        free(temp);
        temp = t;
    }
    head->c = (*tail)->c;
    head->next = (*tail)->next;
}

item *skipSpace(item *phead) {
    item *head = phead;
    while (head && ((head->c == ' ') || (head->c == '\t')) && head->next){
        head = head->next;
    }
    if (phead != head)
        deleteword(phead->next, &head);
    return head;
}

item *extractWord(item *head, int *len, item **lastletter) {
    *len = 0;
    while ((head->c != ' ') && (head->c != '\t')) {
        ++(*len);
        if (head->next == NULL)
            break;
        *lastletter = head;
        head = head->next;
    }
    return head;
}//если последний символ пробельный - возвращает NULL


item *reorg(item *phead) {
    item *head = phead;
    item *tail = head, *first = NULL, *last = NULL;
    int mainlength = 0, len = 0;
    head = skipSpace(head);
    phead = head;
    if (head == NULL)
        return tail;
    head = extractWord(head, &mainlength, &last);
    if (head)
        head->c = ' ';
    while (head) {
        first = head;
        head = skipSpace(head);
        tail = extractWord(head, &len, &last);
        if (mainlength != len) {
            deleteword(first, &tail);
            head = first;
        }
        else
            head = tail;
        if (mainlength != len && head && head->next == NULL)
        head->c = ' ';
        if (head->c == '\t')
            head->c = ' ';
        if (head->next == NULL)
            break;
    }
    //printList(head);
    //printf("\n");
    //printList(head->next);
    //printf("\n");
    //if (mainlength != len && head && head->next == NULL)
      //  head->c = ' ';
    //if (last && last->next &&(last->next->c == ' ' || last->next->c == '\t'))
      //  last->next = NULL;
    return phead;
}
