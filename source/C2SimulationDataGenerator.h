#ifndef C2_SIMULATION_DATA_GENERATOR
#define C2_SIMULATION_DATA_GENERATOR

#define WRITEADD 0x3
#define READADD 0x2
#define WRITEDATA 0x1
#define READDATA 0x0

#include <AnalyzerHelpers.h>
#include <SimulationChannelDescriptor.h>
#include <string>
class C2AnalyzerSettings;

class C2SimulationDataGenerator
{
public:
	C2SimulationDataGenerator();
	~C2SimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, C2AnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );

protected:
	C2AnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;
	U8 mValue;

protected:
	void CreateReset();
	void CreateStartop();
	void CreateInstruction( U8 instruction );
	void CreateAddress( U8 instruction, U8 address);
	void CreateData( U8 instruction, U32 length);
	void CreateBit(BitState bit_state);
	void GetBit(BitState bit_state);
	void CreateByte(U8 data);
	void GetByte(U8 data);

protected:
	std::string mSerialText;
	U32 mStringIndex;

protected:
	ClockGenerator mClockGenerator;

	SimulationChannelDescriptorGroup mC2SimulationChannels;
	SimulationChannelDescriptor* mC2d;
	SimulationChannelDescriptor* mC2ck;
};
#endif //C2_SIMULATION_DATA_GENERATOR