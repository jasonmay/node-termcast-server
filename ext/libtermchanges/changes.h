#ifndef _CHANGES_H
#define _CHANGES_H

typedef struct {
    VTermScreenCell cell;
} ChangeNodeData;

struct ChangeNode;
typedef struct {
    ChangeNodeData data;
    ChangeNode *next;
} ChangeNode;

#endif
