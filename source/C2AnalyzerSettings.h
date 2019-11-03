#ifndef C2_ANALYZER_SETTINGS
#define C2_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class C2AnalyzerSettings : public AnalyzerSettings
{
public:
	C2AnalyzerSettings();
	virtual ~C2AnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	Channel mInputChannel_C2d;
	Channel mInputChannel_C2ck;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface_C2d;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface_C2ck;
};

#endif //C2_ANALYZER_SETTINGS
