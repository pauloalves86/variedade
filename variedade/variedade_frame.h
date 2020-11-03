#pragma once
#include <wx/frame.h>
#include <wx/dataview.h>

class VariedadeTray;

class VariedadeFrame : public wxFrame {
 public:
  VariedadeFrame(const wxString& title);
  virtual ~VariedadeFrame();

 protected:
  void OnClose(wxCommandEvent& event);
  void OnExit(wxCommandEvent& event);
  void OnCloseWindow(wxCloseEvent& event);
  wxWindow* CreateGeneralPage(wxWindow* parent);
  void OnListValueChanged(wxDataViewEvent& event);

  VariedadeTray* task_bar_icon_;
#if defined(__WXOSX__) && wxOSX_USE_COCOA
  MyTaskBarIcon* dock_icon_;
#endif

  wxDECLARE_EVENT_TABLE();
};
