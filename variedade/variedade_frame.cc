#include "variedade/variedade_frame.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/log.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <yaml-cpp/yaml.h>

#include "variedade/get_config_dir.h"
#include "variedade/variedade_tray.h"
#include "variedade/source.h"

// clang-format off
wxBEGIN_EVENT_TABLE(VariedadeFrame, wxFrame)
  EVT_BUTTON(wxID_CLOSE, VariedadeFrame::OnExit)
  EVT_CLOSE(VariedadeFrame::OnCloseWindow)
wxEND_EVENT_TABLE();
// clang-format on

VariedadeFrame::VariedadeFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title) {
  auto notebook = new wxNotebook(this, wxID_ANY);
  notebook->AddPage(CreateGeneralPage(notebook), "General");

  auto close = new wxButton(this, wxID_CLOSE, "&Close");

  wxSizerFlags flags;
  flags.Border(wxALL, 10);

  wxSizer* const sizer_top = new wxBoxSizer(wxVERTICAL);
  sizer_top->Add(notebook, wxSizerFlags(1).Expand());
  sizer_top->Add(close, flags.Right());
  sizer_top->SetMinSize(650, 450);
  SetSizerAndFit(sizer_top);
  Centre();

  task_bar_icon_ = new VariedadeTray(this);
  if (!task_bar_icon_->SetIcon(wxICON(variedade),
                               "Variedade Wallpaper Manager")) {
    wxLogError("Could not set icon.");
  }

#if defined(__WXOSX__) && wxOSX_USE_COCOA
  dock_icon_ = new VariedadeTray(this, wxTBI_DOCK);
  if (!dock_icon_->SetIcon(wxICON(variedade))) {
    wxLogError("Could not set icon.");
  }
#endif
}

VariedadeFrame::~VariedadeFrame() { delete task_bar_icon_; }

wxWindow* VariedadeFrame::CreateGeneralPage(wxWindow* parent) {
  auto config_file = get_config_dir() / "config.yaml";
  YAML::Node config = YAML::LoadFile(config_file.string());

  wxSizerFlags flags = wxSizerFlags().Border().Centre();

  wxPanel* page = new wxPanel(parent);
  auto sizer_change = new wxBoxSizer(wxHORIZONTAL);
  sizer_change->Add(new wxCheckBox(page, wxID_ANY, "Change wallpaper every"),
                    flags);
  auto timer = config["timer"];
  sizer_change->Add(new wxTextCtrl(page, wxID_ANY, timer["value"].as<std::string>("2")), flags);
  wxString items[] = {"seconds", "minutes"};
  auto choice =
      new wxChoice(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, items);
  int selection = timer["unit"].as<std::string>("minutes") == "minutes";
  choice->SetSelection(selection);
  sizer_change->Add(choice, flags);

  wxSizer* sizer_v = new wxBoxSizer(wxVERTICAL);
  sizer_v->Add(sizer_change);

  auto lc = new wxDataViewListCtrl(page, wxID_ANY);
  lc->AppendToggleColumn("Enabled");
  lc->AppendTextColumn("Type");
  lc->AppendTextColumn("Location");

  if (config["sources"]) {
    for (auto source : config["sources"]) {
      wxVector<wxVariant> data;
      data.push_back(source["enabled"].as<bool>());
      data.push_back(source["type"].as<std::string>());
      data.push_back(source["location"].as<std::string>());
      lc->AppendItem(data);
    }
  }

  lc->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED,
           &VariedadeFrame::OnListValueChanged, this);

  auto add = new wxButton(page, wxID_ADD, "&Add...");
  auto edit = new wxButton(page, wxID_EDIT, "&Edit...");
  auto remove = new wxButton(page, wxID_REMOVE, "&Remove...");
  auto sizer_b = new wxBoxSizer(wxVERTICAL);
  sizer_b->Add(add);
  sizer_b->Add(edit);
  sizer_b->Add(remove);
  sizer_change = new wxBoxSizer(wxHORIZONTAL);
  sizer_change->Add(lc, wxSizerFlags(1).Expand());
  sizer_change->Add(sizer_b,
                    wxSizerFlags().Right().Border(wxLEFT | wxRIGHT, 5));
  sizer_v->Add(sizer_change, wxSizerFlags(1).Expand());
  page->SetSizer(sizer_v);
  return page;
}

void VariedadeFrame::OnListValueChanged(wxDataViewEvent& event) {}

void VariedadeFrame::OnClose(wxCommandEvent& WXUNUSED(event)) { Show(false); }

void VariedadeFrame::OnExit(wxCommandEvent& WXUNUSED(event)) { Close(true); }

void VariedadeFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event)) { Destroy(); }
