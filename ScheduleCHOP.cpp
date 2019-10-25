/* Shared Use License: This file is owned by Derivative Inc. (Derivative) and
 * can only be used, and/or modified for use, in conjunction with 
 * Derivative's TouchDesigner software, and only if you are a licensee who has
 * accepted Derivative's TouchDesigner license or assignment agreement (which
 * also govern the use of this file).  You may share a modified version of this
 * file with another authorized licensee of Derivative's TouchDesigner software.
 * Otherwise, no redistribution or sharing of this file, with or without
 * modification, is permitted.
 */

#include "ScheduleCHOP.h"

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>

// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
void
FillCHOPPluginInfo(CHOP_PluginInfo *info)
{
	// Always set this to CHOPCPlusPlusAPIVersion.
	info->apiVersion = CHOPCPlusPlusAPIVersion;

	// The opType is the unique name for this CHOP. It must start with a 
	// capital A-Z character, and all the following characters must lower case
	// or numbers (a-z, 0-9)
	info->customOPInfo.opType->setString("Schedule");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("Schedule");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("Richard Burns");
	info->customOPInfo.authorEmail->setString("richard@richard-burns.com");

	// This CHOP can work with 0 inputs
	info->customOPInfo.minInputs = 0;

	// It can accept up to 1 input though, which changes it's behavior
	info->customOPInfo.maxInputs = 0;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const OP_NodeInfo* info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new ScheduleCHOP(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (ScheduleCHOP*)instance;
}

};


ScheduleCHOP::ScheduleCHOP(const OP_NodeInfo* info) : myNodeInfo(info)
{
	myExecuteCount = 0;
}

ScheduleCHOP::~ScheduleCHOP()
{

}

void
ScheduleCHOP::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;
	ginfo->timeslice = false;
	//ginfo->inputMatchIndex = 0;
}

bool
ScheduleCHOP::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{
	// If there is an input connected, we are going to match it's channel names etc
	// otherwise we'll specify our own.
	
	info->numChannels = 1;

		// Since we are outputting a timeslice, the system will dictate
		// the numSamples and startIndex of the CHOP data
		//info->numSamples = 1;
		//info->startIndex = 0

		// For illustration we are going to output 120hz data
	//info->sampleRate = 60;
	return true;
}

void
ScheduleCHOP::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
	name->setString("chan1");
}

void
ScheduleCHOP::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{
	//myExecuteCount++;

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::string ftime = asctime(timeinfo);
	
	char daybuffer[256];
	const char* dayformat = "%A";
	strftime(daybuffer, 256, dayformat, timeinfo);
	puts(daybuffer);
	//std::array<const char*, 5> day = { daybuffer };
	std::string day = daybuffer;
	inputs->enablePar("Sunday", 1);
	daysEnabled[0] = inputs->getParInt("Sunday");

	inputs->enablePar("Monday", 1);
	daysEnabled[1] = inputs->getParInt("Monday");

	inputs->enablePar("Tuesday", 1);
	daysEnabled[2] = inputs->getParInt("Tuesday");

	inputs->enablePar("Wednesday", 1);
	daysEnabled[3] = inputs->getParInt("Wednesday");

	inputs->enablePar("Thursday", 1);
	daysEnabled[4] = inputs->getParInt("Thursday");

	inputs->enablePar("Friday", 1);
	daysEnabled[5] = inputs->getParInt("Friday");

	inputs->enablePar("Saturday", 1);
	daysEnabled[6] = inputs->getParInt("Saturday");

	inputs->enablePar("Enablehour", 1);
	enableHour = inputs->getParInt("Enablehour")*3600;

	inputs->enablePar("Enableminute", 1);
	enableMinute = inputs->getParInt("Enableminute") * 60;

	inputs->enablePar("Enablesecond", 1);
	enableSecond = inputs->getParInt("Enablesecond");

	enableTotal = enableHour + enableMinute + enableSecond;

	inputs->enablePar("Disablehour", 1);
	disableHour = inputs->getParInt("Disablehour") * 3600;

	inputs->enablePar("Disableminute", 1);
	disableMinute = inputs->getParInt("Disableminute") * 60;

	inputs->enablePar("Disablesecond", 1);
	disableSecond = inputs->getParInt("Disablesecond");

	disableTotal = disableHour + disableMinute + disableSecond;
	
	// get current hour
	char hourbuf[128];
	const char* hourformat = "%H";
	strftime(hourbuf, 128, hourformat, timeinfo);
	puts(hourbuf);
	std::string hour = hourbuf;
	int hourInt = std::stoi(hour);

	// get current minute
	const char* minformat = "%M";
	strftime(hourbuf, 128, minformat, timeinfo);
	puts(hourbuf);
	std::string min = hourbuf;
	int minInt = std::stoi(min);

	// get current second
	const char* secformat = "%S";
	strftime(hourbuf, 128, secformat, timeinfo);
	puts(hourbuf);
	std::string sec = hourbuf;
	int secInt = std::stoi(sec);

	// get current weekday as decimal
	const char* dayformat2 = "%w";
	strftime(hourbuf, 128, dayformat2, timeinfo);
	puts(hourbuf);
	std::string dayDec = hourbuf;
	int dayInt = std::stoi(dayDec);

	int curSeconds = (hourInt * 3600) + (minInt * 60) + secInt;

	int finalValue = 0.0f;

	if (curSeconds >= enableTotal && curSeconds < disableTotal) {
		if (daysEnabled[dayInt] == 1) {
			finalValue = 1.0f;
		}
	}


	output->channels[0][0] = finalValue;

	
}

