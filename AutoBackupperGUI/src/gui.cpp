#include "gui.h"



//// Main App ////
bool MainApp::OnInit()
{
	MainFrame* frame = new MainFrame(NULL);
	frame->Show();

	return true;
}



//// New Trigger Dialog ////
NewTriggerDialog::NewTriggerDialog(wxWindow* parent,
								   ITaskDefinition* taskDefinition) : NewTriggerDialogBase(parent), taskDefinition_(taskDefinition) {}
NewTriggerDialog::~NewTriggerDialog() {}

void NewTriggerDialog::OnCloseWindow(wxCloseEvent& event)
{
	this->EndModal(wxID_CANCEL);
}

void NewTriggerDialog::OnFrequencyRadioboxSelected(wxCommandEvent& event)
{
	switch (radioBoxFrequency->GetSelection())
	{
		case 0: // One Time
			staticTextRecurrence->Hide();
			spinCtrlRecurrence->Hide();
			staticTextRecurrenceUnit->Hide();

			checkListBoxDays->Hide();

			buttonAdd->Enable();

			break;
		case 1: // Daily
			staticTextRecurrence->Show();

			spinCtrlRecurrence->SetMax(999);
			spinCtrlRecurrence->Show();

			staticTextRecurrenceUnit->SetLabel("days");
			staticTextRecurrenceUnit->Show();

			checkListBoxDays->Hide();

			buttonAdd->Enable();

			break;
		case 2: // Weekly
			staticTextRecurrence->Show();

			spinCtrlRecurrence->SetMax(99);
			spinCtrlRecurrence->Show();

			staticTextRecurrenceUnit->SetLabel("weeks on:");
			staticTextRecurrenceUnit->Show();

			for (uint32_t i = 0; i < 7; i++)
				checkListBoxDays->Check(i, false);
			checkListBoxDays->Show();

			buttonAdd->Disable();

			break;
	}
}
void NewTriggerDialog::OnDaysCheckListBoxToggled(wxCommandEvent& event)
{
	bool areAnyChecked = false;

	for (uint32_t i = 0; i < 7; i++)
		if (checkListBoxDays->IsChecked(i))
		{
			areAnyChecked = true;
			buttonAdd->Enable();

			break;
		}

	if (!areAnyChecked)
		buttonAdd->Disable();
}

void NewTriggerDialog::OnAddButtonClicked(wxCommandEvent& event)
{
	std::wstring date = datePickerStart->GetValue().FormatISODate().ToStdWstring();
	std::wstring time = timePickerStart->GetValue().FormatISOTime().ToStdWstring();
	std::wstring dateTime = date + L'T' + time;

	short recurrence{};
	switch (radioBoxFrequency->GetSelection())
	{
		case 0: // One Time
			recurrence = 0;
			break;
		case 1: // Daily
		case 2: // Weekly
			recurrence = spinCtrlRecurrence->GetValue();
			break;
	}

	short daysOfWeek{ 0 };
	switch (radioBoxFrequency->GetSelection())
	{
		case 0:
		case 1:
			break;
		case 2:
			for (uint32_t i = 0; i < 7; i++)
				if (checkListBoxDays->IsChecked(i))
					switch (i)
					{
						case 0: // Monday
							daysOfWeek |= (1 << 1);
							break;
						case 1: // Tuesday
							daysOfWeek |= (1 << 2);
							break;
						case 2: // Wednesday
							daysOfWeek |= (1 << 3);
							break;
						case 3: // Thursday
							daysOfWeek |= (1 << 4);
							break;
						case 4: // Friday
							daysOfWeek |= (1 << 5);
							break;
						case 5: // Saturday
							daysOfWeek |= (1 << 6);
							break;
						case 6: // Sunday
							daysOfWeek |= (1 << 0);
							break;
					}
			break;
	}

	try
	{
		HRESULT hr;

		ITriggerCollection* triggerCollection = nullptr;
		hr = taskDefinition_->get_Triggers(&triggerCollection);
		if (FAILED(hr))
			throw std::exception("Cannot get trigger collection.");

		ITrigger* trigger = nullptr;
		switch (radioBoxFrequency->GetSelection())
		{
			case 0: // One Time
				hr = triggerCollection->Create(TASK_TRIGGER_TIME, &trigger);
				break;
			case 1: // Daily
				hr = triggerCollection->Create(TASK_TRIGGER_DAILY, &trigger);
				break;
			case 2: // Weekly
				hr = triggerCollection->Create(TASK_TRIGGER_WEEKLY, &trigger);
				break;
		}	
		triggerCollection->Release();
		if (FAILED(hr))
			throw std::exception("Could not create the trigger.");

		switch (radioBoxFrequency->GetSelection())
		{
			case 0: // One Time
			{
				ITimeTrigger* timeTrigger = nullptr;
				hr = trigger->QueryInterface(IID_ITimeTrigger, (void**)&timeTrigger);
				trigger->Release();
				if (FAILED(hr))
					throw std::exception("QueryInterface call on ITimeTrigger failed");

				hr = timeTrigger->put_StartBoundary(_bstr_t(dateTime.c_str()));
				timeTrigger->Release();
				if (FAILED(hr))
					throw std::exception("Cannot put start boundary");

				break;
			}
			case 1: // Daily
			{
				IDailyTrigger* dailyTrigger = nullptr;
				hr = trigger->QueryInterface(IID_IDailyTrigger, (void**)&dailyTrigger);
				trigger->Release();
				if (FAILED(hr))
					throw std::exception("QueryInterface call on IDailyTrigger failed");

				hr = dailyTrigger->put_StartBoundary(_bstr_t(dateTime.c_str()));
				if (FAILED(hr))
				{
					dailyTrigger->Release();

					throw std::exception("Cannot put start boundary");
				}

				hr = dailyTrigger->put_DaysInterval(recurrence);
				dailyTrigger->Release();
				if (FAILED(hr))
					throw std::exception("Cannot put days interval");

				break;
			}
			case 2: // Weekly
			{
				IWeeklyTrigger* weeklyTrigger = nullptr;
				hr = trigger->QueryInterface(IID_IWeeklyTrigger, (void**)&weeklyTrigger);
				trigger->Release();
				if (FAILED(hr))
					throw std::exception("QueryInterface call on IWeeklyTrigger failed");

				hr = weeklyTrigger->put_StartBoundary(_bstr_t(dateTime.c_str()));
				if (FAILED(hr))
				{
					weeklyTrigger->Release();

					throw std::exception("Cannot put start boundary");
				}

				hr = weeklyTrigger->put_WeeksInterval(recurrence);
				if (FAILED(hr))
				{
					weeklyTrigger->Release();

					throw std::exception("Cannot put weeks interval");
				}

				hr = weeklyTrigger->put_DaysOfWeek(daysOfWeek);
				weeklyTrigger->Release();
				if (FAILED(hr))
					throw std::exception("Cannot put days of week interval");

				break;
			}
		}

		wxMessageBox(L"Trigger added successfully.", "Success", wxOK | wxICON_INFORMATION, this);
	}
	catch (const std::exception &e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);

		return;
	}

	this->EndModal(wxID_OK);
}
void NewTriggerDialog::OnCancelButtonClicked(wxCommandEvent& event)
{
	this->EndModal(wxID_CANCEL);
}



