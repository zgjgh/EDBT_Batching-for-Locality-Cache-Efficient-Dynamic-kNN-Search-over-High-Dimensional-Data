#include "Item.h"



Item::Item(Eigen::VectorXf C)
{
	v = C;
	epsilon = 1e-4;
	v_d=C.cast<double>();
}


Item::~Item()
{
}

bool Item::operator ==(const Item& I) {

	if ((v - I.v).norm() < epsilon) {
		return true;
	}
	else {
		return false;
	}
}