int32_t
ScheduleCHOP::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 0;
}

void
ScheduleCHOP::getInfoCHOPChan(int32_t index,
										OP_InfoCHOPChan* chan,
										void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.
}

bool		
ScheduleCHOP::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 0;
	infoSize->cols = 0;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
ScheduleCHOP::getInfoDATEntries(int32_t index,
										int32_t nEntries,
										OP_InfoDATEntries* entries, 
										void* reserved1)
{
}

void
ScheduleCHOP::setupParameters(OP_ParameterManager* manager, void *reserved1)
{
	// DAYS PAGE

	// Sunday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Sunday";
		np.label = "Sunday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Monday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Monday";
		np.label = "Monday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Tuesday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Tuesday";
		np.label = "Tuesday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Wednesday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Wednesday";
		np.label = "Wednesday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Thursday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Thursday";
		np.label = "Thursday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Friday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Friday";
		np.label = "Friday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// Saturday
	{
		OP_NumericParameter	np;

		np.page = "Days";
		np.name = "Saturday";
		np.label = "Saturday";
		np.defaultValues[0] = 1.0;

		OP_ParAppendResult res = manager->appendToggle(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// TIME PAGE
	// enable hour
	{
		OP_NumericParameter	np;

		np.page = "Time";
		np.name = "Enablehour";
		np.label = "Enable Hour";
		np.defaultValues[0] = 0.0;
		np.minValues[0] = 0.0f;
		np.clampMins[0] = 0.0f;
		np.maxValues[0] = 23.0f;
		np.clampMaxes[0] = 23.0f;
		np.maxSliders[0] = 23.0f;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// enable minute
	{
		OP_NumericParameter	np;

		np.page = "Time";
		np.name = "Enableminute";
		np.label = "Enable Minute";
		np.defaultValues[0] = 0.0;
		np.minValues[0] = 0.0f;
		np.clampMins[0] = 0.0f;
		np.maxValues[0] = 59.0f;
		np.clampMaxes[0] = 59.0f;
		np.maxSliders[0] = 59.0f;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// enable second
	{
		OP_NumericParameter	np;

		np.page = "Time";
		np.name = "Enablesecond";
		np.label = "Enable Second";
		np.defaultValues[0] = 0.0;
		np.minValues[0] = 0.0f;
		np.clampMins[0] = 0.0f;
		np.maxValues[0] = 59.0f;
		np.clampMaxes[0] = 59.0f;
		np.maxSliders[0] = 59.0f;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// disable hour
	{
		OP_NumericParameter	np;

		np.page = "Time";
		np.name = "Disablehour";
		np.label = "Disable Hour";
		np.defaultValues[0] = 0.0;
		np.minValues[0] = 0.0f;
		np.clampMins[0] = 0.0f;
		np.maxValues[0] = 23.0f;
		np.clampMaxes[0] = 23.0f;
		np.maxSliders[0] = 23.0f;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// enable minute
	{
		OP_NumericParameter	np;

		np.page = "Time";
		np.name = "Disableminute";
		np.label = "Disable Minute";
		np.defaultValues[0] = 0.0;
		np.minValues[0] = 0.0f;
		np.clampMins[0] = 0.0f;
		np.maxValues[0] = 59.0f;
		np.clampMaxes[0] = 59.0f;
		np.maxSliders[0] = 59.0f;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}

	// enable second
	{
		OP_NumericParameter	np;

		np.page = "Time";
		np.name = "Disablesecond";
		np.label = "Disable Second";
		np.defaultValues[0] = 0.0;
		np.minValues[0] = 0.0f;
		np.clampMins[0] = 0.0f;
		np.maxValues[0] = 59.0f;
		np.clampMaxes[0] = 59.0f;
		np.maxSliders[0] = 59.0f;

		OP_ParAppendResult res = manager->appendInt(np);
		assert(res == OP_ParAppendResult::Success);
	}


}

void 
ScheduleCHOP::pulsePressed(const char* name, void* reserved1)
{
}

