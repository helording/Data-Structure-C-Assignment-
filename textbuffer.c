#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "textbuffer.h"

#define TRUE 1
#define FALSE 0

typedef struct newline *Line;
struct newline {
    struct newline *next;
    char *str;
};

struct textbuffer {
    int numLine;
    int size;
    struct newline *head;
    struct newline *tail;
    struct textbuffer *next;
};


////////help function//////////

void printTB(TB tb);
TB AddTo(TB tb, Line text);
Line findline(TB tb, int pos);
void freeText(Line text);
Line freeLine(TB tb, Line prev, Line curr,  int pos);
int End(TB tb, int pos);
int nextTo(int pos1, int pos2);
void sizeCheck(TB tb, int pos1, int pos2);
Line creatNewLine(char *line);
int countline(char text[], TB tb);
void WBT(void);
void deletecopy(int to, int from, TB copyTB);
void swapcase(TB tb, int pos1, int pos2);

///////////////////////////////////////////////////////////////


TB newTB (char text[]){
    if(text == NULL){
        abort();
    }
    TB tb = calloc(1, sizeof(struct textbuffer));
    assert(tb != NULL);
    
    char c;
    int curr = 0;
    int prev = 0;
    tb->head = NULL;
    tb->tail = NULL;
    tb->size = strlen(text);
    tb->numLine = countline(text, tb);
    int i = 0;
    while(i < tb->size + 1){
        c = text[i];
        if(c == '\0'){
            break;
        }if(c == '\n'){
            int num = i - prev + 1;
            char *string = calloc((num+1), sizeof(char));
            curr = 0;
            while(curr < num){
                string[curr] = text[prev+curr];
                curr++;
            }
            string[num] = '\0';
            Line text = creatNewLine(string);
            tb = AddTo(tb,text);
            free(string);
            prev = i+1;
        }
        i++;
    }
    return tb;
}

char *dumpTB (TB tb) {
    assert(tb != NULL);
    size_t alloc_len = 1;

    for (Line curr = tb->head; curr != NULL; curr = curr->next) {
        char *s = curr -> str;
        while(*s != '\0') {
            s++;
        }
        alloc_len = strlen (curr->str) + alloc_len;
    }
    char *str = calloc (alloc_len, sizeof(char));
    Line curr2 = tb->head;
    for (curr2 = tb->head; curr2 != NULL; curr2 = curr2->next) {
        strcat(str, curr2->str);
    }
    return str;
}

int linesTB (TB tb){
    return tb->numLine;
}

void swapTB (TB tb, int pos1, int pos2){
    assert(tb != NULL);
    sizeCheck(tb,pos1, pos2);
    assert(pos1 != pos2);
    if(pos1 > pos2){
        int temp = pos1;
        pos1 = pos2;
        pos2 = temp;
    }
    swapcase(tb, pos1, pos2);
}

void releaseTB (TB tb){
    assert(tb != NULL);
    Line curr = tb->head;
    freeLine(tb, NULL,curr, tb->numLine);
    free(tb);
}

void deleteTB (TB tb, int from, int to){
    assert(tb != NULL);
    sizeCheck(tb,from,to);
    if(from>to){
    
    }else{
        Line curr = findline(tb, from);
        Line prev = NULL;

        if(from != 0){
            prev = findline(tb, from - 1);
        }
        Line head = freeLine(tb, prev, curr, to-from+1);
        tb->head = head;
        tb->numLine = tb->numLine - (to-from+1);
    }
}

void mergeTB (TB tb1, int pos, TB tb2){
    assert(tb1 != NULL);
    assert(tb2 != NULL);
    pasteTB (tb1, pos, tb2);
    releaseTB(tb2);
}

