#include "variedade/variedade_tray.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>

#include "variedade/variedade_frame.h"

enum {
  PU_RESTORE = 10001,
  PU_NEW_ICON,
  PU_EXIT,
  PU_CHECKMARK,
  PU_SUB1,
  PU_SUB2,
  PU_SUBMAIN
};

// clang-format off
wxBEGIN_EVENT_TABLE(VariedadeTray, wxTaskBarIcon)
  EVT_MENU(PU_RESTORE, VariedadeTray::OnMenuRestore)
  EVT_MENU(PU_EXIT, VariedadeTray::OnMenuExit)
  EVT_MENU(PU_CHECKMARK, VariedadeTray::OnMenuCheckmark)
  EVT_UPDATE_UI(PU_CHECKMARK, VariedadeTray::OnMenuUICheckmark)
  EVT_TASKBAR_LEFT_DCLICK(VariedadeTray::OnLeftButtonDClick)
  EVT_MENU(PU_SUB1, VariedadeTray::OnMenuSub)
  EVT_MENU(PU_SUB2, VariedadeTray::OnMenuSub)
wxEND_EVENT_TABLE();
// clang-format on

void VariedadeTray::OnMenuRestore(wxCommandEvent&) { frame_->Show(true); }

void VariedadeTray::OnMenuExit(wxCommandEvent&) { frame_->Close(true); }

static bool check = true;

void VariedadeTray::OnMenuCheckmark(wxCommandEvent&) { check = !check; }

void VariedadeTray::OnMenuUICheckmark(wxUpdateUIEvent& event) {
  event.Check(check);
}

void VariedadeTray::OnMenuSub(wxCommandEvent&) {
  wxMessageBox("You clicked on a submenu!");
}

// Overridables
wxMenu* VariedadeTray::CreatePopupMenu() {
  wxMenu* menu = new wxMenu;
  menu->Append(PU_RESTORE, "&Restore main window");
  menu->AppendSeparator();
  menu->Append(PU_NEW_ICON, "&Set New Icon");
  menu->AppendSeparator();
  menu->AppendCheckItem(PU_CHECKMARK, "Test &check mark");
  menu->AppendSeparator();
  wxMenu* submenu = new wxMenu;
  submenu->Append(PU_SUB1, "One submenu");
  submenu->AppendSeparator();
  submenu->Append(PU_SUB2, "Another submenu");
  menu->Append(PU_SUBMAIN, "Submenu", submenu);
  /* OSX has built-in quit menu for the dock menu, but not for the status item
   */
#ifdef __WXOSX__
  if (OSXIsStatusItem())
#endif
  {
    menu->AppendSeparator();
    menu->Append(PU_EXIT, "E&xit");
  }
  return menu;
}

void VariedadeTray::OnLeftButtonDClick(wxTaskBarIconEvent&) {
  frame_->Show(true);
}
