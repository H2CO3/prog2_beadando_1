// wxkitFrame.cpp
// Beadando 1
// Created by H2CO3 on 10/10/2014
// No copyright, this is homework

// std headers
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <strings.h> // for strcasecmp
#include <algorithm>

// WX headers
#include "wxkitFrame.h"
#include "ProductDialog.h"

#include <wx/msgdlg.h>
#include <wx/slider.h>
#include <wx/gauge.h>
#include <wx/event.h>
#include <wx/dcclient.h>
#include <wx/stc/stc.h>

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/event.h>
#include <wx/timer.h>

#define MENUID_QUIT (wxID_HIGHEST + 1)

enum ProductSortCriterion {
	SORT_BY_NAME,
	SORT_BY_PRICE,
	N_ProductSortCriterion
};


// performs bi-directional case insensitive string comparison
static bool stdStringICmp(const std::string &s, const std::string &t, bool ascending)
{
	return strcasecmp(s.c_str(), t.c_str()) * (ascending ? +1 : -1) < 0;
}


wxkitFrame::wxkitFrame(wxWindow *parent, wxWindowID ID):
	wxFrame(parent, ID, "Storage Manager"),
	db("storage.tmp.sql"),
	categories(db.getCategories().begin(), db.getCategories().end()),
	prodListLabel(new wxStaticText(this, wxID_ANY, "Products in Category:", wxPoint(200, 30))),
	compatListLabel(new wxStaticText(this, wxID_ANY, "Compatible Products:", wxPoint(700, 30))),
	categoryList(new wxChoice(
		this,
		wxID_ANY,
		wxPoint(30, 60),
		wxSize(130, 20),
		categories.size() + 1,
		std::vector<wxString>(categories.size() + 1).data()	
		)
	),
	productList(new wxListCtrl(this, wxID_ANY, wxPoint(200, 60), wxSize(450, 470), wxLC_REPORT)),
	compatList(new wxListCtrl(this, wxID_ANY, wxPoint(700, 60), wxSize(450, 470), wxLC_REPORT)),
	sortCriterion(new wxChoice(
		this,
		wxID_ANY,
		wxPoint(30, 110),
		wxSize(130, 20),
		N_ProductSortCriterion,
		(std::vector<wxString> { "Sort by Name", "Sort by Price" }).data()
		)
	),
	sortOrder(new wxCheckBox(this, wxID_ANY, "Ascending", wxPoint(30, 160))),
	addProductButton(new wxButton(this, wxID_ANY, "Add New Product", wxPoint(30, 260))),
	addCompatButton(new wxButton(this, wxID_ANY, "Make Compatible", wxPoint(30, 310))),
	showCartButton(new wxButton(this, wxID_ANY, "Show Cart", wxPoint(30, 410))),
	cart(this, wxID_ANY)
{
	// Set up menu
	auto menuBar = new wxMenuBar();

	auto fileMenu = new wxMenu();
	fileMenu->Append(MENUID_QUIT, "&Quit\tCTRL+Q");
	menuBar->Append(fileMenu, "File");
	this->SetMenuBar(menuBar);

	fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [&](const wxEvent &evt) {
		this->Close();
	});

	// Set up product and other lists
	// Populate category control
	std::size_t i = 0;
	for (const auto &p : db.getCategories()) {
		categoryList->SetString(i, p.name);
		i++;
	}

	categoryList->SetString(i, "All Categories");
	categoryList->Bind(wxEVT_CHOICE, [&](const wxCommandEvent &evt) {
		this->reloadProductList();
	});

	// Set up product list
	nameCol = productList->AppendColumn("Name", wxLIST_FORMAT_LEFT, 140);
	descCol = productList->AppendColumn("Description", wxLIST_FORMAT_LEFT, 100);
	pCatCol = productList->AppendColumn("Category", wxLIST_FORMAT_LEFT, 120);
	prceCol = productList->AppendColumn("Price", wxLIST_FORMAT_LEFT);


	auto listSelectHndlr = [&](const wxEvent &evt) {
		this->reloadCompatibleList();
		if (productList->GetSelectedItemCount() == 2) {
			addCompatButton->Enable();
		} else {
			addCompatButton->Disable();
		}
	};

	productList->Bind(wxEVT_LIST_ITEM_SELECTED, listSelectHndlr);
	productList->Bind(wxEVT_LIST_ITEM_DESELECTED, listSelectHndlr);

	// Double-click adds to the cart
	productList->Bind(wxEVT_LIST_ITEM_ACTIVATED, [&](const wxListEvent &evt) {
		const auto &p = displayedProducts[evt.GetIndex()];
		cart.addProduct(p);
		wxMessageBox(wxString::Format("'%s' has been added to the shopping cart.", p.name.c_str()));
	});

	// Set up list of compatible products
	compatList->AppendColumn("Name", wxLIST_FORMAT_LEFT, 140);
	compatList->AppendColumn("Description", wxLIST_FORMAT_LEFT, 100);
	compatList->AppendColumn("Category", wxLIST_FORMAT_LEFT, 120);
	compatList->AppendColumn("Price", wxLIST_FORMAT_LEFT);

	// This adds a new product to the database
	addProductButton->Bind(wxEVT_BUTTON, [&](const wxEvent &evt) {
		ProductDialog dlg(this, wxID_ANY, db.getCategories());
		if (dlg.ShowModal() == wxID_OK) {
			db.insertProduct(
				dlg.getName(),
				dlg.getDescription(),
				dlg.getCategory(),
				dlg.getPrice()
			);

			// Reload table view
			this->reloadProductList();
		}
	});

	addCompatButton->Bind(wxEVT_BUTTON, [&](const wxEvent &evt) {
		// make the two selected products compatible
		assert(productList->GetSelectedItemCount() == 2);
		long idx1 = productList->GetNextItem(-1, wxLIST_NEXT_BELOW, wxLIST_STATE_SELECTED);
		long idx2 = productList->GetNextItem(idx1, wxLIST_NEXT_BELOW, wxLIST_STATE_SELECTED);

		auto p = displayedProducts[idx1];
		auto q = displayedProducts[idx2];

		db.makeProductsCompatible(p, q);
		this->reloadProductList();

		wxMessageBox(wxString::Format("'%s' and '%s' have been marked as compatible.",
			p.name.c_str(), q.name.c_str()));
	});

	// By default, no items are selected, so they can't be
	// made compatible.
	addCompatButton->Disable();

	showCartButton->Bind(wxEVT_BUTTON, [&](const wxEvent &evt) {
		cart.Center();
		cart.ShowModal();
	});

	sortCriterion->Bind(wxEVT_CHOICE, [&](const wxEvent &evt) {
		this->reloadProductList();
	});

	sortOrder->Bind(wxEVT_CHECKBOX, [&](const wxEvent &evt) {
		this->reloadProductList();
	});

	// check the 'ascending' check box, sort 'normally' by default
	sortOrder->SetValue(true);

	this->reloadProductList();

	this->SetSize(wxSize(1200, 600));
	this->Center();
}

