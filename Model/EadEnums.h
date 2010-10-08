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
	EadTask_Markers,
	EadTask_Publish,
	EadTask_Filter,
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


/// Marker type
enum MarkerType
{
	MarkerType_Generic = 0,
	MarkerType_Time = 1,
	MarkerType_Value = 2,
	MarkerType_Duration = 3,
	MarkerType_Amplitude = 4,

	MarkerType_FidPeak = 20,
	MarkerType_EadPeakXY = 21,
	MarkerType_EadPeakXYZ = 22,
};
Q_ENUMS(MarkerType);


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
	ChartElement_Grid = 0x0001,
	ChartElement_AxisTime = 0x0002,
	ChartElement_WaveNames = 0x0004,
	ChartElement_WaveComments = 0x0008,
	/// Display standard deviation lines around averaged waves
	ChartElement_StdDev = 0x0010,
};
Q_DECLARE_FLAGS(ChartElements, ChartElement);
Q_DECLARE_OPERATORS_FOR_FLAGS(ChartElements);


/// Maker elements to have on a chart
enum ChartElementFidPeak
{
	ChartElementFidPeak_Show = 1,
	ChartElementFidPeak_Time = 2,
	ChartElementFidPeak_Area = 4,
};
Q_DECLARE_FLAGS(ChartElementFidPeaks, ChartElementFidPeak);
Q_DECLARE_OPERATORS_FOR_FLAGS(ChartElementFidPeaks);


/// Maker elements to have on a chart
enum ChartElementEadPeak
{
	ChartElementEadPeak_Show = 1,
	ChartElementEadPeak_Amplitude = 2,
	ChartElementEadPeak_TimeSpans = 4,
	ChartElementEadPeak_TimeStamps = 8,
};
Q_DECLARE_FLAGS(ChartElementEadPeaks, ChartElementEadPeak);
Q_DECLARE_OPERATORS_FOR_FLAGS(ChartElementEadPeaks);


/// Maker elements to have on a chart
enum ChartElementTimeMarker
{
	ChartElementTimeMarker_Show = 1,
};
Q_DECLARE_FLAGS(ChartElementTimeMarkers, ChartElementTimeMarker);
Q_DECLARE_OPERATORS_FOR_FLAGS(ChartElementTimeMarkers);


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
