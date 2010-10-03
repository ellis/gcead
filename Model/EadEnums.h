/**
 * Copyright (C) 2008,2010  Ellis Whitehead
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EADENUMS_H
#define __EADENUMS_H

#include <QObject> // REFACTOR: Figure out which file to include for Q_ENUMS

#define EAD_SAMPLES_PER_SECOND 100


enum EadTask
{
	EadTask_Review,
	EadTask_Filter,
	EadTask_Markers,
	EadTask_Publish,
};
Q_ENUMS(EadTask);


enum EadView
{
	EadView_Averages,
	EadView_EADs,
	EadView_FIDs,
	EadView_All,
	EadView_Recording,
};
const int EadViewCount = EadView_Recording + 1;
Q_ENUMS(EadView);


/// Mode for handling peak viewing and editing
enum EadMarkerMode
{
	/// Don't show peaks
	EadMarkerMode_Hide,
	/// Show user-selected peaks
	EadMarkerMode_Show,
	/// Edit peaks
	EadMarkerMode_Edit,
};


/// These are the aspects of WaveInfo that can be edited
enum WaveEditorFlag
{
	WaveEditorFlag_Title = 0x01,
	WaveEditorFlag_Visible = 0x02,
	WaveEditorFlag_Sensitivity = 0x04,
	WaveEditorFlag_Timeshift = 0x08,
	WaveEditorFlag_Invert = 0x10,
	WaveEditorFlag_Comment = 0x20,
};
Q_DECLARE_FLAGS(WaveEditorFlags, WaveEditorFlag);
Q_DECLARE_OPERATORS_FOR_FLAGS(WaveEditorFlags);


/// List of events that may lead to emit ViewInfo::changed()
enum ViewChangeEvent
{
	/// Indicate: need to repaint chart
	ViewChangeEvent_Paint = 0x01,
	/// Indicate: need to render waves and repaint chart
	ViewChangeEvent_Render = 0x03,
	/// Indicate: need to recalc averages, render waves, and repaint chart
	ViewChangeEvent_CalcAve = 0x07,
};
Q_DECLARE_FLAGS(ViewChangeEvents, ViewChangeEvent);
Q_DECLARE_OPERATORS_FOR_FLAGS(ViewChangeEvents);


/// Elements to have on a chart
enum ChartElement
{
	ChartElement_Grid = 0x001,
	ChartElement_AxisTime = 0x002,
	ChartElement_WaveNames = 0x004,
	ChartElement_WaveComments = 0x008,
	ChartElement_Markers = 0x010,
	ChartElement_MarkerTime = 0x020,
	ChartElement_MarkerEadAmplitude = 0x040,
	ChartElement_MarkerFidArea = 0x080,
	/// Display standard deviation lines around averaged waves
	ChartElement_StdDev = 0x100,
};
Q_DECLARE_FLAGS(ChartElements, ChartElement);
Q_DECLARE_OPERATORS_FOR_FLAGS(ChartElements);


/// Return value when loading/saving files
enum LoadSaveResult
{
	LoadSaveResult_Ok,
	LoadSaveResult_CouldNotOpen,
	LoadSaveResult_WrongFormat,
	LoadSaveResult_VersionTooLow,
	LoadSaveResult_VersionTooHigh,
	LoadSaveResult_ImportedOldEad,
	LoadSaveResult_DataCorrupt,
};

/// Type of wave (EAD, FID, or digital)
enum WaveType
{
	WaveType_EAD,
	WaveType_FID,
	WaveType_Digital,
};
Q_ENUMS(WaveType);

/// Filter types
enum FilterMode
{
	FilterMode_Off,
	FilterMode_Default,
	FilterMode_Advanced
};
Q_ENUMS(FilterType);

/// Filter types
enum FilterType
{
	FilterType_1,
	FilterType_2
};
Q_ENUMS(FilterType);

#endif
