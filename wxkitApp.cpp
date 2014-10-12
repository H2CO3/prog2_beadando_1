// No way I am regenerating this stuff with the editor!

#include "wxkitApp.h"

#include "wxkitFrame.h"
#include <wx/image.h>

IMPLEMENT_APP(wxkitApp);

bool wxkitApp::OnInit()
{
	bool wxsOK = true;
	wxInitAllImageHandlers();

    if (wxsOK) {
		wxkitFrame *Frame = new wxkitFrame(nullptr);
		Frame->Show();
		this->SetTopWindow(Frame);
    }

    return wxsOK;
}
