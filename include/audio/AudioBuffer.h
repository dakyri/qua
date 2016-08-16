#ifndef _AUDIOBUFFER
#define _AUDIOBUFFER

#include "CSampleBuf.h"

// almost every multi channel plug I know is in stereo pairs.
// we're using this initially for vst plugins, though internal Qua objects may get there as well
// our main real cases are
// - multi channel in to mono buffer
// - multi channel in to multi channel buffer ... we aren't expecting more ins than our buffers
// - multi channel out to mono
// - mono out to multi channel
// - multi channel out to multi channel
// TODO XXX FIXME we aren't being anal checking about validity atm ... maybe that's ok though
//
// we're assuming that nInChannel and nOutChannel are <= nChannel (typically 2 ... atm paths are stereo or mono), except for the mono > stereo cases.
// the hard core multi channel conversions will need a different approach anyway
//
// now we need read and write functions for those higher outputs

#define USE_MEM_CPY_SET

class AudioBuffer {
public:
	AudioBuffer(int nc, int nf) : buffers(nullptr), nChannel(nc), nFrames(nf) {
		allocateBuffers();
	}

	AudioBuffer() : buffers(nullptr), nChannel(2), nFrames(0) {
	}

	virtual ~AudioBuffer() {
		deallocateBuffers();
	}

	void set(int nc, int nf) {
		deallocateBuffers();
		nChannel = nc;
		nFrames = nf;
		allocateBuffers();
	}

	void zero() {
		for (int j = 0; j < nChannel; j++) {
#ifdef USE_MEM_CPY_SET
			memset(buffers[j], 0, nFrames * sizeof(float));
#else
			for (int i = 0; i < nFrames; i++) { // TODO XXXX could be memset
				buffers[j][i] = 0;
			}
#endif
		}
	}

	long copyFrom(float **inSig, int nInChan, int nInFrames) {
		int nf = (nInFrames < nFrames) ? nInFrames : nFrames;
		if (nChannel == 1 && nInChan > 1) { // average incoming into multi channel to mono
			for (int i = 0; i < nf; i++) {
				float s = 0;
				for (int j = 0; j < nInChan; j++) {
					s += inSig[j][i];
				}
				buffers[0][i] = s / nInChan;
			}
		}
		else {
			for (int j = 0; j < nInChan; j++) { // TODO XXXX could be memcpy
				for (int i = 0; i < nf; i++) {
					buffers[j][i] = inSig[j][i];
				}
			}
		}
		return nf;
	}

	long addFrom(float **inSig, int nInChan, int nInFrames) {
		int nf = (nInFrames < nFrames) ? nInFrames : nFrames;

		if (nChannel == 1 && nInChan > 1) { // average incoming into multi channel to mono
			for (int i = 0; i < nf; i++) {
				float s = 0;
				for (int j = 0; j < nInChan; j++) {
					s += inSig[j][i];
				}
				buffers[0][i] += s/nInChan;
			}
		} else {
			for (int j = 0; j < nInChan; j++) {
				for (int i = 0; i < nf; i++) {
					buffers[j][i] += inSig[j][i];
				}
			}
		}
	
		return nf;
	}

	long copyToPairFrom(int pini, float **inSig, int nInChan, int nInFrames) {
		int nf = (nInFrames < nFrames) ? nInFrames : nFrames;
		if (nChannel == 1) { // average incoming into multi channel to mono
			int nc = nInChan;
			if (nc > 2) nc = 2;
			for (int i = 0; i < nf; i++) {
				float s = 0;
				for (int j = 0; j < nInChan; j++) {
					s += inSig[j][i];
				}
				buffers[0][i] = s / nInChan;
			}
		} else {
			int ncc = nChannel - pini;
			if (ncc > 2) ncc = 2;
			for (int j = 0; j < ncc; j++) { // TODO XXXX could be memcpy
				for (int i = 0; i < nf; i++) {
					buffers[pini+j][i] = inSig[j][i];
				}
			}
		}
		return nf;
	}

	long addToPairFrom(int pini, float **inSig, int nInChan, int nInFrames) {
		int nf = (nInFrames < nFrames) ? nInFrames : nFrames;
		if (nChannel == 1) { // average incoming into multi channel to mono
			int nc = nInChan;
			if (nc > 2) nc = 2;
			for (int i = 0; i < nf; i++) {
				float s = 0;
				for (int j = 0; j < nInChan; j++) {
					s += inSig[j][i];
				}
				buffers[0][i] += s / nInChan;
			}
		}
		else {
			int ncc = nChannel - pini;
			if (ncc > 2) ncc = 2;
			for (int j = 0; j < ncc; j++) { // TODO XXXX could be memcpy
				for (int i = 0; i < nf; i++) {
					buffers[pini + j][i] += inSig[j][i];
				}
			}
		}
		return nf;
	}


