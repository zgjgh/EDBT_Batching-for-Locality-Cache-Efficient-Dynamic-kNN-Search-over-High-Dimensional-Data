#pragma once

enum class NodeType {LeafNode, NonLeafNode};

class Node
{
public:

	NodeType type;

	virtual ~Node();
};

