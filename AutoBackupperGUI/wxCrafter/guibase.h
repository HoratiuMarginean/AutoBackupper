//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: AutoBackupper.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#ifndef _AUTOBACKUPPERGUI_WXCRAFTER_AUTOBACKUPPER_BASE_CLASSES_H
#define _AUTOBACKUPPERGUI_WXCRAFTER_AUTOBACKUPPER_BASE_CLASSES_H

#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/dialog.h>
#include <wx/iconbndl.h>
#include <wx/artprov.h>
#include <wx/sizer.h>
#include <wx/radiobox.h>
#include <wx/statbox.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/dateevt.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/filepicker.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/arrstr.h>
#include <wx/checkbox.h>
#include <wx/listctrl.h>
#if wxVERSION_NUMBER >= 2900
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/treebook.h>
#endif

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif


class NewTriggerDialogBase : public wxDialog
{
protected:
    wxRadioBox* radioBoxFrequency;
    wxDatePickerCtrl* datePickerStart;
    wxTimePickerCtrl* timePickerStart;
    wxPanel* panelRecurrenceDays;
    wxStaticText* staticTextRecurrence;
    wxSpinCtrl* spinCtrlRecurrence;
    wxStaticText* staticTextRecurrenceUnit;
    wxCheckListBox* checkListBoxDays;
    wxButton* buttonAdd;
    wxButton* buttonCancel;

protected:
    virtual void OnCloseWindow(wxCloseEvent& event) { event.Skip(); }
    virtual void OnFrequencyRadioboxSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDaysCheckListBoxToggled(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAddButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCancelButtonClicked(wxCommandEvent& event) { event.Skip(); }

public:
    wxRadioBox* GetRadioBoxFrequency() { return radioBoxFrequency; }
    wxDatePickerCtrl* GetDatePickerStart() { return datePickerStart; }
    wxTimePickerCtrl* GetTimePickerStart() { return timePickerStart; }
    wxStaticText* GetStaticTextRecurrence() { return staticTextRecurrence; }
    wxSpinCtrl* GetSpinCtrlRecurrence() { return spinCtrlRecurrence; }
    wxStaticText* GetStaticTextRecurrenceUnit() { return staticTextRecurrenceUnit; }
    wxCheckListBox* GetCheckListBoxDays() { return checkListBoxDays; }
    wxPanel* GetPanelRecurrenceDays() { return panelRecurrenceDays; }
    wxButton* GetButtonAdd() { return buttonAdd; }
    wxButton* GetButtonCancel() { return buttonCancel; }
    NewTriggerDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add a new trigger"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP);
    virtual ~NewTriggerDialogBase();
};


class SettingsFrameBase : public wxFrame
{
protected:
    wxNotebook* notebookSettings;
    wxPanel* panelArchive;
    wxNotebook* notebookArchiveSettings;
    wxPanel* panelGeneral;
    wxStaticText* staticTextDestinationPath;
    wxDirPickerCtrl* dirPickerDestinationPath;
    wxStaticText* staticTextArchiveName;
    wxTextCtrl* textCtrlArchiveName;
    wxStaticText* staticTextUnderscore1;
    wxTextCtrl* textCtrlDate;
    wxStaticText* staticTextUnderscore2;
    wxTextCtrl* textCtrlTime;
    wxChoice* choiceCompressionLevel;
    wxChoice* choiceUpdateMode;
    wxPanel* panelAdvanced;
    wxRadioBox* radioBoxFormat;
    wxChoice* choiceCompressionMethod;
    wxChoice* choiceDictionarySize;
    wxChoice* choiceThreadCount;
    wxTextCtrl* textCtrlVolumeSize;
    wxChoice* choiceVolumeSizeUnit;
    wxCheckBox* checkBoxSolidMode;
    wxPanel* panelSchedule;
    wxStaticText* staticTextTriggerList;
    wxListCtrl* triggerList;
    wxButton* buttonNew;
    wxButton* buttonRemove;
    wxButton* buttonSave;
    wxButton* buttonCancel;

protected:
    virtual void OnCloseWindow(wxCloseEvent& event) { event.Skip(); }
    virtual void OnDestinationPathDirPickerChanged(wxFileDirPickerEvent& event) { event.Skip(); }
    virtual void OnArchiveNameTextUpdated(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCompressionLevelChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnUpdateModeChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnFormatRadioBoxSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCompressionMethodChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnDictionarySizeChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnThreadCountChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnVolumeSizeTextUpdated(wxCommandEvent& event) { event.Skip(); }
    virtual void OnVolumeSizeUnitChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSolidModeCheckboxClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnTriggerListItemSelected(wxListEvent& event) { event.Skip(); }
    virtual void OnTriggerListItemDeselected(wxListEvent& event) { event.Skip(); }
    virtual void OnNewButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnRemoveButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSaveButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCancelButtonClicked(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticTextDestinationPath() { return staticTextDestinationPath; }
    wxDirPickerCtrl* GetDirPickerDestinationPath() { return dirPickerDestinationPath; }
    wxStaticText* GetStaticTextArchiveName() { return staticTextArchiveName; }
    wxTextCtrl* GetTextCtrlArchiveName() { return textCtrlArchiveName; }
    wxStaticText* GetStaticTextUnderscore1() { return staticTextUnderscore1; }
    wxTextCtrl* GetTextCtrlDate() { return textCtrlDate; }
    wxStaticText* GetStaticTextUnderscore2() { return staticTextUnderscore2; }
    wxTextCtrl* GetTextCtrlTime() { return textCtrlTime; }
    wxChoice* GetChoiceCompressionLevel() { return choiceCompressionLevel; }
    wxChoice* GetChoiceUpdateMode() { return choiceUpdateMode; }
    wxPanel* GetPanelGeneral() { return panelGeneral; }
    wxRadioBox* GetRadioBoxFormat() { return radioBoxFormat; }
    wxChoice* GetChoiceCompressionMethod() { return choiceCompressionMethod; }
    wxChoice* GetChoiceDictionarySize() { return choiceDictionarySize; }
    wxChoice* GetChoiceThreadCount() { return choiceThreadCount; }
    wxTextCtrl* GetTextCtrlVolumeSize() { return textCtrlVolumeSize; }
    wxChoice* GetChoiceVolumeSizeUnit() { return choiceVolumeSizeUnit; }
    wxCheckBox* GetCheckBoxSolidMode() { return checkBoxSolidMode; }
    wxPanel* GetPanelAdvanced() { return panelAdvanced; }
    wxNotebook* GetNotebookArchiveSettings() { return notebookArchiveSettings; }
    wxPanel* GetPanelArchive() { return panelArchive; }
    wxStaticText* GetStaticTextTriggerList() { return staticTextTriggerList; }
    wxListCtrl* GetTriggerList() { return triggerList; }
    wxButton* GetButtonNew() { return buttonNew; }
    wxButton* GetButtonRemove() { return buttonRemove; }
    wxPanel* GetPanelSchedule() { return panelSchedule; }
    wxNotebook* GetNotebookSettings() { return notebookSettings; }
    wxButton* GetButtonSave() { return buttonSave; }
    wxButton* GetButtonCancel() { return buttonCancel; }
    SettingsFrameBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxCAPTION|wxMINIMIZE_BOX|wxCLOSE_BOX);
    virtual ~SettingsFrameBase();
};


class ManualBackupDialogBase : public wxDialog
{
protected:
    wxNotebook* notebookArchiveSettings;
    wxPanel* panelGeneralSettings;
    wxStaticText* staticTextDestinationPath;
    wxDirPickerCtrl* dirPickerDestinationPath;
    wxStaticText* staticTextArchiveName;
    wxTextCtrl* textCtrlArchiveName;
    wxStaticText* staticTextUnderscore1;
    wxTextCtrl* textCtrlDate;
    wxStaticText* staticTextUnderscore2;
    wxTextCtrl* textCtrlTime;
    wxChoice* choiceCompressionLevel;
    wxChoice* choiceUpdateMode;
    wxPanel* panelAdvancedSettings;
    wxRadioBox* radioBoxFormat;
    wxChoice* choiceCompressionMethod;
    wxChoice* choiceDictionarySize;
    wxChoice* choiceThreadCount;
    wxTextCtrl* textCtrlVolumeSize;
    wxChoice* choiceVolumeSizeUnit;
    wxCheckBox* checkBoxSolidMode;
    wxButton* buttonGo;
    wxButton* buttonCancel;

protected:
    virtual void OnCloseWindow(wxCloseEvent& event) { event.Skip(); }
    virtual void OnFormatRadioBoxSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCompressionMethodChoiceSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnGoButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnCancelButtonClicked(wxCommandEvent& event) { event.Skip(); }

public:
    wxStaticText* GetStaticTextDestinationPath() { return staticTextDestinationPath; }
    wxDirPickerCtrl* GetDirPickerDestinationPath() { return dirPickerDestinationPath; }
    wxStaticText* GetStaticTextArchiveName() { return staticTextArchiveName; }
    wxTextCtrl* GetTextCtrlArchiveName() { return textCtrlArchiveName; }
    wxStaticText* GetStaticTextUnderscore1() { return staticTextUnderscore1; }
    wxTextCtrl* GetTextCtrlDate() { return textCtrlDate; }
    wxStaticText* GetStaticTextUnderscore2() { return staticTextUnderscore2; }
    wxTextCtrl* GetTextCtrlTime() { return textCtrlTime; }
    wxChoice* GetChoiceCompressionLevel() { return choiceCompressionLevel; }
    wxChoice* GetChoiceUpdateMode() { return choiceUpdateMode; }
    wxPanel* GetPanelGeneralSettings() { return panelGeneralSettings; }
    wxRadioBox* GetRadioBoxFormat() { return radioBoxFormat; }
    wxChoice* GetChoiceCompressionMethod() { return choiceCompressionMethod; }
    wxChoice* GetChoiceDictionarySize() { return choiceDictionarySize; }
    wxChoice* GetChoiceThreadCount() { return choiceThreadCount; }
    wxTextCtrl* GetTextCtrlVolumeSize() { return textCtrlVolumeSize; }
    wxChoice* GetChoiceVolumeSizeUnit() { return choiceVolumeSizeUnit; }
    wxCheckBox* GetCheckBoxSolidMode() { return checkBoxSolidMode; }
    wxPanel* GetPanelAdvancedSettings() { return panelAdvancedSettings; }
    wxNotebook* GetNotebookArchiveSettings() { return notebookArchiveSettings; }
    wxButton* GetButtonGo() { return buttonGo; }
    wxButton* GetButtonCancel() { return buttonCancel; }
    ManualBackupDialogBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Manual Backup"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1,-1), long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~ManualBackupDialogBase();
};


class MainFrameBase : public wxFrame
{
protected:
    wxListCtrl* mainList;
    wxButton* buttonArchive;
    wxButton* buttonAddDirectory;
    wxButton* buttonAddFile;
    wxButton* buttonRemove;
    wxButton* buttonSettings;

protected:
    virtual void OnMainListItemSelected(wxListEvent& event) { event.Skip(); }
    virtual void OnMainListItemDeselected(wxListEvent& event) { event.Skip(); }
    virtual void OnArchiveButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAddDirectoryButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnAddFileButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnRemoveButtonClicked(wxCommandEvent& event) { event.Skip(); }
    virtual void OnSettingsButtonClicked(wxCommandEvent& event) { event.Skip(); }

public:
    wxListCtrl* GetMainList() { return mainList; }
    wxButton* GetButtonArchive() { return buttonArchive; }
    wxButton* GetButtonAddDirectory() { return buttonAddDirectory; }
    wxButton* GetButtonAddFile() { return buttonAddFile; }
    wxButton* GetButtonRemove() { return buttonRemove; }
    wxButton* GetButtonSettings() { return buttonSettings; }
    MainFrameBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("AutoBackupper"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640,360), long style = wxCAPTION|wxMINIMIZE_BOX|wxCLOSE_BOX);
    virtual ~MainFrameBase();
};

#endif