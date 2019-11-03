#include "C2Analyzer.h"
#include "C2AnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <AnalyzerHelpers.h>

C2Analyzer::C2Analyzer()
:	Analyzer2(),  
	mSettings( new C2AnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

C2Analyzer::~C2Analyzer()
{
	KillThread();
}

void C2Analyzer::SetupResults()
{
	mResults.reset( new C2AnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );

	mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel_C2d);
}

void C2Analyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();

	mC2d = GetAnalyzerChannelData(mSettings->mInputChannel_C2d);
	mC2ck = GetAnalyzerChannelData(mSettings->mInputChannel_C2ck);

	//AdvanceToStartBit(); //already skip start bit.

	for (;;)
	{
		AdvanceToStartBit();
		GetIns();
		GetLen();
		GetData();
		CheckIfThreadShouldExit();
	}
}

void C2Analyzer::GetData()
{
	if (mFlag == C2_WRITE_ADDR || mFlag == C2_READ_ADDR)
	{
		U64 value;
		DataBuilder data;
		data.Reset(&value, AnalyzerEnums::LsbFirst, 8);
		BitState bit_state;
		Frame frame;

		frame.mStartingSampleInclusive = mC2ck->GetSampleNumber();

		if (mFlag == C2_READ_ADDR){ mC2ck->AdvanceToNextEdge(); }
		for (U32 i = 0; i < 8; i++)
		{
			GetBit(bit_state);
			data.AddBit(bit_state);
		}
		frame.mEndingSampleInclusive = mC2ck->GetSampleNumber();

		frame.mType = C2Data;
		frame.mData1 = value;
		mResults->AddFrame(frame);
		mResults->CommitResults();
		ReportProgress(frame.mEndingSampleInclusive);

		if (mFlag == C2_WRITE_ADDR){ mC2ck->AdvanceToNextEdge(); }
		RecordStartStopBit();
	}
	else if (mFlag == C2_WRITE_DATA || mFlag == C2_READ_DATA)
	{
		U64 value;
		DataBuilder data;
		data.Reset(&value, AnalyzerEnums::LsbFirst, 8);
		BitState bit_state;
		Frame frame;

		if (mFlag == C2_READ_DATA)
		{ 
			frame.mStartingSampleInclusive = mC2ck->GetSampleNumber();
			mC2ck->AdvanceToNextEdge();
			Wait();
			frame.mEndingSampleInclusive = mC2ck->GetSampleNumber();
			frame.mType = C2Data;
			frame.mFlags = C2_WAIT;
			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);
		}

		for (U32 j = 0; j < mLen; j++)
		{
			data.Reset(&value, AnalyzerEnums::LsbFirst, 8);
			frame.mStartingSampleInclusive = mC2ck->GetSampleNumber();
			for (U32 i = 0; i < 8; i++)
			{
				GetBit(bit_state);
				data.AddBit(bit_state);
			}
			frame.mEndingSampleInclusive = mC2ck->GetSampleNumber();
			frame.mType = C2Data;
			frame.mFlags = C2_NO;
			frame.mData1 = value;
			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);
		}

		if (mFlag == C2_WRITE_DATA)
		{ 
			frame.mStartingSampleInclusive = mC2ck->GetSampleNumber();
			mC2ck->AdvanceToNextEdge();
			Wait();
			frame.mEndingSampleInclusive = mC2ck->GetSampleNumber();
			frame.mType = C2Data;
			frame.mFlags = C2_WAIT;
			mResults->AddFrame(frame);
			mResults->CommitResults();
			ReportProgress(frame.mEndingSampleInclusive);
		}
		RecordStartStopBit();
	}
}

void C2Analyzer::GetBit(BitState& bit_state)
{
	mC2ck->AdvanceToNextEdge();
	mC2d->AdvanceToAbsPosition(mC2ck->GetSampleNumber());

	bit_state = mC2d->GetBitState();
	mC2ck->AdvanceToNextEdge();
}