//// Settings Frame ////
SettingsFrame::SettingsFrame(wxWindow* parent) : SettingsFrameBase(parent)
{
	#pragma region Archive
	try
	{
		isArchiveChanged_ = false;

		fileHandler_ = std::make_unique<SettingsFileHandler>(SETTINGS_FILE);

		initArchiveSettings();
	}
	catch (const std::exception& e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);
	}
	#pragma endregion

	#pragma region Schedule
	try
	{
		taskName_ = L"AutoBackupper Task";

		rootFolder_ = nullptr;
		taskDefinition_ = nullptr;

		isScheduleChanged_ = false;

		initTaskScheduler();
		fillTriggerList();
	}
	catch (const std::exception& e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);
	}
	#pragma endregion

	// New Trigger Dialog
	newTriggerDialog_ = std::make_unique<NewTriggerDialog>(this, taskDefinition_);
}
SettingsFrame::~SettingsFrame()
{
	rootFolder_->Release();
	taskDefinition_->Release();

	CoUninitialize();
}

void SettingsFrame::OnCloseWindow(wxCloseEvent& event)
{
	this->Hide();
}

void SettingsFrame::OnDestinationPathDirPickerChanged(wxFileDirPickerEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnArchiveNameTextUpdated(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnCompressionLevelChoiceSelected(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnUpdateModeChoiceSelected(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnFormatRadioBoxSelected(wxCommandEvent& event)
{
	switch (radioBoxFormat->GetSelection())
	{
		case 0: // 7z
		{
			updateChoiceCompressionMethod(L"7z");
			updateChoiceDictionarySize(L"7z", L"LZMA2");
			updateChoiceThreadCount(L"7z", L"LZMA2");

			break;
		}
		case 1: // zip
		{
			updateChoiceCompressionMethod(L"zip");
			updateChoiceDictionarySize(L"zip", L"Deflate");
			updateChoiceThreadCount(L"zip", L"Deflate");

			break;
		}
	}

	isArchiveChanged_ = true;
}
void SettingsFrame::OnCompressionMethodChoiceSelected(wxCommandEvent& event)
{
	switch (radioBoxFormat->GetSelection())
	{
		case 0:
			switch (choiceCompressionMethod->GetSelection())
			{
				case 0:
					updateChoiceDictionarySize(L"7z", L"BZip2");
					updateChoiceThreadCount(L"7z", L"BZip2");
					break;
				case 1:
					updateChoiceDictionarySize(L"7z", L"LZMA");
					updateChoiceThreadCount(L"7z", L"LZMA");
					break;
				case 2:
					updateChoiceDictionarySize(L"7z", L"LZMA2");
					updateChoiceThreadCount(L"7z", L"LZMA2");
					break;
				case 3:
					updateChoiceDictionarySize(L"7z", L"PPMd");
					updateChoiceThreadCount(L"7z", L"PPMd");
					break;
			}
			break;
		case 1:
			switch (choiceCompressionMethod->GetSelection())
			{
				case 0:
					updateChoiceDictionarySize(L"zip", L"BZip2");
					updateChoiceThreadCount(L"zip", L"BZip2");
					break;
				case 1:
					updateChoiceDictionarySize(L"zip", L"LZMA");
					updateChoiceThreadCount(L"zip", L"LZMA");
					break;
				case 2:
					updateChoiceDictionarySize(L"zip", L"PPMd");
					updateChoiceThreadCount(L"zip", L"PPMd");
					break;
				case 3:
					updateChoiceDictionarySize(L"zip", L"Deflate");
					updateChoiceThreadCount(L"zip", L"Deflate");
					break;
				case 4:
					updateChoiceDictionarySize(L"zip", L"Deflate64");
					updateChoiceThreadCount(L"zip", L"Deflate64");
					break;
			}
			break;
	}

	isArchiveChanged_ = true;
}
void SettingsFrame::OnDictionarySizeChoiceSelected(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnThreadCountChoiceSelected(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnVolumeSizeTextUpdated(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnVolumeSizeUnitChoiceSelected(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}
void SettingsFrame::OnSolidModeCheckboxClicked(wxCommandEvent& event)
{
	isArchiveChanged_ = true;
}

void SettingsFrame::OnTriggerListItemSelected(wxListEvent& event)
{
	if(triggerList->GetSelectedItemCount() > 0)
		buttonRemove->Enable();
}
void SettingsFrame::OnTriggerListItemDeselected(wxListEvent& event)
{
	if (triggerList->GetSelectedItemCount() == 0)
		buttonRemove->Disable();
}

void SettingsFrame::OnNewButtonClicked(wxCommandEvent& event)
{
	if (newTriggerDialog_->ShowModal() == wxID_OK)
	{
		triggerList->DeleteAllItems();
		fillTriggerList();

		isScheduleChanged_ = true;
	}
}
void SettingsFrame::OnRemoveButtonClicked(wxCommandEvent& event)
{
	LONG listItemIndex = -1;
	listItemIndex = triggerList->GetNextItem(listItemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (listItemIndex == -1)
		return;

	removeTrigger(listItemIndex);

	triggerList->DeleteItem(listItemIndex);

	isScheduleChanged_ = true;
}

void SettingsFrame::OnSaveButtonClicked(wxCommandEvent& event)
{
	if (isArchiveChanged_ || isScheduleChanged_)
	{
		if (isArchiveChanged_)
		{
			saveArchiveSettings();

			isArchiveChanged_ = false;
		}

		if (isScheduleChanged_)
		{
			saveScheduleSettings();

			isScheduleChanged_ = false;
		}

		wxMessageBox(L"Settings saved successfully.", "Success", wxOK | wxICON_INFORMATION, this);
	}
}
void SettingsFrame::OnCancelButtonClicked(wxCommandEvent& event)
{
	this->Hide();
}

void SettingsFrame::updateChoiceCompressionMethod(std::wstring archiveFormat)
{
	if (archiveFormat == L"7z")
	{
		const std::wstring sevenZipMethods[] = { L"BZip2", L"LZMA", L"LZMA2", L"PPMd" };

		choiceCompressionMethod->Clear();
		for (std::wstring method : sevenZipMethods)
			choiceCompressionMethod->Append(method);
		choiceCompressionMethod->SetSelection(2);
	}
	else if (archiveFormat == L"zip")
	{
		const std::wstring zipMethods[] = { L"BZip2", L"LZMA", L"PPMd", L"Deflate", L"Deflate64" };

		choiceCompressionMethod->Clear();
		for (std::wstring method : zipMethods)
			choiceCompressionMethod->Append(method);
		choiceCompressionMethod->SetSelection(3);
	}
}
void SettingsFrame::updateChoiceDictionarySize(std::wstring archiveFormat, std::wstring compressionMethod)
{
	choiceDictionarySize->Enable();

	if (archiveFormat == L"7z")
	{
		if (compressionMethod == L"BZip2")
		{
			const std::wstring dictionarySizes[] = { L"100 KB", L"200 KB", L"300 KB",
													 L"400 KB", L"500 KB", L"600 KB",
													 L"700 KB", L"800 KB", L"900 KB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(8);
		}
		else if (compressionMethod == L"LZMA")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(6);
		}
		else if (compressionMethod == L"LZMA2")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(6);
		}
		else if (compressionMethod == L"PPMd")
		{
			const std::wstring dictionarySizes[] = { L"1 MB",   L"2 MB",   L"4 MB",
													 L"8 MB",   L"16 MB",  L"32 MB",
													 L"64 MB",  L"128 MB", L"256 MB",
													 L"512 MB", L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(4);
		}
	}
	else if (archiveFormat == L"zip")
	{
		if (compressionMethod == L"BZip2")
		{
			const std::wstring dictionarySizes[] = { L"100 KB", L"200 KB", L"300 KB",
													 L"400 KB", L"500 KB", L"600 KB",
													 L"700 KB", L"800 KB", L"900 KB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(8);
		}
		else if (compressionMethod == L"LZMA")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(6);
		}
		else if (compressionMethod == L"PPMd")
		{
			const std::wstring dictionarySizes[] = { L"1 MB",   L"2 MB",   L"4 MB",
													 L"8 MB",   L"16 MB",  L"32 MB",
													 L"64 MB",  L"128 MB", L"256 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(4);
		}
		else if (compressionMethod == L"Deflate")
		{
			const std::wstring dictionarySize = { L"32 KB" };

			choiceDictionarySize->Clear();
			choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(0);

			choiceDictionarySize->Disable();
		}
		else if (compressionMethod == L"Deflate64")
		{
			const std::wstring dictionarySize = { L"64 KB" };

			choiceDictionarySize->Clear();
			choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(0);

			choiceDictionarySize->Disable();
		}
	}
}
void SettingsFrame::updateChoiceThreadCount(std::wstring archiveFormat, std::wstring compressionMethod)
{
	uint32_t maxThreadCount = getMaxThreadCount();

	choiceThreadCount->Enable();
	if (archiveFormat == L"7z")
	{
		if (compressionMethod == L"BZip2" ||
			compressionMethod == L"LZMA2")
		{
			choiceThreadCount->Clear();
			for (uint32_t i = 1; i <= maxThreadCount; i++)
				choiceThreadCount->Append(std::to_wstring(i));

			choiceThreadCount->SetSelection(maxThreadCount - 1);
		}
		else if (compressionMethod == L"LZMA")
		{
			choiceThreadCount->Clear();
			for (uint32_t i = 1; i <= 2; i++)
				choiceThreadCount->Append(std::to_wstring(i));

			choiceThreadCount->SetSelection(1);
		}
		else if (compressionMethod == L"PPMd")
		{
			choiceThreadCount->Clear();
			choiceThreadCount->Append(std::to_wstring(1));

			choiceThreadCount->SetSelection(0);

			choiceThreadCount->Disable();
		}
	}
	else if (archiveFormat == L"zip")
	{
		if (compressionMethod == L"BZip2" ||
			compressionMethod == L"LZMA" ||
			compressionMethod == L"PPMd" ||
			compressionMethod == L"Deflate" ||
			compressionMethod == L"Deflate64")
		{
			choiceThreadCount->Clear();
			for (uint32_t i = 1; i <= maxThreadCount; i++)
				choiceThreadCount->Append(std::to_wstring(i));

			choiceThreadCount->SetSelection(maxThreadCount - 1);
		}
	}
}

void SettingsFrame::initArchiveSettings()
{
	std::wstring destinationPath	= fileHandler_->getDestinationPath();
	std::wstring archiveName		= fileHandler_->getArchiveName();
	std::wstring compressionLevel	= fileHandler_->getCompressionLevel();
	std::wstring updateMode			= fileHandler_->getUpdateMode();
	std::wstring archiveFormat		= fileHandler_->getArchiveFormat();
	std::wstring compressionMethod	= fileHandler_->getCompressionMethod();
	std::wstring dictionarySize		= fileHandler_->getDictionarySize();
	std::wstring dictionarySizeUnit = fileHandler_->getDictionarySizeUnit();
	std::wstring threadCount		= fileHandler_->getThreadCount();
	std::wstring volumeSize			= fileHandler_->getVolumeSize();
	std::wstring volumeSizeUnit		= fileHandler_->getVolumeSizeUnit();
	std::wstring isSolid			= fileHandler_->getIsSolid();

	// Destination Path
	dirPickerDestinationPath->SetPath(destinationPath);

	// Archive Name
	textCtrlArchiveName->SetLabel(archiveName);

	// Compression Level
	choiceCompressionLevel->SetStringSelection(compressionLevel);

	// Update Mode
	choiceUpdateMode->SetStringSelection(updateMode);

	// Archive Format
	radioBoxFormat->SetStringSelection(L"*." + archiveFormat);

	// Compression Method
	updateChoiceCompressionMethod(archiveFormat);
	choiceCompressionMethod->SetStringSelection(compressionMethod);

	// Dictionary Size
	updateChoiceDictionarySize(archiveFormat, compressionMethod);
	choiceDictionarySize->SetStringSelection(dictionarySize + L" " + dictionarySizeUnit);

	// Thread Count
	updateChoiceThreadCount(archiveFormat, compressionMethod);
	choiceThreadCount->SetStringSelection(threadCount);

	// Volume Size
	if (volumeSize != L"0")
		textCtrlVolumeSize->SetLabel(volumeSize);
	choiceVolumeSizeUnit->SetStringSelection(volumeSizeUnit);

	volumeSizeValidator_ = std::make_unique<wxTextValidator>(wxFILTER_DIGITS);
	textCtrlVolumeSize->SetValidator(*volumeSizeValidator_);

	// Solid Mode
	if (isSolid == "True")
		checkBoxSolidMode->SetValue(true);
	else
		checkBoxSolidMode->SetValue(false);
}
void SettingsFrame::saveArchiveSettings()
{
	std::wstring destinationPath = dirPickerDestinationPath->GetPath().ToStdWstring();
	std::wstring archiveName = textCtrlArchiveName->GetLineText(0).ToStdWstring();
	std::wstring compressionLevel = choiceCompressionLevel->GetString(choiceCompressionLevel->GetSelection()).ToStdWstring();
	std::wstring updateMode	= choiceUpdateMode->GetString(choiceUpdateMode->GetSelection()).ToStdWstring();
	std::wstring archiveFormat;
	switch (radioBoxFormat->GetSelection())
	{
		case 0:
			archiveFormat = L"7z";
			break;
		case 1:
			archiveFormat = L"zip";
			break;
	}
	std::wstring compressionMethod = choiceCompressionMethod->GetString(choiceCompressionMethod->GetSelection()).ToStdWstring();
	std::wstring dictionarySizeAndUnit = choiceDictionarySize->GetString(choiceDictionarySize->GetSelection()).ToStdWstring();
	std::wstring threadCount = choiceThreadCount->GetString(choiceThreadCount->GetSelection()).ToStdWstring();
	std::wstring volumeSize;
	if (textCtrlVolumeSize->GetLineText(0).IsNull())
		volumeSize = L"0";
	else
		volumeSize = textCtrlVolumeSize->GetLineText(0);
	std::wstring volumeSizeUnit	= choiceVolumeSizeUnit->GetString(choiceVolumeSizeUnit->GetSelection()).ToStdWstring();
	std::wstring isSolid;
	if (checkBoxSolidMode->IsChecked())
		isSolid = L"True";
	else
		isSolid = L"False";

	fileHandler_->saveFile(destinationPath,
						   archiveName,
						   compressionLevel,
						   updateMode,
						   archiveFormat,
						   compressionMethod,
						   dictionarySizeAndUnit,
						   threadCount,
						   volumeSize + L" " + volumeSizeUnit,
						   isSolid);
}

void SettingsFrame::initTaskScheduler()
{
	HRESULT hr;

	std::wstring taskAuthorName = L"Marginean Horatiu";
	std::wstring taskDescription = L"This task makes sure AutoBackupper gets your automatic backups done.";
	std::wstring taskRootFolderPath = L"\\";
	std::wstring executablePath = std::filesystem::current_path().wstring() + L"\\AutoBackupperTask.exe";

	hr = CoInitializeEx(NULL,
						COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
		throw std::exception("CoInitializeEx failed.");

	hr = CoInitializeSecurity(NULL,
							  -1,
							  NULL,
							  NULL,
							  RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
							  RPC_C_IMP_LEVEL_IMPERSONATE,
							  NULL,
							  0,
							  NULL);
	if (FAILED(hr))
		throw std::exception("CoInitializeSecurity failed.");

	// Creating ITaskService to connect to Task Scheduler
	ITaskService* taskService = nullptr;
	hr = CoCreateInstance(CLSID_TaskScheduler,
						  NULL,
						  CLSCTX_INPROC_SERVER,
						  IID_ITaskService,
						  (void**)&taskService);
	if (FAILED(hr))
		throw std::exception("Failed to create an instance of ITaskService.");
	hr = taskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr))
	{
		taskService->Release();

		throw std::exception("ITaskService::Connect failed.");
	}
	// Getting root folder information and creating a new task
	hr = taskService->GetFolder(_bstr_t(taskRootFolderPath.c_str()), &rootFolder_);
	if (FAILED(hr))
	{
		taskService->Release();

		throw std::exception("Cannot get Root Folder pointer.");
	}

	searchExistingTask();
	if (taskDefinition_ == nullptr)
	{
		hr = taskService->NewTask(NULL, &taskDefinition_);
		taskService->Release();
		if (FAILED(hr))
			throw std::exception("Failed to CoCreate an instance of the TaskService class.");

		// Adding author name and description
		IRegistrationInfo* registrationInfo = nullptr;
		hr = taskDefinition_->get_RegistrationInfo(&registrationInfo);
		if (FAILED(hr))
			throw std::exception("Cannot get identification pointer.");
		hr = registrationInfo->put_Author(_bstr_t(taskAuthorName.c_str()));
		if (FAILED(hr))
		{
			registrationInfo->Release();

			throw std::exception("Cannot put task author name.");
		}
		hr = registrationInfo->put_Description(_bstr_t(taskDescription.c_str()));
		registrationInfo->Release();
		if (FAILED(hr))
			throw std::exception("Cannot put task description.");

		// Setting up the action
		IActionCollection* actionCollection = nullptr;
		hr = taskDefinition_->get_Actions(&actionCollection);
		if (FAILED(hr))
			throw std::exception("Cannot get task collection pointer.");
		IAction* action = nullptr;
		hr = actionCollection->Create(TASK_ACTION_EXEC, &action);
		actionCollection->Release();
		if (FAILED(hr))
			throw std::exception("Cannot create action.");
		IExecAction* execAction = nullptr;
		hr = action->QueryInterface(IID_IExecAction, (void**)&execAction);
		action->Release();
		if (FAILED(hr))
			throw std::exception("QueryInterface call failed for IExecAction.");
		hr = execAction->put_Path(_bstr_t(executablePath.c_str()));
		execAction->Release();
		if (FAILED(hr))
			throw std::exception("Cannot put the executable path.");
	}
	else
		taskService->Release();
}
void SettingsFrame::searchExistingTask()
{
	HRESULT hr;

	IRegisteredTaskCollection* registeredTaskCollection = nullptr;
	hr = rootFolder_->GetTasks(NULL, &registeredTaskCollection);
	if (FAILED(hr))
		throw std::exception("Cannot get the registered tasks.");

	LONG registeredTaskCount = 0;
	registeredTaskCollection->get_Count(&registeredTaskCount);
	if (registeredTaskCount == 0)
	{
		registeredTaskCollection->Release();

		throw std::exception("No tasks are currently running.");
	}

	for (LONG i = 1; i <= registeredTaskCount; i++)
	{
		IRegisteredTask* registeredTask = nullptr;
		hr = registeredTaskCollection->get_Item(_variant_t(i), &registeredTask);
		if (SUCCEEDED(hr))
		{
			BSTR registeredTaskName;
			hr = registeredTask->get_Name(&registeredTaskName);
			if (SUCCEEDED(hr))
			{
				if (taskName_ == registeredTaskName)
					registeredTask->get_Definition(&taskDefinition_);

				SysFreeString(registeredTaskName);
			}

			registeredTask->Release();
		}
	}

	registeredTaskCollection->Release();
}
void SettingsFrame::removeTrigger(LONG index)
{
	HRESULT hr;

	ITriggerCollection* triggerCollection = nullptr;
	hr = taskDefinition_->get_Triggers(&triggerCollection);
	if (FAILED(hr))
		throw std::exception("Cannot get trigger collection.");

	hr = triggerCollection->Remove(_variant_t(index + 1));
	if (FAILED(hr))
		throw std::exception("Trigger removal failed.");

	triggerCollection->Release();
}

void SettingsFrame::fillTriggerList()
{
	HRESULT hr;

	ITriggerCollection* triggerCollection = nullptr;
	hr = taskDefinition_->get_Triggers(&triggerCollection);
	if (FAILED(hr))
		throw std::exception("Cannot get trigger collection.");

	LONG triggerCount = 0;
	triggerCollection->get_Count(&triggerCount);
	if (triggerCount == 0)
	{
		triggerCollection->Release();

		return;
	}
	for (LONG i = 1; i <= triggerCount; i++)
	{
		ITrigger* trigger = nullptr;
		hr = triggerCollection->get_Item(_variant_t(i), &trigger);
		if (SUCCEEDED(hr))
		{
			//Only displays Enabled triggers
			VARIANT_BOOL triggerType;
			trigger->get_Enabled(&triggerType);
			if(triggerType == VARIANT_TRUE)
				addTriggerListItem(trigger);

			trigger->Release();
		}
	}

	updateTriggerListColumnWidth();
}
void SettingsFrame::addTriggerListItem(ITrigger* trigger)
{
	HRESULT hr;

	int index = triggerList->GetItemCount();

	std::wstring type;
	TASK_TRIGGER_TYPE2 triggerType;
	trigger->get_Type(&triggerType);
	switch (triggerType)
	{
		case TASK_TRIGGER_TIME: // One Time
			type = L"One Time";
			break;
		case TASK_TRIGGER_DAILY: // Daily
			type = L"Daily";
			break;
		case TASK_TRIGGER_WEEKLY: // Weekly
			type = L"Weekly";
			break;
		default: // Only valid when trigger was not added through the GUI
			type = L"Undefined";
			break;
	}

	std::wstring details;
	switch (triggerType)
	{
		case TASK_TRIGGER_TIME: // One Time
		{
			ITimeTrigger* timeTrigger = nullptr;
			hr = trigger->QueryInterface(IID_ITimeTrigger, (void**)&timeTrigger);
			if (FAILED(hr))
				throw std::exception("QueryInterface call on ITimeTrigger failed");

			BSTR dateTimeBSTR;
			timeTrigger->get_StartBoundary(&dateTimeBSTR);
			wxDateTime dateTime;
			dateTime.ParseISOCombined(dateTimeBSTR);

			details = L"At " + dateTime.Format(wxS("%R")) + L" on " + dateTime.Format(wxS("%d/%m/%Y"));

			timeTrigger->Release();

			break;
		}
		case TASK_TRIGGER_DAILY: // Daily
		{
			IDailyTrigger* dailyTrigger = nullptr;
			hr = trigger->QueryInterface(IID_IDailyTrigger, (void**)&dailyTrigger);
			if (FAILED(hr))
				throw std::exception("QueryInterface call on IDailyTrigger failed");

			BSTR dateTimeBSTR;
			dailyTrigger->get_StartBoundary(&dateTimeBSTR);
			wxDateTime dateTime;
			dateTime.ParseISOCombined(dateTimeBSTR);

			short recurrence;
			dailyTrigger->get_DaysInterval(&recurrence);
			if (recurrence == 1)
				details = L"At " + dateTime.Format(wxS("%R")) + L" every day";
			else
				details = L"At " + dateTime.Format(wxS("%R")) + L" every " + std::to_wstring(recurrence) + L" days";

			dailyTrigger->Release();

			break;
		}
		case TASK_TRIGGER_WEEKLY: // Weekly
		{
			IWeeklyTrigger* weeklyTrigger = nullptr;
			hr = trigger->QueryInterface(IID_IWeeklyTrigger, (void**)&weeklyTrigger);
			if (FAILED(hr))
				throw std::exception("QueryInterface call on IWeeklyTrigger failed");

			BSTR dateTimeBSTR;
			weeklyTrigger->get_StartBoundary(&dateTimeBSTR);
			wxDateTime dateTime;
			dateTime.ParseISOCombined(dateTimeBSTR);

			details = L"At " + dateTime.Format(wxS("%R")) + L" every";

			short daysOfWeek;
			weeklyTrigger->get_DaysOfWeek(&daysOfWeek);

			if (daysOfWeek == ((1 << 7) - 1))
				details += L" day ";
			else
			{
				if ((daysOfWeek & (1 << 1)) == (1 << 1))
					details += L" Mon,";
				if ((daysOfWeek & (1 << 2)) == (1 << 2))
					details += L" Tue,";
				if ((daysOfWeek & (1 << 3)) == (1 << 3))
					details += L" Wed,";
				if ((daysOfWeek & (1 << 4)) == (1 << 4))
					details += L" Thu,";
				if ((daysOfWeek & (1 << 5)) == (1 << 5))
					details += L" Fri,";
				if ((daysOfWeek & (1 << 6)) == (1 << 6))
					details += L" Sat,";
				if ((daysOfWeek & (1 << 0)) == (1 << 0))
					details += L" Sun ";
			}
			details.pop_back();
			details.push_back(L' ');

			short recurrence;
			weeklyTrigger->get_WeeksInterval(&recurrence);

			if (recurrence == 1)
				details += L"every week, ";
			else
				details += L"every " + std::to_wstring(recurrence) + " weeks, ";

			details += L"starting " + dateTime.Format(wxS("%d/%m/%Y"));

			weeklyTrigger->Release();

			break;
		}
		default: // Only valid when trigger of another type was not added through the GUI
			details = L"Undefined";
			break;
	}

	triggerList->InsertItem(index, type);
	triggerList->SetItem(index, 1, details);
}
void SettingsFrame::updateTriggerListColumnWidth()
{
	for (int i = 0; i < triggerList->GetColumnCount(); i++)
	{
		triggerList->SetColumnWidth(i, wxLIST_AUTOSIZE);
		int itemWidth = triggerList->GetColumnWidth(i);

		triggerList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
		int headerWidth = triggerList->GetColumnWidth(i);

		triggerList->SetColumnWidth(i, (std::max)(itemWidth, headerWidth));
	}
}

void SettingsFrame::saveScheduleSettings()
{
	HRESULT hr;

	IRegisteredTask* registeredTask = nullptr;
	hr = rootFolder_->RegisterTaskDefinition(_bstr_t(taskName_.c_str()),
											 taskDefinition_,
											 TASK_CREATE_OR_UPDATE,
											 _variant_t(),
											 _variant_t(),
											 TASK_LOGON_NONE,
											 _variant_t(""),
											 &registeredTask);
	if (FAILED(hr))
		throw std::exception("Error saving the task.");

	registeredTask->Release();
}



//// Manual Backup Dialog ////
ManualBackupDialog::ManualBackupDialog(wxWindow* parent) : ManualBackupDialogBase(parent)
{
	fileHandler_ = std::make_unique<SettingsFileHandler>(SETTINGS_FILE);

	initArchiveSettings();
}
ManualBackupDialog::~ManualBackupDialog() {}

void ManualBackupDialog::OnCloseWindow(wxCloseEvent& event)
{
	this->Hide();
}

void ManualBackupDialog::OnFormatRadioBoxSelected(wxCommandEvent& event)
{
	switch (radioBoxFormat->GetSelection())
	{
		case 0: // 7z
		{
			updateChoiceCompressionMethod(L"7z");
			updateChoiceDictionarySize(L"7z", L"LZMA2");
			updateChoiceThreadCount(L"7z", L"LZMA2");

			break;
		}
		case 1: // zip
		{
			updateChoiceCompressionMethod(L"zip");
			updateChoiceDictionarySize(L"zip", L"Deflate");
			updateChoiceThreadCount(L"zip", L"Deflate");

			break;
		}
	}
}
void ManualBackupDialog::OnCompressionMethodChoiceSelected(wxCommandEvent& event)
{
	switch (radioBoxFormat->GetSelection())
	{
		case 0:
			switch (choiceCompressionMethod->GetSelection())
			{
				case 0:
					updateChoiceDictionarySize(L"7z", L"BZip2");
					updateChoiceThreadCount(L"7z", L"BZip2");
					break;
				case 1:
					updateChoiceDictionarySize(L"7z", L"LZMA");
					updateChoiceThreadCount(L"7z", L"LZMA");
					break;
				case 2:
					updateChoiceDictionarySize(L"7z", L"LZMA2");
					updateChoiceThreadCount(L"7z", L"LZMA2");
					break;
				case 3:
					updateChoiceDictionarySize(L"7z", L"PPMd");
					updateChoiceThreadCount(L"7z", L"PPMd");
					break;
			}
			break;
		case 1:
			switch (choiceCompressionMethod->GetSelection())
			{
				case 0:
					updateChoiceDictionarySize(L"zip", L"BZip2");
					updateChoiceThreadCount(L"zip", L"BZip2");
					break;
				case 1:
					updateChoiceDictionarySize(L"zip", L"LZMA");
					updateChoiceThreadCount(L"zip", L"LZMA");
					break;
				case 2:
					updateChoiceDictionarySize(L"zip", L"PPMd");
					updateChoiceThreadCount(L"zip", L"PPMd");
					break;
				case 3:
					updateChoiceDictionarySize(L"zip", L"Deflate");
					updateChoiceThreadCount(L"zip", L"Deflate");
					break;
				case 4:
					updateChoiceDictionarySize(L"zip", L"Deflate64");
					updateChoiceThreadCount(L"zip", L"Deflate64");
					break;
			}
			break;
	}
}
void ManualBackupDialog::OnGoButtonClicked(wxCommandEvent& event)
{
	// Retrieves destination path of archive
	destinationPath_ = dirPickerDestinationPath->GetPath().ToStdWstring();

	// Retrieves name of archive
	archiveName_ = textCtrlArchiveName->GetLineText(0).ToStdWstring();
	// Concatenates date and time to archive name
	archiveName_ += L"_" + getDate() + L"_" + getTime();

	// Combines destination path and name into the full path
	std::wstring fullPath = destinationPath_ + L'\\' + archiveName_;

	// Retrieves compression level
	switch (choiceCompressionLevel->GetSelection())
	{
		case 0:
			compressionLevel_ = bit7z::BitCompressionLevel::None;
			break;
		case 1:
			compressionLevel_ = bit7z::BitCompressionLevel::Fastest;
			break;
		case 2:
			compressionLevel_ = bit7z::BitCompressionLevel::Fast;
			break;
		case 3:
			compressionLevel_ = bit7z::BitCompressionLevel::Normal;
			break;
		case 4:
			compressionLevel_ = bit7z::BitCompressionLevel::Max;
			break;
		case 5:
			compressionLevel_ = bit7z::BitCompressionLevel::Ultra;
			break;
	}

	// Retrieves update mode
	switch (choiceUpdateMode->GetSelection())
	{
		case 0:
			updateMode_ = bit7z::UpdateMode::None;
			break;
		case 1:
			updateMode_ = bit7z::UpdateMode::Append;
			break;
		case 2:
			updateMode_ = bit7z::UpdateMode::Overwrite;
			break;
	}

	// Retrieves archive format
	// Adds the appropriate extension to full path
	switch (radioBoxFormat->GetSelection())
	{
		case 0: // 7z
			archiveFormat_ = &bit7z::BitFormat::SevenZip;
			fullPath += L".7z";
			break;
		case 1: // zip
			archiveFormat_ = &bit7z::BitFormat::Zip;
			fullPath += L".zip";
			break;
	}

	// Retrieves compression method
	switch (radioBoxFormat->GetSelection())
	{
		case 0: // 7z
		{
			switch (choiceCompressionMethod->GetSelection())
			{
				case 0:
					compressionMethod_ = bit7z::BitCompressionMethod::BZip2;
					break;
				case 1:
					compressionMethod_ = bit7z::BitCompressionMethod::Lzma;
					break;
				case 2:
					compressionMethod_ = bit7z::BitCompressionMethod::Lzma2;
					break;
				case 3:
					compressionMethod_ = bit7z::BitCompressionMethod::Ppmd;
					break;
			}

			break;
		}
		case 1: // zip
		{
			switch (choiceCompressionMethod->GetSelection())
			{
				case 0:
					compressionMethod_ = bit7z::BitCompressionMethod::BZip2;
					break;
				case 1:
					compressionMethod_ = bit7z::BitCompressionMethod::Lzma;
					break;
				case 2:
					compressionMethod_ = bit7z::BitCompressionMethod::Ppmd;
					break;
				case 3:
					compressionMethod_ = bit7z::BitCompressionMethod::Deflate;
					break;
				case 4:
					compressionMethod_ = bit7z::BitCompressionMethod::Deflate64;
					break;
			}

			break;
		}
	}

	// Retrieves dictionary size
	std::wstring dictionarySize, dictionarySizeUnit;
	std::wstringstream sStream(choiceDictionarySize->GetString(choiceDictionarySize->GetSelection()).ToStdWstring());

	sStream >> dictionarySize >> dictionarySizeUnit;

	dictionarySize_ = stoi(dictionarySize);
	if (dictionarySizeUnit == L"KB")
		dictionarySize_ *= (1 << 10);
	else if (dictionarySizeUnit == L"MB")
		dictionarySize_ *= (1 << 20);

	// Retrieves number of threads to be used
	threadCount_ = wxAtoi(choiceThreadCount->GetString(choiceThreadCount->GetSelection()));

	// Retrieves volume size
	if (!textCtrlVolumeSize->GetLineText(0).IsNull())
	{
		volumeSize_ = wxAtoi(textCtrlVolumeSize->GetLineText(0));
		switch (choiceVolumeSizeUnit->GetSelection())
		{
			case 0:
				volumeSize_ *= (1 << 0);
				break;
			case 1:
				volumeSize_ *= (1 << 10);
				break;
			case 2:
				volumeSize_ *= (1 << 20);
				break;
			case 3:
				volumeSize_ *= (1 << 30);
				break;
		}
	}

	// Retrieves solid mode information
	isSolid_ = checkBoxSolidMode->IsChecked();

	// Checks for errors and displays them
	try
	{
		if (destinationPath_.empty())
			throw std::exception("Destination path cannot be empty.");
		if (!std::filesystem::exists(destinationPath_))
			throw std::exception("Destination path does not exist.");
		if (!isValidFileName(archiveName_))
			throw std::exception("Name is invalid.");
		if (fullPath.length() > 247)
			throw std::exception("Resulting path is too long. Try shortening the Name or Destination Path.");
	}
	catch (const std::exception& e)
	{
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);

		return;
	}

	ArchiveHandler archiveHandler(fullPath,
								  updateMode_,
								  compressionLevel_,
								  *archiveFormat_,
								  compressionMethod_,
								  dictionarySize_,
								  threadCount_,
								  volumeSize_,
								  isSolid_);

	std::thread archivalThread([&] { return archiveHandler.archiveEverything(); });
	
	wxProgressDialog dialog(L"Archiving...",
							L"Compression status: 0%",
							100,
							this,
							wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME);
	while (!archiveHandler.isFinished_)
		if (!dialog.Update(archiveHandler.archivalProgress_, "Compression status: " + std::to_string(archiveHandler.archivalProgress_) + "%"))
			archiveHandler.isCancelled_ = true;
	
	archivalThread.join();
}
void ManualBackupDialog::OnCancelButtonClicked(wxCommandEvent& event)
{
	this->Hide();
}

void ManualBackupDialog::updateChoiceCompressionMethod(std::wstring archiveFormat)
{
	if (archiveFormat == L"7z")
	{
		const std::wstring sevenZipMethods[] = { L"BZip2", L"LZMA", L"LZMA2", L"PPMd" };

		choiceCompressionMethod->Clear();
		for (std::wstring method : sevenZipMethods)
			choiceCompressionMethod->Append(method);
		choiceCompressionMethod->SetSelection(2);
	}
	else if (archiveFormat == L"zip")
	{
		const std::wstring zipMethods[] = { L"BZip2", L"LZMA", L"PPMd", L"Deflate", L"Deflate64" };

		choiceCompressionMethod->Clear();
		for (std::wstring method : zipMethods)
			choiceCompressionMethod->Append(method);
		choiceCompressionMethod->SetSelection(3);
	}
}
void ManualBackupDialog::updateChoiceDictionarySize(std::wstring archiveFormat, std::wstring compressionMethod)
{
	choiceDictionarySize->Enable();

	if (archiveFormat == L"7z")
	{
		if (compressionMethod == L"BZip2")
		{
			const std::wstring dictionarySizes[] = { L"100 KB", L"200 KB", L"300 KB",
													 L"400 KB", L"500 KB", L"600 KB",
													 L"700 KB", L"800 KB", L"900 KB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(8);
		}
		else if (compressionMethod == L"LZMA")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(6);
		}
		else if (compressionMethod == L"LZMA2")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(6);
		}
		else if (compressionMethod == L"PPMd")
		{
			const std::wstring dictionarySizes[] = { L"1 MB",   L"2 MB",   L"4 MB",
													 L"8 MB",   L"16 MB",  L"32 MB",
													 L"64 MB",  L"128 MB", L"256 MB",
													 L"512 MB", L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(4);
		}
	}
	else if (archiveFormat == L"zip")
	{
		if (compressionMethod == L"BZip2")
		{
			const std::wstring dictionarySizes[] = { L"100 KB", L"200 KB", L"300 KB",
													 L"400 KB", L"500 KB", L"600 KB",
													 L"700 KB", L"800 KB", L"900 KB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(8);
		}
		else if (compressionMethod == L"LZMA")
		{
			const std::wstring dictionarySizes[] = { L"64 KB",   L"256 KB",  L"1 MB",
													 L"2 MB",    L"4 MB",    L"8 MB",
													 L"16 MB",   L"32 MB",   L"64 MB",
													 L"128 MB",  L"256 MB",  L"512 MB",
													 L"1024 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(6);
		}
		else if (compressionMethod == L"PPMd")
		{
			const std::wstring dictionarySizes[] = { L"1 MB",   L"2 MB",   L"4 MB",
													 L"8 MB",   L"16 MB",  L"32 MB",
													 L"64 MB",  L"128 MB", L"256 MB" };

			choiceDictionarySize->Clear();
			for (std::wstring dictionarySize : dictionarySizes)
				choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(4);
		}
		else if (compressionMethod == L"Deflate")
		{
			const std::wstring dictionarySize = { L"32 KB" };

			choiceDictionarySize->Clear();
			choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(0);

			choiceDictionarySize->Disable();
		}
		else if (compressionMethod == L"Deflate64")
		{
			const std::wstring dictionarySize = { L"64 KB" };

			choiceDictionarySize->Clear();
			choiceDictionarySize->Append(dictionarySize);

			choiceDictionarySize->SetSelection(0);

			choiceDictionarySize->Disable();
		}
	}
}
void ManualBackupDialog::updateChoiceThreadCount(std::wstring archiveFormat, std::wstring compressionMethod)
{
	uint32_t maxThreadCount = getMaxThreadCount();

	choiceThreadCount->Enable();
	if (archiveFormat == L"7z")
	{
		if (compressionMethod == L"BZip2" ||
			compressionMethod == L"LZMA2")
		{
			choiceThreadCount->Clear();
			for (uint32_t i = 1; i <= maxThreadCount; i++)
				choiceThreadCount->Append(std::to_wstring(i));

			choiceThreadCount->SetSelection(maxThreadCount - 1);
		}
		else if (compressionMethod == L"LZMA")
		{
			choiceThreadCount->Clear();
			for (uint32_t i = 1; i <= 2; i++)
				choiceThreadCount->Append(std::to_wstring(i));

			choiceThreadCount->SetSelection(1);
		}
		else if (compressionMethod == L"PPMd")
		{
			choiceThreadCount->Clear();
			choiceThreadCount->Append(std::to_wstring(1));

			choiceThreadCount->SetSelection(0);

			choiceThreadCount->Disable();
		}
	}
	else if (archiveFormat == L"zip")
	{
		if (compressionMethod == L"BZip2" ||
			compressionMethod == L"LZMA" ||
			compressionMethod == L"PPMd" ||
			compressionMethod == L"Deflate" ||
			compressionMethod == L"Deflate64")
		{
			choiceThreadCount->Clear();
			for (uint32_t i = 1; i <= maxThreadCount; i++)
				choiceThreadCount->Append(std::to_wstring(i));

			choiceThreadCount->SetSelection(maxThreadCount - 1);
		}
	}
}

void ManualBackupDialog::initArchiveSettings()
{
	std::wstring destinationPath = fileHandler_->getDestinationPath();
	std::wstring archiveName = fileHandler_->getArchiveName();
	std::wstring compressionLevel = fileHandler_->getCompressionLevel();
	std::wstring updateMode = fileHandler_->getUpdateMode();
	std::wstring archiveFormat = fileHandler_->getArchiveFormat();
	std::wstring compressionMethod = fileHandler_->getCompressionMethod();
	std::wstring dictionarySize = fileHandler_->getDictionarySize();
	std::wstring dictionarySizeUnit = fileHandler_->getDictionarySizeUnit();
	std::wstring threadCount = fileHandler_->getThreadCount();
	std::wstring volumeSize = fileHandler_->getVolumeSize();
	std::wstring volumeSizeUnit = fileHandler_->getVolumeSizeUnit();
	std::wstring isSolid = fileHandler_->getIsSolid();

	// Destination Path
	dirPickerDestinationPath->SetPath(destinationPath);

	// Archive Name
	textCtrlArchiveName->SetLabel(archiveName);

	// Compression Level
	choiceCompressionLevel->SetStringSelection(compressionLevel);

	// Update Mode
	choiceUpdateMode->SetStringSelection(updateMode);

	// Archive Format
	radioBoxFormat->SetStringSelection(L"*." + archiveFormat);

	// Compression Method
	updateChoiceCompressionMethod(archiveFormat);
	choiceCompressionMethod->SetStringSelection(compressionMethod);

	// Dictionary Size
	updateChoiceDictionarySize(archiveFormat, compressionMethod);
	choiceDictionarySize->SetStringSelection(dictionarySize + L" " + dictionarySizeUnit);

	// Thread Count
	updateChoiceThreadCount(archiveFormat, compressionMethod);
	choiceThreadCount->SetStringSelection(threadCount);

	// Volume Size
	if (volumeSize != L"0")
		textCtrlVolumeSize->SetLabel(volumeSize);
	choiceVolumeSizeUnit->SetStringSelection(volumeSizeUnit);

	volumeSizeValidator_ = std::make_unique<wxTextValidator>(wxFILTER_DIGITS);
	textCtrlVolumeSize->SetValidator(*volumeSizeValidator_);

	// Solid Mode
	if (isSolid == "True")
		checkBoxSolidMode->SetValue(true);
	else
		checkBoxSolidMode->SetValue(false);
}



//// Main Frame ////
MainFrame::MainFrame(wxWindow* parent) : MainFrameBase(parent)
{
	directoryDialog_ = std::make_unique<wxDirDialog>(this, L"Choose directory", L"", wxDD_DEFAULT_STYLE);
	fileDialog_ = std::make_unique<wxFileDialog>(this, L"Choose file", L"", L"", L"Any file(*.*)|*.*", wxFD_DEFAULT_STYLE);

	manualBackupDialog_ = std::make_unique<ManualBackupDialog>(this);

	settingsFrame_ = std::make_unique<SettingsFrame>(this);

	initMainList();
}
MainFrame::~MainFrame() {}

void MainFrame::OnMainListItemSelected(wxListEvent& event)
{
	int selectedItemCount = mainList->GetSelectedItemCount();

	if (selectedItemCount > 0)
		buttonRemove->Enable();
	else
		buttonRemove->Disable();
}
void MainFrame::OnMainListItemDeselected(wxListEvent& event)
{
	int selectedItemCount = mainList->GetSelectedItemCount();

	if (selectedItemCount > 0)
		buttonRemove->Enable();
	else
		buttonRemove->Disable();
}

void MainFrame::OnArchiveButtonClicked(wxCommandEvent& event)
{
	manualBackupDialog_->Show();
}
void MainFrame::OnAddDirectoryButtonClicked(wxCommandEvent& event)
{
	if (directoryDialog_->ShowModal() == wxID_OK)
	{
		std::wstring path = directoryDialog_->GetPath().ToStdWstring();

		if (!isDuplicateLine(PATH_LIST_FILE, path))
		{
			addMainListItem(path);
			updateMainListColumnWidth();

			appendFileLine(PATH_LIST_FILE, path);
		}
	}
}
void MainFrame::OnAddFileButtonClicked(wxCommandEvent& event)
{
	if (fileDialog_->ShowModal() == wxID_OK)
	{
		std::wstring path = fileDialog_->GetPath().ToStdWstring();

		if (!isDuplicateLine(PATH_LIST_FILE, path))
		{
			addMainListItem(path);
			updateMainListColumnWidth();

			appendFileLine(PATH_LIST_FILE, path);
		}
	}
}
void MainFrame::OnRemoveButtonClicked(wxCommandEvent& event)
{
	long listItemIndex = -1;
	std::wstring listItemPath;

	while(true)
	{
		listItemIndex = mainList->GetNextItem(listItemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

		if (listItemIndex == -1)
			break;

		listItemPath = mainList->GetItemText(listItemIndex, 2).ToStdWstring();
		removeFileLine(PATH_LIST_FILE, listItemPath);

		mainList->DeleteItem(listItemIndex--);
	}

	updateMainListColumnWidth();
}
void MainFrame::OnSettingsButtonClicked(wxCommandEvent& event)
{
	settingsFrame_->Show();
}

void MainFrame::initMainList()
{
	std::wifstream fileInput(PATH_LIST_FILE);
	std::wstring archivablePath;

	while (getline(fileInput, archivablePath))
		addMainListItem(archivablePath);

	updateMainListColumnWidth();
}
void MainFrame::addMainListItem(std::wstring path)
{
	int index = mainList->GetItemCount();

	std::wstring name = path.substr(path.find_last_of(L'\\') + 1);
	std::wstring pathSize = shortenPathSize(computePathSize(path));

	mainList->InsertItem(index, name);
	mainList->SetItem(index, 1, pathSize);
	mainList->SetItem(index, 2, path);
}
void MainFrame::updateMainListColumnWidth()
{
	for (int i = 0; i < mainList->GetColumnCount(); i++)
	{
		mainList->SetColumnWidth(i, wxLIST_AUTOSIZE);
		int itemWidth = mainList->GetColumnWidth(i);

		mainList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
		int headerWidth = mainList->GetColumnWidth(i);

		mainList->SetColumnWidth(i, (std::max)(itemWidth, headerWidth));
	}
}