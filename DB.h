//
// DB.h
// created by H2CO3, 10/10/2014
// No copyright, this is homework
//

#ifndef BEADANDO1_DB_H
#define BEADANDO1_DB_H

#include <sqlite3.h>

#include <unordered_set>
#include <unordered_map>

#include "Product.h"
#include "Category.h"

class Database {
	sqlite3 *dbHandle;
	std::unordered_set<Product> products;
	std::unordered_set<Category> categories;
	mutable std::unordered_map<int, std::unordered_set<int>> compatList;

public:
	Database(const std::string &fname);
	~Database();

	const std::unordered_set<Product> &getProducts() const;
	const std::unordered_set<Category> &getCategories() const;

	Product insertProduct(
		const std::string &name,
		const std::string &description,
		const Category &category,
		double price
	);
	bool updateProduct(const Product &p);

	bool productsCompatible(const Product &p1, const Product &p2) const;
	std::unordered_set<Product> getCompatibleProducts(const Product &p) const;
	bool makeProductsCompatible(const Product &p, const Product &q);

	const Category &categoryByID(int ID);
};


#endif // BEADANDO1_DB_H

