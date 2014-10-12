//
// ProductDialog.h
// Prog II Beadando 1
// Created by H2CO3 on 11/10/2014
// No copyright, this is homework
//

#ifndef BEADANDO1_PRODUCTDIALOG_H
#define BEADANDO1_PRODUCTDIALOG_H

#include <vector>
#include <unordered_set>
#include <memory>

#include "Category.h"

#include <wx/frame.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/stattext.h>


class ProductDialog: public wxDialog {
	const std::vector<Category> categories;

	std::unique_ptr<wxStaticText> nameLabel;
	std::unique_ptr<wxStaticText> descLabel;
	std::unique_ptr<wxStaticText> priceLabel;
	std::unique_ptr<wxStaticText> categLabel;

	std::unique_ptr<wxTextCtrl> name;
	std::unique_ptr<wxTextCtrl> desc;
	std::unique_ptr<wxTextCtrl> price;
	std::unique_ptr<wxChoice> category;

public:
	ProductDialog(wxWindow *parent, wxWindowID ID, const std::unordered_set<Category> &pCats);

	std::string getName() const;
	std::string getDescription() const;
	double getPrice() const;
	const Category &getCategory() const;
};

#endif // BEADANDO1_PRODUCTDIALOG_H

