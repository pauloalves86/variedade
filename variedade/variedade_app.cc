#include "variedade/variedade_app.h"

#include <wx/wx.h>
#include <wx/taskbar.h>
#include <wx/msgdlg.h>

#include "variedade/variedade_frame.h"

wxIMPLEMENT_APP(VariedadeApp);

bool VariedadeApp::OnInit() {
  if (!wxApp::OnInit()) return false;

  auto frame = new VariedadeFrame("Variedade Preferences");
  frame->Show(true);

  return true;
}
