#include "QuaOSC.h"

QuaOSCManager::QuaOSCManager() {

}

QuaOSCManager::~QuaOSCManager() {

}

QuaPort *
QuaOSCManager::findPortByName(string nm) {
	return nullptr;
}
status_t
QuaOSCManager::connect(Input *) {
	return B_OK;
}

status_t
QuaOSCManager::connect(Output *) {
	return B_OK;
}

status_t
QuaOSCManager::disconnect(Input *) {
	return B_OK;
}

status_t
QuaOSCManager::disconnect(Output *) {
	return B_OK;
}