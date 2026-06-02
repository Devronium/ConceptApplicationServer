//------------ standard header -----------------------------------//
#include "stdlibrary.h"
//------------ end of standard header ----------------------------//
#include "library.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pointer_list.h"

#include "piper.hpp"

struct piper::PiperConfig piper_config;

#ifndef PIPER_ONLY
#include "helper.h"

DEFINE_LIST(supertonic_list);

class SupertonicConfig {
public:
	Ort::Env env;
	Ort::MemoryInfo memory_info;
	std::unique_ptr<TextToSpeech> text_to_speech;
	std::vector<Style> style;

	void loadStyles(char **paths) {
		while ((paths) && (*paths) && ((*paths)[0])) {
			std::vector<std::string> vec;
			vec.push_back(*paths);
			paths ++;

			style.push_back(loadVoiceStyle(vec, true));
		}
	}

	SupertonicConfig(const char *onnx_dir, char **voice_style_path):
		env(ORT_LOGGING_LEVEL_WARNING, "Supertonic"),
		memory_info(Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault)) {

		loadStyles(voice_style_path);

		text_to_speech = loadTextToSpeech(this->env, onnx_dir, false);
	}

	TextToSpeech::SynthesisResult tts(const char *text, const char *lang, int style_index = 0, int total_step = 7, float speed = 1.05f) {
		if ((style_index >= 0) && (style_index < style.size()))
			return text_to_speech->call(this->memory_info, text ? text : "", lang ? lang : "na", style[style_index], total_step, speed);

		TextToSpeech::SynthesisResult null_style;
		return null_style;
	}
};
#endif
//=====================================================================================//
CONCEPT_DLL_API ON_CREATE_CONTEXT MANAGEMENT_PARAMETERS {
    try {
        piper::initialize(piper_config);
    } catch (...) {
    }
#ifndef PIPER_ONLY
    INIT_LIST(supertonic_list);
#endif
    return 0;
}
//=====================================================================================//
CONCEPT_DLL_API ON_DESTROY_CONTEXT MANAGEMENT_PARAMETERS {
    if (!HANDLER) {
        try {
            piper::terminate(piper_config);
        } catch (...) {
        }
#ifndef PIPER_ONLY
        DEINIT_LIST(supertonic_list);
#endif
    }
    return 0;
}
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PiperInit, 2)
    T_STRING(PiperInit, 0);
    T_STRING(PiperInit, 1);

    struct piper::Voice *handle = new piper::Voice;
    if (handle) {
        try {
            std::optional<piper::SpeakerId> speaker_id;
            piper::loadVoice(piper_config, PARAM(0), PARAM(1), *handle, speaker_id, false);
       } catch (...) {
            delete handle;
            handle = NULL;
       }
    }
    RETURN_NUMBER((SYS_INT)handle);
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PiperTTS, 2)
    T_HANDLE(PiperTTS, 0);
    T_STRING(PiperTTS, 1);

    std::vector<int16_t> audioBuffer;
    piper::SynthesisResult result;

    struct piper::Voice *handle = (struct piper::Voice *)(SYS_INT)PARAM(0);
    try {
        textToAudio(piper_config, *handle, PARAM(1), audioBuffer, result, NULL);
    } catch (...) {
    }

    if (audioBuffer.size() > 0) {
        RETURN_BUFFER((const char *)audioBuffer.data(), audioBuffer.size() * 2);
    } else {
        RETURN_STRING("");
    }
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(PiperDone, 1)
    T_NUMBER(PiperDone, 0);

    struct piper::Voice *handle = (struct piper::Voice *)(SYS_INT)PARAM(0);
    if (handle) {
        delete handle;
        SET_NUMBER(0, 0);
    }
    RETURN_NUMBER(0);
END_IMPL
//=====================================================================================//
#ifndef PIPER_ONLY
CONCEPT_FUNCTION_IMPL(SupertonicInit, 2)
	T_STRING(SupertonicInit, 0)
	T_ARRAY(SupertonicInit, 1)

	char **arr = GetCharList(PARAMETER(1), Invoke);

	SupertonicConfig *tts = NULL;
	try {
		tts = new SupertonicConfig(PARAM(0), arr);
	} catch (...) {
		if (tts)
			delete tts;
		if (arr)
			delete[] arr;

		RETURN_NUMBER(0);
		return 0;
	}

	if (arr)
		delete[] arr;

	RETURN_NUMBER(MAP_POINTER(supertonic_list, tts, PARAMETERS->HANDLER));
END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL_MINMAX_PARAMS(SupertonicTTS, 2, 6)
	T_HANDLE(SupertonicTTS, 0)
	T_STRING(SupertonicTTS, 1)

	int style = 0;
	const char *lang = "en";
	int total_step = 7;
	float speed = 1.05f;

	if (PARAMETERS_COUNT > 2) {
		T_NUMBER(SupertonicTTS, 2)
		if (PARAM(2) >= 0)
			style = PARAM_INT(2);
	}

	if (PARAMETERS_COUNT > 3) {
		T_STRING(SupertonicTTS, 3)
		if (PARAM_LEN(3) > 0)
			lang = PARAM(3);
	}

	if (PARAMETERS_COUNT > 4) {
		T_NUMBER(SupertonicTTS, 4)
		total_step = PARAM_INT(4);
		if ((total_step <= 0) || (total_step >= 20))
			total_step = 7;
	}

	if (PARAMETERS_COUNT > 5) {
		T_NUMBER(SupertonicTTS, 5)
		if (speed > 0)
			speed = PARAM(5);
	}

	SupertonicConfig *tts = (SupertonicConfig *)GET_POINTER(supertonic_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);

	if (!tts) {
		RETURN_NUMBER(0);
		return 0;
	}

	TextToSpeech::SynthesisResult wav;

	try {
		wav = tts->tts(PARAM(1), lang, style, total_step, speed);
	} catch (...) {
		RETURN_NUMBER(0);
		return 0;
	}

	int sample_rate = tts->text_to_speech->getSampleRate();

	clearTensorBuffers(tts->text_to_speech.get());

	char *output = NULL;
	CORE_NEW(wav.wav.size() * 2 + 1, output);
	output[wav.wav.size() * 2] = 0;

	short *wav_data = (short *)output;

	for (float sample : wav.wav) {
		float clamped = std::max(-1.0f, std::min(1.0f, sample));
		*wav_data = static_cast<int16_t>(clamped * 32767);

		wav_data ++;
	}

	CREATE_ARRAY(RESULT);

	void *data_var = NULL;
        Invoke(INVOKE_ARRAY_VARIABLE_BY_KEY, RESULT, "data", &data_var);
	SetVariable(data_var, -1, output, wav.wav.size() * 2);

	Invoke(INVOKE_SET_ARRAY_ELEMENT_BY_KEY, RESULT, "sample_rate", (INTEGER)VARIABLE_NUMBER, (char *)"", (NUMBER)sample_rate);
 END_IMPL
//=====================================================================================//
CONCEPT_FUNCTION_IMPL(SupertonicDone, 1)
	T_NUMBER(SupertonicDone, 0)

	SupertonicConfig *tts = (SupertonicConfig *)FREE_POINTER(supertonic_list, (SYS_INT)PARAM(0), PARAMETERS->HANDLER);
	if (tts)
		delete tts;

	SET_NUMBER(0, 0);
END_IMPL
//=====================================================================================//
#endif