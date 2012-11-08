#include "MSequencer.h"
#include "FlMML.h"

namespace FlMML {

MSequencer::~MSequencer() {
	if (engine) engine->setSequencer(NULL);
}
	
void MSequencer::noteOn(int i,int noteNo,int velocity)
{
	m_status = STATUS_SOUND;
	if (i>=0 && i < m_trackArray.size()) {
		m_trackArray[i]->noteOn(noteNo,velocity);
	}
}

void MSequencer::noteOff(int i)
{
	if (i>=0 && i < m_trackArray.size()) {
		m_trackArray[i]->noteOff();
	}
}

}
