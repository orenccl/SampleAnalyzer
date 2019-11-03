#ifndef C2_ANALYZER_H
#define C2_ANALYZER_H

#include <Analyzer.h>
#include "C2AnalyzerResults.h"
#include "C2SimulationDataGenerator.h"


class C2AnalyzerSettings;
class ANALYZER_EXPORT C2Analyzer : public Analyzer2
{
public:
	C2Analyzer();
	virtual ~C2Analyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //functions
	void AdvanceToStartBit();
	void GetData();
	void GetBit(BitState& bit_state);
	void GetByte();
	void RecordStartStopBit();
	void GetIns();
	void GetLen();
	void Wait();

protected: //vars
	std::auto_ptr< C2AnalyzerSettings > mSettings;
	std::auto_ptr< C2AnalyzerResults > mResults;
	AnalyzerChannelData* mC2ck;
	AnalyzerChannelData* mC2d;
	U8 mLen;
	U8 mFlag;

	C2SimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //C2_ANALYZER_H