void pasteTB (TB tb1, int pos, TB tb2){
    assert(tb1 != NULL);
    assert(tb2 != NULL);
    sizeCheck(tb1, pos, 0);

    char *copy = dumpTB(tb2);
    TB temp = newTB(copy);
    free(copy);
    if(tb1->numLine == 0){
        tb1->head = temp->head;
        tb1->tail = temp->tail;
        tb1->numLine = temp->numLine;
    }else if(pos == 0){
        temp->tail->next = tb1->head;
        tb1->head = temp->head;
    }else{
        Line curr = findline(tb1, pos);
        TB curr11 = calloc(1, sizeof(struct textbuffer));
        while(curr != NULL){
            curr11 = AddTo(curr11, curr);
            curr = curr->next;
        }
        curr = findline(tb1, pos-1);
        curr->next = temp->head;
        temp->tail->next = curr11->head;
        tb1->tail = curr11->tail;
        free(curr11);
    }
    tb1->numLine = temp->numLine + tb1->numLine;
    tb1->size = temp->size +tb1->size;
    free(temp);
}

TB cutTB (TB tb, int from, int to){
    assert(tb != NULL);
    sizeCheck(tb,from,to);
    assert(to > from);
    
    TB new = copyTB(tb,from,to);
    deleteTB(tb, from, to);
    return new;
}

TB copyTB (TB tb, int from, int to){
    assert(tb != NULL);
    sizeCheck(tb, from, to);
    char *Text = dumpTB (tb);
    TB copyTB = newTB(Text);
    deletecopy(to, from, copyTB);

    free(Text);
    return copyTB;
}

void replaceText (TB tb, char* str1, char* str2){
    assert(tb != NULL);
    assert(str1[0] != '\0');
    assert(str2[0] != '\0');
    for(Line curr = tb->head; curr!= NULL; curr = curr -> next){
        int len1 = strlen(str1);
        int flag = 0;
        char *line = curr->str;
        while(strstr(line, str1) != NULL){
            line = strstr(line, str1) + len1;
            flag++;
        }
        int lenCurr = strlen(curr->str);
        int len2 = strlen(str2);
        char *newLine = malloc(lenCurr + (len2 - len1)*flag + 1);
        line = curr->str;
        char *temp = strstr(line, str1);
        int i = 0;
        int h = 0;
        while((*line) != '\0'){
            if(line == temp){
                h = 0;
                while(str2[h] != '\0'){
                    newLine[i++] = str2[h++];
                }
                line = line + len1;
                temp = strstr(line, str1);
            } else {
                newLine[i] = line[0];
                line++;
                i++;
            }
        }
        newLine[i] =  '\0';
        free(curr->str);
        curr->str = newLine;
        tb->size = (len2 - len1)*flag + tb->size;
    }
}


/////////////help function///////////////

int End(TB tb, int pos) {
   if(pos == tb->numLine-1) {
      return TRUE;
   }
   return FALSE;
}

int nextTo(int pos1, int pos2) {
   if(abs(pos1 -pos2) ==1) {
      return TRUE;
   }
   return FALSE;
}

Line freeLine(TB tb, Line prev, Line curr, int pos){
    assert(tb != NULL);
    Line curr1 = NULL;
    Line curr2 = curr;
    int i = 0;
    while (i < pos && curr != NULL) {
        curr1 = curr->next;
        freeText(curr);
        curr = curr1;
        i++;
    }

    if(curr2 == tb->head){
        if(pos != tb->numLine){
            return curr1;
        }else{
            return NULL;
        }
    }else{
        prev->next = curr1;
        return tb->head;
    }

}

Line findline(TB tb, int pos){
    assert(tb != NULL);
    Line curr = tb->head;
    if(pos == 0){
        return curr;
    }
    int i = 0;

    while(i != pos){
        curr = curr->next;
        i++;
    }
    return curr;
}

int countline(char text[], TB tb){
    int num = 0;
    char c;
    for(int i = 0; i < tb->size; i++){
        c = text[i];
        if(c == '\n'){
            num++;
        }
    }
    return num;
}

Line creatNewLine(char *line){
    Line new = calloc(1, sizeof(struct newline));
    new->str = strdup(line);
    new->next = NULL;
    return new;
}

void freeText(Line text){
    free(text -> str);
    free(text);
}

