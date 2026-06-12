#pragma once

enum class itemNodeType {itemLeafNode, itemNonLeafNode};

class itemNode
{
public:

	itemNodeType type;

	virtual ~itemNode();
};