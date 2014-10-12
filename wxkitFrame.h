// wxkitFrame.h
// Beadando 1
// Created by H2CO3 on 10/10/2014
// No copyright, this is homework

#ifndef WXKITFRAME_H
#define WXKITFRAME_H

#include <vector>
#include <memory>

#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>

#include "DB.h"
#include "Cart.h"


class wxkitFrame: public wxFrame {
	Database db;

	std::vector<Category> categories;
	std::vector<Product> displayedProducts;
	std::vector<Product> displayedCompatibleProducts;

	std::unique_ptr<wxStaticText> prodListLabel;
	std::unique_ptr<wxStaticText> compatListLabel;

	std::unique_ptr<wxChoice> categoryList;
	std::unique_ptr<wxListCtrl> productList;
	std::unique_ptr<wxListCtrl> compatList;

	std::unique_ptr<wxChoice> sortCriterion;
	std::unique_ptr<wxCheckBox> sortOrder;

	std::unique_ptr<wxButton> addProductButton;
	std::unique_ptr<wxButton> addCompatButton;
	std::unique_ptr<wxButton> showCartButton;

	Cart cart;

	long nameCol, descCol, pCatCol, prceCol;

	void reloadProductList();
	void reloadCompatibleList();
	void populateListFromVector(
		const std::unique_ptr<wxListCtrl> &listCtrl,
		const std::vector<Product> &v
	);

	void sortVectorWRTUserPref(std::vector<Product> *pV);

public:
	wxkitFrame(wxWindow *parent, wxWindowID ID = -1);
	virtual ~wxkitFrame();
};

#endif // WXKITFRAME_H

