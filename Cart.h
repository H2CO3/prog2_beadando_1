//
// Cart.h
// Prog II Beadando 1
// Created by H2CO3 on 12/10/2014
// No copyright, this is homework
//

#ifndef BEADANDO1_CART_H
#define BEADANDO1_CART_H

#include <vector>
#include <unordered_set>
#include <memory>

#include "Product.h"
#include "Category.h"

#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/choice.h>
#include <wx/stattext.h>

class Cart: public wxDialog {
	std::vector<Product> products;
	std::unique_ptr<wxListCtrl> productList;
	std::unique_ptr<wxStaticText> total;
	std::unique_ptr<wxButton> deleteButton;
	std::unique_ptr<wxButton> deleteAllButton;

	long nameCol, prceCol;

	void reloadTable();

public:
	Cart(wxWindow *parent, wxWindowID ID);
	~Cart();
	void addProduct(const Product &p);
};

#endif // BEADANDO1_CART_H

