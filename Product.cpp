//
// Product.cpp
// ProgII Beadando 1
// Created by H2CO3 on 10/10/2014
// No copyright, this is homework
//

#include "Product.h"

Product::Product(int pID, const std::string &pName, const std::string &pDesc, int pCatID, double pPrice):
	ID(pID),
	name(pName),
	description(pDesc),
	category_ID(pCatID),
	price(pPrice)
{}

bool Product::operator==(const Product &that) const
{
	return this->ID == that.ID;
}
