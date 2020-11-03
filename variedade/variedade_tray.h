#pragma once
#include <wx/taskbar.h>

class VariedadeFrame;

class VariedadeTray : public wxTaskBarIcon {
  VariedadeFrame* frame_;

 public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
  VariedadeTray(VariedadeFrame* frame, wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE)
      : wxTaskBarIcon(iconType), frame_(frame)
#else
  VariedadeTray(VariedadeFrame* frame)
      : frame_(frame)
#endif
  {
  }

  void OnLeftButtonDClick(wxTaskBarIconEvent&);
  void OnMenuRestore(wxCommandEvent&);
  void OnMenuExit(wxCommandEvent&);
  void OnMenuCheckmark(wxCommandEvent&);
  void OnMenuUICheckmark(wxUpdateUIEvent&);
  void OnMenuSub(wxCommandEvent&);
  virtual wxMenu* CreatePopupMenu() wxOVERRIDE;

  wxDECLARE_EVENT_TABLE();
};
