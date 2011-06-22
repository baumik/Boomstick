#include "Joysticks.h"

#if defined WIN32
#include "win_common.h"
#include "Win\WinMessageWindow.h"
#endif

using namespace OIS;

const char *g_DeviceType[6] = {
  "OISUnknown", "OISKeyboard", "OISMouse",
  "OISJoyStick", "OISTablet", "OISOther"
};

JoysticksHandler::JoysticksHandler() {
  status = "uninitialized";
}

JoysticksHandler::~JoysticksHandler() {
  if( m_InputManager ) {
    InputManager::destroyInputSystem(m_InputManager);
  }
}

bool JoysticksHandler::buttonPressed( const JoyStickEvent &arg, int button ) {
  return true;
}

bool JoysticksHandler::buttonReleased( const JoyStickEvent &arg, int button ) {
  return true;
}

bool JoysticksHandler::axisMoved( const JoyStickEvent &arg, int axis ) {
  return true;
}

bool JoysticksHandler::povMoved( const JoyStickEvent &arg, int pov ) {
  return true;
}

bool JoysticksHandler::vector3Moved( const JoyStickEvent &arg, int index) {
  return true;
}

void JoysticksHandler::capture() {
#if defined WIN32
  MSG  msg;
  while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }
#endif

  for( int i = 0; i < MAX_JOYSTICKS ; ++i ) {
    if( m_joys[i] ) {
      m_joys[i]->capture();
    }
  }
}

std::vector<JoyStickState> JoysticksHandler::joyStickStates() {
  std::vector<JoyStickState> result;

  for( int i = 0; i < MAX_JOYSTICKS ; ++i ) {
    if( m_joys[i] ) {
      result.push_back(m_joys[i]->getJoyStickState());
    }
  }

  return result;
}

void JoysticksHandler::initialize() {
  try {
    OIS::ParamList pl;

#if defined WIN32
	FB::WinMessageWindow messageWindow;
	HWND hwnd = messageWindow.getHWND();

	std::ostringstream wnd;
	wnd << (size_t)hwnd;

	pl.insert(std::make_pair( std::string("WINDOW"), wnd.str() ));

#endif

    m_InputManager = InputManager::createInputSystem(pl);
    m_InputManager->enableAddOnFactory(InputManager::AddOn_All);

    status = "input manager created";

    try {
      status = "before initializing joysticks";

      //This demo uses at most 8 joysticks - use old way to create (i.e. disregard vendor)
      int numSticks = min(m_InputManager->getNumberOfDevices(OISJoyStick), MAX_JOYSTICKS);
      for( int i = 0; i < numSticks; ++i ) {
        m_joys[i] = (JoyStick*)m_InputManager->createInputObject( OISJoyStick, true );
        m_joys[i]->setEventCallback( this );
      }

      status = (boost::format("Initialized %d joysticks") % numSticks).str();
    } catch(OIS::Exception &ex) {
      status = ex.eText;
    }
  } catch( const Exception &ex ) {
    status = ex.eText;
  } catch(std::exception &ex) {
    status = ex.what();
  }
}
