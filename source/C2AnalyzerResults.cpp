#include "C2AnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "C2Analyzer.h"
#include "C2AnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

C2AnalyzerResults::C2AnalyzerResults( C2Analyzer* analyzer, C2AnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

C2AnalyzerResults::~C2AnalyzerResults()
{
}

void C2AnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame(frame_index);

	if (frame.mType == C2Data)
	{
		char Data[32];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, Data, 32);

		std::stringstream ss;
		if (frame.mFlags == C2_WAIT)
		{
			AddResultString("Wait");
			return;
		}
		ss << "[" << Data << "]";
		AddResultString(ss.str().c_str());
		ss.str("");

		ss << "Data is [" << Data << "]";
		AddResultString(ss.str().c_str());
		ss.str("");
	}
	else if (frame.mType == C2Ins)
	{
		char ins[32];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 2, ins, 32);

		std::stringstream ss;
		switch (frame.mFlags)
		{
			case C2_READ_DATA:
				AddResultString("RD");
				AddResultString("Read Data");
				break;
			case C2_WRITE_DATA:
				AddResultString("WD");
				AddResultString("Write Data");
				break;
			case C2_READ_ADDR:
				AddResultString("RA");
				AddResultString("Read Addr");
				break;
			case C2_WRITE_ADDR:
				AddResultString("WA");
				AddResultString("Write Addr");
				break;
		}
	}
	else if (frame.mType == C2Len)
	{
		char len[32];
		AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 2, len, 32);
		std::stringstream ss;

		switch (frame.mData1)
		{
		case 0:
			AddResultString("1");
			AddResultString("1 Byte");
			AddResultString("Length is 1 Byte");
			break;
		case 1:
			AddResultString("2");
			AddResultString("2 Bytes");
			AddResultString("Length is 2 Byte");
			break;
		case 2:
			AddResultString("3");
			AddResultString("3 Bytes");
			AddResultString("Length is 3 Byte");
			break;
		case 3:
			AddResultString("4");
			AddResultString("4 Bytes");
			AddResultString("Length is 4 Byte");
			break;
		}
	}
}

void C2AnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{

}

void C2AnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{

}

void C2AnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString("not supported");
}

void C2AnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString("not supported");
}