void sizeCheck(TB tb, int pos1, int pos2){
    assert(tb != NULL);
    if(pos1 < 0 || pos2 < 0 ||
        pos1 > tb->numLine-1 || pos2 > tb->numLine-1){
        abort();
    }
}

TB AddTo(TB tb, Line text){
     assert(tb != NULL);
    if(tb->head == NULL){
        tb->head = text;
        tb->tail = text;
    } else {
        tb->tail->next = text;
        tb->tail = text;
    }
    return tb;
}


void printTB(TB tb){
    char *copyOne = dumpTB(tb);
    printf("%s", copyOne);
    free(copyOne);
}

void deletecopy(int to, int from, TB copyTB){
    int last = copyTB->numLine - 1;
    if(to < from){
    
    }else if(to == 0 && from == 0){
        deleteTB (copyTB, from+1, last);
    }else if(from == 0 && to == 0 && last == 0){

    }else if(from == 0 && to != last){
        deleteTB (copyTB, to+1, last);
    } else if(from == 0 && to == last){

    }else if(from == last && to == last){
        deleteTB (copyTB, 0, last-1);
    }else{
        deleteTB (copyTB, 0, from - 1);
        deleteTB (copyTB, to, last-1);
    }
}

void swapcase(TB tb, int pos1, int pos2){
    Line curr1 = findline(tb, pos1);
    Line curr2 = findline(tb, pos2);

    Line prev1 = NULL;
    if(pos1 != 0){
        prev1 = findline(tb, pos1-1);
    }
    Line prev2 = findline(tb, pos2-1);

    Line next1 = curr1->next;
    Line next2 = curr2->next;
    
    if(pos1 != 0 && !End(tb, pos2) && !nextTo(pos1, pos2)){
        prev1->next = curr2;
        curr2->next = next1;
        prev2->next = curr1;
        curr1->next = next2;
    }else if(nextTo(pos1, pos2) && !End(tb,pos2) && pos1 != 0){
        prev1->next = curr2;
        curr2->next = curr1;
        curr1->next = next2;
    }else if(pos1 == 0 && !End(tb, pos2)&& !nextTo(pos1, pos2)){
        prev2->next = curr1;
        curr1->next = next2;
        curr2->next = next1;
        tb->head = curr2;
    }else if(End(tb, pos2) && pos1 != 0&& !nextTo(pos1, pos2)){
        prev1->next = curr2;
        curr2->next = next1;
        prev2->next = curr1;
        curr1->next = NULL;
        tb->tail = curr1;
    }else if(End(tb,pos2) && pos1 == 0 && !nextTo(pos1, pos2)){
        prev2->next = curr1;
        curr2->next = next1;
        curr1->next = NULL;
        tb->tail = curr1;
        tb->head = curr2;
    }else if(End(tb, pos2) && nextTo(pos1, pos2) && pos1 != 0){
        prev1->next = curr2;
        curr2->next = curr1;
        curr1->next = NULL;
        tb->tail = curr1;
    } else if(pos1 == 0&& End(tb, pos2)&& nextTo(pos1, pos2)){
        curr2->next = curr1;
        curr1->next = NULL;
        tb->head = curr2;
    }else{
        curr2->next = curr1;
        curr1->next = next2;
        tb->head = curr2;
    }
}

