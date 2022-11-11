#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/progdlg.h>
#include "guibase.h"

#include <comdef.h>
#include <taskschd.h>

#include <thread>

#include "archivehandler.h"
#include "settingsfilehandler.h"



//// Main Application ////
class MainApp : public wxApp
{
public:
	bool OnInit();
};



//// New Trigger Dialog ////
class NewTriggerDialog : public NewTriggerDialogBase
{
public:
	NewTriggerDialog(wxWindow* parent, 
					 ITaskDefinition* taskDefinition);
	~NewTriggerDialog();

protected:
	void OnCloseWindow(wxCloseEvent& event);

	void OnFrequencyRadioboxSelected(wxCommandEvent& event);
	void OnDaysCheckListBoxToggled(wxCommandEvent& event);

	void OnAddButtonClicked(wxCommandEvent& event);
	void OnCancelButtonClicked(wxCommandEvent& event);

private:
	ITaskDefinition* taskDefinition_;
};



//// Settings Frame ////
class SettingsFrame : public SettingsFrameBase
{
public:
	SettingsFrame(wxWindow* parent);
	~SettingsFrame();

protected:
	void OnCloseWindow(wxCloseEvent& event);

	void OnDestinationPathDirPickerChanged(wxFileDirPickerEvent& event);
	void OnArchiveNameTextUpdated(wxCommandEvent& event);
	void OnCompressionLevelChoiceSelected(wxCommandEvent& event);
	void OnUpdateModeChoiceSelected(wxCommandEvent& event);
	void OnFormatRadioBoxSelected(wxCommandEvent& event);
	void OnCompressionMethodChoiceSelected(wxCommandEvent& event);
	void OnDictionarySizeChoiceSelected(wxCommandEvent& event);
	void OnThreadCountChoiceSelected(wxCommandEvent& event);
	void OnVolumeSizeTextUpdated(wxCommandEvent& event);
	void OnVolumeSizeUnitChoiceSelected(wxCommandEvent& event);
	void OnSolidModeCheckboxClicked(wxCommandEvent& event);

	void OnTriggerListItemSelected(wxListEvent& event);
	void OnTriggerListItemDeselected(wxListEvent& event);

	void OnNewButtonClicked(wxCommandEvent& event);
	void OnRemoveButtonClicked(wxCommandEvent& event);

	void OnSaveButtonClicked(wxCommandEvent& event);
	void OnCancelButtonClicked(wxCommandEvent& event);

private:
	//Archive
	void updateChoiceCompressionMethod(std::wstring archiveFormat);
	void updateChoiceDictionarySize(std::wstring archiveFormat, std::wstring compressionMethod);
	void updateChoiceThreadCount(std::wstring archiveFormat, std::wstring compressionMethod);

	void initArchiveSettings();
	void saveArchiveSettings();

	//Schedule
	void initTaskScheduler();
	void searchExistingTask();
	void removeTrigger(LONG index);

	void fillTriggerList();
	void addTriggerListItem(ITrigger* trigger);
	void updateTriggerListColumnWidth();

	void saveScheduleSettings();

protected:
	std::unique_ptr<NewTriggerDialog> newTriggerDialog_;

private:
	// Archive
	bool isArchiveChanged_;
	std::unique_ptr<SettingsFileHandler> fileHandler_;

	std::unique_ptr<wxTextValidator> volumeSizeValidator_;

	// Schedule
	bool isScheduleChanged_;

	std::wstring taskName_;

	ITaskFolder* rootFolder_;
	ITaskDefinition* taskDefinition_;
};



//// Manual Backup Dialog ////
class ManualBackupDialog : public ManualBackupDialogBase
{
public:
	ManualBackupDialog(wxWindow* parent);
	~ManualBackupDialog();

protected:
	void OnCloseWindow(wxCloseEvent& event);

	void OnFormatRadioBoxSelected(wxCommandEvent& event);
	void OnCompressionMethodChoiceSelected(wxCommandEvent& event);

	void OnGoButtonClicked(wxCommandEvent& event);
	void OnCancelButtonClicked(wxCommandEvent& event);

private:
	void updateChoiceCompressionMethod(std::wstring archiveFormat);
	void updateChoiceDictionarySize(std::wstring archiveFormat, std::wstring compressionMethod);
	void updateChoiceThreadCount(std::wstring archiveFormat, std::wstring compressionMethod);

	void initArchiveSettings();

protected:
	std::unique_ptr<SettingsFileHandler> fileHandler_;

	std::wstring destinationPath_;
	std::wstring archiveName_;
	bit7z::BitCompressionLevel compressionLevel_;
	bit7z::UpdateMode updateMode_;
	const bit7z::BitInOutFormat* archiveFormat_;
	bit7z::BitCompressionMethod compressionMethod_;
	uint32_t dictionarySize_;
	uint32_t threadCount_;
	uint64_t volumeSize_;
	std::unique_ptr<wxTextValidator> volumeSizeValidator_;
	bool isSolid_;
};



//// Main Frame ////
class MainFrame : public MainFrameBase
{
public:
	MainFrame(wxWindow* parent);
	~MainFrame();

protected:
	void OnMainListItemSelected(wxListEvent& event);
	void OnMainListItemDeselected(wxListEvent& event);

	void OnArchiveButtonClicked(wxCommandEvent& event);
	void OnAddDirectoryButtonClicked(wxCommandEvent& event);
	void OnAddFileButtonClicked(wxCommandEvent& event);
	void OnRemoveButtonClicked(wxCommandEvent& event);
	void OnSettingsButtonClicked(wxCommandEvent& event);

private:
	void initMainList();
	void addMainListItem(std::wstring path);
	void updateMainListColumnWidth();

protected:
	std::unique_ptr<wxDirDialog> directoryDialog_;
	std::unique_ptr<wxFileDialog> fileDialog_;
	std::unique_ptr<ManualBackupDialog> manualBackupDialog_;
	std::unique_ptr<SettingsFrame> settingsFrame_;
};