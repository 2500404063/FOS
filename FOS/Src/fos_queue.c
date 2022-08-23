#include "fos_queue.h"

void FOS_QueueNodeInit(FOS_QueueNode *node, void *val) {
	node->nextNode = node;
	node->preNode = node;
	node->value = val;
}

void FOS_QueueAddNext(FOS_QueueNode *parent_node, FOS_QueueNode *node) {
	FOS_QueueNode *parent_node_next = parent_node->nextNode;
	parent_node->nextNode = node;
	node->preNode = parent_node;
	if (parent_node_next != parent_node) {
		node->nextNode = parent_node_next;
		parent_node_next->preNode = node;
	}
}

void FOS_QueueAddPre(FOS_QueueNode *parent_node, FOS_QueueNode *node) {
	FOS_QueueNode *parent_node_pre = parent_node->preNode;
	parent_node->preNode = node;
	node->nextNode = parent_node;
	if (parent_node_pre != parent_node) {
		node->preNode = parent_node_pre;
		parent_node_pre->nextNode = node;
	}
}

void FOS_QueueAddLineTail(FOS_QueueNode *parent_node, FOS_QueueNode *node) {
	while (parent_node->nextNode != parent_node) {
		parent_node = parent_node->nextNode;
	}
	FOS_QueueAddNext(parent_node, node);
}

void FOS_QueueAddLineHead(FOS_QueueNode *parent_node, FOS_QueueNode *node) {
	while (parent_node->preNode != parent_node) {
		parent_node = parent_node->preNode;
	}
	FOS_QueueAddPre(parent_node, node);
}

void FOS_QueueAddRoundTail(FOS_QueueNode *parent_node, FOS_QueueNode *node) {
	FOS_QueueNode *parent_node_pre = parent_node->preNode;
	parent_node->preNode = node;
	node->nextNode = parent_node;
	node->preNode = parent_node_pre;
	parent_node_pre->nextNode = node;
}

void FOS_QueueRemoveNode(FOS_QueueNode *node) {
	if (node->nextNode == node) {
		node->preNode->nextNode = node->preNode;
		node->preNode = node;
	} else if (node->preNode == node) {
		node->nextNode->preNode = node->nextNode;
		node->nextNode = node;
	} else {
		node->preNode->nextNode = node->nextNode;
		node->nextNode->preNode = node->preNode;
		node->preNode = node;
		node->nextNode = node;
	}
}

void FOS_QueueRemoveLineTail(FOS_QueueNode *node) {
	while (node->nextNode != node) {
		node = node->nextNode;
	}
	node->preNode->nextNode = node->preNode;
	node->preNode = node;
}

void FOS_QueueRemoveLineHead(FOS_QueueNode *node) {
	while (node->preNode != node) {
		node = node->preNode;
	}
	node->nextNode->preNode = node->nextNode;
	node->nextNode = node;
}

void FOS_QueueRemoveLineAll(FOS_QueueNode *node) {
	while (node->preNode != node) {
		node = node->preNode;
	}
	while (node->nextNode != node) {
		FOS_QueueNode *nextNode = node->nextNode;
		nextNode->preNode = nextNode;
		node->nextNode = node;
		node = nextNode;
	}
}

void FOS_QueueConvertToLine(FOS_QueueNode *head_node) {
	head_node->preNode->nextNode = head_node->preNode;
	head_node->preNode = head_node;
}

void FOS_QueueConvertToRound(FOS_QueueNode *node) {
	FOS_QueueNode *head_node = node;
	FOS_QueueNode *tail_node = node;
	while (head_node->preNode != head_node) {
		head_node = head_node->preNode;
	}
	while (tail_node->nextNode != tail_node) {
		tail_node = tail_node->nextNode;
	}
	head_node->preNode = tail_node;
	tail_node->nextNode = head_node;
}

FOS_QueueNode* FOS_QueueAt(FOS_QueueNode *head_node, unsigned long pos) {
	for (int i = 0; i < pos; ++i) {
		head_node = head_node->nextNode;
	}
	return head_node;
}

