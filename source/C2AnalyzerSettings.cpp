#include "C2AnalyzerSettings.h"
#include <AnalyzerHelpers.h>


C2AnalyzerSettings::C2AnalyzerSettings()
:	mInputChannel_C2d( UNDEFINED_CHANNEL ),
	mInputChannel_C2ck(UNDEFINED_CHANNEL)

{
	mInputChannelInterface_C2d.reset(new AnalyzerSettingInterfaceChannel());
	mInputChannelInterface_C2d->SetTitleAndTooltip("C2D", "Standard C2 Analyzer for Silabs");
	mInputChannelInterface_C2d->SetChannel(mInputChannel_C2d);

	mInputChannelInterface_C2ck.reset(new AnalyzerSettingInterfaceChannel());
	mInputChannelInterface_C2ck->SetTitleAndTooltip("C2CK", "Standard C2 Analyzer for Silabs");
	mInputChannelInterface_C2ck->SetChannel(mInputChannel_C2ck);

	AddInterface(mInputChannelInterface_C2d.get());
	AddInterface(mInputChannelInterface_C2ck.get());

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel(mInputChannel_C2d, "C2D", false);
	AddChannel(mInputChannel_C2ck, "C2CK", false);
}

C2AnalyzerSettings::~C2AnalyzerSettings()
{
}

bool C2AnalyzerSettings::SetSettingsFromInterfaces()
{
	if (mInputChannelInterface_C2d->GetChannel() == mInputChannelInterface_C2ck->GetChannel())
	{
		SetErrorText("SDA and SCL can't be assigned to the same input.");
		return false;
	}
	mInputChannel_C2d = mInputChannelInterface_C2d->GetChannel();
	mInputChannel_C2ck = mInputChannelInterface_C2ck->GetChannel();

	ClearChannels();
	AddChannel(mInputChannel_C2d, "C2D", true);
	AddChannel(mInputChannel_C2ck, "C2CK", true);

	return true;
}

void C2AnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface_C2d->SetChannel(mInputChannel_C2d);
	mInputChannelInterface_C2ck->SetChannel(mInputChannel_C2ck);
}

void C2AnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel_C2d;
	text_archive >> mInputChannel_C2ck;

	ClearChannels();
	AddChannel(mInputChannel_C2d, "C2D", true);
	AddChannel(mInputChannel_C2ck, "C2CK", true);

	UpdateInterfacesFromSettings();
}

const char* C2AnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel_C2d;
	text_archive << mInputChannel_C2ck;

	return SetReturnString( text_archive.GetString() );
}
