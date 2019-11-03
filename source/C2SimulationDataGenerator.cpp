#include "C2SimulationDataGenerator.h"
#include "C2AnalyzerSettings.h"

C2SimulationDataGenerator::C2SimulationDataGenerator()
:	mSerialText( "My first analyzer, woo hoo!" ),
	mStringIndex( 0 )
{
}

C2SimulationDataGenerator::~C2SimulationDataGenerator()
{
}

void C2SimulationDataGenerator::Initialize( U32 simulation_sample_rate, C2AnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mClockGenerator.Init(1000000, simulation_sample_rate);

	mC2d = mC2SimulationChannels.Add(settings->mInputChannel_C2d, mSimulationSampleRateHz, BIT_HIGH);
	mC2ck = mC2SimulationChannels.Add(settings->mInputChannel_C2ck, mSimulationSampleRateHz, BIT_HIGH);
	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10.0)); //insert 10 bit-periods of idle

	mValue = 0;
}

U32 C2SimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{ 
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while (mC2ck->GetCurrentSampleNumber() < adjusted_largest_sample_requested)
	{
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(50));
		
		CreateReset();
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateAddress(READADD, mValue++ + 12);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		U8 randomAddr = mValue++ + (rand() % 100);
		CreateAddress(WRITEADD, randomAddr);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateAddress(READADD, randomAddr);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateAddress(WRITEADD, randomAddr+1);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateAddress(WRITEADD, randomAddr+2);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateAddress(WRITEADD, randomAddr+3);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		
		
		CreateData(READDATA, 0);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateData(READDATA, rand() % 4);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateData(WRITEDATA, rand() % 4);
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(10));
		CreateData(WRITEDATA, rand() % 4);
	}

	*simulation_channels = mC2SimulationChannels.GetArray();
	return mC2SimulationChannels.GetCount();
}

void C2SimulationDataGenerator::CreateReset()
{
	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(1.0));

	//Reset Signal, C2ck low at least 20us plus.
	mC2ck->Transition();
	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByTimeS(2E-05));
	mC2ck->Transition();
	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByTimeS(2E-06));
}

void C2SimulationDataGenerator::CreateStartop()
{
	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));
	mC2ck->Transition();
	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));
	mC2ck->Transition();
}

void C2SimulationDataGenerator::CreateInstruction(U8 instruction)
{
	if (mC2ck->GetCurrentBitState() == BIT_HIGH)
	{
		mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));
		mC2ck->Transition();
	}

	BitExtractor bit_extractor(instruction, AnalyzerEnums::MsbFirst, 2);

	for (U32 i = 0; i<2; i++)
	{
		CreateBit(bit_extractor.GetNextBit());
	}
}

void C2SimulationDataGenerator::CreateBit(BitState bit_state)
{
	if (mC2ck->GetCurrentBitState() != BIT_LOW)
		AnalyzerHelpers::Assert("CreateBit expects to be entered with scl low");

	mC2d->TransitionIfNeeded(bit_state);

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));

	mC2ck->Transition(); //posedge

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));

	mC2ck->Transition(); //negedge
}

void C2SimulationDataGenerator::GetBit(BitState bit_state)
{
	if (mC2ck->GetCurrentBitState() != BIT_LOW)
		AnalyzerHelpers::Assert("CreateBit expects to be entered with scl low");

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));

	mC2ck->Transition(); //posedge

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByTimeS(1E-08));

	mC2d->TransitionIfNeeded(bit_state);

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));

	mC2ck->Transition(); //negedge
}

void C2SimulationDataGenerator::CreateByte(U8 data)
{
	BitExtractor bit_extractor(data, AnalyzerEnums::LsbFirst, 8);

	for (U32 i = 0; i<8; i++)
	{
		CreateBit(bit_extractor.GetNextBit());
	}
}

void C2SimulationDataGenerator::GetByte(U8 data)
{
	BitExtractor bit_extractor(data, AnalyzerEnums::LsbFirst, 8);

	for (U32 i = 0; i<8; i++)
	{
		GetBit(bit_extractor.GetNextBit());
	}
}

void C2SimulationDataGenerator::CreateAddress(U8 instruction, U8 address)
{
	if (instruction == WRITEADD)
	{
		CreateStartop();
		CreateInstruction(WRITEADD);
		CreateByte(address);
	}
	else if (instruction == READADD)
	{
		CreateStartop();
		CreateInstruction(READADD);
		GetByte(address);
	}
	else
		AnalyzerHelpers::Assert("Wrong instruction");

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));
	mC2ck->Transition();
	mC2d->TransitionIfNeeded(BIT_HIGH);
}

void C2SimulationDataGenerator::CreateData(U8 instruction, U32 length)
{
	U32 wait = (rand() % 8) +1;
	if (instruction == WRITEDATA)
	{
		CreateStartop();
		CreateInstruction(WRITEDATA);
		CreateInstruction(length);
	
		for (U32 i = 0; i <= length; i++)
		{
			CreateByte(mValue++ - 43 + (rand() % 100));
		}

		for (U32 i = 0; i<wait; i++)
		{
			GetBit(BIT_LOW);
		}
		GetBit(BIT_HIGH);
	}
	else if (instruction == READDATA)
	{
		CreateStartop();
		CreateInstruction(READDATA);
		CreateInstruction(length);
		for (U32 i = 0; i<wait; i++)
		{
			GetBit(BIT_LOW);
		}
		GetBit(BIT_HIGH);

		for (U32 i = 0; i <= length; i++)
		{
			GetByte(mValue++ + 16 + (rand() % 100));
		}
	}
	else
		AnalyzerHelpers::Assert("Wrong instruction");

	mC2SimulationChannels.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(0.5));
	mC2ck->Transition();
	mC2d->TransitionIfNeeded(BIT_HIGH);
}