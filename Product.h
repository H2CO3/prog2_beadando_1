//
// Product.h
// ProgII Beadando 1
// Created by H2CO3 on 10/10/2014
// No copyright, this is homework
//

#ifndef BEADANDO1_PRODUCT_H
#define BEADANDO1_PRODUCT_H

#include <string>
#include <unordered_set>

struct Product {
	int ID;
	std::string name;
	std::string description;
	int category_ID;
	double price;

	Product(int pID, const std::string &pName, const std::string &pDesc, int pCatID, double pPrice);

	bool operator==(const Product &that) const;
};

namespace std {
	template<> struct hash<Product> {
		std::size_t operator()(const Product &p) const
		{
			return std::hash<int>()(p.ID);
		}
	};
}

#endif // BEADANDO1_PRODUCT_H