void WBT(void){
    printf("this is the whiteBoxtest for the swap function\n");
    char *str1 = "Hello\nworld\nHello\nworld\n";
    TB tb1 = newTB(str1);
    assert(tb1 != NULL);
    assert(tb1->head->str[0] == 'H');
    assert(tb1->head->next->str[0] == 'w');
    assert(tb1->head->next->next->str[0] == 'H');
    assert(tb1->head->next->next->next->str[0] == 'w');
    assert(linesTB(tb1) == 4);
    Line test2 = findline(tb1, 2);
    Line test3 = findline(tb1, 3);
    assert(test2->next == test3 );
    swapTB(tb1,0,1);//world\nHello\nHello\nworld\n
    Line test0 = findline(tb1, 0);
    Line test1 = findline(tb1, 1);
    assert(test0->str[0] == 'w');
    assert(test1->str[0] == 'H');
    assert(tb1->numLine == 4);
    char *n = dumpTB(tb1);
    assert(n[0] == 'w');
    assert(n[6] == 'H');
    swapTB(tb1,0,2);//Hello\nHello\nworld\nworld\n
    char *u = dumpTB(tb1);
    assert(u[0] == 'H');
    assert(u[6] == 'H');
    assert(u[12] == 'w');
    assert(u[18] == 'w');
    free(n);
    free(u);
    releaseTB(tb1);

    char *str2 = "what\nthe\nhell\nis\ngoing\non\n";
    TB tb2 = newTB(str2);
    assert(tb2 != NULL);
    assert(linesTB(tb2) == 6);
    swapTB(tb2,3,4);//what\nthe\nhell\ngoing\nis\non\n
    assert(tb2->head->str[0] == 'w');
    assert(tb2->head->next->str[0] == 't');
    assert(tb2->head->next->next->str[0] == 'h');
    assert(tb2->head->next->next->next->str[0] == 'g');
    char *h = dumpTB(tb2);
    assert(h[4] == '\n');
    assert(h[5] == 't');
    assert(h[15] == 'o');
    assert(h[21] == 's');
    free(h);
    releaseTB(tb2);

    char *str4 = "Hello\nworld\nHello\nworld\n";
    char *str5 = "what\nthe\nhell\nis\ngoing\non\n";
    TB tb4 = newTB(str4);
    assert(tb4 != NULL);
    assert(tb4->head->str[0] == 'H');
    assert(tb4->head->next->str[0] == 'w');
    assert(tb4->head->next->next->str[0] == 'H');
    assert(tb4->head->next->next->next->str[0] == 'w');
    TB tb5 = newTB(str5);
    assert(tb5->head->str[0] == 'w');
    assert(tb5->head->next->str[0] == 't');
    assert(tb5->head->next->next->str[0] == 'h');
    assert(tb5->head->next->next->next->str[0] == 'i');
    mergeTB(tb4,2,tb5);
    char *line = dumpTB(tb4);
    assert(line[2] = 'w');
    assert(line[29] = 'o');
    assert(line[30] = 'H');
    assert(line[31] = 'e');
    assert(tb4->head->str[0] == 'H');
    assert(tb4->head->next->str[0] == 'w');
    assert(tb4->head->next->next->str[0] == 'w');
    assert(tb4->head->next->next->next->str[0] == 't');
    
    char *new = "line1\nline2\nline3\nline4\nline5\nline6\nline7\n"
    "line8\nline9\nline10\n";
    TB tbnew = newTB(new);
    assert(tbnew != NULL);
    assert(linesTB(tbnew) == 10);
    char *old = "old1\nold2\nold3\nold4\nold5\nold6"
    "\nold7\nold8\nold9\nold10\n";
    TB tbold = newTB(old);
    assert(tbold != NULL);
    assert(linesTB(tbold) == 10);
    
    deleteTB(tbnew, 2,9);
    assert(tbnew->numLine == 2);
    assert(tbnew->head->str[4] == '1');
    assert(tbnew->head->next->str[4] == '2');
    assert(tbnew->head->next->next == NULL);
    char *q = dumpTB(tbnew);
    assert(q[strlen(q)-2] == '2');
    TB copyold = copyTB (tbold, 0, 9);
    assert(copyold->numLine == 10);
    replaceText (copyold, "old", "new");
    replaceText (copyold, "new", "fly");
    mergeTB(copyold,0,tbnew);
    //printTB(copyold);
    assert(copyold->head->str[4] == '1');
    assert(copyold->head->next->str[4] == '2');
    assert(copyold->head->next->next->str[0] == 'f');
    assert(copyold->head->next->next->next->str[0] == 'f');
    releaseTB(copyold);
    releaseTB(tbold);
    free(q);
    
    printf("all white boxes tests have passed\n");
}

