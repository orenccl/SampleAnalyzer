#ifndef C2_ANALYZER_RESULTS
#define C2_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define C2_READ_DATA (1 << 0)
#define C2_WRITE_DATA (1 << 1)
#define C2_READ_ADDR (1 << 2)
#define C2_WRITE_ADDR (1 << 3)
#define C2_WAIT (1 << 4)
#define C2_NO (1 << 5)

enum C2FrameType { C2Data, C2Ins, C2Len };

class C2Analyzer;
class C2AnalyzerSettings;

class C2AnalyzerResults : public AnalyzerResults
{
public:
	C2AnalyzerResults( C2Analyzer* analyzer, C2AnalyzerSettings* settings );
	virtual ~C2AnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	C2AnalyzerSettings* mSettings;
	C2Analyzer* mAnalyzer;
};

#endif //C2_ANALYZER_RESULTS
