#ifndef FK_INPUTRECEIVER_CLASS
#define FK_INPUTRECEIVER_CLASS

#include<irrlicht.h>
#include<map>

using namespace irr;

namespace fk_engine{

	class FK_InputReceiver : public IEventReceiver
	{
	public:
		//costructor
		FK_InputReceiver();
		//destructor
		~FK_InputReceiver();
		// set joypad info
		virtual void setJoypadInfo(core::array<SJoystickInfo> &joypadInfo);
		// This is the one method that we have to implement
		virtual bool OnEvent(const SEvent& event);
		// This is used to check whether a key is being held down
		virtual bool IsKeyDown(EKEY_CODE keyCode) const;
		// get joypad status
		virtual bool *JoypadStatus(int joypadId);
		virtual bool *KeyboardStatus();
		virtual void updateJoypadStatus(const SEvent& event);
		virtual void reset();
	private:
		// We use this array to store the current state of each key
		bool KeyIsDown[KEY_KEY_CODES_COUNT];
		/*bool KeyIsDownJoypad1[32];
		bool KeyIsDownJoypad2[32];*/
		std::map<int, bool*> KeyIsDownJoypad;
		std::map<int, SEvent::SJoystickEvent> JoypadState;
		//std::map<int, int> joypadIndexMap;
		const f32 AxisMaximumMovement = 32767.f;
		const int maxInputSize = 256;
		u32 joypadRecordedNumber;
	};
}
#endif