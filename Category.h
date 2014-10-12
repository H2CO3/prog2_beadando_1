//
// Category.h
// ProgII Beadando 1
// Created by H2CO3 on 10/10/2014
// No copyright, this is homework
//

#ifndef BEADANDO1_CATEGORY_H
#define BEADANDO1_CATEGORY_H

#include <string>
#include <unordered_set>

struct Category {
	int ID;
	std::string name;

	Category(int pID, const std::string &pName);
	bool operator==(const Category &that) const;
};

namespace std {
	template<> struct hash<Category> {
		std::size_t operator()(const Category &c) const
		{
			return std::hash<int>()(c.ID);
		}
	};
}

#endif // BEADANDO1_CATEGORY_H
