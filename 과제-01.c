
#define _CRT_SECURE_NO_WARNINGS



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN     1000    
#define MAX_STACK   100     

  
typedef struct {
    char data[MAX_STACK];
    int  top;
} CharStack;

typedef struct {
    int  data[MAX_STACK];
    int  top;
} IntStack;

static void csInit(CharStack* s) { s->top = -1; }
static int  csIsEmpty(CharStack* s) { return s->top == -1; }
static int  csIsFull(CharStack* s) { return s->top == MAX_STACK - 1; }
static void csPush(CharStack* s, char c) {
    if (csIsFull(s)) { fprintf(stderr, "[오류] 스택 오버플로우(깊이 초과)\n"); exit(1); }
    s->data[++(s->top)] = c;
}
static char csPop(CharStack* s) {
    if (csIsEmpty(s)) { fprintf(stderr, "[오류] 스택 언더플로우\n"); exit(1); }
    return s->data[(s->top)--];
}
static char csPeek(CharStack* s) {
    if (csIsEmpty(s)) { fprintf(stderr, "[오류] 스택이 비어 있음\n"); exit(1); }
    return s->data[s->top];
}
static int  csSize(CharStack* s) { return s->top + 1; }

static void isInit(IntStack* s) { s->top = -1; }
static int  isIsEmpty(IntStack* s) { return s->top == -1; }
static int  isIsFull(IntStack* s) { return s->top == MAX_STACK - 1; }
static void isPush(IntStack* s, int v) {
    if (isIsFull(s)) { fprintf(stderr, "[오류] 스택 오버플로우\n"); exit(1); }
    s->data[++(s->top)] = v;
}
static int  isPop(IntStack* s) {
    if (isIsEmpty(s)) { fprintf(stderr, "[오류] 스택 언더플로우\n"); exit(1); }
    return s->data[(s->top)--];
}
static int* isPeekRef(IntStack* s) {
    if (isIsEmpty(s)) { fprintf(stderr, "[오류] 스택이 비어 있음\n"); exit(1); }
    return &s->data[s->top];
}

static int isValidTree(const char* s) {
    int len = (int)strlen(s);
    if (len == 0 || len >= MAX_LEN) return 0;

    int depth = 0;         
    int expectNode = 1;     
    int seen[26] = { 0 };

    for (int i = 0; i < len; i++) {
        char c = s[i];

        if (isupper((unsigned char)c)) {
            if (!expectNode) return 0;         

            if (seen[c - 'A']) return 0;        

            expectNode = 0;
        }
        else if (c == '(') {
            if (expectNode) return 0;  
            depth++;
            expectNode = 1;            
        }
        else if (c == ')') {
            if (expectNode) return 0;   
            depth--;
            if (depth < 0) return 0;    
            expectNode = 0;
        }
        else if (c == ',') {
            if (expectNode) return 0;  
            expectNode = 1;             
        }
        else {
            return 0;   
        }
    }

    if (depth != 0) return 0;    
    if (expectNode) return 0;    

    return 1;
}

typedef struct {
    int  totalNodes;
    int  leafNodes;
    int  nonLeafNodes;
    int  height;
    int  degree;

    int  cFound;         
    int  hasParentC;      
    char parentOfC;

    char childrenOfC[MAX_STACK];
    int  numChildrenOfC;
} TreeInfo;