wxkitFrame::~wxkitFrame()
{
}

void wxkitFrame::reloadProductList()
{
	displayedProducts.clear();

	// Get category
	int idx = categoryList->GetSelection();
	if (idx < categories.size()) {
		const Category &cat = categories[idx];
		for (const auto &p : db.getProducts()) {
			if (p.category_ID == cat.ID) {
				displayedProducts.push_back(p);
			}
		}
	} else {
		// show all from all categories
		displayedProducts = std::vector<Product>(db.getProducts().begin(), db.getProducts().end());
	}

	// Sort the product list
	this->sortVectorWRTUserPref(&displayedProducts);

	// Display them in the list control
	this->populateListFromVector(productList, displayedProducts);

	this->reloadCompatibleList();
}

void wxkitFrame::reloadCompatibleList()
{
	std::vector<Product> selectedProducts;

	// obtain all selected items by iterating through the list
	long row = -1;
	while ((row = productList->GetNextItem(
		row,
		wxLIST_NEXT_BELOW,
		wxLIST_STATE_SELECTED
		)
	) >= 0) {
		selectedProducts.push_back(displayedProducts[row]);
	}

	// iterate through all products, filter for those which
	// are compatible with **each of** the selected ones
	displayedCompatibleProducts.clear();
	for (const auto &p : db.getProducts()) {
		// we need 'not selectedProducts.empty()' because 'std::all_of()'
		// returns true if the conatiner is empty, but in that case,
		// we don't want to display *any* of the products in the
		// compatibility list...
		if (not selectedProducts.empty() and std::all_of(
			selectedProducts.begin(),
			selectedProducts.end(),
			[&](const Product &q) {
				return db.productsCompatible(p, q);
			}
		)) {
			displayedCompatibleProducts.push_back(p);
		}
	}

	// Sort list of compatible products
	this->sortVectorWRTUserPref(&displayedCompatibleProducts);
	this->populateListFromVector(compatList, displayedCompatibleProducts);
}

void wxkitFrame::populateListFromVector(
	const std::unique_ptr<wxListCtrl> &listCtrl,
	const std::vector<Product> &v
)
{
	listCtrl->DeleteAllItems();
	long row = 0;
	for (const auto &p : v) {
		listCtrl->InsertItem(row, "Product");

		listCtrl->SetItem(row, nameCol, p.name);
		listCtrl->SetItem(row, descCol, p.description);
		listCtrl->SetItem(row, pCatCol, db.categoryByID(p.category_ID).name);
		listCtrl->SetItem(row, prceCol, wxString::Format("%.2lf", p.price));

		row++;
	}
}

void wxkitFrame::sortVectorWRTUserPref(std::vector<Product> *pV)
{
	bool ascending = sortOrder->GetValue();
	
	std::sort(pV->begin(), pV->end(), [&](const Product &p, const Product &q) {
		switch (sortCriterion->GetSelection()) {
		case SORT_BY_NAME:
			return stdStringICmp(p.name, q.name, ascending);
		case SORT_BY_PRICE:
			return (p.price - q.price) * (ascending ? +1 : -1) < 0;
		default:
			// this code path must never be reached
			throw std::runtime_error("should not be reached");
			break;
		}
		return false;
	});
}

