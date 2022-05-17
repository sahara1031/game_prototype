/*#include <iostream>
#include <time.h>
#include <alc.h>
#include <al.h>

#include "audio.h"

#pragma comment(lib,"OpenAL32.lib")

#define DEFAULT_GAIN (.1f)
#define DEFAULT_FREQ (440)


static ALuint sid;
static int waveform;
static ALuint buffers[AUDIO_WAVEFORM_MAX];
static unsigned int length;
static unsigned int start;
static float decay;
static float sweep;
static float gain;
static float pitch;
static float pitchTarget;
static float freqStart = DEFAULT_FREQ;
static float freq;
static float freqEnd;

int audioInit() {
	ALCdevice* device = alcOpenDevice(NULL);
	if (device == NULL) {
		return 1;
	}

	ALCcontext* context = alcCreateContext(
		device,
		NULL
	);

	if (context == NULL) {
		return 1;
	}
	alcMakeContextCurrent(context);

	alGenBuffers(
		AUDIO_WAVEFORM_MAX,
		buffers
	);

	unsigned char pulse[][8] = {
		{0xff,0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 	//AUDIO_WAVEFORM_PULSE_12_5,
		{0xff,0xff,0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	//AUDIO_WAVEFORM_PULSE_25,
		{0xff,0xff,0xff, 0xff, 0x00, 0x00, 0x00, 0x00 },	//AUDIO_WAVEFORM_PULSE_50,
		{0xff,0xff,0xff, 0xff, 0xff, 0xff, 0x00, 0x00 }	//AUDIO_WAVEFORM_PULSE_75,
	};

	for (int i = AUDIO_WAVEFORM_PULSE_12_5; i <= AUDIO_WAVEFORM_PULSE_75; i++) {
		ALsizei size = sizeof pulse[0];
		alBufferData(
			buffers[i],
			AL_FORMAT_MONO8,
			pulse[i],
			size,
			size * DEFAULT_FREQ
		);
	}

	unsigned char triangle[] = {
		0xff,0xee,0xdd,0xcc,0xbb,0xaa,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,
		0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff
	};
	alBufferData(
		buffers[AUDIO_WAVEFORM_TRIANGLE],
		AL_FORMAT_MONO8,
		triangle,
		sizeof triangle,
		sizeof triangle * DEFAULT_FREQ
	);
	//ALuint sid;
	alGenSources(
		1,
		&sid
	);


	alSourcei(
		sid,
		AL_LOOPING,
		AL_TRUE
	);

	return 0;
}

void audioWaveform(int _waveform) {
	waveform = _waveform;
}

void audioLength(unsigned int _millis) {
	length = _millis;
}

void audioDecay(float _decay) {
	decay = _decay;
}

void audioSweep(float _sweep) {
	sweep = _sweep;
}

void audioPitchTarget(float _sweepTarget) {
	pitchTarget = _sweepTarget;
}

void audioFreq(float _freq) {
	freqStart = _freq;
}

void audioPlay() {


	alSourcef(
		sid,
		AL_GAIN,
		gain = DEFAULT_GAIN
	);

	pitch = freqStart / DEFAULT_FREQ;
	alSourcef(
		sid,
		AL_PITCH,
		pitch
	);

	alSourcei(
		sid,
		AL_BUFFER,
		buffers[waveform]
	);
	alSourcePlay(sid);
	start = clock();
}

void audioStop() {
	alSourceStop(sid);
}

void audioUpdate() {
	if ((length > 0) && (clock() - start >= length))
		audioStop();

	if ((decay != 0) && (decay < 1)) {
		gain *= decay;
		alSourcef(
			sid,
			AL_GAIN,
			gain
		);
	}

	if (sweep != 0) {
		gain *= decay;
		if (pitchTarget != 0) {
			if (
				((sweep > 1) && (pitch >= pitchTarget))
				|| ((sweep < 1) && (pitch <= pitchTarget))
				)
			{
				audioStop();
			}
		}
		alSourcef(
			sid,
			AL_PITCH,
			pitch *= sweep
		);
	}
}*/