void C2Analyzer::GetIns()
{
	U64 value;
	DataBuilder data;
	data.Reset(&value, AnalyzerEnums::MsbFirst, 2);
	BitState bit_state;
	Frame frame;

	frame.mStartingSampleInclusive = mC2ck->GetSampleNumber();
	GetBit(bit_state);
	data.AddBit(bit_state);
	GetBit(bit_state);
	data.AddBit(bit_state);
	frame.mEndingSampleInclusive = mC2ck->GetSampleNumber();

	frame.mType = C2Ins;
	frame.mData1 = value;

	switch (value){
		case 0 :
			frame.mFlags = C2_READ_DATA;
			mFlag = C2_READ_DATA;
			break;
		case 1:
			frame.mFlags = C2_WRITE_DATA;
			mFlag = C2_WRITE_DATA;
			break;
		case 2:
			frame.mFlags = C2_READ_ADDR;
			mFlag = C2_READ_ADDR;
			break;
		case 3:
			frame.mFlags = C2_WRITE_ADDR;
			mFlag = C2_WRITE_ADDR;
			break;
	}

	mResults->AddFrame(frame);
	mResults->CommitResults();
	ReportProgress(frame.mEndingSampleInclusive);
}

void C2Analyzer::GetLen()
{
	if (mFlag == C2_READ_ADDR || mFlag == C2_WRITE_ADDR)
	{
		mLen = 1;
	}
	else
	{
		U64 value;
		DataBuilder data;
		data.Reset(&value, AnalyzerEnums::MsbFirst, 2);
		BitState bit_state;
		Frame frame;

		frame.mStartingSampleInclusive = mC2ck->GetSampleNumber();
		GetBit(bit_state);
		data.AddBit(bit_state);
		GetBit(bit_state);
		data.AddBit(bit_state);
		frame.mEndingSampleInclusive = mC2ck->GetSampleNumber();

		frame.mType = C2Len;
		frame.mData1 = value;
		mLen = 1 + value;

		mResults->AddFrame(frame);
		mResults->CommitResults();
		ReportProgress(frame.mEndingSampleInclusive);
	}
}

void C2Analyzer::Wait()
{
	mC2ck->AdvanceToNextEdge();
	mC2d->AdvanceToAbsPosition(mC2ck->GetSampleNumber());

	while (!(mC2d->GetBitState()))
	{
		mC2ck->AdvanceToNextEdge();
		mC2ck->AdvanceToNextEdge();
		mC2d->AdvanceToAbsPosition(mC2ck->GetSampleNumber());
	}
	mC2ck->AdvanceToNextEdge();
}

void C2Analyzer::AdvanceToStartBit()
{

	for (;;)
	{
		mC2ck->AdvanceToNextEdge();
		if (mC2ck->GetBitState() == BIT_LOW)
		{
			for (;;)
			{
				if ((mC2ck->GetSampleOfNextEdge() - mC2ck->GetSampleNumber()) < mSampleRateHz/50000)
				{
					RecordStartStopBit();
					break;
				}
				mC2ck->AdvanceToNextEdge();
				mC2ck->AdvanceToNextEdge();
			}
			//mC2ck negedge
			mC2d->AdvanceToAbsPosition(mC2ck->GetSampleNumber());
			if (mC2d->GetBitState() == BIT_HIGH)
			{
				mC2ck->AdvanceToNextEdge();
				mC2d->AdvanceToAbsPosition(mC2ck->GetSampleNumber());
				if (mC2d->GetBitState() == BIT_HIGH)
					break;
			}
		}
	}
	mC2ck->AdvanceToNextEdge(); //C2ck low
}

void C2Analyzer::RecordStartStopBit()
{
	if (mC2ck->GetBitState() == BIT_LOW)
	{
		//negedge -> START / restart
		mResults->AddMarker(mC2ck->GetSampleNumber(), AnalyzerResults::Start, mSettings->mInputChannel_C2ck);
	}
	else
	{
		//posedge -> STOP
		mResults->AddMarker(mC2ck->GetSampleNumber(), AnalyzerResults::Stop, mSettings->mInputChannel_C2ck);
	}

	mResults->CommitPacketAndStartNewPacket();
	mResults->CommitResults();
}

bool C2Analyzer::NeedsRerun()
{
	return false;
}

U32 C2Analyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 C2Analyzer::GetMinimumSampleRateHz()
{
	return 2000000;
}

const char* C2Analyzer::GetAnalyzerName() const
{
	return "C2 Analyzer for Silabs";
}

const char* GetAnalyzerName()
{
	return "C2 Analyzer for Silabs";
}

Analyzer* CreateAnalyzer()
{
	return new C2Analyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}