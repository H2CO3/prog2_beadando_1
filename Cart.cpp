//
// Cart.cpp
// Prog II Beadando 1
// Created by H2CO3 on 12/10/2014
// No copyright, this is homework
//

#include "Cart.h"

Cart::Cart(wxWindow *parent, wxWindowID ID):
	wxDialog(parent, ID, "Shopping Cart"),
	productList(new wxListCtrl(this, wxID_ANY, wxPoint(160, 20), wxSize(300, 220), wxLC_REPORT | wxLC_SINGLE_SEL)),
	total(new wxStaticText(this, wxID_ANY, "Total: 0.00", wxPoint(20, 140))),
	deleteButton(new wxButton(this, wxID_ANY, "Delete Selected", wxPoint(20, 20))),
	deleteAllButton(new wxButton(this, wxID_ANY, "Delete All", wxPoint(20, 80)))
{
	nameCol = productList->AppendColumn("Name", wxLIST_FORMAT_LEFT, 200);
	prceCol = productList->AppendColumn("Price");

	deleteButton->Bind(wxEVT_BUTTON, [&](const wxEvent &evt) {
		if (productList->GetSelectedItemCount() > 0) {
			long row = productList->GetNextItem(-1, wxLIST_NEXT_BELOW, wxLIST_STATE_SELECTED);
			assert(row < products.size());
			products.erase(products.begin() + row);
			this->reloadTable();
		}
	});

	deleteAllButton->Bind(wxEVT_BUTTON, [&](const wxEvent &evt) {
		products.clear();
		this->reloadTable();
	});

	this->SetSize(wxSize(500, 300));
}

Cart::~Cart()
{
}

void Cart::reloadTable()
{
	productList->DeleteAllItems();
	double price = 0.0;

	for (std::size_t i = 0; i < products.size(); i++) {
		productList->InsertItem(i, "product");
		productList->SetItem(i, nameCol, products[i].name);
		productList->SetItem(i, prceCol, wxString::Format("%.2lf", products[i].price));
		price += products[i].price;
	}

	total->SetLabel(wxString::Format("Total: %.2f", price));
}

void Cart::addProduct(const Product &p)
{
	products.push_back(p);
	this->reloadTable();
}