static void analyzeTree(const char* s, TreeInfo* info) {
    CharStack nodeStack;   
    IntStack  cntStack;    
    csInit(&nodeStack);
    isInit(&cntStack);
    memset(info, 0, sizeof(TreeInfo));

    int len = (int)strlen(s);
    char lastNode = '\0';  

    for (int i = 0; i < len; i++) {
        char c = s[i];

        if (isupper((unsigned char)c)) {
            info->totalNodes++;

            int depth = csSize(&nodeStack);              
            if (depth + 1 > info->height) info->height = depth + 1;

            char parent = csIsEmpty(&nodeStack) ? '\0' : csPeek(&nodeStack);

            if (!csIsEmpty(&nodeStack)) {
               
                int* cnt = isPeekRef(&cntStack);
                (*cnt)++;
            }

            if (c == 'C') {
                info->cFound = 1;
                info->hasParentC = (parent != '\0');
                info->parentOfC = parent;
            }
            if (parent == 'C') {
                info->childrenOfC[info->numChildrenOfC++] = c;
            }

            lastNode = c;
        }
        else if (c == '(') {
            
            csPush(&nodeStack, lastNode);
            isPush(&cntStack, 0);
            info->nonLeafNodes++;
        }
        else if (c == ')') {
            int childCount = isPop(&cntStack);   
            csPop(&nodeStack);
            if (childCount > info->degree) info->degree = childCount;
        }
      
    }

    info->leafNodes = info->totalNodes - info->nonLeafNodes;
}


static int findChildEnd(const char* s, int idx) {
    int depth = 0;
    while (1) {
        char c = s[idx];
        if (c == '(') {
            depth++;
        }
        else if (c == ')') {
            if (depth == 0) return idx;
            depth--;
        }
        else if (c == ',' && depth == 0) {
            return idx;
        }
        idx++;
    }
}


static int printTree(const char* s, int idx, const char* prefix,
    int isLast, int isRoot) {
    char node = s[idx++];

    if (isRoot) {
        printf("%c\n", node);
    }
    else {
        printf("%s+---%c\n", prefix, node);
    }

    if (s[idx] == '(') {
        idx++;  

        char childPrefix[MAX_LEN];
        if (isRoot) {
           
            strcpy(childPrefix, prefix);
        }
        else {
            snprintf(childPrefix, sizeof(childPrefix), "%s%s",
                prefix, isLast ? "    " : "|   ");
        }

        while (1) {
            int endPos = findChildEnd(s, idx);   
            int childIsLast = (s[endPos] == ')');

            idx = printTree(s, idx, childPrefix, childIsLast, 0);

            if (s[idx] == ',') { idx++; continue; }
            break;  
        }
        idx++;  
    }
    return idx;
}


static void trimNewline(char* s) {
    int len = (int)strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[--len] = '\0';
    }
}

int main(void) {
    char input[MAX_LEN];

    printf("트리의 괄호 표기법을 입력하세요: ");
    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "입력을 읽을 수 없습니다.\n");
        return 1;
    }
    trimNewline(input);

    if (!isValidTree(input)) {
        printf("\n[오류] 올바른 트리의 괄호 표기법이 아닙니다.\n");
        return 1;
    }

    TreeInfo info;
    analyzeTree(input, &info);

    printf("\n입력된 트리 : %s\n", input);
    printf("-----------------------------------\n");
    printf("전체 노드의 수   : %d\n", info.totalNodes);
    printf("단말 노드의 수   : %d\n", info.leafNodes);
    printf("비단말 노드의 수 : %d\n", info.nonLeafNodes);
    printf("트리의 높이      : %d\n", info.height);
    printf("트리의 차수      : %d\n", info.degree);

    printf("-----------------------------------\n");
    if (!info.cFound) {
        printf("노드 C          : 트리에 존재하지 않습니다.\n");
    }
    else {
        if (info.hasParentC)
            printf("C의 부모 노드   : %c\n", info.parentOfC);
        else
            printf("C의 부모 노드   : 없음 (C가 루트 노드)\n");

        if (info.numChildrenOfC == 0) {
            printf("C의 자식 노드   : 없음 (C는 단말 노드)\n");
        }
        else {
            printf("C의 자식 노드   : ");
            for (int i = 0; i < info.numChildrenOfC; i++) {
                printf("%c%s", info.childrenOfC[i],
                    (i == info.numChildrenOfC - 1) ? "\n" : ", ");
            }
        }
    }

    printf("-----------------------------------\n");
    printf("트리 구조 (왼쪽으로 눕힌 형태)\n\n");
    printTree(input, 0, "", 1, 1);

    return 0;
}
