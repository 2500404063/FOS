#ifndef FOS_QUEUE
#define FOS_QUEUE

typedef struct _fos_queue_node {
	void *value;
	struct _fos_queue_node *preNode;
	struct _fos_queue_node *nextNode;
} FOS_QueueNode;

void FOS_QueueNodeInit(FOS_QueueNode *node, void *val);
void FOS_QueueAddNext(FOS_QueueNode *parent_node, FOS_QueueNode *node);
void FOS_QueueAddPre(FOS_QueueNode *parent_node, FOS_QueueNode *node);
void FOS_QueueAddLineTail(FOS_QueueNode *parent_node, FOS_QueueNode *node);
void FOS_QueueAddLineHead(FOS_QueueNode *parent_node, FOS_QueueNode *node);
void FOS_QueueAddRoundTail(FOS_QueueNode *parent_node, FOS_QueueNode *node);
void FOS_QueueRemoveNode(FOS_QueueNode *node);
void FOS_QueueRemoveLineTail(FOS_QueueNode *node);
void FOS_QueueRemoveLineHead(FOS_QueueNode *node);
void FOS_QueueRemoveLineAll(FOS_QueueNode *node);
void FOS_QueueConvertToLine(FOS_QueueNode *head_node);
void FOS_QueueConvertToRound(FOS_QueueNode *node);
FOS_QueueNode* FOS_QueueLineAt(FOS_QueueNode *head_node, unsigned long pos);
#endif
