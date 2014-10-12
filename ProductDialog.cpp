//
// ProductDialog.cpp
// Prog II Beadando 1
// Created by H2CO3 on 11/10/2014
// No copyright, this is homework
//

#include "ProductDialog.h"
#include <wx/sizer.h>

#include <string>

ProductDialog::ProductDialog(
	wxWindow *parent,
	wxWindowID ID,
	const std::unordered_set<Category> &pCats
):
	wxDialog(parent, ID, "Add Product"),
	categories(pCats.begin(), pCats.end()),
	nameLabel(new wxStaticText(this, wxID_ANY, "Name:")),
	descLabel(new wxStaticText(this, wxID_ANY, "Description:")),
	priceLabel(new wxStaticText(this, wxID_ANY, "Price:")),
	categLabel(new wxStaticText(this, wxID_ANY, "Category:")),
	name(new wxTextCtrl(this, wxID_ANY, "")),
	desc(new wxTextCtrl(this, wxID_ANY, "")),
	price(new wxTextCtrl(this, wxID_ANY, "")),
	category(new wxChoice(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		categories.size(),
		std::vector<wxString>(categories.size()).data()
		)
	)
{
	// Compute geometry
	// Set up standard <OK> and <Cancel> buttons
	int w = 500, h = 300, wOK, hOK, wCancel, hCancel;

	auto sizer = this->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
	wxButton *okButton = sizer->GetAffirmativeButton();
	wxButton *cancelButton = sizer->GetCancelButton();
	okButton->GetSize(&wOK, &hOK);
	cancelButton->GetSize(&wCancel, &hCancel);

	int x0 = 30, y0 = 20, x1 = 150, bottomPad = 45;
	int vPad = (h - hOK - bottomPad - y0) / 4;
	int hPad = 30;

	okButton->SetPosition(wxPoint(x0, h - bottomPad - hOK));	
	cancelButton->SetPosition(wxPoint(x0 + wOK + hPad, h - bottomPad - hCancel));

	this->SetSize(wxSize(w, h));
	this->Center();

	// Set up labels and controls
	// nameLabel = new wxStaticText(this, wxID_ANY, "Name:", wxPoint(x0, y0 + 0 * vPad));
	nameLabel->SetPosition(wxPoint(x0, y0 + 0 * vPad));
	descLabel->SetPosition(wxPoint(x0, y0 + 1 * vPad));
	priceLabel->SetPosition(wxPoint(x0, y0 + 2 * vPad));
	categLabel->SetPosition(wxPoint(x0, y0 + 3 * vPad));
	
	int defHeight;
	name->GetSize(nullptr, &defHeight);

	name->SetPosition(wxPoint(x1, y0 + 0 * vPad));
	name->SetSize(wxSize(w - x1 - hPad, defHeight));
	desc->SetPosition(wxPoint(x1, y0 + 1 * vPad));
	desc->SetSize(wxSize(w - x1 - hPad, defHeight));
	price->SetPosition(wxPoint(x1, y0 + 2 * vPad));
	price->SetSize(wxSize(w - x1 - hPad, defHeight));
	category->SetPosition(wxPoint(x1, y0 + 3 * vPad));
	category->SetSize(wxSize(w - x1 - hPad, defHeight));

	// Populate category control
	std::size_t i = 0;
	for (const auto &p : categories) {
		category->SetString(i, p.name);
		i++;
	}
}

std::string ProductDialog::getName() const
{
	return std::string(name->GetValue());
}

std::string ProductDialog::getDescription() const
{
	return std::string(desc->GetValue());
}

double ProductDialog::getPrice() const
{
	try {
		return std::stod(std::string(price->GetValue()));
	} catch (const std::exception &e) {
		return 0.0;
	}
}

const Category &ProductDialog::getCategory() const
{
	return categories[category->GetSelection()];
}

