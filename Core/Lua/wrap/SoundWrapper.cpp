#include "stdafx.h"
#include "SoundWrapper.h"


	void CLuaSound::DoExport()
	{
		using namespace Sound;
		using namespace luabind;
		//base openAL functions
		module(lua::GetState(),"Sound")
		[
			def("CreateSound"             ,CreateSound),
			def("PlaySound"               ,Sound::PlaySound),
			def("PlayOnce"                ,PlayOnce),
			def("StopSound"               ,StopSound),
			def("KillSound"               ,KillSound),
			def("SetSoundPos"             ,SetSoundPos),
			def("DuplicateSound"          ,DuplicateSound),
			def("SetListenerPosition"     ,SetListenerPosition),
			def("SetListenerOrientation"  ,SetListenerOrientation),
			def("SetOverallGain"          ,SetOverallGain),
			def("SetGainFor"              ,SetGainFor),
			def("SetWorldScale"           ,SetWorldScale),
			def("ClearThreadedGainControl",ClearThreadedGainControl),
			def("AddThreadedGainControl"  ,AddThreadedGainControl)
		];/**/


		//**/	   


		LuaObject::DoExport();
	}




