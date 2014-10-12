//
// Category.cpp
// ProgII Beadando 1
// Created by H2CO3 on 10/10/2014
// No copyright, this is homework
//

#include "Category.h"

Category::Category(int pID, const std::string &pName):
	ID(pID),
	name(pName)
{}

bool Category::operator==(const Category &that) const
{
	return this->ID == that.ID;
}
