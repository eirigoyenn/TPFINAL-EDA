#include "GEN_FSM.h"


GEN_FSM::~GEN_FSM() {

}
void GEN_FSM::setRandomTime(unsigned long int Randomtime_) {
	RandomTime = Randomtime_;
}

void GEN_FSM::RutaDefault(genericEvent* ev)
{
	return;
}
void GEN_FSM::Noevent_r_acc(genericEvent* ev)
{

	return;
}
void GEN_FSM::idle_r_acc(genericEvent* ev)
{
	if (!RandomTime) {
		//cambiar estado  a mandar mensaje
	}
	return;
}