	long copyTo(float **outSig, int nOutChan, int nOutFrames) {
		int nf = (nOutFrames < nFrames) ? nOutFrames : nFrames;
		if (nChannel == 1 && nOutChan > nChannel) { // mono multi-plex to all output channels
													// TODO XXXX could be memcpy
			for (int j = 0; j < nOutChan; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] = buffers[0][i];
				}
			}
		} else if (nOutChan == 1 && nChannel > nOutChan) {
			// TODO XXXX hardcoded to stereo to mono 
			/*
			for (int i = 0; i < nf; i++) {
				float s = 0;
				for (int j = 0; j < nChannel; j++) {
					s += buffers[j][i];
				}
				outSig[0][i] = s / nChannel;
			}
			*/
			for (int i = 0; i < nf; i++) {
				outSig[0][i] = (buffers[0][i] + buffers[1][i]) / 2;
			}
		} else { // TODO XXXX could be memcpy
			for (int j = 0; j < nOutChan; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] = buffers[j][i];
				}
			}
		}
		return nf;
	}

	long addTo(float **outSig, int nOutChan, int nOutFrames) {
		int nf = (nOutFrames < nFrames) ? nOutFrames : nFrames;
		if (nChannel == 1 && nOutChan > nChannel) { // mono multi-plex to all output channels
			for (int j = 0; j < nOutChan; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] += buffers[0][i];
				}
			}
		} else if (nOutChan == 1 && nChannel > nOutChan) {
			// TODO XXXX hardcoded to stereo to mono 
			/*
			for (int i = 0; i < nf; i++) {
			float s = 0;
			for (int j = 0; j < nChannel; j++) {
			s += buffers[j][i];
			}
			outSig[0][i] = s / nChannel;
			}
			*/
			for (int i = 0; i < nf; i++) {
				outSig[0][i] += (buffers[0][i] + buffers[1][i]) / 2;
			}
		} else {
			for (int j = 0; j < nOutChan; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] += buffers[j][i];
				}
			}
		}

		return nf;
	}


	/**
	* @param 0 based index of the pair
	*/
	long copyPairTo(int pini, float **outSig, int nOutChan, int nOutFrames) {
		int nf = (nOutFrames < nFrames) ? nOutFrames : nFrames;
		if (nChannel == 1 && nOutChan > nChannel) { // mono multi-plex to all output channels
													// TODO XXXX could be memcpy
			for (int j = 0; j < nOutChan; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] = buffers[0][i];
				}
			}
		} else if (nOutChan == 1) {
			// TODO XXXX hardcoded to stereo to mono 
			/*
			for (int i = 0; i < nf; i++) {
			float s = 0;
			for (int j = 0; j < nChannel; j++) {
			s += buffers[j][i];
			}
			outSig[0][i] = s / nChannel;
			}
			*/
			if ((nChannel - pini) > 1) {
				for (int i = 0; i < nf; i++) {
					outSig[0][i] = (buffers[pini][i] + buffers[pini + 1][i]) / 2;
				}
			} else {// TODO XXXX could be memcpy
				for (int i = 0; i < nf; i++) {
					outSig[0][i] = buffers[pini][i];
				}
			}
		} else { // TODO XXXX could be memcpy
			int ncc = nChannel - pini;
			if (ncc > 2) ncc = 2;
			for (int j = 0; j < ncc; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] = buffers[pini + j][i];
				}
			}
		}
		return nf;
	}

	/**
	* @param 0 based index of the pair
	*/
	long addPairTo(int pini, float **outSig, int nOutChan, int nOutFrames) {
		int nf = (nOutFrames < nFrames) ? nOutFrames : nFrames;
		if (nChannel == 1 && nOutChan > nChannel) { // mono multi-plex to all output channels
													// TODO XXXX could be memcpy
			for (int j = 0; j < nOutChan; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] += buffers[0][i];
				}
			}
		} else if (nOutChan == 1) {
			// TODO XXXX hardcoded to stereo to mono 
			/*
			for (int i = 0; i < nf; i++) {
			float s = 0;
			for (int j = 0; j < nChannel; j++) {
			s += buffers[j][i];
			}
			outSig[0][i] = s / nChannel;
			}
			*/
			if ((nChannel - pini) > 1) {
				for (int i = 0; i < nf; i++) {
					outSig[0][i] += (buffers[pini][i] + buffers[pini + 1][i]) / 2;
				}
			}
			else {// TODO XXXX could be memcpy
				for (int i = 0; i < nf; i++) {
					outSig[0][i] += buffers[pini][i];
				}
			}
		} else { // TODO XXXX could be memcpy
			int ncc = nChannel - pini;
			if (ncc > 2) ncc = 2;
			for (int j = 0; j < ncc; j++) {
				for (int i = 0; i < nf; i++) {
					outSig[j][i] += buffers[pini + j][i];
				}
			}
		}
		return nf;
	}

	float** getBuffers() { return buffers; }

protected:
	void allocateBuffers() {
		if (nChannel > 0 && nFrames > 0) {
			buffers = new float*[nChannel];
			for (int i = 0; i < nChannel; i++) {
				buffers[i] = new float[nFrames];
			}
		}
	}

	void deallocateBuffers() {
		if (buffers != nullptr) {
			for (int i = 0; i < nChannel; i++) {
				delete[] buffers[i];
			}
			delete[] buffers;
			buffers = nullptr;
		}
	}

	float **buffers;
	int nChannel;
	int nFrames;
};

#endif