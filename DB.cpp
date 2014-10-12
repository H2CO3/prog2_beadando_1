//
// DB.cpp
// created by H2CO3, 10/10/2014
// No copyright, this is homework
//

#include "DB.h"
#include <cstdlib>
#include <exception>


static void die(sqlite3 *dbHandle, const std::string &cmsg = "")
{
	std::string errmsg("database error: ");
	errmsg += cmsg.length() ? cmsg : sqlite3_errmsg(dbHandle);
	sqlite3_close(dbHandle);
	throw std::runtime_error(errmsg);
}

static int param_index(sqlite3_stmt *stmt, const char *pName)
{
	int idx = sqlite3_bind_parameter_index(stmt, pName);

	if (idx == 0) {
		die(nullptr, std::string("no parameter with name '") + pName + "' was found");
	}

	return idx;
}

static void bind_int(sqlite3_stmt *stmt, const char *pName, int value)
{
	int idx = param_index(stmt, pName);
	sqlite3_bind_int(stmt, idx, value);
}

static void bind_double(sqlite3_stmt *stmt, const char *pName, double value)
{
	int idx = param_index(stmt, pName);
	sqlite3_bind_double(stmt, idx, value);
}

static void bind_string(sqlite3_stmt *stmt, const char *pName, const std::string &value)
{
	int idx = param_index(stmt, pName);
	sqlite3_bind_text(stmt, idx, value.c_str(), value.length(), SQLITE_TRANSIENT);
}

Database::Database(const std::string &fname)
{
	if (sqlite3_open(fname.c_str(), &dbHandle) != SQLITE_OK) {
		die(dbHandle);
	}

	sqlite3_stmt *stmt;

	// Read products
	if (sqlite3_prepare_v2(dbHandle, "SELECT * FROM product", -1, &stmt, nullptr) != SQLITE_OK) {
		die(dbHandle);
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		// Columns:
		// 0 -> id
		// 1 -> name
		// 2 -> description
		// 3 -> category_id
		// 4 -> price
		int ID = sqlite3_column_int(stmt, 0);	
		std::string name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
		std::string description = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
		int category_ID = sqlite3_column_int(stmt, 3);
		double price = sqlite3_column_double(stmt, 4);

		products.emplace(ID, std::move(name), std::move(description), category_ID, price);
	}

	sqlite3_finalize(stmt);

	// Read categories
	if (sqlite3_prepare_v2(dbHandle, "SELECT * FROM category", -1, &stmt, nullptr) != SQLITE_OK) {
		die(dbHandle);
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		// Columns:
		// 0 -> id
		// 1 -> name
		int ID = sqlite3_column_int(stmt, 0);
		std::string name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));

		categories.emplace(ID, std::move(name));
	}

	sqlite3_finalize(stmt);

	// Read compatibility list
	if (sqlite3_prepare_v2(dbHandle, "SELECT * FROM compat", -1, &stmt, nullptr) != SQLITE_OK) {
		die(dbHandle);
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		// Columns:
		// 0 -> ID of first product
		// 1 -> ID of second product
		int a = sqlite3_column_int(stmt, 0);
		int b = sqlite3_column_int(stmt, 1);

		compatList[a].insert(b);
		compatList[b].insert(a);
	}

	sqlite3_finalize(stmt);
}

Database::~Database()
{
	sqlite3_close(dbHandle);
}

const std::unordered_set<Product> &Database::getProducts() const
{
	return products;
}

const std::unordered_set<Category> &Database::getCategories() const
{
	return categories;
}

// Returns a copy rather than a reference so that it does not
// get invalidated when the set is rehashed

Product Database::insertProduct(
	const std::string &name,
	const std::string &description,
	const Category &category,
	double price
)
{
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(dbHandle, "INSERT INTO product(name, description, category_id, price) VALUES(:name, :description, :category_id, :price)", -1, &stmt, nullptr) != SQLITE_OK) {
		die(dbHandle);
	}

	bind_string(stmt, ":name", name);
	bind_string(stmt, ":description", description);
	bind_int(stmt, ":category_id", category.ID);
	bind_double(stmt, ":price", price);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		die(dbHandle);
	}

	sqlite3_finalize(stmt);

	int ID = sqlite3_last_insert_rowid(dbHandle);
	auto pr = products.emplace(ID, name, description, category.ID, price);
	return *pr.first;
}


bool Database::updateProduct(const Product &p)
{
	auto it = products.find(p);
	if (it == products.end()) {
		return false;
	}

	// This does not work because the std::commitee thought that it was a
	// good idea. It probably wasn't, since we now have to erase the element
	// and re-insert it. This invalidates iterators **AND** references to
	// the element, so this introduces problems. It is likely to be the source
	// of as hard-to-track-down errors as accidentally changing the hash
	// would have done so.
	//
	// it->name = p.name;
	// it->description = p.description;
	// it->category_ID = p.category_ID;
	// it->price = p.price;

	products.erase(it);
	products.insert(p);

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(dbHandle, "UPDATE product SET name=:name, description=:description, category_id=:category_id, price=:price WHERE id=:id", -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}

	bind_int(stmt, ":id", p.ID);
	bind_string(stmt, ":name", p.name);
	bind_string(stmt, ":description", p.description);
	bind_int(stmt, ":category_id", p.category_ID);
	bind_double(stmt, ":price", p.price);

	bool retval = (sqlite3_step(stmt) != SQLITE_DONE);
	sqlite3_finalize(stmt);
	return retval;
}

bool Database::productsCompatible(const Product &p, const Product &q) const
{
	const auto &m = compatList[p.ID];
	return m.find(q.ID) != m.end();
}

std::unordered_set<Product> Database::getCompatibleProducts(const Product &p) const
{
	std::unordered_set<Product> ps;
	const auto &m = compatList[p.ID];

	for (const auto &pr : products) {
		if (m.find(pr.ID) != m.end()) {
			ps.insert(pr);
		}
	}

	return ps;
}

bool Database::makeProductsCompatible(const Product &p, const Product &q)
{
	// This is sloppy and may cause duplicate compaitibility
	// entries, but as long as we don't need to delete, I don't care
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(dbHandle, "INSERT INTO compat(id_1, id_2) VALUES(:id_1, :id_2)", -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}

	bind_int(stmt, ":id_1", p.ID);
	bind_int(stmt, ":id_2", q.ID);

	bool succ = (sqlite3_step(stmt) == SQLITE_DONE);
	sqlite3_finalize(stmt);

	if (succ) {
		// add to local data structure as well
		compatList[p.ID].insert(q.ID);
		compatList[q.ID].insert(p.ID);
	}

	return succ;
}

const Category &Database::categoryByID(int ID)
{
	Category dummy(ID, "");
	auto it = categories.find(dummy);

	if (it == categories.end()) {
		throw std::runtime_error("category with ID '" + std::to_string(ID) + "' not found");
	}

	return *it;
}

