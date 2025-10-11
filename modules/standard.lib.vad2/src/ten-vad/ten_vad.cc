//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "aed.h"
#include "aed_st.h"
#include "coeff.h"
#include "pitch_est.h"
#include "stft.h"
#include <assert.h>
#include <memory>

#include "ten_vad.onnx.h"

#define AUP_AED_ALIGN8(o) (((o) + 7) & (~7))
#define AUP_AED_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define AUP_AED_MIN(x, y) (((x) > (y)) ? (y) : (x))
#define AUP_AED_EPS (1e-20f)

/// ///////////////////////////////////////////////////////////////////////
/// Internal Utils
/// ///////////////////////////////////////////////////////////////////////

AUP_MODULE_AIVAD::AUP_MODULE_AIVAD(const unsigned char* data, int len) {
  ort_api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
  OrtStatus* status =
      ort_api->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "TEN-VAD", &ort_env);
  if (status) {
    printf("Failed to create env: %s\n", ort_api->GetErrorMessage(status));
    ort_api->ReleaseStatus(status);
    ort_api->ReleaseEnv(ort_env);
    ort_env = NULL;
    return;
  }

  OrtSessionOptions* session_options;
  ort_api->CreateSessionOptions(&session_options);
  ort_api->SetIntraOpNumThreads(session_options, 1);
  status = ort_api->CreateSessionFromArray(ort_env, (const void *)data, (size_t)len, session_options, &ort_session);

  ort_api->ReleaseSessionOptions(session_options);
  if (status) {
    printf("Failed to create ort_session: %s\n",
           ort_api->GetErrorMessage(status));
    ort_api->ReleaseStatus(status);
    ort_api->ReleaseEnv(ort_env);
    ort_env = NULL;
    return;
  }

  ort_api->GetAllocatorWithDefaultOptions(&ort_allocator);
  size_t num_inputs;
  ort_api->SessionGetInputCount(ort_session, &num_inputs);
  assert(num_inputs == AUP_AED_MODEL_IO_NUM);
  for (size_t i = 0; i < num_inputs; i++) {
    char* input_name;
    ort_api->SessionGetInputName(ort_session, i, ort_allocator, &input_name);
    strncpy(input_names_buf[i], input_name, sizeof(input_names_buf[i]));
    input_names[i] = input_names_buf[i];
    ort_api->AllocatorFree(ort_allocator, input_name);
  }

  size_t num_outputs;
  ort_api->SessionGetOutputCount(ort_session, &num_outputs);
  assert(num_outputs == AUP_AED_MODEL_IO_NUM);
  for (size_t i = 0; i < num_outputs; i++) {
    char* output_name;
    ort_api->SessionGetOutputName(ort_session, i, ort_allocator, &output_name);
    strncpy(output_names_buf[i], output_name, sizeof(output_names_buf[i]));
    output_names[i] = output_names_buf[i];
    ort_api->AllocatorFree(ort_allocator, output_name);
  }

  OrtMemoryInfo* memory_info;
  status = ort_api->CreateCpuMemoryInfo(OrtDeviceAllocator, OrtMemTypeDefault,
                                        &memory_info);
  if (status != NULL) {
    printf("Failed to create memory info: %s\n",
           ort_api->GetErrorMessage(status));
    ort_api->ReleaseStatus(status);
    ort_api->ReleaseSession(ort_session);
    ort_api->ReleaseEnv(ort_env);
    ort_session = NULL;
    ort_env = NULL;
    return;
  }
  int64_t input_shapes0[] = {1, AUP_AED_CONTEXT_WINDOW_LEN, AUP_AED_FEA_LEN};
  int64_t input_shapes1234[] = {1, AUP_AED_MODEL_HIDDEN_DIM};
  for (int i = 0; i < num_inputs; i++) {
    status = ort_api->CreateTensorWithDataAsOrtValue(
        memory_info, i == 0 ? input_data_buf_0 : input_data_buf_1234[i - 1],
        i == 0 ? sizeof(input_data_buf_0) : sizeof(input_data_buf_1234[i - 1]),
        i == 0 ? input_shapes0 : input_shapes1234,
        i == 0 ? sizeof(input_shapes0) / sizeof(input_shapes0[0])
               : sizeof(input_shapes1234) / sizeof(input_shapes1234[0]),
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &ort_input_tensors[i]);
    if (status != NULL) {
      printf("Failed to create input tensor %d: %s\n", i,
             ort_api->GetErrorMessage(status));
      ort_api->ReleaseStatus(status);
      ort_api->ReleaseSession(ort_session);
      ort_api->ReleaseEnv(ort_env);
      ort_session = NULL;
      ort_env = NULL;
      return;
    }
  }

  int64_t output_shapes0[] = {1, 1, 1};
  int64_t output_shapes1234[] = {1, AUP_AED_MODEL_HIDDEN_DIM};
  for (int i = 0; i < num_outputs; i++) {
    status = ort_api->CreateTensorAsOrtValue(
        ort_allocator, i == 0 ? output_shapes0 : output_shapes1234,
        i == 0 ? sizeof(output_shapes0) / sizeof(output_shapes0[0])
               : sizeof(output_shapes1234) / sizeof(output_shapes1234[0]),
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &ort_output_tensors[i]);
    if (status != NULL) {
      printf("Failed to create output tensor %d: %s\n", i,
             ort_api->GetErrorMessage(status));
      ort_api->ReleaseStatus(status);
      ort_api->ReleaseSession(ort_session);
      ort_api->ReleaseEnv(ort_env);
      ort_session = NULL;
      ort_env = NULL;
      return;
    }
  }
  inited = 1;
}

AUP_MODULE_AIVAD::~AUP_MODULE_AIVAD() {
  for (int i = 0; i < AUP_AED_MODEL_IO_NUM; i++) {
    if (ort_output_tensors[i]) {
      ort_api->ReleaseValue(ort_output_tensors[i]);
    }
  }
  if (ort_session) {
    ort_api->ReleaseSession(ort_session);
  }
  if (ort_env) {
    ort_api->ReleaseEnv(ort_env);
  }
}

int AUP_MODULE_AIVAD::Process(float* input, float* output) {
  if (!inited) {
    printf("not inited!\n");
    return -1;
  }

  memcpy(input_data_buf_0, input, sizeof(input_data_buf_0));
  if (clear_hidden) {
    memset(input_data_buf_1234, 0, sizeof(input_data_buf_1234));
    clear_hidden = 0;
  }
  OrtStatus* status = ort_api->Run(
      ort_session, NULL, input_names, ort_input_tensors, AUP_AED_MODEL_IO_NUM,
      output_names, AUP_AED_MODEL_IO_NUM, ort_output_tensors);
  float* output_data;
  ort_api->GetTensorMutableData(ort_output_tensors[0], (void**)&output_data);
  *output = output_data[0];
  for (int i = 1; i < AUP_AED_MODEL_IO_NUM; i++) {
    ort_api->GetTensorMutableData(ort_output_tensors[i], (void**)&output_data);
    memcpy(input_data_buf_1234[i - 1], output_data,
           sizeof(input_data_buf_1234[i - 1]));
  }

  return 0;
}

int AUP_MODULE_AIVAD::Reset() {
  if (!inited) {
    return -1;
  }

  clear_hidden = 1;
  return 0;
}

static int AUP_Aed_checkStatCfg(Aed_StaticCfg* pCfg) {
  if (pCfg == NULL) {
    return -1;
  }

#if AUP_AED_FEA_LEN < AUP_AED_MEL_FILTER_BANK_NUM
  return -1;
#endif

  if (pCfg->hopSz < 32) {
    return -1;
  }

  if (pCfg->frqInputAvailableFlag == 1) {
    if (pCfg->fftSz < 128 || pCfg->fftSz < pCfg->hopSz) {
      return -1;
    }
    if (pCfg->anaWindowSz > pCfg->fftSz || pCfg->anaWindowSz < pCfg->hopSz) {
      return -1;
    }
  }

  return 0;
}

static int AUP_Aed_publishStaticCfg(Aed_St* stHdl) {
  const Aed_StaticCfg* pStatCfg;

  if (stHdl == NULL) {
    return -1;
  }
  pStatCfg = (const Aed_StaticCfg*)(&(stHdl->stCfg));

  stHdl->extFftSz = 0;
  stHdl->extNBins = 0;
  stHdl->extWinSz = 0;
  if (pStatCfg->frqInputAvailableFlag == 1) {
    stHdl->extFftSz = pStatCfg->fftSz;
    stHdl->extNBins = (stHdl->extFftSz >> 1) + 1;
    stHdl->extWinSz = pStatCfg->anaWindowSz;
  }
  stHdl->extHopSz = pStatCfg->hopSz;

  stHdl->intFftSz = AUP_AED_ASSUMED_FFTSZ;
  stHdl->intHopSz = AUP_AED_ASSUMED_HOPSZ;
  stHdl->intWinSz = AUP_AED_ASSUMED_WINDOWSZ;
  stHdl->intNBins = (stHdl->intFftSz >> 1) + 1;
  stHdl->intAnalyWindowPtr = AUP_AED_STFTWindow_Hann768;

  if (pStatCfg->frqInputAvailableFlag == 0 ||
      stHdl->extHopSz != stHdl->intHopSz) {
    // external STFT analysis framework is not supported at all
    stHdl->intAnalyFlag =
        2;  // internally redo analysis based on input time signal
  } else if (stHdl->extFftSz == stHdl->intFftSz) {
    // external STFT analysis framework completely match with internal
    // requirement
    stHdl->intAnalyFlag = 0;  // directly use external spectrum
  } else {  // external spectrum need to be interpolated or extrapolated before
            // AIVAD
    stHdl->intAnalyFlag =
        1;  // use external spectrum with interpolation / exterpolation
  }
  stHdl->inputTimeFIFOLen = stHdl->extHopSz + stHdl->intHopSz;

  // for aiaed release2.0.0, pre-emphasis for input time-signal is needed,
  // therefore, we need redo analysis based on input time signal preprocessed by
  // pre-emphasis.
  stHdl->intAnalyFlag =
      2;  // internally redo analysis based on input time signal

  stHdl->feaSz = (size_t)AUP_AED_FEA_LEN;
  stHdl->melFbSz = (size_t)AUP_AED_MEL_FILTER_BANK_NUM;
  stHdl->algDelay = (size_t)AUP_AED_LOOKAHEAD_NFRM;
  stHdl->algCtxtSz = (size_t)AUP_AED_CONTEXT_WINDOW_LEN;
  stHdl->frmRmsBufLen = AUP_AED_MAX(1, stHdl->algDelay);

  return 0;
}

static int AUP_Aed_publishDynamCfg(Aed_St* stHdl) {
  const Aed_DynamCfg* pDynmCfg;
  PE_DynamCfg peDynmCfg;
  if (stHdl == NULL) {
    return -1;
  }

  pDynmCfg = (const Aed_DynamCfg*)(&(stHdl->dynamCfg));
  stHdl->aivadResetFrmNum = pDynmCfg->resetFrameNum;
  stHdl->voiceDecideThresh = pDynmCfg->extVoiceThr;

  if (stHdl->pitchEstStPtr != NULL) {
    peDynmCfg.voicedThr = pDynmCfg->pitchEstVoicedThr;
    AUP_PE_setDynamCfg(stHdl->pitchEstStPtr, &peDynmCfg);
  }

  return 0;
}

static int AUP_Aed_resetVariables(Aed_St* stHdl) {
  if (stHdl == NULL) {
    return -1;
  }

  // first clear all the dynamic memory, all the dynamic variables which are
  // not listed bellow are cleared to 0 by this step
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  float* melFbCoef = stHdl->melFilterBankCoef;
  size_t* melBinBuff = stHdl->melFilterBinBuff;
  size_t i, j;
  size_t nBins = stHdl->intNBins;
  size_t melFbSz = stHdl->melFbSz;

  stHdl->aedProcFrmCnt = 0;
  stHdl->inputTimeFIFOIdx = 0;
  stHdl->aivadResetCnt = 0;
  stHdl->timeSignalPre = 0.0f;
  stHdl->aivadScore =
      -1.0f;  // as default value, labeling as aed is not working yet
  stHdl->aivadScorePre = -1.0f;

  stHdl->pitchFreq = 0.0f;

  // generate mel filter-bank coefficients
  float low_mel = 2595.0f * log10f(1.0f + 0.0f / 700.0f);
  float high_mel = 2595.0f * log10f(1.0f + 8000.0f / 700.0f);
  float mel_points = 0.0f;
  float hz_points = 0.0f;
  size_t idx = 0;

  for (i = 0; i < melFbSz + 2; i++) {
    mel_points = i * (high_mel - low_mel) / ((float)melFbSz + 1.0f) + low_mel;
    hz_points = 700.0f * (powf(10.0f, mel_points / 2595.0f) - 1.0f);
    melBinBuff[i] =
        (size_t)((stHdl->intFftSz + 1.0f) * hz_points / (float)AUP_AED_FS);
    if (i > 0 && melBinBuff[i] == melBinBuff[i - 1]) {
      return -1;
    }
  }

  for (j = 0; j < melFbSz; j++) {
    for (i = melBinBuff[j]; i < melBinBuff[j + 1]; i++) {
      idx = j * nBins + i;
      melFbCoef[idx] = (float)(i - melBinBuff[j]) /
                       (float)(melBinBuff[j + 1] - melBinBuff[j]);
    }
    for (i = melBinBuff[j + 1]; i < melBinBuff[j + 2]; i++) {
      idx = j * nBins + i;
      melFbCoef[idx] = (float)(melBinBuff[j + 2] - i) /
                       (float)(melBinBuff[j + 2] - melBinBuff[j + 1]);
    }
  }

  if (stHdl->pitchEstStPtr != NULL) {
    if (AUP_PE_init(stHdl->pitchEstStPtr) < 0) {
      return -1;
    }
  }

  if (stHdl->aivadInf != NULL) {
    stHdl->aivadInf->Reset();
  }

  if (stHdl->timeInAnalysis != NULL) {
    if (AUP_Analyzer_init(stHdl->timeInAnalysis) < 0) {
      return -1;
    }
  }

  return 0;
}

static int AUP_Aed_addOneCnter(int cnter) {
  cnter++;
  if (cnter >= 1000000000) {
    cnter = 0;  // reset every half year
  }
  return (cnter);
}

static void AUP_Aed_binPowerConvert(const float* src, float* tgt, int srcNBins,
                                    int tgtNBins) {
  float rate;
  int srcIdx, tgtIdx;
  if (srcNBins == tgtNBins) {
    memcpy(tgt, src, sizeof(float) * tgtNBins);
    return;
  }

  memset(tgt, 0, sizeof(float) * tgtNBins);

  rate = (float)(srcNBins - 1) / (float)(tgtNBins - 1);
  for (tgtIdx = 0; tgtIdx < tgtNBins; tgtIdx++) {
    srcIdx = (int)(tgtIdx * rate);
    srcIdx = AUP_AED_MIN(srcNBins - 1, AUP_AED_MAX(srcIdx, 0));
    tgt[tgtIdx] = src[srcIdx];
  }

  return;
}

static void AUP_Aed_CalcBinPow(int nBins, const float* cmplxSpctr,
                               float* binPow) {
  int idx, realIdx, imagIdx;

  // bin-0
  binPow[0] = cmplxSpctr[0] * cmplxSpctr[0];

  // bin-(NBins-1)
  binPow[nBins - 1] = cmplxSpctr[1] * cmplxSpctr[1];

  for (idx = 1; idx < (nBins - 1); idx++) {
    realIdx = idx << 1;
    imagIdx = realIdx + 1;

    binPow[idx] = cmplxSpctr[realIdx] * cmplxSpctr[realIdx] +
                  cmplxSpctr[imagIdx] * cmplxSpctr[imagIdx];
  }
  return;
}

static int AUP_Aed_pitch_proc(void* pitchModule, const float* timeSignal,
                              size_t timeLen, const float* binPow, size_t nBins,
                              PE_OutputData* pOut) {
  PE_InputData peInData;

  peInData.timeSignal = timeSignal;
  peInData.hopSz = (int)timeLen;
  peInData.inBinPow = binPow;
  peInData.nBins = (int)nBins;
  pOut->pitchFreq = 0;
  pOut->voiced = -1;
  return AUP_PE_proc(pitchModule, &peInData, pOut);
}

static int AUP_Aed_aivad_proc(Aed_St* stHdl, const float* inBinPow,
                              float* aivadScore) {
  if (stHdl == NULL || inBinPow == NULL || aivadScore == NULL) {
    return -1;
  }

  size_t i, j;
  size_t nBins = stHdl->intNBins;
  size_t melFbSz = stHdl->melFbSz;
  size_t srcOffset;
  size_t srcLen;

  float* aivadInputFeatStack = stHdl->aivadInputFeatStack;
  float* melFbCoef = stHdl->melFilterBankCoef;
  const float* aivadFeatMean = AUP_AED_FEATURE_MEANS;
  const float* aivadFeatStd = AUP_AED_FEATURE_STDS;
  float* curMelFbCoefPtr = NULL;
  float* curInputFeatPtr = NULL;
  float perBandValue = 0.0f;
  float powerNormal = 32768.0f * 32768.0f;

  // update aivad feature buff.
  srcOffset = stHdl->feaSz;
  srcLen = (stHdl->algCtxtSz - 1) * stHdl->feaSz;
  memmove(aivadInputFeatStack, aivadInputFeatStack + srcOffset,
          sizeof(float) * srcLen);
  curInputFeatPtr = aivadInputFeatStack + srcLen;

  // cal. mel-filter-bank feature
  for (i = 0; i < melFbSz; i++) {
    perBandValue = 0.0f;
    curMelFbCoefPtr = melFbCoef + i * nBins;
    for (j = 0; j < nBins; j++) {
      perBandValue += (inBinPow[j] * curMelFbCoefPtr[j]);
    }
    perBandValue = perBandValue / powerNormal;
    perBandValue = logf(perBandValue + AUP_AED_EPS);
    curInputFeatPtr[i] =
        (perBandValue - aivadFeatMean[i]) / (aivadFeatStd[i] + AUP_AED_EPS);
  }

  // extra feat.
  for (i = melFbSz; i < stHdl->feaSz; i++) {
    curInputFeatPtr[i] =
        (stHdl->pitchFreq - aivadFeatMean[i]) / (aivadFeatStd[i] + AUP_AED_EPS);
  }

  // exe. aivad
  // exe. aivad
  float aivadOutput;
  if (stHdl->aivadInf != NULL &&
      stHdl->aivadInf->Process(stHdl->aivadInputFeatStack, &aivadOutput) != 0) {
    return -1;
  }

  (*aivadScore) = aivadOutput;

  stHdl->aivadResetCnt += 1;
  if (stHdl->aivadResetCnt >= stHdl->aivadResetFrmNum) {
    if (stHdl->aivadInf != NULL && stHdl->aivadInf->Reset() != 0) {
    }
    stHdl->aivadResetCnt = 0;
  }

  return 0;
}

static int AUP_Aed_dynamMemPrepare(Aed_St* stHdl, void* memPtrExt,
                                   size_t memSize) {
  if (stHdl == NULL) {
    return -1;
  }
  size_t pitchInNBins = stHdl->intNBins;
  size_t totalMemSize = 0;
  size_t inputTimeFIFOMemSize = 0;
  size_t inputEmphTimeFIFOMemSize = 0;
  size_t aivadInputCmplxSptrmMemSize = 0;
  size_t aivadInputBinPowMemSize = 0;
  size_t frameRmsBuffMemSize = 0;
  size_t aivadInputFeatStackMemSize = 0;
  size_t aimdInputFeatStackMemSize = 0;
  size_t melFilterBankCoefMemSize = 0;
  size_t melFilterBinBuffMemSize = 0;
  size_t inputFloatBuffMemSize = 0;

  // size_t vadScoreOutputBuffDelaySample = 384; // buff. delay for output
  char* memPtr = NULL;

  // size_t nBinsBufferMemSize = AUP_AED_ALIGN8(sizeof(float) * nBins);
  // size_t spctrmMemSize = AUP_AED_ALIGN8(sizeof(float) * (nBins - 1) * 2);

  inputTimeFIFOMemSize =
      AUP_AED_ALIGN8(sizeof(float) * stHdl->inputTimeFIFOLen);
  totalMemSize += inputTimeFIFOMemSize;

  inputEmphTimeFIFOMemSize =
      AUP_AED_ALIGN8(sizeof(float) * stHdl->inputTimeFIFOLen);
  totalMemSize += inputEmphTimeFIFOMemSize;

  aivadInputCmplxSptrmMemSize = AUP_AED_ALIGN8(sizeof(float) * stHdl->intFftSz);
  totalMemSize += aivadInputCmplxSptrmMemSize;

  aivadInputBinPowMemSize = AUP_AED_ALIGN8(sizeof(float) * stHdl->intNBins);
  totalMemSize += aivadInputBinPowMemSize;

  aivadInputFeatStackMemSize =
      AUP_AED_ALIGN8(sizeof(float) * stHdl->algCtxtSz * stHdl->feaSz);
  totalMemSize += aivadInputFeatStackMemSize;

  aimdInputFeatStackMemSize =
      AUP_AED_ALIGN8(sizeof(float) * stHdl->algCtxtSz * stHdl->feaSz);
  totalMemSize += aimdInputFeatStackMemSize;

  melFilterBankCoefMemSize =
      AUP_AED_ALIGN8(sizeof(float) * pitchInNBins * stHdl->feaSz);
  totalMemSize += melFilterBankCoefMemSize;

  melFilterBinBuffMemSize = AUP_AED_ALIGN8(sizeof(size_t) * (stHdl->feaSz + 2));
  totalMemSize += melFilterBinBuffMemSize;

  frameRmsBuffMemSize = AUP_AED_ALIGN8(stHdl->frmRmsBufLen * sizeof(float));
  totalMemSize += frameRmsBuffMemSize;

  inputFloatBuffMemSize = AUP_AED_ALIGN8(stHdl->extHopSz * sizeof(float));
  totalMemSize += inputFloatBuffMemSize;

  if (memPtrExt == NULL) {
    return ((int)totalMemSize);
  }

  if (totalMemSize > memSize) {
    return -1;
  }

  memPtr = (char*)memPtrExt;

  stHdl->inputTimeFIFO = (float*)memPtr;
  memPtr += inputTimeFIFOMemSize;

  stHdl->inputEmphTimeFIFO = (float*)memPtr;
  memPtr += inputEmphTimeFIFOMemSize;

  stHdl->aivadInputCmplxSptrm = (float*)memPtr;
  memPtr += aivadInputCmplxSptrmMemSize;

  stHdl->aivadInputBinPow = (float*)memPtr;
  memPtr += aivadInputBinPowMemSize;

  stHdl->aivadInputFeatStack = (float*)memPtr;
  memPtr += aivadInputFeatStackMemSize;

  stHdl->melFilterBankCoef = (float*)memPtr;
  memPtr += melFilterBankCoefMemSize;

  stHdl->melFilterBinBuff = (size_t*)memPtr;
  memPtr += melFilterBinBuffMemSize;

  stHdl->frameRmsBuff = (float*)memPtr;
  memPtr += frameRmsBuffMemSize;

  stHdl->inputFloatBuff = (float*)memPtr;
  memPtr += inputFloatBuffMemSize;

  if (((size_t)(memPtr - (char*)memPtrExt)) > totalMemSize) {
    return -1;
  }

  return ((int)totalMemSize);
}

static int AUP_Aed_runOneFrm(Aed_St* stHdl, const float* tSignal, int hopSz,
                             const float* binPowPtr, int nBins) {
  PE_OutputData peOutData = {0, 0};
  float aivadScore = -1.0f;
  float mediaFilterout = 0;
  int mediaIdx = (int)(AUP_AED_OUTPUT_SMOOTH_FILTER_LEN) / 2;
  int i;

  if (AUP_Aed_pitch_proc(stHdl->pitchEstStPtr, tSignal, hopSz, binPowPtr, nBins,
                         &peOutData) < 0) {
    return -1;
  }
  stHdl->pitchFreq = peOutData.pitchFreq;
  if (AUP_Aed_aivad_proc(stHdl, binPowPtr, &aivadScore) < 0) {
    return -1;
  }
  stHdl->aivadScore = aivadScore;

  return 0;
}

/// ///////////////////////////////////////////////////////////////////////
/// Public API
/// ///////////////////////////////////////////////////////////////////////

int AUP_Aed_create(void** stPtr) {
  if (stPtr == NULL) {
    return -1;
  }
  Aed_St* tmpPtr = (Aed_St*)malloc(sizeof(Aed_St));
  if (tmpPtr == NULL) {
    return -1;
  }
  memset(tmpPtr, 0, sizeof(Aed_St));

  if (AUP_PE_create(&(tmpPtr->pitchEstStPtr)) < 0) {
    return -1;
  }
  if (AUP_Analyzer_create(&(tmpPtr->timeInAnalysis)) < 0) {
    return -1;
  }

  tmpPtr->stCfg.enableFlag = 1;  // as default, module enabled
  tmpPtr->stCfg.fftSz = 1024;
  tmpPtr->stCfg.hopSz = 256;
  tmpPtr->stCfg.anaWindowSz = 768;
  tmpPtr->stCfg.frqInputAvailableFlag = 0;

  tmpPtr->dynamCfg.extVoiceThr = 0.5f;
  tmpPtr->dynamCfg.extMusicThr = 0.5f;
  tmpPtr->dynamCfg.extEnergyThr = 10.0f;
  tmpPtr->dynamCfg.resetFrameNum = 1875;  // TODO
  tmpPtr->dynamCfg.pitchEstVoicedThr = AUP_AED_PITCH_EST_DEFAULT_VOICEDTHR;

  (*stPtr) = (void*)tmpPtr;

  return 0;
}

int AUP_Aed_destroy(void** stPtr) {
  if (stPtr == NULL || (*stPtr) == NULL) {
    return -1;
  }
  Aed_St* stHdl = (Aed_St*)(*stPtr);

  if (stHdl->aivadInf != NULL) {
    delete stHdl->aivadInf;
  }
  stHdl->aivadInf = NULL;

  if (AUP_PE_destroy(&(stHdl->pitchEstStPtr)) < 0) {
    return -1;
  }
  if (AUP_Analyzer_destroy(&(stHdl->timeInAnalysis)) < 0) {
    return -1;
  }

  if (stHdl->dynamMemPtr != NULL) {
    free(stHdl->dynamMemPtr);
  }
  stHdl->dynamMemPtr = NULL;

  if (stHdl != NULL) {
    free(stHdl);
  }
  (*stPtr) = NULL;

  return 0;
}

int AUP_Aed_memAllocate(void* stPtr, const Aed_StaticCfg* pCfg) {
  Aed_St* stHdl = (Aed_St*)(stPtr);
  Aed_StaticCfg aedStatCfg;
  PE_StaticCfg pitchStatCfg;
  Analyzer_StaticCfg analyzerStatCfg;
  int totalMemSize = 0;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }

  // 1th: check static cfg.
  memcpy(&aedStatCfg, pCfg, sizeof(Aed_StaticCfg));
  if (AUP_Aed_checkStatCfg(&aedStatCfg) < 0) {
    return -1;
  }

  memcpy(&(stHdl->stCfg), &aedStatCfg, sizeof(Aed_StaticCfg));

  // 2th: publish static configuration to internal statical configuration
  // registers
  if (AUP_Aed_publishStaticCfg(stHdl) < 0) {
    return -1;
  }

  // 3th: create aivad instance
  if (stHdl->aivadInf == NULL) {
    stHdl->aivadInf = new AUP_MODULE_AIVAD(ten_vad_onnx, ten_vad_onnx_size);
    if (stHdl->aivadInf == NULL) {
      return -1;
    }
  }
  stHdl->aivadInf->Reset();

  // 4th: memAllocate operation for Pitch-Estimator ............
  if (AUP_PE_getStaticCfg(stHdl->pitchEstStPtr, &pitchStatCfg) < 0) {
    return -1;
  }
  pitchStatCfg.fftSz = stHdl->intFftSz;
  pitchStatCfg.anaWindowSz = stHdl->intWinSz;
  pitchStatCfg.hopSz = stHdl->intHopSz;
  pitchStatCfg.useLPCPreFiltering = AUP_AED_PITCH_EST_USE_LPC;
  pitchStatCfg.procFs = AUP_AED_PITCH_EST_PROCFS;
  if (AUP_PE_memAllocate(stHdl->pitchEstStPtr, &pitchStatCfg) < 0) {
    return -1;
  }

  // creation and initialization with time-analysis module ......
  AUP_Analyzer_getStaticCfg(stHdl->timeInAnalysis, &analyzerStatCfg);
  analyzerStatCfg.win_len = (int)stHdl->intWinSz;
  analyzerStatCfg.hop_size = (int)stHdl->intHopSz;
  analyzerStatCfg.fft_size = (int)stHdl->intFftSz;
  analyzerStatCfg.ana_win_coeff = stHdl->intAnalyWindowPtr;
  if (AUP_Analyzer_memAllocate(stHdl->timeInAnalysis, &analyzerStatCfg) < 0) {
    return -1;
  }

  // 5th: check memory requirement ..............................
  totalMemSize = AUP_Aed_dynamMemPrepare(stHdl, NULL, 0);
  if (totalMemSize < 0) {
    return -1;
  }

  // 6th: allocate dynamic memory
  if (totalMemSize > (int)stHdl->dynamMemSize) {
    if (stHdl->dynamMemPtr != NULL) {
      free(stHdl->dynamMemPtr);
      stHdl->dynamMemPtr = NULL;
      stHdl->dynamMemSize = 0;
    }
    stHdl->dynamMemPtr = malloc(totalMemSize);
    if (stHdl->dynamMemPtr == NULL) {
      return -1;
    }
    stHdl->dynamMemSize = totalMemSize;
  }
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  // 7th: setup the pointers/variable
  if (AUP_Aed_dynamMemPrepare(stHdl, stHdl->dynamMemPtr, stHdl->dynamMemSize) <
      0) {
    return -1;
  }

  // 8th: publish internal dynamic config registers
  if (AUP_Aed_publishDynamCfg(stHdl) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Aed_init(void* stPtr) {
  Aed_St* stHdl = (Aed_St*)(stPtr);
  if (stPtr == NULL) {
    return -1;
  }

  // publish internal dynamic config registers
  if (AUP_Aed_publishDynamCfg(stHdl) < 0) {
    return -1;
  }

  // clear/reset run-time variables
  if (AUP_Aed_resetVariables(stHdl) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Aed_setDynamCfg(void* stPtr, const Aed_DynamCfg* pCfg) {
  Aed_St* stHdl = (Aed_St*)(stPtr);

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }

  memcpy(&(stHdl->dynamCfg), pCfg, sizeof(Aed_DynamCfg));

  // publish internal dynamic configuration registers
  if (AUP_Aed_publishDynamCfg(stHdl) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Aed_getStaticCfg(const void* stPtr, Aed_StaticCfg* pCfg) {
  const Aed_St* stHdl = (const Aed_St*)(stPtr);

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }

  memcpy(pCfg, &(stHdl->stCfg), sizeof(Aed_StaticCfg));

  return 0;
}

int AUP_Aed_getDynamCfg(const void* stPtr, Aed_DynamCfg* pCfg) {
  const Aed_St* stHdl = (const Aed_St*)(stPtr);

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }

  memcpy(pCfg, &(stHdl->dynamCfg), sizeof(Aed_DynamCfg));

  return 0;
}

int AUP_Aed_getAlgDelay(const void* stPtr, int* delayInFrms) {
  const Aed_St* stHdl = (const Aed_St*)(stPtr);

  if (stPtr == NULL || delayInFrms == NULL) {
    return -1;
  }

  (*delayInFrms) = (int)stHdl->algDelay;

  return 0;
}

int AUP_Aed_proc(void* stPtr, const Aed_InputData* pIn, Aed_OutputData* pOut) {
  Analyzer_InputData analyzerInput;
  Analyzer_OutputData analyzerOutput;
  Aed_St* stHdl = (Aed_St*)(stPtr);

  const float* binPowPtr = NULL;
  float frameRms = 0.0f;
  float frameEnergy = 0.0f;
  float powerNormal = 32768.0f * 32768.0f;
  int idx;

  if (stPtr == NULL) {
    return -1;
  }
  if (stHdl->stCfg.enableFlag == 0) {  // this module is disabled
    return 0;
  }
  if (pIn == NULL || pIn->timeSignal == NULL || pOut == NULL) {
    return -1;
  }

  if (stHdl->intAnalyFlag != 2) {  // the external spectra is going to be used
    if (pIn->binPower == NULL) {
      return -1;
    }
    if (pIn->nBins != (int)((stHdl->stCfg.fftSz >> 1) + 1) ||
        pIn->hopSz != (int)(stHdl->stCfg.hopSz)) {
      return -1;
    }
  }

  // cal. input frame energy ....
  for (idx = 0; idx < pIn->hopSz; idx++) {
    frameRms += (pIn->timeSignal[idx] * pIn->timeSignal[idx]);
  }
  frameEnergy = frameRms;
  frameRms = sqrtf(frameRms / (float)pIn->hopSz);
  memmove(stHdl->frameRmsBuff, stHdl->frameRmsBuff + 1,
          sizeof(float) * (stHdl->frmRmsBufLen - 1));
  stHdl->frameRmsBuff[stHdl->frmRmsBufLen - 1] = frameRms;

  // input signal conversion .........
  if ((stHdl->inputTimeFIFOIdx + pIn->hopSz) > (int)stHdl->inputTimeFIFOLen) {
    return -1;
  }

  // update pre-emphasis time signal FIFO
  float* timeSigEphaPtr = stHdl->inputEmphTimeFIFO + stHdl->inputTimeFIFOIdx;
  for (idx = 0; idx < pIn->hopSz; idx++) {
    timeSigEphaPtr[idx] = pIn->timeSignal[idx] - 0.97f * stHdl->timeSignalPre;
    stHdl->timeSignalPre = pIn->timeSignal[idx];
  }

  memcpy(stHdl->inputTimeFIFO + stHdl->inputTimeFIFOIdx, pIn->timeSignal,
         sizeof(float) * (pIn->hopSz));
  stHdl->inputTimeFIFOIdx += pIn->hopSz;

  if (stHdl->intAnalyFlag == 0) {  // directly use external spectra
    if (stHdl->inputTimeFIFOIdx != (int)(stHdl->intHopSz) ||
        (int)(stHdl->intNBins) != pIn->nBins) {
      return -1;
    }

    // one-time processing ...
    stHdl->aedProcFrmCnt = AUP_Aed_addOneCnter(stHdl->aedProcFrmCnt);
    binPowPtr = pIn->binPower;

    // update: stHdl->pitchFreq, stHdl->aivadScore
    if (AUP_Aed_runOneFrm(stHdl, stHdl->inputTimeFIFO, (int)stHdl->intHopSz,
                          binPowPtr, (int)stHdl->intNBins) < 0) {
      return -1;
    }

    // update the inputTimeFIFO
    stHdl->inputTimeFIFOIdx = 0;
  } else if (stHdl->intAnalyFlag ==
             1) {  // do interpolation or extrapolation with external spectra
    if (stHdl->inputTimeFIFOIdx != (int)(stHdl->intHopSz) ||
        (int)(stHdl->extNBins) != pIn->nBins) {
      return -1;
    }

    // one-time processing ....
    stHdl->aedProcFrmCnt = AUP_Aed_addOneCnter(stHdl->aedProcFrmCnt);
    AUP_Aed_binPowerConvert(pIn->binPower, stHdl->aivadInputBinPow,
                            (int)stHdl->extNBins, (int)stHdl->intNBins);
    binPowPtr = stHdl->aivadInputBinPow;

    // update: stHdl->pitchFreq, stHdl->aivadScore
    if (AUP_Aed_runOneFrm(stHdl, stHdl->inputTimeFIFO, (int)stHdl->intHopSz,
                          binPowPtr, (int)stHdl->intNBins) < 0) {
      return -1;
    }

    // update the inputTimeFIFO
    stHdl->inputTimeFIFOIdx = 0;
  } else {  // we need to do STFT on the input time-signal
    if (stHdl->timeInAnalysis == NULL) {
      return -1;
    }

    // loop processing .....
    while (stHdl->inputTimeFIFOIdx >= (int)stHdl->intHopSz) {
      stHdl->aedProcFrmCnt = AUP_Aed_addOneCnter(stHdl->aedProcFrmCnt);

      analyzerInput.input = stHdl->inputEmphTimeFIFO;
      analyzerInput.iLength = (int)stHdl->intHopSz;
      analyzerOutput.output = stHdl->aivadInputCmplxSptrm;
      analyzerOutput.oLength = (int)stHdl->intFftSz;
      if (AUP_Analyzer_proc(stHdl->timeInAnalysis, &analyzerInput,
                            &analyzerOutput) < 0) {
        return -1;
      }

      AUP_Aed_CalcBinPow((int)stHdl->intNBins, stHdl->aivadInputCmplxSptrm,
                         stHdl->aivadInputBinPow);
      binPowPtr = stHdl->aivadInputBinPow;

      // update: stHdl->pitchFreq, stHdl->aivadScore
      if (AUP_Aed_runOneFrm(stHdl, stHdl->inputTimeFIFO, (int)stHdl->intHopSz,
                            binPowPtr, (int)stHdl->intNBins) < 0) {
        return -1;
      }

      // update the inputTimeFIFO & inputEmphTimeFIFO.....
      if (stHdl->inputTimeFIFOIdx > (int)stHdl->intHopSz) {
        memcpy(stHdl->inputTimeFIFO, stHdl->inputTimeFIFO + stHdl->intHopSz,
               sizeof(float) * (stHdl->inputTimeFIFOIdx - stHdl->intHopSz));
        memcpy(stHdl->inputEmphTimeFIFO,
               stHdl->inputEmphTimeFIFO + stHdl->intHopSz,
               sizeof(float) * (stHdl->inputTimeFIFOIdx - stHdl->intHopSz));
      }
      stHdl->inputTimeFIFOIdx -= (int)stHdl->intHopSz;
    }
  }

  // write to output res.
  pOut->frameEnergy = frameEnergy / powerNormal;
  pOut->frameRms = stHdl->frameRmsBuff[0];
  pOut->pitchFreq = stHdl->pitchFreq;
  pOut->voiceProb = stHdl->aivadScore;
  if (pOut->voiceProb < 0.0f) {
    pOut->vadRes = -1;
  } else if (pOut->voiceProb <= stHdl->voiceDecideThresh) {
    pOut->vadRes = 0;
  } else {
    pOut->vadRes = 1;
  }

  return 0;
}

//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
#include "biquad.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "biquad_st.h"

#define AUP_BIQUAD_NUM_DUMP_FILES (20)
#define AUP_BIQUAD_DUMP_FILENAMES (200)

// ==========================================================================================
// internal tools
// ==========================================================================================

static int AUP_Biquad_checkStatCfg(const Biquad_StaticCfg* pCfg) {
  int secIdx;
  if (pCfg == NULL) {
    return -1;
  }

  if (pCfg->maxNSample == 0 ||
      pCfg->maxNSample > AGORA_UAP_BIQUAD_MAX_INPUT_LEN) {
    return -1;
  }
  if (pCfg->nsect > AGORA_UAP_BIQUAD_MAX_SECTION) {
    return -1;
  }

  // if external filter coefficients are required, we need to check the
  //    external filter coeff pointers' validness
  if (pCfg->nsect > 0) {
    for (secIdx = 0; secIdx < pCfg->nsect; secIdx++) {
      if (pCfg->B[secIdx] == NULL || pCfg->A[secIdx] == NULL) {
        return -1;
      }
    }
    if (pCfg->G == NULL) {
      return -1;
    }
  }

  return 0;
}

static int AUP_Biquad_publishStaticCfg(Biquad_St* stHdl) {
  const Biquad_StaticCfg* pStatCfg;
  int idx;

  if (stHdl == NULL) {
    return -1;
  }
  pStatCfg = (const Biquad_StaticCfg*)(&(stHdl->stCfg));

  stHdl->maxNSample = (int)pStatCfg->maxNSample;

  // first, give default (all-pass-filter) values to filter coeffs
  for (idx = 0; idx < AGORA_UAP_BIQUAD_MAX_SECTION; idx++) {
    stHdl->BCoeff[idx][0] = 1.0f;
    stHdl->BCoeff[idx][1] = 0;
    stHdl->BCoeff[idx][2] = 0;
    stHdl->ACoeff[idx][0] = 1.0f;
    stHdl->ACoeff[idx][1] = 0;
    stHdl->ACoeff[idx][2] = 0;
    stHdl->GCoeff[idx] = 1.0f;
  }

  if (pStatCfg->nsect <= 0) {
    stHdl->nsect = _BIQUAD_DC_REMOVAL_NSECT;
    for (idx = 0; idx < stHdl->nsect; idx++) {
      stHdl->BCoeff[idx][0] = _BIQUAD_DC_REMOVAL_B[idx][0];
      stHdl->BCoeff[idx][1] = _BIQUAD_DC_REMOVAL_B[idx][1];
      stHdl->BCoeff[idx][2] = _BIQUAD_DC_REMOVAL_B[idx][2];
      stHdl->ACoeff[idx][0] = _BIQUAD_DC_REMOVAL_A[idx][0];
      stHdl->ACoeff[idx][1] = _BIQUAD_DC_REMOVAL_A[idx][1];
      stHdl->ACoeff[idx][2] = _BIQUAD_DC_REMOVAL_A[idx][2];
      stHdl->GCoeff[idx] = _BIQUAD_DC_REMOVAL_G[idx];
    }
  } else {
    stHdl->nsect = pStatCfg->nsect;
    for (idx = 0; idx < stHdl->nsect; idx++) {
      stHdl->BCoeff[idx][0] = pStatCfg->B[idx][0];
      stHdl->BCoeff[idx][1] = pStatCfg->B[idx][1];
      stHdl->BCoeff[idx][2] = pStatCfg->B[idx][2];

      stHdl->ACoeff[idx][0] = pStatCfg->A[idx][0];
      stHdl->ACoeff[idx][1] = pStatCfg->A[idx][1];
      stHdl->ACoeff[idx][2] = pStatCfg->A[idx][2];

      stHdl->GCoeff[idx] = pStatCfg->G[idx];
    }
  }

  return 0;
}

static int AUP_Biquad_resetVariables(Biquad_St* stHdl) {
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);
  memset(stHdl->sectW, 0, sizeof(stHdl->sectW));

  return 0;
}

// ==========================================================================================
// public APIS
// ==========================================================================================

int AUP_Biquad_create(void** stPtr) {
  Biquad_St* tmpPtr;

  if (stPtr == NULL) {
    return -1;
  }
  *stPtr = (void*)malloc(sizeof(Biquad_St));
  if (*stPtr == NULL) {
    return -1;
  }
  memset(*stPtr, 0, sizeof(Biquad_St));

  tmpPtr = (Biquad_St*)(*stPtr);

  tmpPtr->dynamMemPtr = NULL;
  tmpPtr->dynamMemSize = 0;

  tmpPtr->stCfg.maxNSample = 768;
  tmpPtr->stCfg.nsect = 0;
  for (int idx = 0; idx < AGORA_UAP_BIQUAD_MAX_SECTION; idx++) {
    tmpPtr->stCfg.A[idx] = NULL;
    tmpPtr->stCfg.B[idx] = NULL;
  }
  tmpPtr->stCfg.G = NULL;

  return 0;
}

int AUP_Biquad_destroy(void** stPtr) {
  Biquad_St* stHdl;

  if (stPtr == NULL) {
    return 0;
  }

  stHdl = (Biquad_St*)(*stPtr);
  if (stHdl == NULL) {
    return 0;
  }

  if (stHdl->dynamMemPtr != NULL) {
    free(stHdl->dynamMemPtr);
  }
  stHdl->dynamMemPtr = NULL;

  free(stHdl);

  (*stPtr) = NULL;

  return 0;
}

int AUP_Biquad_memAllocate(void* stPtr, const Biquad_StaticCfg* pCfg) {
  Biquad_St* stHdl = NULL;
  char* memPtr = NULL;
  int maxNSample, nsect, idx;

  int inputTempBufMemSize = 0;
  int sectOutputBufMemSize_EACH = 0;
  int totalMemSize = 0;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (Biquad_St*)(stPtr);

  if (AUP_Biquad_checkStatCfg(pCfg) < 0) {
    return -1;
  }
  memcpy(&(stHdl->stCfg), pCfg, sizeof(Biquad_StaticCfg));

  if (AUP_Biquad_publishStaticCfg(stHdl) < 0) {
    return -1;
  }
  maxNSample = stHdl->maxNSample;
  nsect = stHdl->nsect;

  // check memory requirement
  inputTempBufMemSize = AGORA_UAP_BIQUAD_ALIGN8(sizeof(float) * maxNSample);
  totalMemSize += inputTempBufMemSize;

  sectOutputBufMemSize_EACH =
      AGORA_UAP_BIQUAD_ALIGN8(sizeof(float) * maxNSample);
  totalMemSize += sectOutputBufMemSize_EACH * nsect;

  // allocate dynamic memory
  if ((size_t)totalMemSize > stHdl->dynamMemSize) {
    if (stHdl->dynamMemPtr != NULL) {
      free(stHdl->dynamMemPtr);
      stHdl->dynamMemSize = 0;
    }
    stHdl->dynamMemPtr = malloc(totalMemSize);
    if (stHdl->dynamMemPtr == NULL) {
      return -1;
    }
    stHdl->dynamMemSize = totalMemSize;
  }
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  // setup the pointers/variable
  memPtr = (char*)(stHdl->dynamMemPtr);

  stHdl->inputTempBuf = (float*)memPtr;
  memPtr += inputTempBufMemSize;

  for (idx = 0; idx < nsect; idx++) {
    stHdl->sectOutputBuf[idx] = (float*)memPtr;
    memPtr += sectOutputBufMemSize_EACH;
  }
  for (; idx < AGORA_UAP_BIQUAD_MAX_SECTION; idx++) {
    stHdl->sectOutputBuf[idx] = NULL;
  }

  if (((int)(memPtr - (char*)(stHdl->dynamMemPtr))) > totalMemSize) {
    return -1;
  }

  return 0;
}

int AUP_Biquad_init(void* stPtr) {
  Biquad_St* stHdl;

  if (stPtr == NULL) {
    return -1;
  }
  stHdl = (Biquad_St*)(stPtr);

  if (AUP_Biquad_resetVariables(stHdl) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Biquad_getStaticCfg(const void* stPtr, Biquad_StaticCfg* pCfg) {
  const Biquad_St* stHdl;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (const Biquad_St*)(stPtr);

  memcpy(pCfg, &(stHdl->stCfg), sizeof(Biquad_StaticCfg));

  return 0;
}

int AUP_Biquad_getAlgDelay(const void* stPtr, int* delayInSamples) {
  const Biquad_St* stHdl;

  if (stPtr == NULL || delayInSamples == NULL) {
    return -1;
  }
  stHdl = (const Biquad_St*)(stPtr);

  *delayInSamples = stHdl->nsect;

  return 0;
}

int AUP_Biquad_proc(void* stPtr, const Biquad_InputData* pIn,
                    Biquad_OutputData* pOut) {
  Biquad_St* stHdl = NULL;
  int isFloatIO = 0;
  int inputNSamples, nSect;
  int sectIdx, smplIdx;
  float tmp1;
  const short* pShortTemp;
  float* src;
  float* tgt;

  if (stPtr == NULL || pIn == NULL || pOut == NULL) {  //  pCtrl == NULL
    return -1;
  }
  if (pIn->samplesPtr == NULL || pOut->outputBuff == NULL) {
    return -1;
  }

  stHdl = (Biquad_St*)(stPtr);

  if (((int)pIn->nsamples) > stHdl->maxNSample) {
    return -1;
  }

  isFloatIO = 0;
  if (pIn->sampleType != 0) {
    isFloatIO = 1;
  }

  inputNSamples = (int)pIn->nsamples;
  nSect = stHdl->nsect;

  // special handle for input
  if (isFloatIO == 0) {
    pShortTemp = (const short*)pIn->samplesPtr;
    for (smplIdx = 0; smplIdx < inputNSamples; smplIdx++) {
      stHdl->inputTempBuf[smplIdx] = (float)pShortTemp[smplIdx];
    }
  } else {
    memcpy(stHdl->inputTempBuf, (const float*)pIn->samplesPtr,
           sizeof(float) * inputNSamples);
  }

  for (sectIdx = 0; sectIdx < nSect; sectIdx++) {
    if (sectIdx == 0) {
      src = stHdl->inputTempBuf;
    } else {
      src = stHdl->sectOutputBuf[sectIdx - 1];
    }
    tgt = stHdl->sectOutputBuf[sectIdx];

    for (smplIdx = 0; smplIdx < inputNSamples; smplIdx++) {
      tmp1 = src[smplIdx] -
             stHdl->ACoeff[sectIdx][1] * stHdl->sectW[sectIdx][0] -
             stHdl->ACoeff[sectIdx][2] * stHdl->sectW[sectIdx][1];

      tgt[smplIdx] = stHdl->GCoeff[sectIdx] *
                     (stHdl->BCoeff[sectIdx][0] * tmp1 +
                      stHdl->BCoeff[sectIdx][1] * stHdl->sectW[sectIdx][0] +
                      stHdl->BCoeff[sectIdx][2] * stHdl->sectW[sectIdx][1]);

      stHdl->sectW[sectIdx][1] = stHdl->sectW[sectIdx][0];
      stHdl->sectW[sectIdx][0] = tmp1;
    }
  }

  // prepare output buffer
  if (isFloatIO == 0) {
    for (smplIdx = 0; smplIdx < inputNSamples; smplIdx++) {
      ((short*)pOut->outputBuff)[smplIdx] =
          (short)_BIQUAD_FLOAT2SHORT(stHdl->sectOutputBuf[nSect - 1][smplIdx]);
    }
  } else {
    memcpy(pOut->outputBuff, stHdl->sectOutputBuf[nSect - 1],
           sizeof(float) * inputNSamples);
  }

  return 0;
}

//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fscvrt.h"
#include "fscvrt_st.h"
#include "biquad.h"

// ==========================================================================================
// internal tools
// ==========================================================================================

static int AUP_Fscvrt_FilterSet(int resampleRate, int* nsect,
                                const float* B[_FSCVRT_MAXNSEC],
                                const float* A[_FSCVRT_MAXNSEC],
                                const float** G) {
  int idx;
  if (resampleRate == 2) {
    *nsect = _FSCVRT_1over2_LOWPASS_NSEC;
    for (idx = 0; idx < (*nsect); idx++) {
      B[idx] = &(_FSCVRT_1over2_LOWPASS_B[idx][0]);
      A[idx] = &(_FSCVRT_1over2_LOWPASS_A[idx][0]);
    }
    *G = _FSCVRT_1over2_LOWPASS_G;
  } else if (resampleRate == 3) {
    *nsect = _FSCVRT_1over3_LOWPASS_NSEC;
    for (idx = 0; idx < (*nsect); idx++) {
      B[idx] = &(_FSCVRT_1over3_LOWPASS_B[idx][0]);
      A[idx] = &(_FSCVRT_1over3_LOWPASS_A[idx][0]);
    }
    *G = _FSCVRT_1over3_LOWPASS_G;
  } else if (resampleRate == 4) {
    *nsect = _FSCVRT_1over4_LOWPASS_NSEC;
    for (idx = 0; idx < (*nsect); idx++) {
      B[idx] = &(_FSCVRT_1over4_LOWPASS_B[idx][0]);
      A[idx] = &(_FSCVRT_1over4_LOWPASS_A[idx][0]);
    }
    *G = _FSCVRT_1over4_LOWPASS_G;
  } else if (resampleRate == 6) {
    *nsect = _FSCVRT_1over6_LOWPASS_NSEC;
    for (idx = 0; idx < (*nsect); idx++) {
      B[idx] = &(_FSCVRT_1over6_LOWPASS_B[idx][0]);
      A[idx] = &(_FSCVRT_1over6_LOWPASS_A[idx][0]);
    }
    *G = _FSCVRT_1over6_LOWPASS_G;
  } else {  // unknown resample rate
    return -1;
  }

  return 0;
}

static int AUP_Fscvrt_dynamMemPrepare(FscvrtSt* stHdl, void* memPtrExt,
                                      size_t memSize) {
  char* memPtr = NULL;
  int biquadInBufMemSize = 0;
  int biquadOutBufMemSize = 0;
  int totalMemSize = 0;

  if (stHdl == NULL) {
    return -1;
  }

  biquadInBufMemSize = _FSCVRT_ALIGN8(sizeof(float) * stHdl->biquadInBufLen);
  totalMemSize += biquadInBufMemSize;

  biquadOutBufMemSize = _FSCVRT_ALIGN8(sizeof(float) * stHdl->biquadOutBufLen);
  totalMemSize += biquadOutBufMemSize;

  totalMemSize = _FSCVRT_MAX(totalMemSize, 80);

  // if no external memory provided, we are only profiling the memory
  // requirement
  if (memPtrExt == NULL) {
    return (totalMemSize);
  }

  // if required memory is more than provided, error
  if ((size_t)totalMemSize > memSize) {
    return -1;
  }
  memPtr = (char*)memPtrExt;

  stHdl->biquadInBuf = NULL;
  if (biquadInBufMemSize != 0) {
    stHdl->biquadInBuf = (float*)memPtr;
    memPtr += biquadInBufMemSize;
  }

  stHdl->biquadOutBuf = NULL;
  if (biquadOutBufMemSize != 0) {
    stHdl->biquadOutBuf = (float*)memPtr;
    memPtr += biquadOutBufMemSize;
  }

  if (((int)(memPtr - (char*)memPtrExt)) > totalMemSize) {
    return -1;
  }

  return (totalMemSize);
}

static int AUP_Fscvrt_checkStatCfg(FscvrtStaticCfg* pCfg) {
  if (pCfg == NULL) {
    return -1;
  }

  if (pCfg->inputFs != 16000 && pCfg->inputFs != 24000 &&
      pCfg->inputFs != 32000 && pCfg->inputFs != 48000) {
    return -1;
  }

  if (pCfg->outputFs != 16000 && pCfg->outputFs != 24000 &&
      pCfg->outputFs != 32000 && pCfg->outputFs != 48000) {
    return -1;
  }

  if (pCfg->stepSz > AUP_FSCVRT_MAX_INPUT_LEN || pCfg->stepSz < 1) {
    return -1;
  }

  if (pCfg->inputType != 0) {
    pCfg->inputType = 1;
  }

  if (pCfg->outputType != 0) {
    pCfg->outputType = 1;
  }

  return 0;
}

static int AUP_Fscvrt_publishStaticCfg(FscvrtSt* stHdl) {
  int tmpRatio;
  int ret;
  int maxResmplRate = 0;

  stHdl->mode = 0;
  stHdl->upSmplRate = 1;
  stHdl->downSmplRate = 1;
  if (stHdl->stCfg.inputFs != stHdl->stCfg.outputFs) {
    if (stHdl->stCfg.outputFs > stHdl->stCfg.inputFs) {
      tmpRatio = (stHdl->stCfg.outputFs / stHdl->stCfg.inputFs);
      if (stHdl->stCfg.outputFs == tmpRatio * stHdl->stCfg.inputFs) {
        stHdl->mode = 1;
        stHdl->upSmplRate = tmpRatio;
        stHdl->downSmplRate = 1;
      } else {
        stHdl->mode = 3;
        stHdl->upSmplRate = _FSCVRT_COMMON_FS / stHdl->stCfg.inputFs;
        stHdl->downSmplRate = _FSCVRT_COMMON_FS / stHdl->stCfg.outputFs;
      }
    } else {  // stHdl->stCfg.outputFs < stHdl->stCfg.inputFs
      tmpRatio = (stHdl->stCfg.inputFs / stHdl->stCfg.outputFs);
      if (stHdl->stCfg.inputFs == tmpRatio * stHdl->stCfg.outputFs) {
        stHdl->mode = 2;
        stHdl->upSmplRate = 1;
        stHdl->downSmplRate = tmpRatio;
      } else {
        stHdl->mode = 3;
        stHdl->upSmplRate = _FSCVRT_COMMON_FS / stHdl->stCfg.inputFs;
        stHdl->downSmplRate = _FSCVRT_COMMON_FS / stHdl->stCfg.outputFs;
      }
    }
  }

  if (stHdl->mode == 0) {
    stHdl->biquadInBufLen = 0;
    stHdl->biquadOutBufLen = 0;
  } else {
    stHdl->biquadInBufLen = stHdl->stCfg.stepSz * stHdl->upSmplRate;
    stHdl->biquadOutBufLen = 2 * (stHdl->stCfg.stepSz * stHdl->upSmplRate);
  }

  maxResmplRate = _FSCVRT_MAX(stHdl->upSmplRate, stHdl->downSmplRate);

  stHdl->nSec = 0;
  memset(stHdl->biquadB, 0, sizeof(stHdl->biquadB));
  memset(stHdl->biquadA, 0, sizeof(stHdl->biquadA));
  stHdl->biquadG = NULL;  // gain for each section

  if (stHdl->mode != 0) {
    ret = AUP_Fscvrt_FilterSet(maxResmplRate, &(stHdl->nSec), stHdl->biquadB,
                               stHdl->biquadA, &(stHdl->biquadG));
    if (ret < 0) {
      return -1;
    }
  }

  return 0;
}

static int AUP_Fscvrt_resetVariables(FscvrtSt* stHdl) {
  stHdl->biquadInBufCnt = 0;
  stHdl->biquadOutBufCnt = 0;

  if (stHdl->dynamMemPtr != NULL && stHdl->dynamMemSize > 0) {
    memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);
  }
  return 0;
}

// ==========================================================================================
// public APIs
// ==========================================================================================

int AUP_Fscvrt_create(void** stPtr) {
  FscvrtSt* tmpPtr;

  if (stPtr == NULL) {
    return -1;
  }
  *stPtr = (void*)malloc(sizeof(FscvrtSt));
  if (*stPtr == NULL) {
    return -1;
  }
  memset(*stPtr, 0, sizeof(FscvrtSt));

  tmpPtr = (FscvrtSt*)(*stPtr);

  tmpPtr->dynamMemPtr = NULL;
  tmpPtr->dynamMemSize = 0;

  tmpPtr->stCfg.inputFs = 24000;
  tmpPtr->stCfg.outputFs = 32000;
  tmpPtr->stCfg.stepSz = 240;    // 10ms processing step
  tmpPtr->stCfg.inputType = 0;   // short in
  tmpPtr->stCfg.outputType = 0;  // short out

  if (AUP_Biquad_create(&(tmpPtr->biquadSt)) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Fscvrt_destroy(void** stPtr) {
  FscvrtSt* stHdl;

  if (stPtr == NULL) {
    return 0;
  }

  stHdl = (FscvrtSt*)(*stPtr);
  if (stHdl == NULL) {
    return 0;
  }

  AUP_Biquad_destroy(&(stHdl->biquadSt));
  if (stHdl->dynamMemPtr != NULL) {
    free(stHdl->dynamMemPtr);
  }
  stHdl->dynamMemPtr = NULL;

  free(stHdl);
  (*stPtr) = NULL;

  return 0;
}

int AUP_Fscvrt_memAllocate(void* stPtr, const FscvrtStaticCfg* pCfg) {
  FscvrtSt* stHdl = NULL;
  FscvrtStaticCfg tmpStatCfg = {0};
  Biquad_StaticCfg bqStatCfg;
  int idx, ret;
  int totalMemSize = 0;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (FscvrtSt*)(stPtr);

  memcpy(&tmpStatCfg, pCfg, sizeof(FscvrtStaticCfg));
  if (AUP_Fscvrt_checkStatCfg(&tmpStatCfg) < 0) {
    return -1;
  }
  memcpy(&(stHdl->stCfg), &tmpStatCfg, sizeof(FscvrtStaticCfg));

  if (AUP_Fscvrt_publishStaticCfg(stHdl) < 0) {
    return -1;
  }

  // check memory requirement
  totalMemSize = AUP_Fscvrt_dynamMemPrepare(stHdl, NULL, 0);
  if (totalMemSize < 0) {
    return -1;
  }

  // allocate dynamic memory
  if ((size_t)totalMemSize > stHdl->dynamMemSize) {
    if (stHdl->dynamMemPtr != NULL) {
      free(stHdl->dynamMemPtr);
      stHdl->dynamMemSize = 0;
    }
    stHdl->dynamMemPtr = (void*)malloc(totalMemSize);
    if (stHdl->dynamMemPtr == NULL) {
      return -1;
    }
    stHdl->dynamMemSize = totalMemSize;
  }
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  // setup the pointers/variable
  if (AUP_Fscvrt_dynamMemPrepare(stHdl, stHdl->dynamMemPtr,
                                 stHdl->dynamMemSize) < 0) {
    return -1;
  }

  // memAllocation for upSmplBiquadSt and downSmplBiquadSt
  if (stHdl->nSec != 0) {
    if (stHdl->nSec > AGORA_UAP_BIQUAD_MAX_SECTION) {
      return -1;
    }
    memset(&bqStatCfg, 0, sizeof(Biquad_StaticCfg));
    bqStatCfg.maxNSample = (size_t)(stHdl->biquadInBufLen);
    bqStatCfg.nsect = stHdl->nSec;
    for (idx = 0; idx < stHdl->nSec; idx++) {
      bqStatCfg.B[idx] = stHdl->biquadB[idx];
      bqStatCfg.A[idx] = stHdl->biquadA[idx];
    }
    bqStatCfg.G = stHdl->biquadG;

    ret = AUP_Biquad_memAllocate(stHdl->biquadSt, &bqStatCfg);
    if (ret < 0) {
      return -1;
    }
  }

  return 0;
}

int AUP_Fscvrt_init(void* stPtr) {
  FscvrtSt* stHdl;

  if (stPtr == NULL) {
    return -1;
  }
  stHdl = (FscvrtSt*)(stPtr);

  // clear/reset run-time variables
  if (AUP_Fscvrt_resetVariables(stHdl) < 0) {
    return -1;
  }

  // init submodules ...
  if (stHdl->biquadSt != NULL && stHdl->nSec != 0) {
    if (AUP_Biquad_init(stHdl->biquadSt) < 0) {
      return -1;
    }
  }

  return 0;
}

int AUP_Fscvrt_getStaticCfg(const void* stPtr, FscvrtStaticCfg* pCfg) {
  const FscvrtSt* stHdl;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (const FscvrtSt*)(stPtr);

  memcpy(pCfg, &(stHdl->stCfg), sizeof(FscvrtStaticCfg));

  return 0;
}

int AUP_Fscvrt_getInfor(const void* stPtr, FscvrtGetData* buff) {
  const FscvrtSt* stHdl;
  int delayBiquad = 0;
  int tmp;

  if (stPtr == NULL || buff == NULL) {
    return -1;
  }
  stHdl = (const FscvrtSt*)(stPtr);

  if (stHdl->nSec != 0) {
    if (AUP_Biquad_getAlgDelay(stHdl->biquadSt, &delayBiquad) < 0) {
      return -1;
    }
  }

  if (stHdl->mode == 0) {
    buff->delayInInputFs = 0;
  } else if (stHdl->mode == 1) {
    buff->delayInInputFs =
        (int)roundf(delayBiquad / (float)(stHdl->upSmplRate));
  } else if (stHdl->mode == 2) {  // direct downsampling
    buff->delayInInputFs = delayBiquad;
  } else {  // stHdl->mode == 3
    buff->delayInInputFs =
        (int)roundf(delayBiquad / (float)(stHdl->upSmplRate));
  }
  tmp = stHdl->stCfg.stepSz * stHdl->upSmplRate / stHdl->downSmplRate;
  if (tmp * stHdl->downSmplRate == stHdl->stCfg.stepSz * stHdl->upSmplRate) {
    buff->maxOutputStepSz = tmp;
  } else {
    buff->maxOutputStepSz = tmp + 1;
  }

  return 0;
}

int AUP_Fscvrt_proc(void* stPtr, const FscvrtInData* pIn, FscvrtOutData* pOut) {
  FscvrtSt* stHdl = NULL;
  const FscvrtStaticCfg* pCfg;
  Biquad_InputData bqdInData;
  Biquad_OutputData bqdOutData;
  const short* shortSrcPtr = NULL;
  const float* floatSrcPtr = NULL;
  short* shortTgtPtr = NULL;
  float* floatTgtPtr = NULL;
  int idx, tgtIdx;
  int nOutSamples = 0, samplesTaken = 0, samplesLeft = 0;
  int jumpRate;

  if (stPtr == NULL || pIn == NULL || pOut == NULL || pIn->inDataSeq == NULL ||
      pOut->outDataSeq == NULL) {  //  pCtrl == NULL
    return -1;
  }

  stHdl = (FscvrtSt*)(stPtr);
  pCfg = (const FscvrtStaticCfg*)&(stHdl->stCfg);
  shortSrcPtr = (const short*)(pIn->inDataSeq);
  floatSrcPtr = (const float*)(pIn->inDataSeq);
  // ==============================================================================
  // mode-0: bypass
  if (stHdl->mode == 0) {  // direct bypass
    if (pIn->outDataSeqLen < pCfg->stepSz) {
      return -1;
    }
    pOut->nOutData = pCfg->stepSz;
    pOut->outDataType = pCfg->outputType;
    if (pIn->inDataSeq == pOut->outDataSeq) {
      if (pCfg->outputType == pCfg->inputType)
        return 0;  // we don't need to do anything
      return -1;
      // if input buffer and the output buffer are the same, but required
      // different data type: error, we currently do not support such usecase
    }

    if (pCfg->inputType == 0 && pCfg->outputType == 0) {
      memcpy(pOut->outDataSeq, pIn->inDataSeq, sizeof(short) * pCfg->stepSz);
    } else if (pCfg->inputType == 1 && pCfg->outputType == 1) {
      memcpy(pOut->outDataSeq, pIn->inDataSeq, sizeof(float) * pCfg->stepSz);
    } else if (pCfg->inputType == 0 && pCfg->outputType == 1) {
      for (idx = 0; idx < pCfg->stepSz; idx++) {
        ((float*)pOut->outDataSeq)[idx] = ((short*)pIn->inDataSeq)[idx];
      }
    } else {  // if (pCfg->inputType == 1 && pCfg->outputType == 0)
      for (idx = 0; idx < pCfg->stepSz; idx++) {
        ((short*)pOut->outDataSeq)[idx] =
            (short)_FSCVRT_FLOAT2SHORT(((float*)pIn->inDataSeq)[idx]);
      }
    }

    return 0;
  }

  // prepare input buffer for Biquad .....
  memset(stHdl->biquadInBuf, 0, sizeof(float) * stHdl->biquadInBufLen);
  if (pCfg->inputType == 0) {
    for (idx = 0; idx < pCfg->stepSz; idx++) {
      stHdl->biquadInBuf[idx * (stHdl->upSmplRate)] =
          ((float)shortSrcPtr[idx]) * stHdl->upSmplRate;
    }
  } else {
    for (idx = 0; idx < pCfg->stepSz; idx++) {
      stHdl->biquadInBuf[idx * (stHdl->upSmplRate)] =
          floatSrcPtr[idx] * stHdl->upSmplRate;
    }
  }

  // biquad filtering ......
  memset(&bqdInData, 0, sizeof(Biquad_InputData));
  memset(&bqdOutData, 0, sizeof(Biquad_OutputData));
  bqdInData.samplesPtr = (const void*)(stHdl->biquadInBuf);
  bqdInData.sampleType = 1;
  bqdInData.nsamples = (size_t)(pCfg->stepSz * stHdl->upSmplRate);
  bqdOutData.outputBuff = (void*)&(stHdl->biquadOutBuf[stHdl->biquadOutBufCnt]);
  if (stHdl->biquadOutBufCnt + (pCfg->stepSz * stHdl->upSmplRate) >
      stHdl->biquadOutBufLen) {
    return -1;
  }
  if (AUP_Biquad_proc(stHdl->biquadSt, &bqdInData, &bqdOutData) < 0) {
    return -1;
  }
  stHdl->biquadOutBufCnt += (pCfg->stepSz * stHdl->upSmplRate);

  // checking the output buffer .........
  nOutSamples = stHdl->biquadOutBufCnt / stHdl->downSmplRate;
  if (pIn->outDataSeqLen < nOutSamples) {
    return -1;
  }

  // prepare output data, downsampling and throwing out ......
  pOut->nOutData = nOutSamples;
  pOut->outDataType = pCfg->outputType;

  shortTgtPtr = (short*)pOut->outDataSeq;
  floatTgtPtr = (float*)pOut->outDataSeq;
  jumpRate = stHdl->downSmplRate;
  if (pCfg->outputType == 0) {  // -> shortTgtPtr
    for (idx = (jumpRate - 1), tgtIdx = 0; idx < stHdl->biquadOutBufCnt;
         idx += jumpRate, tgtIdx++) {
      shortTgtPtr[tgtIdx] = _FSCVRT_FLOAT2SHORT(stHdl->biquadOutBuf[idx]);
    }
  } else {  // -> floatTgtPtr
    for (idx = (jumpRate - 1), tgtIdx = 0; idx < stHdl->biquadOutBufCnt;
         idx += jumpRate, tgtIdx++) {
      floatTgtPtr[tgtIdx] = stHdl->biquadOutBuf[idx];
    }
  }
  if (nOutSamples != tgtIdx) {
    return -1;
  }

  // update the stHdl->biquadOutBuf and stHdl->biquadOutBufCnt
  samplesTaken = nOutSamples * jumpRate;
  samplesLeft = stHdl->biquadOutBufCnt - samplesTaken;
  if (samplesLeft == 0) {
    stHdl->biquadOutBufCnt = 0;
  } else if (samplesLeft > 0) {
    stHdl->biquadOutBufCnt = samplesLeft;
    memmove(stHdl->biquadOutBuf, &(stHdl->biquadOutBuf[samplesTaken]),
            sizeof(float) * samplesLeft);
  } else {  // samplesLeft < 0
    stHdl->biquadOutBufCnt = 0;
    return -1;
  }

  return 0;
}

//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
// This file contains modified code derived from LPCNet (https://github.com/mozilla/LPCNet),
// specifically from the following functions:
//   - compute_frame_features() in lpcnet_enc.c
//   - process_superframe() in lpcnet_enc.c
//
// Original lpcnet_enc.c code LICENSE Text, licensed under the BSD-2-Clause License:
//   Copyright (c) 2017-2019 Mozilla
//
//   Redistribution and use in source and binary forms, with or without modification,
//   are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//
//   - Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//
// Original LPCNet LICENSE Text, licensed under the BSD-3-Clause License:
//   Copyright (c) 2017-2018, Mozilla
//   Copyright (c) 2007-2017, Jean-Marc Valin
//   Copyright (c) 2005-2017, Xiph.Org Foundation
//   Copyright (c) 2003-2004, Mark Borgerding
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  
//   - Neither the name of the Xiph.Org Foundation nor the names of its
//     contributors may be used to endorse or promote products derived from
//     this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//   

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "biquad.h"
#include "pitch_est.h"
#include "pitch_est_st.h"
#include "fftw.h"

// ==========================================================================================
// internal tools
// ==========================================================================================

static int AUP_PE_checkStatCfg(PE_StaticCfg* pCfg) {
  if (pCfg == NULL) {
    return -1;
  }

  if (pCfg->fftSz != 256 && pCfg->fftSz != 512 && pCfg->fftSz != 1024) {
    return -1;
  }
  if (pCfg->fftSz > AUP_PE_MAX_FFTSIZE) {
    return -1;
  }
  if (pCfg->anaWindowSz > pCfg->fftSz || pCfg->anaWindowSz < pCfg->hopSz) {
    return -1;
  }
  if (pCfg->hopSz != 64 && pCfg->hopSz != 80 && pCfg->hopSz != 128 &&
      pCfg->hopSz != 160 && pCfg->hopSz != 256 && pCfg->hopSz != 512) {
    return -1;
  }

  if (pCfg->useLPCPreFiltering != 0) {
    pCfg->useLPCPreFiltering = 1;
  }

  if (pCfg->procFs != 2000 && pCfg->procFs != 4000 && pCfg->procFs != 8000 &&
      pCfg->procFs != 16000) {
    pCfg->procFs = 4000;
  }

  return 0;
}

static int AUP_PE_checkDynamCfg(PE_DynamCfg* pCfg) {
  if (pCfg == NULL) {
    return -1;
  }

  pCfg->voicedThr = AUP_PE_MIN(2.0f, AUP_PE_MAX(pCfg->voicedThr, -1.0f));

  return 0;
}

static int AUP_PE_publishStaticCfg(PE_St* stHdl) {
  const PE_StaticCfg* pStatCfg;
  int idx, jdx;
  int hopSz;
  int excBufShiftLen;

  if (stHdl == NULL) {
    return -1;
  }
  pStatCfg = (const PE_StaticCfg*)(&(stHdl->stCfg));
  hopSz = (int)pStatCfg->hopSz;

  stHdl->nBins = ((int)(pStatCfg->fftSz >> 1)) + 1;
  stHdl->procResampleRate = AUP_PE_FS / (int)pStatCfg->procFs;
  stHdl->minPeriod = AUP_PE_MIN_PERIOD_16KHZ / stHdl->procResampleRate;
  stHdl->maxPeriod = AUP_PE_MAX_PERIOD_16KHZ / stHdl->procResampleRate;
  stHdl->difPeriod = stHdl->maxPeriod - stHdl->minPeriod;
  stHdl->inputResampleBufLen = hopSz * 2;  // give it a max. value
  stHdl->inputQLen = AUP_PE_MAX(AUP_PE_XCORR_TRAINING_OFFSET, hopSz) + hopSz;

  excBufShiftLen = (int)ceilf(hopSz / (float)stHdl->procResampleRate);
  stHdl->excBufLen = stHdl->maxPeriod + excBufShiftLen + 1;

  stHdl->nFeat = (int)ceilf(AUP_PE_FEAT_TIME_WINDOW * AUP_PE_FS /
                            ((float)hopSz * 1000.0f));
  stHdl->nFeat = AUP_PE_MIN(stHdl->nFeat, AUP_PE_FEAT_MAX_NFRM);
  stHdl->estDelay = 0;

  // publish DCT-table coeff.
  for (idx = 0; idx < AUP_PE_NB_BANDS; idx++) {
    for (jdx = 0; jdx < AUP_PE_NB_BANDS; jdx++) {
      stHdl->dct_table[idx * AUP_PE_NB_BANDS + jdx] =
          cosf((idx + .5f) * jdx * AUP_PE_PI / AUP_PE_NB_BANDS);
      if (jdx == 0) stHdl->dct_table[idx * AUP_PE_NB_BANDS + jdx] *= sqrtf(.5f);
    }
  }

  return 0;
}

static int AUP_PE_resetVariables(PE_St* stHdl) {
  // int nBins;
  int idx;

  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  stHdl->inputResampleBufIdx = 0;

  for (idx = 0; idx < AUP_PE_LPC_ORDER; idx++) {
    stHdl->lpc[idx] = 0;
    stHdl->pitch_mem[idx] = 0;
  }
  stHdl->pitch_filt = 0;

  memset(stHdl->tmpFeat, 0, sizeof(stHdl->tmpFeat));

  stHdl->xCorrOffsetIdx = 0;
  for (idx = 0; idx < (AUP_PE_FEAT_MAX_NFRM * 2); idx++) {
    stHdl->frmWeight[idx] = 0;
    stHdl->frmWeightNorm[idx] = 0;
  }

  stHdl->pitchMaxPathAll = 0;
  stHdl->bestPeriodEst = 0;

  stHdl->voiced = 0;
  stHdl->pitchEstResult = 0;  // label as no speech

  if (stHdl->procResampleRate != 1) {
    if (AUP_Biquad_init(stHdl->biquadIIRPtr) < 0) {
      return -1;
    }
  }

  return 0;
}

static int AUP_PE_dynamMemPrepare(PE_St* stHdl, void* memPtrExt,
                                  size_t memSize) {
  int idx;

  int inputResampleBufMemSize = 0;
  int inputQMemSize = 0;
  int alignedInMemSize = 0;
  int lpcFilterOutBufMemSize = 0;
  int excBufMemSize = 0;
  int excBufSqMemSize = 0;
  int xCorrInstMemSize = 0;
  int xCorrPerFeatMemSize = 0;
  int xCorrPerFeatTmpMemSize = 0;
  int pitchMaxPathRegPerRegMemSize = 0;
  int pitchPrevPerFeatMemSize = 0;
  int totalMemSize = 0;
  char* memPtr = NULL;

  inputResampleBufMemSize =
      AUP_PE_ALIGN8(sizeof(float) * stHdl->inputResampleBufLen);
  totalMemSize += inputResampleBufMemSize;

  inputQMemSize = AUP_PE_ALIGN8(sizeof(float) * stHdl->inputQLen);
  totalMemSize += inputQMemSize;

  alignedInMemSize = AUP_PE_ALIGN8(sizeof(float) * stHdl->stCfg.hopSz);
  totalMemSize += alignedInMemSize;

  lpcFilterOutBufMemSize = AUP_PE_ALIGN8(sizeof(float) * stHdl->stCfg.hopSz);
  totalMemSize += lpcFilterOutBufMemSize;

  excBufMemSize = AUP_PE_ALIGN8(sizeof(float) * stHdl->excBufLen);
  totalMemSize += excBufMemSize;
  excBufSqMemSize = excBufMemSize;
  totalMemSize += excBufSqMemSize;

  xCorrInstMemSize = AUP_PE_ALIGN8(sizeof(float) * (stHdl->maxPeriod));
  totalMemSize += xCorrInstMemSize;

  xCorrPerFeatMemSize = AUP_PE_ALIGN8(sizeof(float) * (stHdl->maxPeriod + 1));
  xCorrPerFeatTmpMemSize = xCorrPerFeatMemSize;
  totalMemSize +=
      (xCorrPerFeatMemSize + xCorrPerFeatTmpMemSize) * (stHdl->nFeat * 2);

  pitchMaxPathRegPerRegMemSize =
      AUP_PE_ALIGN8(sizeof(float) * (stHdl->maxPeriod));
  totalMemSize += pitchMaxPathRegPerRegMemSize * 2;

  pitchPrevPerFeatMemSize = AUP_PE_ALIGN8(sizeof(int) * (stHdl->maxPeriod));
  totalMemSize += pitchPrevPerFeatMemSize * (stHdl->nFeat * 2);

  // if no external memory provided, we are only profiling the memory
  // requirement
  if (memPtrExt == NULL) {
    return (totalMemSize);
  }

  // if required memory is more than provided, error
  if ((size_t)totalMemSize > memSize) {
    return -1;
  }

  memPtr = (char*)memPtrExt;

  stHdl->inputResampleBuf = (float*)memPtr;
  memPtr += inputResampleBufMemSize;

  stHdl->inputQ = (float*)memPtr;
  memPtr += inputQMemSize;

  stHdl->alignedIn = (float*)memPtr;
  memPtr += alignedInMemSize;

  stHdl->lpcFilterOutBuf = (float*)memPtr;
  memPtr += lpcFilterOutBufMemSize;

  stHdl->excBuf = (float*)memPtr;
  memPtr += excBufMemSize;

  stHdl->excBufSq = (float*)memPtr;
  memPtr += excBufSqMemSize;

  stHdl->xCorrInst = (float*)memPtr;
  memPtr += xCorrInstMemSize;

  for (idx = 0; idx < AUP_PE_FEAT_MAX_NFRM * 2; idx++) {
    stHdl->xCorr[idx] = NULL;
    stHdl->xCorrTmp[idx] = NULL;
    stHdl->pitchPrev[idx] = NULL;
  }
  for (idx = 0; idx < (stHdl->nFeat * 2); idx++) {
    stHdl->xCorr[idx] = (float*)memPtr;
    memPtr += xCorrPerFeatMemSize;

    stHdl->xCorrTmp[idx] = (float*)memPtr;
    memPtr += xCorrPerFeatTmpMemSize;

    stHdl->pitchPrev[idx] = (int*)memPtr;
    memPtr += pitchPrevPerFeatMemSize;
  }

  stHdl->pitchMaxPathReg[0] = (float*)memPtr;
  memPtr += pitchMaxPathRegPerRegMemSize;
  stHdl->pitchMaxPathReg[1] = (float*)memPtr;
  memPtr += pitchMaxPathRegPerRegMemSize;

  if (((int)(memPtr - (char*)memPtrExt)) > totalMemSize) {
    return -1;
  }

  return (totalMemSize);
}

static void AUP_PE_computeBandEnergy(const float* inBinPower,
                                     const int binPowNFFT,
                                     float bandE[AUP_PE_NB_BANDS]) {
  int i, j, bandSz;
  // float sum[NB_BANDS] = { 0 };
  float frac;
  float indexConvRate = 1.0;
  int indexOffset = 0, accIdx;
  int nBins = (binPowNFFT >> 1) + 1;

  indexConvRate = (float)binPowNFFT / AUP_PE_ASSUMED_FFT_4_BAND_ENG;
  for (i = 0; i < AUP_PE_NB_BANDS; i++) {
    bandE[i] = 0;
  }

  for (i = 0; i < AUP_PE_NB_BANDS - 1; i++) {
    bandSz = (int)roundf(
        (AUP_PE_BAND_START_INDEX[i + 1] - AUP_PE_BAND_START_INDEX[i]) *
        indexConvRate);  // WINDOW_SIZE_5MS;
    indexOffset = (int)roundf(AUP_PE_BAND_START_INDEX[i] *
                              indexConvRate);  // WINDOW_SIZE_5MS;

    for (j = 0; j < bandSz; j++) {
      frac = (float)j / bandSz;
      accIdx = AUP_PE_MIN(nBins - 1, (indexOffset + j));

      bandE[i] += (1 - frac) * inBinPower[accIdx];
      bandE[i + 1] += frac * inBinPower[accIdx];
    }
  }
  bandE[0] *= 2;
  bandE[AUP_PE_NB_BANDS - 1] *= 2;

  return;
}

static void AUP_PE_dct(const float DctTable[AUP_PE_NB_BANDS * AUP_PE_NB_BANDS],
                       const float* in, float* out) {
  int idx, j;
  float sum;
  float ratio = sqrtf(2.0f / AUP_PE_NB_BANDS);
  for (idx = 0; idx < AUP_PE_NB_BANDS; idx++) {
    sum = 0;
    for (j = 0; j < AUP_PE_NB_BANDS; j++) {
      sum += in[j] * DctTable[j * AUP_PE_NB_BANDS + idx];
    }
    out[idx] = sum * ratio;
  }
  return;
}

static void AUP_PE_idct(const float DctTable[AUP_PE_NB_BANDS * AUP_PE_NB_BANDS],
                        const float* in, float* out) {
  int idx, j;
  float sum;
  float ratio = sqrtf(2.0f / AUP_PE_NB_BANDS);
  for (idx = 0; idx < AUP_PE_NB_BANDS; idx++) {
    sum = 0;
    for (j = 0; j < AUP_PE_NB_BANDS; j++) {
      sum += in[j] * DctTable[idx * AUP_PE_NB_BANDS + j];
    }
    out[idx] = sum * ratio;
  }
  return;
}

static void AUP_PE_interp_band_gain(const int nBins,
                                    const float bandE[AUP_PE_NB_BANDS],
                                    float* g) {
  int idx, j, bandSz;
  float indexConvRate = 1.0f;
  int fftSz = (nBins - 1) * 2;
  int indexOffset = 0, accIdx;
  float frac;

  indexConvRate = ((float)fftSz) / AUP_PE_ASSUMED_FFT_4_BAND_ENG;
  memset(g, 0, sizeof(float) * nBins);

  for (idx = 0; idx < AUP_PE_NB_BANDS - 1; idx++) {
    bandSz = (int)roundf(
        (AUP_PE_BAND_START_INDEX[idx + 1] - AUP_PE_BAND_START_INDEX[idx]) *
        indexConvRate);  // WINDOW_SIZE_5MS;
    indexOffset = (int)roundf(AUP_PE_BAND_START_INDEX[idx] *
                              indexConvRate);  // WINDOW_SIZE_5MS;

    for (j = 0; j < bandSz; j++) {
      frac = (float)j / bandSz;
      accIdx = AUP_PE_MIN(nBins - 1, (indexOffset + j));

      g[accIdx] = (1 - frac) * bandE[idx] + frac * bandE[idx + 1];
    }
  }

  return;
}

// ac: in:  [0...p] autocorrelation values
// p: in: buffer length of _lpc and rc
// _lpc: out: [0...p-1] LPC coefficients
static float AUP_PE_celt_lpc(const float* ac, const int p, float* _lpc,
                             float* rc) {
  int i, j;
  float r;
  float error = ac[0];
  float* lpc = _lpc;
  float rr;
  float tmp1, tmp2;

  // RNN_CLEAR(lpc, p);
  memset(lpc, 0, sizeof(float) * p);
  // RNN_CLEAR(rc, p);
  memset(rc, 0, sizeof(float) * p);

  if (ac[0] != 0) {
    for (i = 0; i < p; i++) {
      /* Sum up this iteration's reflection coefficient */
      rr = 0;
      for (j = 0; j < i; j++) rr += lpc[j] * ac[i - j];
      rr += ac[i + 1];
      r = (-rr) / error;
      rc[i] = r;
      /*  Update LPC coefficients and total error */
      lpc[i] = r;
      for (j = 0; j<(i + 1)>> 1; j++) {
        tmp1 = lpc[j];
        tmp2 = lpc[i - 1 - j];
        lpc[j] = tmp1 + (r * tmp2);
        lpc[i - 1 - j] = tmp2 + (r * tmp1);
      }

      error = error - (r * r * error);
      /* Bail out once we get 30 dB gain */

      if (error < .001f * ac[0]) break;
    }
  }

  return error;
}

static float AUP_PE_lpc_from_bands(const int windowSz, const int nBins,
                                   const float Ex[AUP_PE_NB_BANDS],
                                   float lpc[AUP_PE_LPC_ORDER]) {
  int i;
  float e;
  float ac[AUP_PE_LPC_ORDER + 1] = {0};
  float rc[AUP_PE_LPC_ORDER] = {0};
  float Xr[AUP_PE_MAX_NBINS] = {0};
  float X_auto[AUP_PE_MAX_FFTSIZE + 4] = {0};
  float x_auto[AUP_PE_MAX_FFTSIZE + 4] = {0};
  float DC0_BIAS;
  int fftSz = (nBins - 1) * 2;

  AUP_PE_interp_band_gain(nBins, Ex, Xr);
  Xr[nBins - 1] = 0;  // remove nyquist freq.

  // RNN_CLEAR(X_auto, FREQ_SIZE);
  X_auto[0] = Xr[0];  // reformat as complex spectrum data
  X_auto[1] = Xr[nBins - 1];
  for (i = 1; i < (nBins - 1); i++) {
    X_auto[i << 1] = Xr[i];  // give value to its real part
  }                          // leave all the imaginary part as 0

  // inverse_transform(x_auto, X_auto); // IFFT, transform back to time domain
  // (X_auto -> x_auto)
  AUP_FFTW_InplaceTransf(0, fftSz, X_auto);
  if (fftSz == 256) {
    AUP_FFTW_c2r_256(X_auto, x_auto);
  } else if (fftSz == 512) {
    AUP_FFTW_c2r_512(X_auto, x_auto);
  } else if (fftSz == 1024) {
    AUP_FFTW_c2r_1024(X_auto, x_auto);
  }
  AUP_FFTW_RescaleIFFTOut(fftSz, x_auto);

  for (i = 0; i < (AUP_PE_LPC_ORDER + 1);
       i++) {  // take only the first LPC_ORDER + 1 coeff.
    ac[i] = x_auto[i];
  }

  // -40 dB noise floor
  DC0_BIAS = (windowSz / 12 / 38.0f);

  ac[0] += ac[0] * 1e-4f + DC0_BIAS;
  // Lag windowing
  for (i = 1; i < (AUP_PE_LPC_ORDER + 1); i++) {
    ac[i] *= (1 - 6e-5f * i * i);
  }

  e = AUP_PE_celt_lpc(ac, AUP_PE_LPC_ORDER, lpc, rc);

  return (e);
}

// lpc_from_cepstrum
static float AUP_PE_lpcCompute(
    const int windowSz, const int nBins,
    const float DctTable[AUP_PE_NB_BANDS * AUP_PE_NB_BANDS],
    const float* cepstrum, float* lpc) {
  int i;
  float Ex[AUP_PE_NB_BANDS] = {0};
  float tmp[AUP_PE_NB_BANDS] = {0};
  float errValue = 0;

  // RNN_COPY(tmp, cepstrum, NB_BANDS);
  memcpy(tmp, cepstrum, sizeof(float) * AUP_PE_NB_BANDS);

  AUP_PE_idct(DctTable, tmp, Ex);  // idct(Ex, tmp);
  for (i = 0; i < AUP_PE_NB_BANDS; i++) {
    Ex[i] = powf(10.f, Ex[i]) * AUP_PE_BAND_LPC_COMP[i];
  }

  errValue = AUP_PE_lpc_from_bands(windowSz, nBins, Ex, lpc);

  return (errValue);
}

static void AUP_PE_xcorr_kernel(const float* x, const float* y, float sum[4],
                                int len) {
  int j;
  float y_0, y_1, y_2, y_3;
  y_3 = 0; /* gcc doesn't realize that y_3 can't be used uninitialized */
  y_0 = *y++;
  y_1 = *y++;
  y_2 = *y++;
  for (j = 0; j < len - 3; j += 4) {
    float tmp;
    tmp = *x++;
    y_3 = *y++;
    sum[0] += tmp * y_0;
    sum[1] += tmp * y_1;
    sum[2] += tmp * y_2;
    sum[3] += tmp * y_3;
    tmp = *x++;
    y_0 = *y++;
    sum[0] += tmp * y_1;
    sum[1] += tmp * y_2;
    sum[2] += tmp * y_3;
    sum[3] += tmp * y_0;
    tmp = *x++;
    y_1 = *y++;
    sum[0] += tmp * y_2;
    sum[1] += tmp * y_3;
    sum[2] += tmp * y_0;
    sum[3] += tmp * y_1;
    tmp = *x++;
    y_2 = *y++;
    sum[0] += tmp * y_3;
    sum[1] += tmp * y_0;
    sum[2] += tmp * y_1;
    sum[3] += tmp * y_2;
  }
  if (j++ < len) {
    float tmp = *x++;
    y_3 = *y++;
    sum[0] += tmp * y_0;
    sum[1] += tmp * y_1;
    sum[2] += tmp * y_2;
    sum[3] += tmp * y_3;
  }
  if (j++ < len) {
    float tmp = *x++;
    y_0 = *y++;
    sum[0] += tmp * y_1;
    sum[1] += tmp * y_2;
    sum[2] += tmp * y_3;
    sum[3] += tmp * y_0;
  }
  if (j < len) {
    float tmp = *x++;
    y_1 = *y++;
    sum[0] += tmp * y_2;
    sum[1] += tmp * y_3;
    sum[2] += tmp * y_0;
    sum[3] += tmp * y_1;
  }
  return;
}

static float AUP_PE_celt_inner_prod(const float* x, const float* y, int N) {
  int i;
  float xy = 0;
  for (i = 0; i < N; i++) {
    xy += (x[i] * y[i]);
  }

  return (xy);
}

static void AUP_PE_MvingXCorr(int corrWindowLen, int corrShiftTimes,
                              const float* refIn, const float* yInToShift,
                              float* xcorr) {
  /* Unrolled version of the pitch correlation -- runs faster on x86 and ARM */
  int i;
  float tmp;

  for (i = 0; i < corrShiftTimes - 3; i += 4) {
    float sum[4] = {0, 0, 0, 0};
    AUP_PE_xcorr_kernel(refIn, yInToShift + i, sum, corrWindowLen);
    xcorr[i] = sum[0];
    xcorr[i + 1] = sum[1];
    xcorr[i + 2] = sum[2];
    xcorr[i + 3] = sum[3];
  }
  /* In case corrShiftTimes isn't a multiple of 4, do non-unrolled version. */
  for (; i < corrShiftTimes; i++) {
    tmp = AUP_PE_celt_inner_prod(refIn, yInToShift + i, corrWindowLen);
    xcorr[i] = tmp;
  }
  return;
}

// ==========================================================================================
// public APIs
// ==========================================================================================

int AUP_PE_create(void** stPtr) {
  PE_St* tmpPtr;

  if (stPtr == NULL) {
    return -1;
  }

  *stPtr = (void*)malloc(sizeof(PE_St));
  if (*stPtr == NULL) {
    return -1;
  }
  memset(*stPtr, 0, sizeof(PE_St));

  tmpPtr = (PE_St*)(*stPtr);

  tmpPtr->dynamMemPtr = NULL;
  tmpPtr->dynamMemSize = 0;

  if (AUP_Biquad_create(&(tmpPtr->biquadIIRPtr)) < 0 ||
      tmpPtr->biquadIIRPtr == NULL) {
    return -1;
  }

  tmpPtr->stCfg.fftSz = 1024;
  tmpPtr->stCfg.anaWindowSz = 768;
  tmpPtr->stCfg.hopSz = 256;
  tmpPtr->stCfg.useLPCPreFiltering = 1;
  tmpPtr->stCfg.procFs = 4000;  // 4KHz resampling rate

  tmpPtr->dynamCfg.voicedThr = 0.4f;

  return 0;
}

int AUP_PE_destroy(void** stPtr) {
  PE_St* stHdl;

  if (stPtr == NULL) {
    return 0;
  }

  stHdl = (PE_St*)(*stPtr);
  if (stHdl == NULL) {
    return 0;
  }

  if (stHdl->dynamMemPtr != NULL) {
    free(stHdl->dynamMemPtr);
  }
  stHdl->dynamMemPtr = NULL;

  if (stHdl->biquadIIRPtr != NULL) {
    AUP_Biquad_destroy(&(stHdl->biquadIIRPtr));
  }

  free(stHdl);
  (*stPtr) = NULL;

  return 0;
}

int AUP_PE_memAllocate(void* stPtr, const PE_StaticCfg* pCfg) {
  PE_St* stHdl = NULL;
  PE_StaticCfg localStCfg;
  Biquad_StaticCfg biquadStCfg = {0, 0, 0, 0, 0, 0};
  int idx;
  int totalMemSize = 0;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (PE_St*)(stPtr);

  memcpy(&localStCfg, pCfg, sizeof(PE_StaticCfg));
  if (AUP_PE_checkStatCfg(&localStCfg) < 0) {
    return -1;
  }

  memcpy(&(stHdl->stCfg), &localStCfg, sizeof(PE_StaticCfg));

  // publish internal static configuration registers
  if (AUP_PE_publishStaticCfg(stHdl) < 0) {
    return -1;
  }

  // check memory requirement
  totalMemSize = AUP_PE_dynamMemPrepare(stHdl, NULL, 0);
  if (totalMemSize < 0) {
    return -1;
  }

  // allocate dynamic memory
  if ((size_t)totalMemSize > stHdl->dynamMemSize) {
    if (stHdl->dynamMemPtr != NULL) {
      free(stHdl->dynamMemPtr);
      stHdl->dynamMemSize = 0;
    }
    stHdl->dynamMemPtr = malloc(totalMemSize);
    if (stHdl->dynamMemPtr == NULL) {
      return -1;
    }
    stHdl->dynamMemSize = totalMemSize;
  }
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  // setup the pointers/variable
  if (AUP_PE_dynamMemPrepare(stHdl, stHdl->dynamMemPtr, stHdl->dynamMemSize) <
      0) {
    return -1;
  }

  if (AUP_Biquad_getStaticCfg(stHdl->biquadIIRPtr, &biquadStCfg) < 0) {
    return -1;
  }
  biquadStCfg.maxNSample = stHdl->stCfg.hopSz;
  if (stHdl->procResampleRate != 1) {
    biquadStCfg.nsect = AUP_PE_LOWPSS_NSEC;
    if (stHdl->stCfg.procFs == 2000) {
      biquadStCfg.G = AUP_PE_G_2KHZ;
      for (idx = 0; idx < biquadStCfg.nsect; idx++) {
        biquadStCfg.B[idx] = AUP_PE_B_2KHZ[idx];
        biquadStCfg.A[idx] = AUP_PE_A_2KHZ[idx];
      }
    } else if (stHdl->stCfg.procFs == 4000) {
      biquadStCfg.G = AUP_PE_G_4KHZ;
      for (idx = 0; idx < biquadStCfg.nsect; idx++) {
        biquadStCfg.B[idx] = AUP_PE_B_4KHZ[idx];
        biquadStCfg.A[idx] = AUP_PE_A_4KHZ[idx];
      }
    } else if (stHdl->stCfg.procFs == 8000) {
      biquadStCfg.G = AUP_PE_G_8KHZ;
      for (idx = 0; idx < biquadStCfg.nsect; idx++) {
        biquadStCfg.B[idx] = AUP_PE_B_8KHZ[idx];
        biquadStCfg.A[idx] = AUP_PE_A_8KHZ[idx];
      }
    }
  } else {
    biquadStCfg.nsect = -1;
  }
  if (AUP_Biquad_memAllocate(stHdl->biquadIIRPtr, &biquadStCfg) < 0) {
    return -1;
  }

  return 0;
}

int AUP_PE_init(void* stPtr) {
  PE_St* stHdl;

  if (stPtr == NULL) {
    return -1;
  }
  stHdl = (PE_St*)(stPtr);

  if (AUP_PE_resetVariables(stHdl) < 0) {
    return -1;
  }

  return 0;
}

int AUP_PE_setDynamCfg(void* stPtr, const PE_DynamCfg* pCfg) {
  PE_St* stHdl;
  PE_DynamCfg localCfg;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }

  memcpy(&localCfg, pCfg, sizeof(PE_DynamCfg));
  if (AUP_PE_checkDynamCfg(&localCfg) < 0) {
    return -1;
  }

  stHdl = (PE_St*)(stPtr);

  memcpy(&(stHdl->dynamCfg), &localCfg, sizeof(PE_DynamCfg));

  return 0;
}

int AUP_PE_getStaticCfg(const void* stPtr, PE_StaticCfg* pCfg) {
  const PE_St* stHdl;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (const PE_St*)(stPtr);

  memcpy(pCfg, &(stHdl->stCfg), sizeof(PE_StaticCfg));

  return 0;
}

int AUP_PE_getDynamCfg(const void* stPtr, PE_DynamCfg* pCfg) {
  const PE_St* stHdl;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (const PE_St*)(stPtr);

  memcpy(pCfg, &(stHdl->dynamCfg), sizeof(PE_DynamCfg));

  return 0;
}

int AUP_PE_getAlgDelay(const void* stPtr, int* delayInFrms) {
  const PE_St* stHdl;

  if (stPtr == NULL || delayInFrms == NULL) {
    return -1;
  }
  stHdl = (const PE_St*)(stPtr);

  *delayInFrms = stHdl->estDelay;

  return 0;
}

int AUP_PE_proc(void* stPtr, const PE_InputData* pIn, PE_OutputData* pOut) {
  PE_St* stHdl = NULL;
  Biquad_InputData bqInData = {0, 0, 0};
  Biquad_OutputData bqOutData = {0};
  int nBins, fftSz, hopSz, idx, jdx, sub, offset, tmpInt, xcorrAccIdx;
  float bandPow[AUP_PE_NB_BANDS] = {0};  // Ex
  float Ly[AUP_PE_NB_BANDS] = {0};
  float follow, lpcErr, logMax;
  float energy0, slidWinSum, tmpDenom = 0, maxTrackReg = 0, maxPathReg = 0;
  float frmCorr = 0;  // frmCorrCorrection = 0;
  const float* startPtr = NULL;
  const float* refSeqPtr = NULL;
  const float* mvSeqPtr = NULL;
  int CORR_HALF_HOPSZ, SIDXT, XCIdx;
  int bestPeriodEstLocal[AUP_PE_TOTAL_NFEAT * 2] = {0};
  float w, sx = 0, sxx = 0, sxy = 0, sy = 0, sw = 0;
  float bestA = 0, bestB = 0;
  float estimatedPeriod;

  if (stPtr == NULL || pIn == NULL || pIn->timeSignal == NULL) {
    return -1;
  }
  stHdl = (PE_St*)(stPtr);

  nBins = (int)(stHdl->nBins);
  fftSz = (int)(stHdl->stCfg.fftSz);
  hopSz = (int)(stHdl->stCfg.hopSz);
  CORR_HALF_HOPSZ = hopSz / (stHdl->procResampleRate * 2);

  if (pIn->hopSz != (int)stHdl->stCfg.hopSz) {
    return -1;
  }

  if (stHdl->stCfg.useLPCPreFiltering == 1 && pIn->inBinPow == NULL) {
    return -1;
  }
  if (stHdl->stCfg.useLPCPreFiltering == 1 && pIn->nBins != stHdl->nBins) {
    return -1;
  }

  //////////////////////////////////////////////////////////////////////////////////////
  // Feature Pre-Calculation .... from compute_frame_features of lpcnet_enc.cc
  //////////////////////////////////////////////////////////////////////////////////////
  if (stHdl->stCfg.useLPCPreFiltering == 1) {
    // first. generate features and pre-raw information ...
    AUP_PE_computeBandEnergy(pIn->inBinPow, fftSz, bandPow);
    logMax = -2.0f;
    follow = -2.0f;
    for (idx = 0; idx < AUP_PE_NB_BANDS; idx++) {
      Ly[idx] = log10f(1e-2f + bandPow[idx]);  // Ex
      Ly[idx] = AUP_PE_MAX(logMax - 8.0f, AUP_PE_MAX(follow - 2.5f, Ly[idx]));
      logMax = AUP_PE_MAX(logMax, Ly[idx]);

      follow = AUP_PE_MAX(follow - 2.5f, Ly[idx]);
    }

    AUP_PE_dct(stHdl->dct_table, Ly, stHdl->tmpFeat);

    lpcErr = AUP_PE_lpcCompute((int)(stHdl->stCfg.anaWindowSz), nBins,
                               stHdl->dct_table, stHdl->tmpFeat, stHdl->lpc);

    memmove(stHdl->inputQ, stHdl->inputQ + hopSz,
            sizeof(float) * (stHdl->inputQLen - hopSz));
    memcpy(&(stHdl->inputQ[stHdl->inputQLen - hopSz]), pIn->timeSignal,
           sizeof(float) * hopSz);
    // then, take part out into alignedIn for later correlation calculation
    offset =
        AUP_PE_MAX(0, stHdl->inputQLen - hopSz - AUP_PE_XCORR_TRAINING_OFFSET);
    memcpy(stHdl->alignedIn, stHdl->inputQ + offset, sizeof(float) * hopSz);

    for (idx = 0; idx < hopSz; idx++) {
      // FIR LPC filtering .....
      slidWinSum = stHdl->alignedIn[idx];
      for (jdx = 0; jdx < AUP_PE_LPC_ORDER; jdx++) {
        slidWinSum += stHdl->lpc[jdx] * stHdl->pitch_mem[jdx];
      }

      memmove(stHdl->pitch_mem + 1, stHdl->pitch_mem,
              sizeof(float) * (AUP_PE_LPC_ORDER - 1));
      stHdl->pitch_mem[0] =
          stHdl->alignedIn[idx];  // push the latest base-sample into the tail
                                  // of FIFO

      stHdl->lpcFilterOutBuf[idx] = slidWinSum + 0.7f * stHdl->pitch_filt;
      stHdl->pitch_filt = slidWinSum;
    }

    if (stHdl->procResampleRate != 1) {
      // resample of lpcFilterOutBuf
      bqInData.nsamples = (size_t)hopSz;
      bqInData.samplesPtr = (const void*)(stHdl->lpcFilterOutBuf);
      bqInData.sampleType = 1;
      bqOutData.outputBuff =
          (void*)(stHdl->inputResampleBuf + stHdl->inputResampleBufIdx);
      if (AUP_Biquad_proc(stHdl->biquadIIRPtr, &bqInData, &bqOutData) < 0) {
        return -1;
      }
      tmpInt = stHdl->inputResampleBufIdx;
      for (idx = tmpInt; idx < (tmpInt + hopSz);
           idx += stHdl->procResampleRate) {
        stHdl->inputResampleBuf[stHdl->inputResampleBufIdx] =
            stHdl->inputResampleBuf[idx];
        stHdl->inputResampleBufIdx++;
      }
      // update the excBuf ....
      tmpInt = stHdl->inputResampleBufIdx;
      memmove(stHdl->excBuf, stHdl->excBuf + tmpInt,
              sizeof(float) * (stHdl->excBufLen - tmpInt));
      memcpy(stHdl->excBuf + (stHdl->excBufLen - tmpInt),
             stHdl->inputResampleBuf, sizeof(float) * tmpInt);
      stHdl->inputResampleBufIdx = 0;
    } else {
      tmpInt = hopSz;
      memmove(stHdl->excBuf, stHdl->excBuf + tmpInt,
              sizeof(float) * (stHdl->excBufLen - tmpInt));
      memcpy(stHdl->excBuf + (stHdl->excBufLen - tmpInt),
             stHdl->lpcFilterOutBuf, sizeof(float) * tmpInt);
    }

  } else {
    if (stHdl->procResampleRate != 1) {
      // resample of lpcFilterOutBuf
      bqInData.nsamples = (size_t)hopSz;
      bqInData.samplesPtr = (const void*)(pIn->timeSignal);
      bqInData.sampleType = 1;
      bqOutData.outputBuff =
          (void*)(stHdl->inputResampleBuf + stHdl->inputResampleBufIdx);
      if (AUP_Biquad_proc(stHdl->biquadIIRPtr, &bqInData, &bqOutData) < 0) {
        return -1;
      }
      tmpInt = stHdl->inputResampleBufIdx;
      for (idx = tmpInt; idx < (tmpInt + hopSz);
           idx += stHdl->procResampleRate) {
        stHdl->inputResampleBuf[stHdl->inputResampleBufIdx] =
            stHdl->inputResampleBuf[idx];
        stHdl->inputResampleBufIdx++;
      }

      // update the excBuf ....
      tmpInt = stHdl->inputResampleBufIdx;
      memmove(stHdl->excBuf, stHdl->excBuf + tmpInt,
              sizeof(float) * (stHdl->excBufLen - tmpInt));
      memcpy(stHdl->excBuf + (stHdl->excBufLen - tmpInt),
             stHdl->inputResampleBuf, sizeof(float) * tmpInt);
      stHdl->inputResampleBufIdx = 0;
    } else {
      tmpInt = hopSz;
      memmove(stHdl->excBuf, stHdl->excBuf + tmpInt,
              sizeof(float) * (stHdl->excBufLen - tmpInt));
      memcpy(stHdl->excBuf + (stHdl->excBufLen - tmpInt), pIn->timeSignal,
             sizeof(float) * tmpInt);
    }
  }

  // prepare for cross-correlation computation ....
  for (idx = 0; idx < stHdl->excBufLen; idx++) {
    stHdl->excBufSq[idx] = (stHdl->excBuf[idx] * stHdl->excBuf[idx]);
  }

  // shift the frmWeight queue to left space for this new frame
  for (idx = 0; idx < (stHdl->nFeat - 1); idx++) {
    stHdl->frmWeight[2 * (idx)] = stHdl->frmWeight[2 * (idx + 1)];
    stHdl->frmWeight[2 * (idx) + 1] = stHdl->frmWeight[2 * (idx + 1) + 1];
  }

  // do the cross-correlation .....
  for (sub = 0; sub < 2; sub++) {
    xcorrAccIdx = 2 * (stHdl->xCorrOffsetIdx) + sub;
    offset = sub * CORR_HALF_HOPSZ;

    refSeqPtr = stHdl->excBuf + (stHdl->maxPeriod + offset);
    mvSeqPtr = stHdl->excBuf + offset;
    AUP_PE_MvingXCorr(CORR_HALF_HOPSZ, stHdl->maxPeriod, refSeqPtr, mvSeqPtr,
                      stHdl->xCorrInst);

    energy0 = 0;
    startPtr = stHdl->excBufSq + (stHdl->maxPeriod + offset);
    for (idx = 0; idx < CORR_HALF_HOPSZ; idx++) {
      energy0 += startPtr[idx];
    }
    stHdl->frmWeight[2 * (stHdl->nFeat - 1) + sub] = energy0;

    slidWinSum = 0;
    startPtr = stHdl->excBufSq + offset;
    for (idx = 0; idx < CORR_HALF_HOPSZ; idx++) {
      slidWinSum += startPtr[idx];
    }

    // special hanlding for the 0th element
    tmpDenom = AUP_PE_MAX(1e-12f, slidWinSum + (1 + energy0));
    stHdl->xCorr[xcorrAccIdx][0] = 2 * stHdl->xCorrInst[0] / tmpDenom;

    for (idx = 1; idx < stHdl->maxPeriod; idx++) {
      // update the slidWinSum
      slidWinSum =
          AUP_PE_MAX(0, slidWinSum - stHdl->excBufSq[offset + idx - 1]);
      slidWinSum += stHdl->excBufSq[offset + idx + CORR_HALF_HOPSZ - 1];

      tmpDenom = AUP_PE_MAX(1e-12f, slidWinSum + (1 + energy0));
      stHdl->xCorr[xcorrAccIdx][idx] = 2 * stHdl->xCorrInst[idx] / tmpDenom;
    }

    // shrink/sharpen the values in xCorr array ...
    for (idx = 0; idx < (stHdl->maxPeriod - 2 * stHdl->minPeriod); idx++) {
      tmpDenom = stHdl->xCorr[xcorrAccIdx][(stHdl->maxPeriod + idx) / 2];
      tmpDenom = AUP_PE_MAX(
          tmpDenom,
          stHdl->xCorr[xcorrAccIdx][(stHdl->maxPeriod + idx + 2) / 2]);
      tmpDenom = AUP_PE_MAX(
          tmpDenom,
          stHdl->xCorr[xcorrAccIdx][(stHdl->maxPeriod + idx - 1) / 2]);

      if (stHdl->xCorr[xcorrAccIdx][idx] < (tmpDenom * 1.1f))
        stHdl->xCorr[xcorrAccIdx][idx] *= 0.8f;
    }
  }
  stHdl->xCorrOffsetIdx++;
  if (stHdl->xCorrOffsetIdx >= stHdl->nFeat) {
    stHdl->xCorrOffsetIdx = 0;
  }

  //////////////////////////////////////////////////////////////////////////////////////
  // Pitch Estimation .... from process_superframe of lpcnet_enc.cc
  //////////////////////////////////////////////////////////////////////////////////////
  slidWinSum = 1e-15f;
  for (sub = 0; sub < (stHdl->nFeat * 2); sub++) {
    slidWinSum += stHdl->frmWeight[sub];
  }
  for (sub = 0; sub < (stHdl->nFeat * 2); sub++) {
    stHdl->frmWeightNorm[sub] =
        stHdl->frmWeight[sub] * ((stHdl->nFeat * 2) / slidWinSum);
  }

  // copy xCorr to xCorrTmp, so that later-on we can modify the content in
  // xCorrTmp without impacting next hop's processing
  for (idx = 0; idx < (stHdl->nFeat * 2); idx++) {
    memcpy(stHdl->xCorrTmp[idx], stHdl->xCorr[idx],
           sizeof(float) * (stHdl->maxPeriod + 1));
  }

  // shift pitchPrev buffer to left space for this new frame's result
  for (sub = 0; sub < (stHdl->nFeat * 2 - 2); sub += 2) {
    memcpy(stHdl->pitchPrev[sub], stHdl->pitchPrev[sub + 2],
           sizeof(int) * stHdl->maxPeriod);
    memcpy(stHdl->pitchPrev[sub + 1], stHdl->pitchPrev[sub + 3],
           sizeof(int) * stHdl->maxPeriod);
  }
  for (sub = (stHdl->nFeat * 2 - 2); sub < (stHdl->nFeat * 2); sub++) {
    XCIdx = sub + (stHdl->xCorrOffsetIdx * 2);
    if (XCIdx >= (2 * stHdl->nFeat)) {
      XCIdx -= (2 * stHdl->nFeat);
    }

    for (idx = 0; idx < stHdl->difPeriod; idx++) {
      maxTrackReg = stHdl->pitchMaxPathAll - 1e10f;
      stHdl->pitchPrev[sub][idx] = stHdl->bestPeriodEst;

      SIDXT = AUP_PE_MIN(0, 4 - idx);
      for (jdx = SIDXT; jdx <= 4 && (idx + jdx) < stHdl->difPeriod; jdx++) {
        tmpDenom = stHdl->pitchMaxPathReg[0][idx + jdx] -
                   (AUP_PE_PITCHMAXPATH_W * abs(jdx) * abs(jdx));
        if (tmpDenom > maxTrackReg) {
          maxTrackReg = tmpDenom;
          stHdl->pitchPrev[sub][idx] = idx + jdx;
        }
      }

      // store the max search result into pitch_max_path[1][...]
      stHdl->pitchMaxPathReg[1][idx] =
          maxTrackReg + stHdl->frmWeightNorm[sub] * stHdl->xCorrTmp[XCIdx][idx];
    }

    maxPathReg = -1e15f;
    tmpInt = 0;
    for (idx = 0; idx < stHdl->difPeriod; idx++) {
      if (stHdl->pitchMaxPathReg[1][idx] > maxPathReg) {
        maxPathReg = stHdl->pitchMaxPathReg[1][idx];
        tmpInt = idx;
      }
    }
    stHdl->pitchMaxPathAll = maxPathReg;
    stHdl->bestPeriodEst = tmpInt;

    memcpy(&(stHdl->pitchMaxPathReg[0][0]), &(stHdl->pitchMaxPathReg[1][0]),
           sizeof(float) * stHdl->maxPeriod);
    for (idx = 0; idx < stHdl->difPeriod; idx++) {
      stHdl->pitchMaxPathReg[0][idx] -= maxPathReg;
    }
  }

  tmpInt = stHdl->bestPeriodEst;
  frmCorr = 0;
  // Backward pass
  for (sub = (stHdl->nFeat * 2) - 1; sub >= 0; sub--) {
    bestPeriodEstLocal[sub] = stHdl->maxPeriod - tmpInt;

    XCIdx = sub + (stHdl->xCorrOffsetIdx * 2);
    if (XCIdx >= (2 * stHdl->nFeat)) {
      XCIdx -= (2 * stHdl->nFeat);
    }
    frmCorr += stHdl->frmWeightNorm[sub] * stHdl->xCorrTmp[XCIdx][tmpInt];
    tmpInt = stHdl->pitchPrev[sub][tmpInt];
  }
  frmCorr = AUP_PE_MAX(0, frmCorr / (float)(stHdl->nFeat * 2));
  stHdl->voiced = (frmCorr >= stHdl->dynamCfg.voicedThr) ? 1 : 0;

  for (sub = 0; sub < (stHdl->nFeat * 2); sub++) {
    w = stHdl->frmWeightNorm[sub];
    sw += w;
    sx += w * sub;
    sxx += w * sub * sub;
    sxy += w * sub * bestPeriodEstLocal[sub];
    sy += w * bestPeriodEstLocal[sub];
  }

  // Linear regression to figure out the pitch contour
  // frmCorrCorrection = frmCorr;
  tmpDenom = (sw * sxx - sx * sx);
  if (tmpDenom == 0)
    bestA = (sw * sxy - sx * sy) / 1e-15f;
  else
    bestA = (sw * sxy - sx * sy) / tmpDenom;

  if (stHdl->voiced == 1) {
    tmpDenom = (sy / sw) / (4 * 2 * stHdl->nFeat);
    bestA = AUP_PE_MIN(tmpDenom, AUP_PE_MAX(-tmpDenom, bestA));
  } else {  // if there is no voice inside this frame
    bestA = 0;
  }
  bestB = (sy - bestA * sx) / sw;
  estimatedPeriod = bestB + 5.5f * bestA;

  if (stHdl->voiced == 1) {
    stHdl->pitchEstResult =
        ((float)(stHdl->stCfg.procFs)) / AUP_PE_MAX(1.0f, estimatedPeriod);
  } else {
    stHdl->pitchEstResult = 0;
  }

  if (pOut != NULL) {
    pOut->pitchFreq = stHdl->pitchEstResult;
    pOut->voiced = stHdl->voiced;
  }

  return 0;
}

//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stft.h"
#include "stft_st.h"
#include "fftw.h"

// ==========================================================================================
// internal tools
// ==========================================================================================

static int AUP_Analyzer_checkStatCfg(Analyzer_StaticCfg* pCfg) {
  if (pCfg == NULL) {
    return -1;
  }

  if (pCfg->fft_size != 256 && pCfg->fft_size != 512 &&
      pCfg->fft_size != 1024 && pCfg->fft_size != 2048 &&
      pCfg->fft_size != 4096) {
    return -1;
  }

  if (pCfg->win_len <= 0 || pCfg->win_len < pCfg->hop_size ||
      pCfg->win_len > pCfg->fft_size) {
    return -1;
  }

  if (pCfg->hop_size <= 0) {
    return -1;
  }

  return 0;
}

static int AUP_Analyzer_publishStaticCfg(Analyzer_St* stHdl) {
  const Analyzer_StaticCfg* pStatCfg;
  int idx;

  if (stHdl == NULL) {
    return -1;
  }
  pStatCfg = (const Analyzer_StaticCfg*)(&(stHdl->stCfg));

  stHdl->nBins = (pStatCfg->fft_size >> 1) + 1;
  if (pStatCfg->ana_win_coeff != NULL) {
    memcpy(stHdl->windowCoffCopy, pStatCfg->ana_win_coeff,
           sizeof(float) * pStatCfg->win_len);
  } else {
    for (idx = 0; idx < AUP_STFT_MAX_FFTSZ; idx++) {
      stHdl->windowCoffCopy[idx] = 1.0f;
    }
  }
  return 0;
}

static int AUP_Analyzer_resetVariables(Analyzer_St* stHdl) {
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);
  return 0;
}

static int AUP_Analyzer_dynamMemPrepare(Analyzer_St* stHdl, void* memPtrExt,
                                        size_t memSize) {
  int inputQMemSz = 0;
  int fftInputBufMemSz = 0;
  int totalMemSize = 0;
  char* memPtr = NULL;

  inputQMemSz = AUP_STFT_ALIGN8(sizeof(float) * (stHdl->stCfg.win_len + 4));
  totalMemSize += inputQMemSz;

  fftInputBufMemSz =
      AUP_STFT_ALIGN8(sizeof(float) * (stHdl->stCfg.fft_size + 4));
  totalMemSize += fftInputBufMemSz;

  // if no external memory provided, we are only profiling the memory
  // requirement
  if (memPtrExt == NULL) {
    return (totalMemSize);
  }

  // if required memory is more than provided, error
  if ((size_t)totalMemSize > memSize) {
    return -1;
  }

  memPtr = (char*)memPtrExt;

  stHdl->inputQ = (float*)memPtr;
  memPtr += inputQMemSz;

  stHdl->fftInputBuf = (float*)memPtr;
  memPtr += fftInputBufMemSz;

  if (((int)(memPtr - (char*)memPtrExt)) > totalMemSize) {
    return -1;
  }

  return (totalMemSize);
}

// ==========================================================================================
// public APIs
// ==========================================================================================

int AUP_Analyzer_create(void** stPtr) {
  Analyzer_St* tmpPtr;

  if (stPtr == NULL) {
    return -1;
  }

  *stPtr = (void*)malloc(sizeof(Analyzer_St));
  if (*stPtr == NULL) {
    return -1;
  }
  memset(*stPtr, 0, sizeof(Analyzer_St));

  tmpPtr = (Analyzer_St*)(*stPtr);

  tmpPtr->dynamMemPtr = NULL;
  tmpPtr->dynamMemSize = 0;

  tmpPtr->stCfg.win_len = 768;
  tmpPtr->stCfg.hop_size = 256;
  tmpPtr->stCfg.fft_size = 1024;
  tmpPtr->stCfg.ana_win_coeff = NULL;

  return 0;
}

int AUP_Analyzer_destroy(void** stPtr) {
  Analyzer_St* stHdl;

  if (stPtr == NULL) {
    return 0;
  }

  stHdl = (Analyzer_St*)(*stPtr);
  if (stHdl == NULL) {
    return 0;
  }

  if (stHdl->dynamMemPtr != NULL) {
    free(stHdl->dynamMemPtr);
  }
  stHdl->dynamMemPtr = NULL;

  free(stHdl);
  (*stPtr) = NULL;

  return 0;
}

int AUP_Analyzer_memAllocate(void* stPtr, const Analyzer_StaticCfg* pCfg) {
  Analyzer_St* stHdl = NULL;
  Analyzer_StaticCfg localStCfg;
  int totalMemSize = 0;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (Analyzer_St*)(stPtr);

  memcpy(&localStCfg, pCfg, sizeof(Analyzer_StaticCfg));
  if (AUP_Analyzer_checkStatCfg(&localStCfg) < 0) {
    return -1;
  }

  memcpy(&(stHdl->stCfg), &localStCfg, sizeof(Analyzer_StaticCfg));

  // 1st. publish internal static configuration registers
  if (AUP_Analyzer_publishStaticCfg(stHdl) < 0) {
    return -1;
  }

  // 4th: check memory requirement
  totalMemSize = AUP_Analyzer_dynamMemPrepare(stHdl, NULL, 0);
  if (totalMemSize < 0) {
    return -1;
  }

  // 5th: allocate dynamic memory
  if ((size_t)totalMemSize > stHdl->dynamMemSize) {
    if (stHdl->dynamMemPtr != NULL) {
      free(stHdl->dynamMemPtr);
      stHdl->dynamMemSize = 0;
    }
    stHdl->dynamMemPtr = malloc(totalMemSize);
    if (stHdl->dynamMemPtr == NULL) {
      return -1;
    }
    stHdl->dynamMemSize = totalMemSize;
  }
  memset(stHdl->dynamMemPtr, 0, stHdl->dynamMemSize);

  // 6th: setup the pointers/variable
  if (AUP_Analyzer_dynamMemPrepare(stHdl, stHdl->dynamMemPtr,
                                   stHdl->dynamMemSize) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Analyzer_init(void* stPtr) {
  Analyzer_St* stHdl;

  if (stPtr == NULL) {
    return -1;
  }
  stHdl = (Analyzer_St*)(stPtr);

  if (AUP_Analyzer_resetVariables(stHdl) < 0) {
    return -1;
  }

  return 0;
}

int AUP_Analyzer_getStaticCfg(const void* stPtr, Analyzer_StaticCfg* pCfg) {
  const Analyzer_St* stHdl;

  if (stPtr == NULL || pCfg == NULL) {
    return -1;
  }
  stHdl = (const Analyzer_St*)(stPtr);

  memcpy(pCfg, &(stHdl->stCfg), sizeof(Analyzer_StaticCfg));

  return 0;
}

int AUP_Analyzer_proc(void* stPtr, const Analyzer_InputData* pIn,
                      Analyzer_OutputData* pOut) {
  Analyzer_St* stHdl = NULL;
  int hopSz, fftSz, winLen, nBins;
  int idx = 0;

  if (stPtr == NULL || pIn == NULL || pIn->input == NULL || pOut == NULL ||
      pOut->output == NULL) {
    return -1;
  }
  stHdl = (Analyzer_St*)(stPtr);

  if (pIn->iLength != stHdl->stCfg.hop_size ||
      pOut->oLength < stHdl->stCfg.fft_size) {
    return -1;
  }
  hopSz = stHdl->stCfg.hop_size;
  fftSz = stHdl->stCfg.fft_size;
  nBins = (fftSz >> 1) + 1;
  winLen = stHdl->stCfg.win_len;

  memset(pOut->output, 0, sizeof(float) * pOut->oLength);
  memmove(stHdl->inputQ, stHdl->inputQ + hopSz,
          sizeof(float) * (winLen - hopSz));
  memcpy(stHdl->inputQ + (winLen - hopSz), pIn->input, sizeof(float) * hopSz);

  if (stHdl->stCfg.ana_win_coeff != NULL) {
    for (idx = 0; idx < winLen; idx++) {
      stHdl->fftInputBuf[idx] = stHdl->inputQ[idx] * stHdl->windowCoffCopy[idx];
    }
  } else {
    for (idx = 0; idx < winLen; idx++) {
      stHdl->fftInputBuf[idx] = stHdl->inputQ[idx];
    }
  }
  for (; idx < fftSz; idx++) {
    stHdl->fftInputBuf[idx] = 0;
  }

  if (fftSz == 256) {
    AUP_FFTW_r2c_256(stHdl->fftInputBuf, pOut->output);
  } else if (fftSz == 512) {
    AUP_FFTW_r2c_512(stHdl->fftInputBuf, pOut->output);
  } else if (fftSz == 1024) {
    AUP_FFTW_r2c_1024(stHdl->fftInputBuf, pOut->output);
  } else if (fftSz == 2048) {
    AUP_FFTW_r2c_2048(stHdl->fftInputBuf, pOut->output);
  } else if (fftSz == 4096) {
    AUP_FFTW_r2c_4096(stHdl->fftInputBuf, pOut->output);
  }
  AUP_FFTW_InplaceTransf(1, fftSz, pOut->output);
  AUP_FFTW_RescaleFFTOut(fftSz, pOut->output);

  return 0;
}

//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fftw.h"

static const int AUP_FFTW_g_ip256[] = {64, 64, 0, 16, 0, 64, 32, 96};
static const float AUP_FFTW_g_w256[] = {
    1.000000e+00f, 7.071068e-01f, 5.006030e-01f,  5.054710e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    1.000000e+00f, 7.071068e-01f, 5.024193e-01f,  5.224986e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    1.000000e+00f, 7.071068e-01f, 5.097956e-01f,  6.013449e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    1.000000e+00f, 7.071068e-01f, 9.238795e-01f,  3.826834e-01f,
    1.000000e+00f, 7.071068e-01f, 0.000000e+00f,  0.000000e+00f,
    7.071068e-01f, 4.998494e-01f, 4.993977e-01f,  4.986452e-01f,
    4.975924e-01f, 4.962398e-01f, 4.945883e-01f,  4.926388e-01f,
    4.903926e-01f, 4.878511e-01f, 4.850156e-01f,  4.818880e-01f,
    4.784702e-01f, 4.747641e-01f, 4.707720e-01f,  4.664964e-01f,
    4.619398e-01f, 4.571049e-01f, 4.519946e-01f,  4.466122e-01f,
    4.409606e-01f, 4.350435e-01f, 4.288643e-01f,  4.224268e-01f,
    4.157348e-01f, 4.087924e-01f, 4.016038e-01f,  3.941732e-01f,
    3.865052e-01f, 3.786044e-01f, 3.704756e-01f,  3.621235e-01f,
    3.535534e-01f, 3.447703e-01f, 3.357795e-01f,  3.265864e-01f,
    3.171966e-01f, 3.076158e-01f, 2.978497e-01f,  2.879041e-01f,
    2.777851e-01f, 2.674988e-01f, 2.570514e-01f,  2.464491e-01f,
    2.356984e-01f, 2.248057e-01f, 2.137775e-01f,  2.026207e-01f,
    1.913417e-01f, 1.799475e-01f, 1.684449e-01f,  1.568409e-01f,
    1.451423e-01f, 1.333564e-01f, 1.214901e-01f,  1.095506e-01f,
    9.754516e-02f, 8.548094e-02f, 7.336524e-02f,  6.120534e-02f,
    4.900857e-02f, 3.678228e-02f, 2.453384e-02f,  1.227061e-02f};
static const int AUP_FFTW_g_ip512[] = {128, 128, 0, 16, 0, 64, 32, 96};
static const float AUP_FFTW_g_w512[] = {
    1.000000e+00f, 7.071068e-01f, 5.001506e-01f,  5.013585e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    1.000000e+00f, 7.071068e-01f, 5.006030e-01f,  5.054710e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    1.000000e+00f, 7.071068e-01f, 5.024193e-01f,  5.224986e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    1.000000e+00f, 7.071068e-01f, 5.097956e-01f,  6.013449e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    1.000000e+00f, 7.071068e-01f, 9.238795e-01f,  3.826834e-01f,
    1.000000e+00f, 7.071068e-01f, 0.000000e+00f,  0.000000e+00f,
    7.071068e-01f, 4.999624e-01f, 4.998494e-01f,  4.996612e-01f,
    4.993977e-01f, 4.990591e-01f, 4.986452e-01f,  4.981563e-01f,
    4.975924e-01f, 4.969535e-01f, 4.962398e-01f,  4.954513e-01f,
    4.945883e-01f, 4.936507e-01f, 4.926388e-01f,  4.915527e-01f,
    4.903926e-01f, 4.891587e-01f, 4.878511e-01f,  4.864700e-01f,
    4.850156e-01f, 4.834882e-01f, 4.818880e-01f,  4.802153e-01f,
    4.784702e-01f, 4.766530e-01f, 4.747641e-01f,  4.728037e-01f,
    4.707720e-01f, 4.686695e-01f, 4.664964e-01f,  4.642530e-01f,
    4.619398e-01f, 4.595569e-01f, 4.571049e-01f,  4.545840e-01f,
    4.519946e-01f, 4.493372e-01f, 4.466122e-01f,  4.438198e-01f,
    4.409606e-01f, 4.380350e-01f, 4.350435e-01f,  4.319864e-01f,
    4.288643e-01f, 4.256776e-01f, 4.224268e-01f,  4.191124e-01f,
    4.157348e-01f, 4.122947e-01f, 4.087924e-01f,  4.052286e-01f,
    4.016038e-01f, 3.979185e-01f, 3.941732e-01f,  3.903686e-01f,
    3.865052e-01f, 3.825836e-01f, 3.786044e-01f,  3.745682e-01f,
    3.704756e-01f, 3.663271e-01f, 3.621235e-01f,  3.578654e-01f,
    3.535534e-01f, 3.491881e-01f, 3.447703e-01f,  3.403005e-01f,
    3.357795e-01f, 3.312079e-01f, 3.265864e-01f,  3.219158e-01f,
    3.171966e-01f, 3.124297e-01f, 3.076158e-01f,  3.027555e-01f,
    2.978497e-01f, 2.928989e-01f, 2.879041e-01f,  2.828659e-01f,
    2.777851e-01f, 2.726625e-01f, 2.674988e-01f,  2.622948e-01f,
    2.570514e-01f, 2.517692e-01f, 2.464491e-01f,  2.410919e-01f,
    2.356984e-01f, 2.302694e-01f, 2.248057e-01f,  2.193081e-01f,
    2.137775e-01f, 2.082148e-01f, 2.026207e-01f,  1.969960e-01f,
    1.913417e-01f, 1.856586e-01f, 1.799475e-01f,  1.742093e-01f,
    1.684449e-01f, 1.626551e-01f, 1.568409e-01f,  1.510030e-01f,
    1.451423e-01f, 1.392598e-01f, 1.333564e-01f,  1.274328e-01f,
    1.214901e-01f, 1.155291e-01f, 1.095506e-01f,  1.035557e-01f,
    9.754516e-02f, 9.151994e-02f, 8.548094e-02f,  7.942907e-02f,
    7.336524e-02f, 6.729035e-02f, 6.120534e-02f,  5.511110e-02f,
    4.900857e-02f, 4.289866e-02f, 3.678228e-02f,  3.066037e-02f,
    2.453384e-02f, 1.840361e-02f, 1.227061e-02f,  6.135769e-03f};
static const int AUP_FFTW_g_ip1024[] = {256, 256, 0,   16,  0,  64,  32,  96,
                                        0,   256, 128, 384, 64, 320, 192, 448};
static const float AUP_FFTW_g_w1024[] = {
    1.000000e+00f, 7.071068e-01f, 5.000377e-01f,  5.003390e-01f,
    9.996988e-01f, 2.454123e-02f, 9.972905e-01f,  -7.356456e-02f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.972905e-01f, 7.356456e-02f, 9.757021e-01f,  -2.191012e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.924795e-01f, 1.224107e-01f, 9.329928e-01f,  -3.598950e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.852776e-01f, 1.709619e-01f, 8.700870e-01f,  -4.928982e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.757021e-01f, 2.191012e-01f, 7.883464e-01f,  -6.152316e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.637761e-01f, 2.667128e-01f, 6.895405e-01f,  -7.242471e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.495282e-01f, 3.136817e-01f, 5.758082e-01f,  -8.175848e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.329928e-01f, 3.598950e-01f, 4.496113e-01f,  -8.932243e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.142098e-01f, 4.052413e-01f, 3.136817e-01f,  -9.495282e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.932243e-01f, 4.496113e-01f, 1.709619e-01f,  -9.852776e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.700870e-01f, 4.928982e-01f, 2.454123e-02f,  -9.996988e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.448536e-01f, 5.349976e-01f, -1.224107e-01f, -9.924795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.175848e-01f, 5.758082e-01f, -2.667128e-01f, -9.637761e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.883464e-01f, 6.152316e-01f, -4.052413e-01f, -9.142098e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.572088e-01f, 6.531728e-01f, -5.349976e-01f, -8.448536e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    7.242471e-01f, 6.895405e-01f, -6.531728e-01f, -7.572088e-01f,
    1.000000e+00f, 7.071068e-01f, 5.001506e-01f,  5.013585e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    1.000000e+00f, 7.071068e-01f, 5.006030e-01f,  5.054710e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    1.000000e+00f, 7.071068e-01f, 5.024193e-01f,  5.224986e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    1.000000e+00f, 7.071068e-01f, 5.097956e-01f,  6.013449e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    1.000000e+00f, 7.071068e-01f, 9.238795e-01f,  3.826834e-01f,
    1.000000e+00f, 7.071068e-01f, 0.000000e+00f,  0.000000e+00f,
    7.071068e-01f, 4.999906e-01f, 4.999624e-01f,  4.999153e-01f,
    4.998494e-01f, 4.997647e-01f, 4.996612e-01f,  4.995389e-01f,
    4.993977e-01f, 4.992378e-01f, 4.990591e-01f,  4.988615e-01f,
    4.986452e-01f, 4.984101e-01f, 4.981563e-01f,  4.978837e-01f,
    4.975924e-01f, 4.972823e-01f, 4.969535e-01f,  4.966060e-01f,
    4.962398e-01f, 4.958549e-01f, 4.954513e-01f,  4.950291e-01f,
    4.945883e-01f, 4.941288e-01f, 4.936507e-01f,  4.931540e-01f,
    4.926388e-01f, 4.921050e-01f, 4.915527e-01f,  4.909819e-01f,
    4.903926e-01f, 4.897849e-01f, 4.891587e-01f,  4.885141e-01f,
    4.878511e-01f, 4.871697e-01f, 4.864700e-01f,  4.857519e-01f,
    4.850156e-01f, 4.842610e-01f, 4.834882e-01f,  4.826972e-01f,
    4.818880e-01f, 4.810607e-01f, 4.802153e-01f,  4.793517e-01f,
    4.784702e-01f, 4.775706e-01f, 4.766530e-01f,  4.757175e-01f,
    4.747641e-01f, 4.737928e-01f, 4.728037e-01f,  4.717967e-01f,
    4.707720e-01f, 4.697296e-01f, 4.686695e-01f,  4.675918e-01f,
    4.664964e-01f, 4.653835e-01f, 4.642530e-01f,  4.631051e-01f,
    4.619398e-01f, 4.607570e-01f, 4.595569e-01f,  4.583395e-01f,
    4.571049e-01f, 4.558530e-01f, 4.545840e-01f,  4.532979e-01f,
    4.519946e-01f, 4.506744e-01f, 4.493372e-01f,  4.479831e-01f,
    4.466122e-01f, 4.452244e-01f, 4.438198e-01f,  4.423985e-01f,
    4.409606e-01f, 4.395061e-01f, 4.380350e-01f,  4.365475e-01f,
    4.350435e-01f, 4.335231e-01f, 4.319864e-01f,  4.304335e-01f,
    4.288643e-01f, 4.272790e-01f, 4.256776e-01f,  4.240602e-01f,
    4.224268e-01f, 4.207775e-01f, 4.191124e-01f,  4.174314e-01f,
    4.157348e-01f, 4.140225e-01f, 4.122947e-01f,  4.105513e-01f,
    4.087924e-01f, 4.070182e-01f, 4.052286e-01f,  4.034238e-01f,
    4.016038e-01f, 3.997686e-01f, 3.979185e-01f,  3.960533e-01f,
    3.941732e-01f, 3.922783e-01f, 3.903686e-01f,  3.884442e-01f,
    3.865052e-01f, 3.845517e-01f, 3.825836e-01f,  3.806012e-01f,
    3.786044e-01f, 3.765934e-01f, 3.745682e-01f,  3.725289e-01f,
    3.704756e-01f, 3.684083e-01f, 3.663271e-01f,  3.642322e-01f,
    3.621235e-01f, 3.600013e-01f, 3.578654e-01f,  3.557161e-01f,
    3.535534e-01f, 3.513774e-01f, 3.491881e-01f,  3.469857e-01f,
    3.447703e-01f, 3.425418e-01f, 3.403005e-01f,  3.380464e-01f,
    3.357795e-01f, 3.335000e-01f, 3.312079e-01f,  3.289033e-01f,
    3.265864e-01f, 3.242572e-01f, 3.219158e-01f,  3.195622e-01f,
    3.171966e-01f, 3.148191e-01f, 3.124297e-01f,  3.100286e-01f,
    3.076158e-01f, 3.051914e-01f, 3.027555e-01f,  3.003082e-01f,
    2.978497e-01f, 2.953799e-01f, 2.928989e-01f,  2.904070e-01f,
    2.879041e-01f, 2.853904e-01f, 2.828659e-01f,  2.803308e-01f,
    2.777851e-01f, 2.752290e-01f, 2.726625e-01f,  2.700857e-01f,
    2.674988e-01f, 2.649018e-01f, 2.622948e-01f,  2.596780e-01f,
    2.570514e-01f, 2.544151e-01f, 2.517692e-01f,  2.491138e-01f,
    2.464491e-01f, 2.437751e-01f, 2.410919e-01f,  2.383996e-01f,
    2.356984e-01f, 2.329882e-01f, 2.302694e-01f,  2.275418e-01f,
    2.248057e-01f, 2.220611e-01f, 2.193081e-01f,  2.165469e-01f,
    2.137775e-01f, 2.110001e-01f, 2.082148e-01f,  2.054216e-01f,
    2.026207e-01f, 1.998121e-01f, 1.969960e-01f,  1.941725e-01f,
    1.913417e-01f, 1.885037e-01f, 1.856586e-01f,  1.828065e-01f,
    1.799475e-01f, 1.770818e-01f, 1.742093e-01f,  1.713304e-01f,
    1.684449e-01f, 1.655532e-01f, 1.626551e-01f,  1.597510e-01f,
    1.568409e-01f, 1.539248e-01f, 1.510030e-01f,  1.480754e-01f,
    1.451423e-01f, 1.422038e-01f, 1.392598e-01f,  1.363107e-01f,
    1.333564e-01f, 1.303971e-01f, 1.274328e-01f,  1.244638e-01f,
    1.214901e-01f, 1.185118e-01f, 1.155291e-01f,  1.125420e-01f,
    1.095506e-01f, 1.065552e-01f, 1.035557e-01f,  1.005523e-01f,
    9.754516e-02f, 9.453433e-02f, 9.151994e-02f,  8.850211e-02f,
    8.548094e-02f, 8.245656e-02f, 7.942907e-02f,  7.639859e-02f,
    7.336524e-02f, 7.032912e-02f, 6.729035e-02f,  6.424906e-02f,
    6.120534e-02f, 5.815932e-02f, 5.511110e-02f,  5.206082e-02f,
    4.900857e-02f, 4.595448e-02f, 4.289866e-02f,  3.984122e-02f,
    3.678228e-02f, 3.372196e-02f, 3.066037e-02f,  2.759762e-02f,
    2.453384e-02f, 2.146913e-02f, 1.840361e-02f,  1.533740e-02f,
    1.227061e-02f, 9.203365e-03f, 6.135769e-03f,  3.067942e-03f};
static const int AUP_FFTW_g_ip2048[] = {512, 512, 0,   16,  0,  64,  32,  96,
                                        0,   256, 128, 384, 64, 320, 192, 448};
static const float AUP_FFTW_g_w2048[] = {
    1.000000e+00f, 7.071068e-01f, 5.000094e-01f,  5.000847e-01f,
    9.999247e-01f, 1.227154e-02f, 9.993224e-01f,  -3.680722e-02f,
    9.996988e-01f, 2.454123e-02f, 9.972905e-01f,  -7.356456e-02f,
    9.993224e-01f, 3.680722e-02f, 9.939070e-01f,  -1.102222e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.981181e-01f, 6.132074e-02f, 9.831055e-01f,  -1.830399e-01f,
    9.972905e-01f, 7.356456e-02f, 9.757021e-01f,  -2.191012e-01f,
    9.963126e-01f, 8.579731e-02f, 9.669765e-01f,  -2.548657e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.939070e-01f, 1.102222e-01f, 9.456073e-01f,  -3.253103e-01f,
    9.924795e-01f, 1.224107e-01f, 9.329928e-01f,  -3.598950e-01f,
    9.909026e-01f, 1.345807e-01f, 9.191139e-01f,  -3.939920e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.873014e-01f, 1.588581e-01f, 8.876396e-01f,  -4.605387e-01f,
    9.852776e-01f, 1.709619e-01f, 8.700870e-01f,  -4.928982e-01f,
    9.831055e-01f, 1.830399e-01f, 8.513552e-01f,  -5.245897e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.783174e-01f, 2.071114e-01f, 8.104572e-01f,  -5.857979e-01f,
    9.757021e-01f, 2.191012e-01f, 7.883464e-01f,  -6.152316e-01f,
    9.729400e-01f, 2.310581e-01f, 7.651673e-01f,  -6.438315e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.669765e-01f, 2.548657e-01f, 7.157308e-01f,  -6.983762e-01f,
    9.637761e-01f, 2.667128e-01f, 6.895405e-01f,  -7.242471e-01f,
    9.604305e-01f, 2.785197e-01f, 6.624158e-01f,  -7.491364e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.533060e-01f, 3.020059e-01f, 6.055110e-01f,  -7.958369e-01f,
    9.495282e-01f, 3.136817e-01f, 5.758082e-01f,  -8.175848e-01f,
    9.456073e-01f, 3.253103e-01f, 5.453250e-01f,  -8.382247e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.373390e-01f, 3.484187e-01f, 4.821838e-01f,  -8.760701e-01f,
    9.329928e-01f, 3.598950e-01f, 4.496113e-01f,  -8.932243e-01f,
    9.285061e-01f, 3.713172e-01f, 4.164296e-01f,  -9.091680e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.191139e-01f, 3.939920e-01f, 3.484187e-01f,  -9.373390e-01f,
    9.142098e-01f, 4.052413e-01f, 3.136817e-01f,  -9.495282e-01f,
    9.091680e-01f, 4.164296e-01f, 2.785197e-01f,  -9.604305e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.986745e-01f, 4.386162e-01f, 2.071114e-01f,  -9.783174e-01f,
    8.932243e-01f, 4.496113e-01f, 1.709619e-01f,  -9.852776e-01f,
    8.876396e-01f, 4.605387e-01f, 1.345807e-01f,  -9.909026e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.760701e-01f, 4.821838e-01f, 6.132074e-02f,  -9.981181e-01f,
    8.700870e-01f, 4.928982e-01f, 2.454123e-02f,  -9.996988e-01f,
    8.639729e-01f, 5.035384e-01f, -1.227154e-02f, -9.999247e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.513552e-01f, 5.245897e-01f, -8.579731e-02f, -9.963126e-01f,
    8.448536e-01f, 5.349976e-01f, -1.224107e-01f, -9.924795e-01f,
    8.382247e-01f, 5.453250e-01f, -1.588581e-01f, -9.873014e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.245893e-01f, 5.657318e-01f, -2.310581e-01f, -9.729400e-01f,
    8.175848e-01f, 5.758082e-01f, -2.667128e-01f, -9.637761e-01f,
    8.104572e-01f, 5.857979e-01f, -3.020059e-01f, -9.533060e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.958369e-01f, 6.055110e-01f, -3.713172e-01f, -9.285061e-01f,
    7.883464e-01f, 6.152316e-01f, -4.052413e-01f, -9.142098e-01f,
    7.807372e-01f, 6.248595e-01f, -4.386162e-01f, -8.986745e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.651673e-01f, 6.438315e-01f, -5.035384e-01f, -8.639729e-01f,
    7.572088e-01f, 6.531728e-01f, -5.349976e-01f, -8.448536e-01f,
    7.491364e-01f, 6.624158e-01f, -5.657318e-01f, -8.245893e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    7.326543e-01f, 6.806010e-01f, -6.248595e-01f, -7.807372e-01f,
    7.242471e-01f, 6.895405e-01f, -6.531728e-01f, -7.572088e-01f,
    7.157308e-01f, 6.983762e-01f, -6.806010e-01f, -7.326543e-01f,
    1.000000e+00f, 7.071068e-01f, 5.000377e-01f,  5.003390e-01f,
    9.996988e-01f, 2.454123e-02f, 9.972905e-01f,  -7.356456e-02f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.972905e-01f, 7.356456e-02f, 9.757021e-01f,  -2.191012e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.924795e-01f, 1.224107e-01f, 9.329928e-01f,  -3.598950e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.852776e-01f, 1.709619e-01f, 8.700870e-01f,  -4.928982e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.757021e-01f, 2.191012e-01f, 7.883464e-01f,  -6.152316e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.637761e-01f, 2.667128e-01f, 6.895405e-01f,  -7.242471e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.495282e-01f, 3.136817e-01f, 5.758082e-01f,  -8.175848e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.329928e-01f, 3.598950e-01f, 4.496113e-01f,  -8.932243e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.142098e-01f, 4.052413e-01f, 3.136817e-01f,  -9.495282e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.932243e-01f, 4.496113e-01f, 1.709619e-01f,  -9.852776e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.700870e-01f, 4.928982e-01f, 2.454123e-02f,  -9.996988e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.448536e-01f, 5.349976e-01f, -1.224107e-01f, -9.924795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.175848e-01f, 5.758082e-01f, -2.667128e-01f, -9.637761e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.883464e-01f, 6.152316e-01f, -4.052413e-01f, -9.142098e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.572088e-01f, 6.531728e-01f, -5.349976e-01f, -8.448536e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    7.242471e-01f, 6.895405e-01f, -6.531728e-01f, -7.572088e-01f,
    1.000000e+00f, 7.071068e-01f, 5.001506e-01f,  5.013585e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    1.000000e+00f, 7.071068e-01f, 5.006030e-01f,  5.054710e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    1.000000e+00f, 7.071068e-01f, 5.024193e-01f,  5.224986e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    1.000000e+00f, 7.071068e-01f, 5.097956e-01f,  6.013449e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    1.000000e+00f, 7.071068e-01f, 9.238795e-01f,  3.826834e-01f,
    1.000000e+00f, 7.071068e-01f, 0.000000e+00f,  0.000000e+00f,
    7.071068e-01f, 4.999976e-01f, 4.999906e-01f,  4.999788e-01f,
    4.999624e-01f, 4.999412e-01f, 4.999153e-01f,  4.998847e-01f,
    4.998494e-01f, 4.998094e-01f, 4.997647e-01f,  4.997153e-01f,
    4.996612e-01f, 4.996024e-01f, 4.995389e-01f,  4.994706e-01f,
    4.993977e-01f, 4.993201e-01f, 4.992378e-01f,  4.991508e-01f,
    4.990591e-01f, 4.989626e-01f, 4.988615e-01f,  4.987557e-01f,
    4.986452e-01f, 4.985300e-01f, 4.984101e-01f,  4.982856e-01f,
    4.981563e-01f, 4.980224e-01f, 4.978837e-01f,  4.977404e-01f,
    4.975924e-01f, 4.974397e-01f, 4.972823e-01f,  4.971202e-01f,
    4.969535e-01f, 4.967821e-01f, 4.966060e-01f,  4.964252e-01f,
    4.962398e-01f, 4.960497e-01f, 4.958549e-01f,  4.956554e-01f,
    4.954513e-01f, 4.952425e-01f, 4.950291e-01f,  4.948110e-01f,
    4.945883e-01f, 4.943608e-01f, 4.941288e-01f,  4.938921e-01f,
    4.936507e-01f, 4.934047e-01f, 4.931540e-01f,  4.928988e-01f,
    4.926388e-01f, 4.923743e-01f, 4.921050e-01f,  4.918312e-01f,
    4.915527e-01f, 4.912697e-01f, 4.909819e-01f,  4.906896e-01f,
    4.903926e-01f, 4.900911e-01f, 4.897849e-01f,  4.894741e-01f,
    4.891587e-01f, 4.888387e-01f, 4.885141e-01f,  4.881849e-01f,
    4.878511e-01f, 4.875127e-01f, 4.871697e-01f,  4.868221e-01f,
    4.864700e-01f, 4.861132e-01f, 4.857519e-01f,  4.853861e-01f,
    4.850156e-01f, 4.846406e-01f, 4.842610e-01f,  4.838769e-01f,
    4.834882e-01f, 4.830950e-01f, 4.826972e-01f,  4.822949e-01f,
    4.818880e-01f, 4.814766e-01f, 4.810607e-01f,  4.806402e-01f,
    4.802153e-01f, 4.797858e-01f, 4.793517e-01f,  4.789132e-01f,
    4.784702e-01f, 4.780226e-01f, 4.775706e-01f,  4.771140e-01f,
    4.766530e-01f, 4.761875e-01f, 4.757175e-01f,  4.752430e-01f,
    4.747641e-01f, 4.742807e-01f, 4.737928e-01f,  4.733005e-01f,
    4.728037e-01f, 4.723024e-01f, 4.717967e-01f,  4.712866e-01f,
    4.707720e-01f, 4.702530e-01f, 4.697296e-01f,  4.692018e-01f,
    4.686695e-01f, 4.681328e-01f, 4.675918e-01f,  4.670463e-01f,
    4.664964e-01f, 4.659421e-01f, 4.653835e-01f,  4.648204e-01f,
    4.642530e-01f, 4.636813e-01f, 4.631051e-01f,  4.625246e-01f,
    4.619398e-01f, 4.613506e-01f, 4.607570e-01f,  4.601591e-01f,
    4.595569e-01f, 4.589504e-01f, 4.583395e-01f,  4.577244e-01f,
    4.571049e-01f, 4.564811e-01f, 4.558530e-01f,  4.552206e-01f,
    4.545840e-01f, 4.539431e-01f, 4.532979e-01f,  4.526484e-01f,
    4.519946e-01f, 4.513367e-01f, 4.506744e-01f,  4.500079e-01f,
    4.493372e-01f, 4.486623e-01f, 4.479831e-01f,  4.472997e-01f,
    4.466122e-01f, 4.459204e-01f, 4.452244e-01f,  4.445242e-01f,
    4.438198e-01f, 4.431113e-01f, 4.423985e-01f,  4.416817e-01f,
    4.409606e-01f, 4.402354e-01f, 4.395061e-01f,  4.387726e-01f,
    4.380350e-01f, 4.372933e-01f, 4.365475e-01f,  4.357975e-01f,
    4.350435e-01f, 4.342854e-01f, 4.335231e-01f,  4.327568e-01f,
    4.319864e-01f, 4.312120e-01f, 4.304335e-01f,  4.296509e-01f,
    4.288643e-01f, 4.280737e-01f, 4.272790e-01f,  4.264803e-01f,
    4.256776e-01f, 4.248709e-01f, 4.240602e-01f,  4.232455e-01f,
    4.224268e-01f, 4.216041e-01f, 4.207775e-01f,  4.199469e-01f,
    4.191124e-01f, 4.182739e-01f, 4.174314e-01f,  4.165851e-01f,
    4.157348e-01f, 4.148806e-01f, 4.140225e-01f,  4.131605e-01f,
    4.122947e-01f, 4.114249e-01f, 4.105513e-01f,  4.096738e-01f,
    4.087924e-01f, 4.079072e-01f, 4.070182e-01f,  4.061253e-01f,
    4.052286e-01f, 4.043281e-01f, 4.034238e-01f,  4.025157e-01f,
    4.016038e-01f, 4.006881e-01f, 3.997686e-01f,  3.988454e-01f,
    3.979185e-01f, 3.969877e-01f, 3.960533e-01f,  3.951151e-01f,
    3.941732e-01f, 3.932276e-01f, 3.922783e-01f,  3.913253e-01f,
    3.903686e-01f, 3.894083e-01f, 3.884442e-01f,  3.874766e-01f,
    3.865052e-01f, 3.855303e-01f, 3.845517e-01f,  3.835695e-01f,
    3.825836e-01f, 3.815942e-01f, 3.806012e-01f,  3.796046e-01f,
    3.786044e-01f, 3.776007e-01f, 3.765934e-01f,  3.755826e-01f,
    3.745682e-01f, 3.735503e-01f, 3.725289e-01f,  3.715040e-01f,
    3.704756e-01f, 3.694437e-01f, 3.684083e-01f,  3.673694e-01f,
    3.663271e-01f, 3.652814e-01f, 3.642322e-01f,  3.631796e-01f,
    3.621235e-01f, 3.610641e-01f, 3.600013e-01f,  3.589350e-01f,
    3.578654e-01f, 3.567924e-01f, 3.557161e-01f,  3.546364e-01f,
    3.535534e-01f, 3.524670e-01f, 3.513774e-01f,  3.502844e-01f,
    3.491881e-01f, 3.480886e-01f, 3.469857e-01f,  3.458796e-01f,
    3.447703e-01f, 3.436577e-01f, 3.425418e-01f,  3.414228e-01f,
    3.403005e-01f, 3.391750e-01f, 3.380464e-01f,  3.369145e-01f,
    3.357795e-01f, 3.346413e-01f, 3.335000e-01f,  3.323555e-01f,
    3.312079e-01f, 3.300572e-01f, 3.289033e-01f,  3.277464e-01f,
    3.265864e-01f, 3.254233e-01f, 3.242572e-01f,  3.230880e-01f,
    3.219158e-01f, 3.207405e-01f, 3.195622e-01f,  3.183809e-01f,
    3.171966e-01f, 3.160094e-01f, 3.148191e-01f,  3.136259e-01f,
    3.124297e-01f, 3.112306e-01f, 3.100286e-01f,  3.088237e-01f,
    3.076158e-01f, 3.064050e-01f, 3.051914e-01f,  3.039749e-01f,
    3.027555e-01f, 3.015333e-01f, 3.003082e-01f,  2.990804e-01f,
    2.978497e-01f, 2.966161e-01f, 2.953799e-01f,  2.941408e-01f,
    2.928989e-01f, 2.916543e-01f, 2.904070e-01f,  2.891569e-01f,
    2.879041e-01f, 2.866486e-01f, 2.853904e-01f,  2.841295e-01f,
    2.828659e-01f, 2.815997e-01f, 2.803308e-01f,  2.790593e-01f,
    2.777851e-01f, 2.765084e-01f, 2.752290e-01f,  2.739470e-01f,
    2.726625e-01f, 2.713754e-01f, 2.700857e-01f,  2.687935e-01f,
    2.674988e-01f, 2.662016e-01f, 2.649018e-01f,  2.635996e-01f,
    2.622948e-01f, 2.609876e-01f, 2.596780e-01f,  2.583659e-01f,
    2.570514e-01f, 2.557344e-01f, 2.544151e-01f,  2.530933e-01f,
    2.517692e-01f, 2.504427e-01f, 2.491138e-01f,  2.477826e-01f,
    2.464491e-01f, 2.451132e-01f, 2.437751e-01f,  2.424346e-01f,
    2.410919e-01f, 2.397469e-01f, 2.383996e-01f,  2.370501e-01f,
    2.356984e-01f, 2.343444e-01f, 2.329882e-01f,  2.316299e-01f,
    2.302694e-01f, 2.289067e-01f, 2.275418e-01f,  2.261748e-01f,
    2.248057e-01f, 2.234344e-01f, 2.220611e-01f,  2.206856e-01f,
    2.193081e-01f, 2.179285e-01f, 2.165469e-01f,  2.151632e-01f,
    2.137775e-01f, 2.123898e-01f, 2.110001e-01f,  2.096084e-01f,
    2.082148e-01f, 2.068192e-01f, 2.054216e-01f,  2.040221e-01f,
    2.026207e-01f, 2.012173e-01f, 1.998121e-01f,  1.984050e-01f,
    1.969960e-01f, 1.955852e-01f, 1.941725e-01f,  1.927580e-01f,
    1.913417e-01f, 1.899236e-01f, 1.885037e-01f,  1.870820e-01f,
    1.856586e-01f, 1.842334e-01f, 1.828065e-01f,  1.813779e-01f,
    1.799475e-01f, 1.785155e-01f, 1.770818e-01f,  1.756464e-01f,
    1.742093e-01f, 1.727707e-01f, 1.713304e-01f,  1.698884e-01f,
    1.684449e-01f, 1.669998e-01f, 1.655532e-01f,  1.641049e-01f,
    1.626551e-01f, 1.612038e-01f, 1.597510e-01f,  1.582967e-01f,
    1.568409e-01f, 1.553836e-01f, 1.539248e-01f,  1.524646e-01f,
    1.510030e-01f, 1.495399e-01f, 1.480754e-01f,  1.466096e-01f,
    1.451423e-01f, 1.436737e-01f, 1.422038e-01f,  1.407325e-01f,
    1.392598e-01f, 1.377859e-01f, 1.363107e-01f,  1.348342e-01f,
    1.333564e-01f, 1.318773e-01f, 1.303971e-01f,  1.289156e-01f,
    1.274328e-01f, 1.259489e-01f, 1.244638e-01f,  1.229775e-01f,
    1.214901e-01f, 1.200015e-01f, 1.185118e-01f,  1.170210e-01f,
    1.155291e-01f, 1.140360e-01f, 1.125420e-01f,  1.110468e-01f,
    1.095506e-01f, 1.080534e-01f, 1.065552e-01f,  1.050559e-01f,
    1.035557e-01f, 1.020545e-01f, 1.005523e-01f,  9.904921e-02f,
    9.754516e-02f, 9.604020e-02f, 9.453433e-02f,  9.302758e-02f,
    9.151994e-02f, 9.001145e-02f, 8.850211e-02f,  8.699194e-02f,
    8.548094e-02f, 8.396915e-02f, 8.245656e-02f,  8.094320e-02f,
    7.942907e-02f, 7.791420e-02f, 7.639859e-02f,  7.488227e-02f,
    7.336524e-02f, 7.184752e-02f, 7.032912e-02f,  6.881006e-02f,
    6.729035e-02f, 6.577001e-02f, 6.424906e-02f,  6.272749e-02f,
    6.120534e-02f, 5.968261e-02f, 5.815932e-02f,  5.663548e-02f,
    5.511110e-02f, 5.358621e-02f, 5.206082e-02f,  5.053493e-02f,
    4.900857e-02f, 4.748175e-02f, 4.595448e-02f,  4.442678e-02f,
    4.289866e-02f, 4.137013e-02f, 3.984122e-02f,  3.831193e-02f,
    3.678228e-02f, 3.525229e-02f, 3.372196e-02f,  3.219132e-02f,
    3.066037e-02f, 2.912913e-02f, 2.759762e-02f,  2.606585e-02f,
    2.453384e-02f, 2.300159e-02f, 2.146913e-02f,  1.993646e-02f,
    1.840361e-02f, 1.687059e-02f, 1.533740e-02f,  1.380407e-02f,
    1.227061e-02f, 1.073704e-02f, 9.203365e-03f,  7.669603e-03f,
    6.135769e-03f, 4.601877e-03f, 3.067942e-03f,  1.533978e-03f};
static const int AUP_FFTW_g_ip4096[] = {
    1024, 1024, 0,   16,   0,   64,   32,   96,   0,    256, 128,
    384,  64,   320, 192,  448, 0,    1024, 512,  1536, 256, 1280,
    768,  1792, 128, 1152, 640, 1664, 384,  1408, 896,  1920};
static const float AUP_FFTW_g_w4096[] = {
    1.000000e+00f, 7.071068e-01f, 5.000024e-01f,  5.000212e-01f,
    9.999812e-01f, 6.135885e-03f, 9.998306e-01f,  -1.840673e-02f,
    9.999247e-01f, 1.227154e-02f, 9.993224e-01f,  -3.680722e-02f,
    9.998306e-01f, 1.840673e-02f, 9.984756e-01f,  -5.519524e-02f,
    9.996988e-01f, 2.454123e-02f, 9.972905e-01f,  -7.356456e-02f,
    9.995294e-01f, 3.067480e-02f, 9.957674e-01f,  -9.190896e-02f,
    9.993224e-01f, 3.680722e-02f, 9.939070e-01f,  -1.102222e-01f,
    9.990777e-01f, 4.293826e-02f, 9.917098e-01f,  -1.284981e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.984756e-01f, 5.519524e-02f, 9.863081e-01f,  -1.649131e-01f,
    9.981181e-01f, 6.132074e-02f, 9.831055e-01f,  -1.830399e-01f,
    9.977231e-01f, 6.744392e-02f, 9.795698e-01f,  -2.011046e-01f,
    9.972905e-01f, 7.356456e-02f, 9.757021e-01f,  -2.191012e-01f,
    9.968203e-01f, 7.968244e-02f, 9.715039e-01f,  -2.370236e-01f,
    9.963126e-01f, 8.579731e-02f, 9.669765e-01f,  -2.548657e-01f,
    9.957674e-01f, 9.190896e-02f, 9.621214e-01f,  -2.726214e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.945646e-01f, 1.041216e-01f, 9.514350e-01f,  -3.078496e-01f,
    9.939070e-01f, 1.102222e-01f, 9.456073e-01f,  -3.253103e-01f,
    9.932119e-01f, 1.163186e-01f, 9.394592e-01f,  -3.426607e-01f,
    9.924795e-01f, 1.224107e-01f, 9.329928e-01f,  -3.598950e-01f,
    9.917098e-01f, 1.284981e-01f, 9.262102e-01f,  -3.770074e-01f,
    9.909026e-01f, 1.345807e-01f, 9.191139e-01f,  -3.939920e-01f,
    9.900582e-01f, 1.406582e-01f, 9.117060e-01f,  -4.108432e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.882576e-01f, 1.527972e-01f, 8.959662e-01f,  -4.441221e-01f,
    9.873014e-01f, 1.588581e-01f, 8.876396e-01f,  -4.605387e-01f,
    9.863081e-01f, 1.649131e-01f, 8.790122e-01f,  -4.767992e-01f,
    9.852776e-01f, 1.709619e-01f, 8.700870e-01f,  -4.928982e-01f,
    9.842101e-01f, 1.770042e-01f, 8.608669e-01f,  -5.088301e-01f,
    9.831055e-01f, 1.830399e-01f, 8.513552e-01f,  -5.245897e-01f,
    9.819639e-01f, 1.890687e-01f, 8.415550e-01f,  -5.401715e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.795698e-01f, 2.011046e-01f, 8.211025e-01f,  -5.707807e-01f,
    9.783174e-01f, 2.071114e-01f, 8.104572e-01f,  -5.857979e-01f,
    9.770281e-01f, 2.131103e-01f, 7.995373e-01f,  -6.006165e-01f,
    9.757021e-01f, 2.191012e-01f, 7.883464e-01f,  -6.152316e-01f,
    9.743394e-01f, 2.250839e-01f, 7.768885e-01f,  -6.296382e-01f,
    9.729400e-01f, 2.310581e-01f, 7.651673e-01f,  -6.438315e-01f,
    9.715039e-01f, 2.370236e-01f, 7.531868e-01f,  -6.578067e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.685221e-01f, 2.489276e-01f, 7.284644e-01f,  -6.850837e-01f,
    9.669765e-01f, 2.548657e-01f, 7.157308e-01f,  -6.983762e-01f,
    9.653944e-01f, 2.607941e-01f, 7.027547e-01f,  -7.114322e-01f,
    9.637761e-01f, 2.667128e-01f, 6.895405e-01f,  -7.242471e-01f,
    9.621214e-01f, 2.726214e-01f, 6.760927e-01f,  -7.368166e-01f,
    9.604305e-01f, 2.785197e-01f, 6.624158e-01f,  -7.491364e-01f,
    9.587035e-01f, 2.844075e-01f, 6.485144e-01f,  -7.612024e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.551412e-01f, 2.961509e-01f, 6.200572e-01f,  -7.845566e-01f,
    9.533060e-01f, 3.020059e-01f, 6.055110e-01f,  -7.958369e-01f,
    9.514350e-01f, 3.078496e-01f, 5.907597e-01f,  -8.068476e-01f,
    9.495282e-01f, 3.136817e-01f, 5.758082e-01f,  -8.175848e-01f,
    9.475856e-01f, 3.195020e-01f, 5.606616e-01f,  -8.280450e-01f,
    9.456073e-01f, 3.253103e-01f, 5.453250e-01f,  -8.382247e-01f,
    9.435935e-01f, 3.311063e-01f, 5.298036e-01f,  -8.481203e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.394592e-01f, 3.426607e-01f, 4.982277e-01f,  -8.670462e-01f,
    9.373390e-01f, 3.484187e-01f, 4.821838e-01f,  -8.760701e-01f,
    9.351835e-01f, 3.541635e-01f, 4.659765e-01f,  -8.847971e-01f,
    9.329928e-01f, 3.598950e-01f, 4.496113e-01f,  -8.932243e-01f,
    9.307670e-01f, 3.656130e-01f, 4.330938e-01f,  -9.013488e-01f,
    9.285061e-01f, 3.713172e-01f, 4.164296e-01f,  -9.091680e-01f,
    9.262102e-01f, 3.770074e-01f, 3.996242e-01f,  -9.166791e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.215140e-01f, 3.883450e-01f, 3.656130e-01f,  -9.307670e-01f,
    9.191139e-01f, 3.939920e-01f, 3.484187e-01f,  -9.373390e-01f,
    9.166791e-01f, 3.996242e-01f, 3.311063e-01f,  -9.435935e-01f,
    9.142098e-01f, 4.052413e-01f, 3.136817e-01f,  -9.495282e-01f,
    9.117060e-01f, 4.108432e-01f, 2.961509e-01f,  -9.551412e-01f,
    9.091680e-01f, 4.164296e-01f, 2.785197e-01f,  -9.604305e-01f,
    9.065957e-01f, 4.220003e-01f, 2.607941e-01f,  -9.653944e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    9.013488e-01f, 4.330938e-01f, 2.250839e-01f,  -9.743394e-01f,
    8.986745e-01f, 4.386162e-01f, 2.071114e-01f,  -9.783174e-01f,
    8.959662e-01f, 4.441221e-01f, 1.890687e-01f,  -9.819639e-01f,
    8.932243e-01f, 4.496113e-01f, 1.709619e-01f,  -9.852776e-01f,
    8.904487e-01f, 4.550836e-01f, 1.527972e-01f,  -9.882576e-01f,
    8.876396e-01f, 4.605387e-01f, 1.345807e-01f,  -9.909026e-01f,
    8.847971e-01f, 4.659765e-01f, 1.163186e-01f,  -9.932119e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.790122e-01f, 4.767992e-01f, 7.968244e-02f,  -9.968203e-01f,
    8.760701e-01f, 4.821838e-01f, 6.132074e-02f,  -9.981181e-01f,
    8.730950e-01f, 4.875502e-01f, 4.293826e-02f,  -9.990777e-01f,
    8.700870e-01f, 4.928982e-01f, 2.454123e-02f,  -9.996988e-01f,
    8.670462e-01f, 4.982277e-01f, 6.135885e-03f,  -9.999812e-01f,
    8.639729e-01f, 5.035384e-01f, -1.227154e-02f, -9.999247e-01f,
    8.608669e-01f, 5.088301e-01f, -3.067480e-02f, -9.995294e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.545580e-01f, 5.193560e-01f, -6.744392e-02f, -9.977231e-01f,
    8.513552e-01f, 5.245897e-01f, -8.579731e-02f, -9.963126e-01f,
    8.481203e-01f, 5.298036e-01f, -1.041216e-01f, -9.945646e-01f,
    8.448536e-01f, 5.349976e-01f, -1.224107e-01f, -9.924795e-01f,
    8.415550e-01f, 5.401715e-01f, -1.406582e-01f, -9.900582e-01f,
    8.382247e-01f, 5.453250e-01f, -1.588581e-01f, -9.873014e-01f,
    8.348629e-01f, 5.504580e-01f, -1.770042e-01f, -9.842101e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.280450e-01f, 5.606616e-01f, -2.131103e-01f, -9.770281e-01f,
    8.245893e-01f, 5.657318e-01f, -2.310581e-01f, -9.729400e-01f,
    8.211025e-01f, 5.707807e-01f, -2.489276e-01f, -9.685221e-01f,
    8.175848e-01f, 5.758082e-01f, -2.667128e-01f, -9.637761e-01f,
    8.140363e-01f, 5.808140e-01f, -2.844075e-01f, -9.587035e-01f,
    8.104572e-01f, 5.857979e-01f, -3.020059e-01f, -9.533060e-01f,
    8.068476e-01f, 5.907597e-01f, -3.195020e-01f, -9.475856e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.995373e-01f, 6.006165e-01f, -3.541635e-01f, -9.351835e-01f,
    7.958369e-01f, 6.055110e-01f, -3.713172e-01f, -9.285061e-01f,
    7.921066e-01f, 6.103828e-01f, -3.883450e-01f, -9.215140e-01f,
    7.883464e-01f, 6.152316e-01f, -4.052413e-01f, -9.142098e-01f,
    7.845566e-01f, 6.200572e-01f, -4.220003e-01f, -9.065957e-01f,
    7.807372e-01f, 6.248595e-01f, -4.386162e-01f, -8.986745e-01f,
    7.768885e-01f, 6.296382e-01f, -4.550836e-01f, -8.904487e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.691033e-01f, 6.391244e-01f, -4.875502e-01f, -8.730950e-01f,
    7.651673e-01f, 6.438315e-01f, -5.035384e-01f, -8.639729e-01f,
    7.612024e-01f, 6.485144e-01f, -5.193560e-01f, -8.545580e-01f,
    7.572088e-01f, 6.531728e-01f, -5.349976e-01f, -8.448536e-01f,
    7.531868e-01f, 6.578067e-01f, -5.504580e-01f, -8.348629e-01f,
    7.491364e-01f, 6.624158e-01f, -5.657318e-01f, -8.245893e-01f,
    7.450578e-01f, 6.669999e-01f, -5.808140e-01f, -8.140363e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    7.368166e-01f, 6.760927e-01f, -6.103828e-01f, -7.921066e-01f,
    7.326543e-01f, 6.806010e-01f, -6.248595e-01f, -7.807372e-01f,
    7.284644e-01f, 6.850837e-01f, -6.391244e-01f, -7.691033e-01f,
    7.242471e-01f, 6.895405e-01f, -6.531728e-01f, -7.572088e-01f,
    7.200025e-01f, 6.939715e-01f, -6.669999e-01f, -7.450578e-01f,
    7.157308e-01f, 6.983762e-01f, -6.806010e-01f, -7.326543e-01f,
    7.114322e-01f, 7.027547e-01f, -6.939715e-01f, -7.200025e-01f,
    1.000000e+00f, 7.071068e-01f, 5.000094e-01f,  5.000847e-01f,
    9.999247e-01f, 1.227154e-02f, 9.993224e-01f,  -3.680722e-02f,
    9.996988e-01f, 2.454123e-02f, 9.972905e-01f,  -7.356456e-02f,
    9.993224e-01f, 3.680722e-02f, 9.939070e-01f,  -1.102222e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.981181e-01f, 6.132074e-02f, 9.831055e-01f,  -1.830399e-01f,
    9.972905e-01f, 7.356456e-02f, 9.757021e-01f,  -2.191012e-01f,
    9.963126e-01f, 8.579731e-02f, 9.669765e-01f,  -2.548657e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.939070e-01f, 1.102222e-01f, 9.456073e-01f,  -3.253103e-01f,
    9.924795e-01f, 1.224107e-01f, 9.329928e-01f,  -3.598950e-01f,
    9.909026e-01f, 1.345807e-01f, 9.191139e-01f,  -3.939920e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.873014e-01f, 1.588581e-01f, 8.876396e-01f,  -4.605387e-01f,
    9.852776e-01f, 1.709619e-01f, 8.700870e-01f,  -4.928982e-01f,
    9.831055e-01f, 1.830399e-01f, 8.513552e-01f,  -5.245897e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.783174e-01f, 2.071114e-01f, 8.104572e-01f,  -5.857979e-01f,
    9.757021e-01f, 2.191012e-01f, 7.883464e-01f,  -6.152316e-01f,
    9.729400e-01f, 2.310581e-01f, 7.651673e-01f,  -6.438315e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.669765e-01f, 2.548657e-01f, 7.157308e-01f,  -6.983762e-01f,
    9.637761e-01f, 2.667128e-01f, 6.895405e-01f,  -7.242471e-01f,
    9.604305e-01f, 2.785197e-01f, 6.624158e-01f,  -7.491364e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.533060e-01f, 3.020059e-01f, 6.055110e-01f,  -7.958369e-01f,
    9.495282e-01f, 3.136817e-01f, 5.758082e-01f,  -8.175848e-01f,
    9.456073e-01f, 3.253103e-01f, 5.453250e-01f,  -8.382247e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.373390e-01f, 3.484187e-01f, 4.821838e-01f,  -8.760701e-01f,
    9.329928e-01f, 3.598950e-01f, 4.496113e-01f,  -8.932243e-01f,
    9.285061e-01f, 3.713172e-01f, 4.164296e-01f,  -9.091680e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.191139e-01f, 3.939920e-01f, 3.484187e-01f,  -9.373390e-01f,
    9.142098e-01f, 4.052413e-01f, 3.136817e-01f,  -9.495282e-01f,
    9.091680e-01f, 4.164296e-01f, 2.785197e-01f,  -9.604305e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.986745e-01f, 4.386162e-01f, 2.071114e-01f,  -9.783174e-01f,
    8.932243e-01f, 4.496113e-01f, 1.709619e-01f,  -9.852776e-01f,
    8.876396e-01f, 4.605387e-01f, 1.345807e-01f,  -9.909026e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.760701e-01f, 4.821838e-01f, 6.132074e-02f,  -9.981181e-01f,
    8.700870e-01f, 4.928982e-01f, 2.454123e-02f,  -9.996988e-01f,
    8.639729e-01f, 5.035384e-01f, -1.227154e-02f, -9.999247e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.513552e-01f, 5.245897e-01f, -8.579731e-02f, -9.963126e-01f,
    8.448536e-01f, 5.349976e-01f, -1.224107e-01f, -9.924795e-01f,
    8.382247e-01f, 5.453250e-01f, -1.588581e-01f, -9.873014e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.245893e-01f, 5.657318e-01f, -2.310581e-01f, -9.729400e-01f,
    8.175848e-01f, 5.758082e-01f, -2.667128e-01f, -9.637761e-01f,
    8.104572e-01f, 5.857979e-01f, -3.020059e-01f, -9.533060e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.958369e-01f, 6.055110e-01f, -3.713172e-01f, -9.285061e-01f,
    7.883464e-01f, 6.152316e-01f, -4.052413e-01f, -9.142098e-01f,
    7.807372e-01f, 6.248595e-01f, -4.386162e-01f, -8.986745e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.651673e-01f, 6.438315e-01f, -5.035384e-01f, -8.639729e-01f,
    7.572088e-01f, 6.531728e-01f, -5.349976e-01f, -8.448536e-01f,
    7.491364e-01f, 6.624158e-01f, -5.657318e-01f, -8.245893e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    7.326543e-01f, 6.806010e-01f, -6.248595e-01f, -7.807372e-01f,
    7.242471e-01f, 6.895405e-01f, -6.531728e-01f, -7.572088e-01f,
    7.157308e-01f, 6.983762e-01f, -6.806010e-01f, -7.326543e-01f,
    1.000000e+00f, 7.071068e-01f, 5.000377e-01f,  5.003390e-01f,
    9.996988e-01f, 2.454123e-02f, 9.972905e-01f,  -7.356456e-02f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.972905e-01f, 7.356456e-02f, 9.757021e-01f,  -2.191012e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.924795e-01f, 1.224107e-01f, 9.329928e-01f,  -3.598950e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.852776e-01f, 1.709619e-01f, 8.700870e-01f,  -4.928982e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.757021e-01f, 2.191012e-01f, 7.883464e-01f,  -6.152316e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.637761e-01f, 2.667128e-01f, 6.895405e-01f,  -7.242471e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.495282e-01f, 3.136817e-01f, 5.758082e-01f,  -8.175848e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.329928e-01f, 3.598950e-01f, 4.496113e-01f,  -8.932243e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.142098e-01f, 4.052413e-01f, 3.136817e-01f,  -9.495282e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.932243e-01f, 4.496113e-01f, 1.709619e-01f,  -9.852776e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.700870e-01f, 4.928982e-01f, 2.454123e-02f,  -9.996988e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.448536e-01f, 5.349976e-01f, -1.224107e-01f, -9.924795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.175848e-01f, 5.758082e-01f, -2.667128e-01f, -9.637761e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.883464e-01f, 6.152316e-01f, -4.052413e-01f, -9.142098e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.572088e-01f, 6.531728e-01f, -5.349976e-01f, -8.448536e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    7.242471e-01f, 6.895405e-01f, -6.531728e-01f, -7.572088e-01f,
    1.000000e+00f, 7.071068e-01f, 5.001506e-01f,  5.013585e-01f,
    9.987955e-01f, 4.906767e-02f, 9.891765e-01f,  -1.467305e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.891765e-01f, 1.467305e-01f, 9.039893e-01f,  -4.275551e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.700313e-01f, 2.429802e-01f, 7.409511e-01f,  -6.715590e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.415441e-01f, 3.368899e-01f, 5.141027e-01f,  -8.577286e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    9.039893e-01f, 4.275551e-01f, 2.429802e-01f,  -9.700313e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.577286e-01f, 5.141027e-01f, -4.906767e-02f, -9.987955e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    8.032075e-01f, 5.956993e-01f, -3.368899e-01f, -9.415441e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    7.409511e-01f, 6.715590e-01f, -5.956993e-01f, -8.032075e-01f,
    1.000000e+00f, 7.071068e-01f, 5.006030e-01f,  5.054710e-01f,
    9.951847e-01f, 9.801714e-02f, 9.569403e-01f,  -2.902847e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.569403e-01f, 2.902847e-01f, 6.343933e-01f,  -7.730105e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.819213e-01f, 4.713967e-01f, 9.801714e-02f,  -9.951847e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    7.730105e-01f, 6.343933e-01f, -4.713967e-01f, -8.819213e-01f,
    1.000000e+00f, 7.071068e-01f, 5.024193e-01f,  5.224986e-01f,
    9.807853e-01f, 1.950903e-01f, 8.314696e-01f,  -5.555702e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    8.314696e-01f, 5.555702e-01f, -1.950903e-01f, -9.807853e-01f,
    1.000000e+00f, 7.071068e-01f, 5.097956e-01f,  6.013449e-01f,
    9.238795e-01f, 3.826834e-01f, 3.826834e-01f,  -9.238795e-01f,
    1.000000e+00f, 7.071068e-01f, 9.238795e-01f,  3.826834e-01f,
    1.000000e+00f, 7.071068e-01f, 0.000000e+00f,  0.000000e+00f,
    7.071068e-01f, 4.999994e-01f, 4.999976e-01f,  4.999947e-01f,
    4.999906e-01f, 4.999853e-01f, 4.999788e-01f,  4.999712e-01f,
    4.999624e-01f, 4.999524e-01f, 4.999412e-01f,  4.999288e-01f,
    4.999153e-01f, 4.999006e-01f, 4.998847e-01f,  4.998676e-01f,
    4.998494e-01f, 4.998300e-01f, 4.998094e-01f,  4.997876e-01f,
    4.997647e-01f, 4.997406e-01f, 4.997153e-01f,  4.996888e-01f,
    4.996612e-01f, 4.996324e-01f, 4.996024e-01f,  4.995712e-01f,
    4.995389e-01f, 4.995053e-01f, 4.994706e-01f,  4.994348e-01f,
    4.993977e-01f, 4.993595e-01f, 4.993201e-01f,  4.992795e-01f,
    4.992378e-01f, 4.991949e-01f, 4.991508e-01f,  4.991055e-01f,
    4.990591e-01f, 4.990114e-01f, 4.989626e-01f,  4.989127e-01f,
    4.988615e-01f, 4.988092e-01f, 4.987557e-01f,  4.987011e-01f,
    4.986452e-01f, 4.985882e-01f, 4.985300e-01f,  4.984707e-01f,
    4.984101e-01f, 4.983484e-01f, 4.982856e-01f,  4.982215e-01f,
    4.981563e-01f, 4.980899e-01f, 4.980224e-01f,  4.979536e-01f,
    4.978837e-01f, 4.978126e-01f, 4.977404e-01f,  4.976670e-01f,
    4.975924e-01f, 4.975166e-01f, 4.974397e-01f,  4.973616e-01f,
    4.972823e-01f, 4.972018e-01f, 4.971202e-01f,  4.970374e-01f,
    4.969535e-01f, 4.968684e-01f, 4.967821e-01f,  4.966946e-01f,
    4.966060e-01f, 4.965162e-01f, 4.964252e-01f,  4.963331e-01f,
    4.962398e-01f, 4.961453e-01f, 4.960497e-01f,  4.959529e-01f,
    4.958549e-01f, 4.957557e-01f, 4.956554e-01f,  4.955540e-01f,
    4.954513e-01f, 4.953475e-01f, 4.952425e-01f,  4.951364e-01f,
    4.950291e-01f, 4.949206e-01f, 4.948110e-01f,  4.947002e-01f,
    4.945883e-01f, 4.944751e-01f, 4.943608e-01f,  4.942454e-01f,
    4.941288e-01f, 4.940110e-01f, 4.938921e-01f,  4.937720e-01f,
    4.936507e-01f, 4.935283e-01f, 4.934047e-01f,  4.932800e-01f,
    4.931540e-01f, 4.930270e-01f, 4.928988e-01f,  4.927694e-01f,
    4.926388e-01f, 4.925071e-01f, 4.923743e-01f,  4.922402e-01f,
    4.921050e-01f, 4.919687e-01f, 4.918312e-01f,  4.916926e-01f,
    4.915527e-01f, 4.914118e-01f, 4.912697e-01f,  4.911264e-01f,
    4.909819e-01f, 4.908363e-01f, 4.906896e-01f,  4.905417e-01f,
    4.903926e-01f, 4.902424e-01f, 4.900911e-01f,  4.899386e-01f,
    4.897849e-01f, 4.896301e-01f, 4.894741e-01f,  4.893170e-01f,
    4.891587e-01f, 4.889993e-01f, 4.888387e-01f,  4.886770e-01f,
    4.885141e-01f, 4.883500e-01f, 4.881849e-01f,  4.880185e-01f,
    4.878511e-01f, 4.876824e-01f, 4.875127e-01f,  4.873418e-01f,
    4.871697e-01f, 4.869965e-01f, 4.868221e-01f,  4.866466e-01f,
    4.864700e-01f, 4.862922e-01f, 4.861132e-01f,  4.859332e-01f,
    4.857519e-01f, 4.855696e-01f, 4.853861e-01f,  4.852014e-01f,
    4.850156e-01f, 4.848287e-01f, 4.846406e-01f,  4.844514e-01f,
    4.842610e-01f, 4.840696e-01f, 4.838769e-01f,  4.836831e-01f,
    4.834882e-01f, 4.832922e-01f, 4.830950e-01f,  4.828967e-01f,
    4.826972e-01f, 4.824966e-01f, 4.822949e-01f,  4.820920e-01f,
    4.818880e-01f, 4.816829e-01f, 4.814766e-01f,  4.812692e-01f,
    4.810607e-01f, 4.808510e-01f, 4.806402e-01f,  4.804283e-01f,
    4.802153e-01f, 4.800011e-01f, 4.797858e-01f,  4.795693e-01f,
    4.793517e-01f, 4.791330e-01f, 4.789132e-01f,  4.786923e-01f,
    4.784702e-01f, 4.782470e-01f, 4.780226e-01f,  4.777972e-01f,
    4.775706e-01f, 4.773429e-01f, 4.771140e-01f,  4.768841e-01f,
    4.766530e-01f, 4.764208e-01f, 4.761875e-01f,  4.759531e-01f,
    4.757175e-01f, 4.754808e-01f, 4.752430e-01f,  4.750041e-01f,
    4.747641e-01f, 4.745229e-01f, 4.742807e-01f,  4.740373e-01f,
    4.737928e-01f, 4.735472e-01f, 4.733005e-01f,  4.730526e-01f,
    4.728037e-01f, 4.725536e-01f, 4.723024e-01f,  4.720501e-01f,
    4.717967e-01f, 4.715422e-01f, 4.712866e-01f,  4.710299e-01f,
    4.707720e-01f, 4.705131e-01f, 4.702530e-01f,  4.699919e-01f,
    4.697296e-01f, 4.694662e-01f, 4.692018e-01f,  4.689362e-01f,
    4.686695e-01f, 4.684017e-01f, 4.681328e-01f,  4.678628e-01f,
    4.675918e-01f, 4.673196e-01f, 4.670463e-01f,  4.667719e-01f,
    4.664964e-01f, 4.662198e-01f, 4.659421e-01f,  4.656634e-01f,
    4.653835e-01f, 4.651025e-01f, 4.648204e-01f,  4.645373e-01f,
    4.642530e-01f, 4.639677e-01f, 4.636813e-01f,  4.633937e-01f,
    4.631051e-01f, 4.628154e-01f, 4.625246e-01f,  4.622327e-01f,
    4.619398e-01f, 4.616457e-01f, 4.613506e-01f,  4.610543e-01f,
    4.607570e-01f, 4.604586e-01f, 4.601591e-01f,  4.598586e-01f,
    4.595569e-01f, 4.592542e-01f, 4.589504e-01f,  4.586455e-01f,
    4.583395e-01f, 4.580325e-01f, 4.577244e-01f,  4.574152e-01f,
    4.571049e-01f, 4.567935e-01f, 4.564811e-01f,  4.561676e-01f,
    4.558530e-01f, 4.555374e-01f, 4.552206e-01f,  4.549029e-01f,
    4.545840e-01f, 4.542641e-01f, 4.539431e-01f,  4.536210e-01f,
    4.532979e-01f, 4.529736e-01f, 4.526484e-01f,  4.523220e-01f,
    4.519946e-01f, 4.516662e-01f, 4.513367e-01f,  4.510061e-01f,
    4.506744e-01f, 4.503417e-01f, 4.500079e-01f,  4.496731e-01f,
    4.493372e-01f, 4.490003e-01f, 4.486623e-01f,  4.483232e-01f,
    4.479831e-01f, 4.476420e-01f, 4.472997e-01f,  4.469565e-01f,
    4.466122e-01f, 4.462668e-01f, 4.459204e-01f,  4.455729e-01f,
    4.452244e-01f, 4.448748e-01f, 4.445242e-01f,  4.441725e-01f,
    4.438198e-01f, 4.434661e-01f, 4.431113e-01f,  4.427554e-01f,
    4.423985e-01f, 4.420406e-01f, 4.416817e-01f,  4.413217e-01f,
    4.409606e-01f, 4.405986e-01f, 4.402354e-01f,  4.398713e-01f,
    4.395061e-01f, 4.391399e-01f, 4.387726e-01f,  4.384044e-01f,
    4.380350e-01f, 4.376647e-01f, 4.372933e-01f,  4.369209e-01f,
    4.365475e-01f, 4.361730e-01f, 4.357975e-01f,  4.354210e-01f,
    4.350435e-01f, 4.346649e-01f, 4.342854e-01f,  4.339047e-01f,
    4.335231e-01f, 4.331405e-01f, 4.327568e-01f,  4.323721e-01f,
    4.319864e-01f, 4.315997e-01f, 4.312120e-01f,  4.308232e-01f,
    4.304335e-01f, 4.300427e-01f, 4.296509e-01f,  4.292581e-01f,
    4.288643e-01f, 4.284695e-01f, 4.280737e-01f,  4.276768e-01f,
    4.272790e-01f, 4.268802e-01f, 4.264803e-01f,  4.260795e-01f,
    4.256776e-01f, 4.252747e-01f, 4.248709e-01f,  4.244660e-01f,
    4.240602e-01f, 4.236533e-01f, 4.232455e-01f,  4.228366e-01f,
    4.224268e-01f, 4.220159e-01f, 4.216041e-01f,  4.211913e-01f,
    4.207775e-01f, 4.203627e-01f, 4.199469e-01f,  4.195301e-01f,
    4.191124e-01f, 4.186936e-01f, 4.182739e-01f,  4.178531e-01f,
    4.174314e-01f, 4.170088e-01f, 4.165851e-01f,  4.161604e-01f,
    4.157348e-01f, 4.153082e-01f, 4.148806e-01f,  4.144521e-01f,
    4.140225e-01f, 4.135920e-01f, 4.131605e-01f,  4.127281e-01f,
    4.122947e-01f, 4.118603e-01f, 4.114249e-01f,  4.109886e-01f,
    4.105513e-01f, 4.101130e-01f, 4.096738e-01f,  4.092336e-01f,
    4.087924e-01f, 4.083503e-01f, 4.079072e-01f,  4.074632e-01f,
    4.070182e-01f, 4.065722e-01f, 4.061253e-01f,  4.056774e-01f,
    4.052286e-01f, 4.047788e-01f, 4.043281e-01f,  4.038764e-01f,
    4.034238e-01f, 4.029702e-01f, 4.025157e-01f,  4.020602e-01f,
    4.016038e-01f, 4.011464e-01f, 4.006881e-01f,  4.002288e-01f,
    3.997686e-01f, 3.993075e-01f, 3.988454e-01f,  3.983824e-01f,
    3.979185e-01f, 3.974536e-01f, 3.969877e-01f,  3.965210e-01f,
    3.960533e-01f, 3.955847e-01f, 3.951151e-01f,  3.946446e-01f,
    3.941732e-01f, 3.937009e-01f, 3.932276e-01f,  3.927534e-01f,
    3.922783e-01f, 3.918023e-01f, 3.913253e-01f,  3.908474e-01f,
    3.903686e-01f, 3.898889e-01f, 3.894083e-01f,  3.889267e-01f,
    3.884442e-01f, 3.879608e-01f, 3.874766e-01f,  3.869913e-01f,
    3.865052e-01f, 3.860182e-01f, 3.855303e-01f,  3.850414e-01f,
    3.845517e-01f, 3.840610e-01f, 3.835695e-01f,  3.830770e-01f,
    3.825836e-01f, 3.820894e-01f, 3.815942e-01f,  3.810981e-01f,
    3.806012e-01f, 3.801033e-01f, 3.796046e-01f,  3.791050e-01f,
    3.786044e-01f, 3.781030e-01f, 3.776007e-01f,  3.770975e-01f,
    3.765934e-01f, 3.760884e-01f, 3.755826e-01f,  3.750758e-01f,
    3.745682e-01f, 3.740597e-01f, 3.735503e-01f,  3.730400e-01f,
    3.725289e-01f, 3.720169e-01f, 3.715040e-01f,  3.709902e-01f,
    3.704756e-01f, 3.699600e-01f, 3.694437e-01f,  3.689264e-01f,
    3.684083e-01f, 3.678893e-01f, 3.673694e-01f,  3.668487e-01f,
    3.663271e-01f, 3.658047e-01f, 3.652814e-01f,  3.647572e-01f,
    3.642322e-01f, 3.637063e-01f, 3.631796e-01f,  3.626520e-01f,
    3.621235e-01f, 3.615942e-01f, 3.610641e-01f,  3.605331e-01f,
    3.600013e-01f, 3.594686e-01f, 3.589350e-01f,  3.584006e-01f,
    3.578654e-01f, 3.573293e-01f, 3.567924e-01f,  3.562547e-01f,
    3.557161e-01f, 3.551767e-01f, 3.546364e-01f,  3.540953e-01f,
    3.535534e-01f, 3.530106e-01f, 3.524670e-01f,  3.519226e-01f,
    3.513774e-01f, 3.508313e-01f, 3.502844e-01f,  3.497367e-01f,
    3.491881e-01f, 3.486388e-01f, 3.480886e-01f,  3.475376e-01f,
    3.469857e-01f, 3.464331e-01f, 3.458796e-01f,  3.453254e-01f,
    3.447703e-01f, 3.442144e-01f, 3.436577e-01f,  3.431002e-01f,
    3.425418e-01f, 3.419827e-01f, 3.414228e-01f,  3.408620e-01f,
    3.403005e-01f, 3.397382e-01f, 3.391750e-01f,  3.386111e-01f,
    3.380464e-01f, 3.374808e-01f, 3.369145e-01f,  3.363474e-01f,
    3.357795e-01f, 3.352108e-01f, 3.346413e-01f,  3.340710e-01f,
    3.335000e-01f, 3.329281e-01f, 3.323555e-01f,  3.317821e-01f,
    3.312079e-01f, 3.306329e-01f, 3.300572e-01f,  3.294806e-01f,
    3.289033e-01f, 3.283253e-01f, 3.277464e-01f,  3.271668e-01f,
    3.265864e-01f, 3.260053e-01f, 3.254233e-01f,  3.248407e-01f,
    3.242572e-01f, 3.236730e-01f, 3.230880e-01f,  3.225023e-01f,
    3.219158e-01f, 3.213285e-01f, 3.207405e-01f,  3.201517e-01f,
    3.195622e-01f, 3.189720e-01f, 3.183809e-01f,  3.177892e-01f,
    3.171966e-01f, 3.166034e-01f, 3.160094e-01f,  3.154146e-01f,
    3.148191e-01f, 3.142229e-01f, 3.136259e-01f,  3.130282e-01f,
    3.124297e-01f, 3.118306e-01f, 3.112306e-01f,  3.106300e-01f,
    3.100286e-01f, 3.094265e-01f, 3.088237e-01f,  3.082201e-01f,
    3.076158e-01f, 3.070108e-01f, 3.064050e-01f,  3.057986e-01f,
    3.051914e-01f, 3.045835e-01f, 3.039749e-01f,  3.033656e-01f,
    3.027555e-01f, 3.021448e-01f, 3.015333e-01f,  3.009211e-01f,
    3.003082e-01f, 2.996946e-01f, 2.990804e-01f,  2.984654e-01f,
    2.978497e-01f, 2.972332e-01f, 2.966161e-01f,  2.959983e-01f,
    2.953799e-01f, 2.947607e-01f, 2.941408e-01f,  2.935202e-01f,
    2.928989e-01f, 2.922770e-01f, 2.916543e-01f,  2.910310e-01f,
    2.904070e-01f, 2.897823e-01f, 2.891569e-01f,  2.885308e-01f,
    2.879041e-01f, 2.872767e-01f, 2.866486e-01f,  2.860198e-01f,
    2.853904e-01f, 2.847603e-01f, 2.841295e-01f,  2.834980e-01f,
    2.828659e-01f, 2.822331e-01f, 2.815997e-01f,  2.809656e-01f,
    2.803308e-01f, 2.796954e-01f, 2.790593e-01f,  2.784225e-01f,
    2.777851e-01f, 2.771471e-01f, 2.765084e-01f,  2.758690e-01f,
    2.752290e-01f, 2.745883e-01f, 2.739470e-01f,  2.733051e-01f,
    2.726625e-01f, 2.720193e-01f, 2.713754e-01f,  2.707309e-01f,
    2.700857e-01f, 2.694400e-01f, 2.687935e-01f,  2.681465e-01f,
    2.674988e-01f, 2.668505e-01f, 2.662016e-01f,  2.655520e-01f,
    2.649018e-01f, 2.642510e-01f, 2.635996e-01f,  2.629475e-01f,
    2.622948e-01f, 2.616416e-01f, 2.609876e-01f,  2.603331e-01f,
    2.596780e-01f, 2.590223e-01f, 2.583659e-01f,  2.577089e-01f,
    2.570514e-01f, 2.563932e-01f, 2.557344e-01f,  2.550750e-01f,
    2.544151e-01f, 2.537545e-01f, 2.530933e-01f,  2.524316e-01f,
    2.517692e-01f, 2.511062e-01f, 2.504427e-01f,  2.497786e-01f,
    2.491138e-01f, 2.484485e-01f, 2.477826e-01f,  2.471162e-01f,
    2.464491e-01f, 2.457815e-01f, 2.451132e-01f,  2.444444e-01f,
    2.437751e-01f, 2.431051e-01f, 2.424346e-01f,  2.417635e-01f,
    2.410919e-01f, 2.404197e-01f, 2.397469e-01f,  2.390735e-01f,
    2.383996e-01f, 2.377251e-01f, 2.370501e-01f,  2.363745e-01f,
    2.356984e-01f, 2.350217e-01f, 2.343444e-01f,  2.336666e-01f,
    2.329882e-01f, 2.323093e-01f, 2.316299e-01f,  2.309499e-01f,
    2.302694e-01f, 2.295883e-01f, 2.289067e-01f,  2.282245e-01f,
    2.275418e-01f, 2.268586e-01f, 2.261748e-01f,  2.254905e-01f,
    2.248057e-01f, 2.241203e-01f, 2.234344e-01f,  2.227480e-01f,
    2.220611e-01f, 2.213736e-01f, 2.206856e-01f,  2.199971e-01f,
    2.193081e-01f, 2.186186e-01f, 2.179285e-01f,  2.172380e-01f,
    2.165469e-01f, 2.158553e-01f, 2.151632e-01f,  2.144706e-01f,
    2.137775e-01f, 2.130839e-01f, 2.123898e-01f,  2.116952e-01f,
    2.110001e-01f, 2.103045e-01f, 2.096084e-01f,  2.089119e-01f,
    2.082148e-01f, 2.075172e-01f, 2.068192e-01f,  2.061206e-01f,
    2.054216e-01f, 2.047221e-01f, 2.040221e-01f,  2.033216e-01f,
    2.026207e-01f, 2.019192e-01f, 2.012173e-01f,  2.005149e-01f,
    1.998121e-01f, 1.991088e-01f, 1.984050e-01f,  1.977007e-01f,
    1.969960e-01f, 1.962908e-01f, 1.955852e-01f,  1.948791e-01f,
    1.941725e-01f, 1.934655e-01f, 1.927580e-01f,  1.920501e-01f,
    1.913417e-01f, 1.906329e-01f, 1.899236e-01f,  1.892139e-01f,
    1.885037e-01f, 1.877931e-01f, 1.870820e-01f,  1.863705e-01f,
    1.856586e-01f, 1.849462e-01f, 1.842334e-01f,  1.835202e-01f,
    1.828065e-01f, 1.820924e-01f, 1.813779e-01f,  1.806629e-01f,
    1.799475e-01f, 1.792317e-01f, 1.785155e-01f,  1.777988e-01f,
    1.770818e-01f, 1.763643e-01f, 1.756464e-01f,  1.749281e-01f,
    1.742093e-01f, 1.734902e-01f, 1.727707e-01f,  1.720507e-01f,
    1.713304e-01f, 1.706096e-01f, 1.698884e-01f,  1.691669e-01f,
    1.684449e-01f, 1.677226e-01f, 1.669998e-01f,  1.662767e-01f,
    1.655532e-01f, 1.648292e-01f, 1.641049e-01f,  1.633802e-01f,
    1.626551e-01f, 1.619297e-01f, 1.612038e-01f,  1.604776e-01f,
    1.597510e-01f, 1.590240e-01f, 1.582967e-01f,  1.575690e-01f,
    1.568409e-01f, 1.561124e-01f, 1.553836e-01f,  1.546544e-01f,
    1.539248e-01f, 1.531949e-01f, 1.524646e-01f,  1.517340e-01f,
    1.510030e-01f, 1.502716e-01f, 1.495399e-01f,  1.488079e-01f,
    1.480754e-01f, 1.473427e-01f, 1.466096e-01f,  1.458761e-01f,
    1.451423e-01f, 1.444082e-01f, 1.436737e-01f,  1.429389e-01f,
    1.422038e-01f, 1.414683e-01f, 1.407325e-01f,  1.399963e-01f,
    1.392598e-01f, 1.385230e-01f, 1.377859e-01f,  1.370485e-01f,
    1.363107e-01f, 1.355726e-01f, 1.348342e-01f,  1.340954e-01f,
    1.333564e-01f, 1.326170e-01f, 1.318773e-01f,  1.311374e-01f,
    1.303971e-01f, 1.296565e-01f, 1.289156e-01f,  1.281743e-01f,
    1.274328e-01f, 1.266910e-01f, 1.259489e-01f,  1.252065e-01f,
    1.244638e-01f, 1.237208e-01f, 1.229775e-01f,  1.222340e-01f,
    1.214901e-01f, 1.207459e-01f, 1.200015e-01f,  1.192568e-01f,
    1.185118e-01f, 1.177665e-01f, 1.170210e-01f,  1.162752e-01f,
    1.155291e-01f, 1.147827e-01f, 1.140360e-01f,  1.132891e-01f,
    1.125420e-01f, 1.117945e-01f, 1.110468e-01f,  1.102988e-01f,
    1.095506e-01f, 1.088021e-01f, 1.080534e-01f,  1.073044e-01f,
    1.065552e-01f, 1.058057e-01f, 1.050559e-01f,  1.043059e-01f,
    1.035557e-01f, 1.028052e-01f, 1.020545e-01f,  1.013035e-01f,
    1.005523e-01f, 9.980088e-02f, 9.904921e-02f,  9.829730e-02f,
    9.754516e-02f, 9.679279e-02f, 9.604020e-02f,  9.528738e-02f,
    9.453433e-02f, 9.378106e-02f, 9.302758e-02f,  9.227387e-02f,
    9.151994e-02f, 9.076580e-02f, 9.001145e-02f,  8.925689e-02f,
    8.850211e-02f, 8.774713e-02f, 8.699194e-02f,  8.623654e-02f,
    8.548094e-02f, 8.472515e-02f, 8.396915e-02f,  8.321295e-02f,
    8.245656e-02f, 8.169997e-02f, 8.094320e-02f,  8.018623e-02f,
    7.942907e-02f, 7.867173e-02f, 7.791420e-02f,  7.715649e-02f,
    7.639859e-02f, 7.564052e-02f, 7.488227e-02f,  7.412384e-02f,
    7.336524e-02f, 7.260646e-02f, 7.184752e-02f,  7.108840e-02f,
    7.032912e-02f, 6.956967e-02f, 6.881006e-02f,  6.805029e-02f,
    6.729035e-02f, 6.653026e-02f, 6.577001e-02f,  6.500961e-02f,
    6.424906e-02f, 6.348835e-02f, 6.272749e-02f,  6.196649e-02f,
    6.120534e-02f, 6.044404e-02f, 5.968261e-02f,  5.892103e-02f,
    5.815932e-02f, 5.739746e-02f, 5.663548e-02f,  5.587336e-02f,
    5.511110e-02f, 5.434872e-02f, 5.358621e-02f,  5.282358e-02f,
    5.206082e-02f, 5.129793e-02f, 5.053493e-02f,  4.977181e-02f,
    4.900857e-02f, 4.824522e-02f, 4.748175e-02f,  4.671817e-02f,
    4.595448e-02f, 4.519068e-02f, 4.442678e-02f,  4.366277e-02f,
    4.289866e-02f, 4.213444e-02f, 4.137013e-02f,  4.060572e-02f,
    3.984122e-02f, 3.907662e-02f, 3.831193e-02f,  3.754715e-02f,
    3.678228e-02f, 3.601733e-02f, 3.525229e-02f,  3.448716e-02f,
    3.372196e-02f, 3.295668e-02f, 3.219132e-02f,  3.142588e-02f,
    3.066037e-02f, 2.989479e-02f, 2.912913e-02f,  2.836341e-02f,
    2.759762e-02f, 2.683177e-02f, 2.606585e-02f,  2.529987e-02f,
    2.453384e-02f, 2.376774e-02f, 2.300159e-02f,  2.223539e-02f,
    2.146913e-02f, 2.070282e-02f, 1.993646e-02f,  1.917006e-02f,
    1.840361e-02f, 1.763712e-02f, 1.687059e-02f,  1.610401e-02f,
    1.533740e-02f, 1.457075e-02f, 1.380407e-02f,  1.303736e-02f,
    1.227061e-02f, 1.150384e-02f, 1.073704e-02f,  9.970214e-03f,
    9.203365e-03f, 8.436494e-03f, 7.669603e-03f,  6.902694e-03f,
    6.135769e-03f, 5.368830e-03f, 4.601877e-03f,  3.834914e-03f,
    3.067942e-03f, 2.300963e-03f, 1.533978e-03f,  7.669901e-04f};

// ==========================================================================================
// internal tools
// ==========================================================================================
static void AUP_FFTW_cftleaf(int n, int isplt, float* a, int nw, float* w);
static void AUP_FFTW_cftf1st(int n, float* a, float* w);
static void AUP_FFTW_cftrec4(int n, float* a, int nw, float* w);
static void AUP_FFTW_cftfsub(int n, float* a, int* ip, int nw, float* w);
static void AUP_FFTW_cftbsub(int n, float* a, int* ip, int nw, float* w);
static void AUP_FFTW_bitrv2(int n, int* ip, float* a);
static void AUP_FFTW_bitrv2conj(int n, int* ip, float* a);
static void AUP_FFTW_bitrv216(float* a);
static void AUP_FFTW_bitrv216neg(float* a);
static void AUP_FFTW_bitrv208(float* a);
static void AUP_FFTW_bitrv208neg(float* a);
static void AUP_FFTW_cftb1st(int n, float* a, float* w);
static int AUP_FFTW_cfttree(int n, int j, int k, float* a, int nw, float* w);
static void AUP_FFTW_cftmdl1(int n, float* a, float* w);
static void AUP_FFTW_cftmdl2(int n, float* a, float* w);
static void AUP_FFTW_cftfx41(int n, float* a, int nw, float* w);
static void AUP_FFTW_cftf161(float* a, float* w);
static void AUP_FFTW_cftf162(float* a, float* w);
static void AUP_FFTW_cftf081(float* a, float* w);
static void AUP_FFTW_cftf082(float* a, float* w);
static void AUP_FFTW_cftf040(float* a);
static void AUP_FFTW_cftb040(float* a);
static void AUP_FFTW_cftx020(float* a);
static void AUP_FFTW_rftfsub(int n, float* a, int nc, float* c);
static void AUP_FFTW_rftbsub(int n, float* a, int nc, float* c);
static void AUP_FFTW_dctsub(int n, float* a, int nc, float* c);
static void AUP_FFTW_dstsub(int n, float* a, int nc, float* c);
static void AUP_FFTW_rdft(int n, int isgn, float* a, int* ip, float* w);

static void AUP_FFTW_cftleaf(int n, int isplt, float* a, int nw, float* w) {
  if (n == 512) {
    AUP_FFTW_cftmdl1(128, a, &w[nw - 64]);
    AUP_FFTW_cftf161(a, &w[nw - 8]);
    AUP_FFTW_cftf162(&a[32], &w[nw - 32]);
    AUP_FFTW_cftf161(&a[64], &w[nw - 8]);
    AUP_FFTW_cftf161(&a[96], &w[nw - 8]);
    AUP_FFTW_cftmdl2(128, &a[128], &w[nw - 128]);
    AUP_FFTW_cftf161(&a[128], &w[nw - 8]);
    AUP_FFTW_cftf162(&a[160], &w[nw - 32]);
    AUP_FFTW_cftf161(&a[192], &w[nw - 8]);
    AUP_FFTW_cftf162(&a[224], &w[nw - 32]);
    AUP_FFTW_cftmdl1(128, &a[256], &w[nw - 64]);
    AUP_FFTW_cftf161(&a[256], &w[nw - 8]);
    AUP_FFTW_cftf162(&a[288], &w[nw - 32]);
    AUP_FFTW_cftf161(&a[320], &w[nw - 8]);
    AUP_FFTW_cftf161(&a[352], &w[nw - 8]);
    if (isplt != 0) {
      AUP_FFTW_cftmdl1(128, &a[384], &w[nw - 64]);
      AUP_FFTW_cftf161(&a[480], &w[nw - 8]);
    } else {
      AUP_FFTW_cftmdl2(128, &a[384], &w[nw - 128]);
      AUP_FFTW_cftf162(&a[480], &w[nw - 32]);
    }
    AUP_FFTW_cftf161(&a[384], &w[nw - 8]);
    AUP_FFTW_cftf162(&a[416], &w[nw - 32]);
    AUP_FFTW_cftf161(&a[448], &w[nw - 8]);
  } else {
    AUP_FFTW_cftmdl1(64, a, &w[nw - 32]);
    AUP_FFTW_cftf081(a, &w[nw - 8]);
    AUP_FFTW_cftf082(&a[16], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[32], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[48], &w[nw - 8]);
    AUP_FFTW_cftmdl2(64, &a[64], &w[nw - 64]);
    AUP_FFTW_cftf081(&a[64], &w[nw - 8]);
    AUP_FFTW_cftf082(&a[80], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[96], &w[nw - 8]);
    AUP_FFTW_cftf082(&a[112], &w[nw - 8]);
    AUP_FFTW_cftmdl1(64, &a[128], &w[nw - 32]);
    AUP_FFTW_cftf081(&a[128], &w[nw - 8]);
    AUP_FFTW_cftf082(&a[144], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[160], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[176], &w[nw - 8]);
    if (isplt != 0) {
      AUP_FFTW_cftmdl1(64, &a[192], &w[nw - 32]);
      AUP_FFTW_cftf081(&a[240], &w[nw - 8]);
    } else {
      AUP_FFTW_cftmdl2(64, &a[192], &w[nw - 64]);
      AUP_FFTW_cftf082(&a[240], &w[nw - 8]);
    }
    AUP_FFTW_cftf081(&a[192], &w[nw - 8]);
    AUP_FFTW_cftf082(&a[208], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[224], &w[nw - 8]);
  }
}

static void AUP_FFTW_cftf1st(int n, float* a, float* w) {
  int j, j0, j1, j2, j3, k, m, mh;
  float wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
  float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r, y2i,
      y3r, y3i;

  mh = n >> 3;
  m = 2 * mh;
  j1 = m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[0] + a[j2];
  x0i = a[1] + a[j2 + 1];
  x1r = a[0] - a[j2];
  x1i = a[1] - a[j2 + 1];
  x2r = a[j1] + a[j3];
  x2i = a[j1 + 1] + a[j3 + 1];
  x3r = a[j1] - a[j3];
  x3i = a[j1 + 1] - a[j3 + 1];
  a[0] = x0r + x2r;
  a[1] = x0i + x2i;
  a[j1] = x0r - x2r;
  a[j1 + 1] = x0i - x2i;
  a[j2] = x1r - x3i;
  a[j2 + 1] = x1i + x3r;
  a[j3] = x1r + x3i;
  a[j3 + 1] = x1i - x3r;
  wn4r = w[1];
  csc1 = w[2];
  csc3 = w[3];
  wd1r = 1;
  wd1i = 0;
  wd3r = 1;
  wd3i = 0;
  k = 0;
  for (j = 2; j < mh - 2; j += 4) {
    k += 4;
    wk1r = csc1 * (wd1r + w[k]);
    wk1i = csc1 * (wd1i + w[k + 1]);
    wk3r = csc3 * (wd3r + w[k + 2]);
    wk3i = csc3 * (wd3i + w[k + 3]);
    wd1r = w[k];
    wd1i = w[k + 1];
    wd3r = w[k + 2];
    wd3i = w[k + 3];
    j1 = j + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j] + a[j2];
    x0i = a[j + 1] + a[j2 + 1];
    x1r = a[j] - a[j2];
    x1i = a[j + 1] - a[j2 + 1];
    y0r = a[j + 2] + a[j2 + 2];
    y0i = a[j + 3] + a[j2 + 3];
    y1r = a[j + 2] - a[j2 + 2];
    y1i = a[j + 3] - a[j2 + 3];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    y2r = a[j1 + 2] + a[j3 + 2];
    y2i = a[j1 + 3] + a[j3 + 3];
    y3r = a[j1 + 2] - a[j3 + 2];
    y3i = a[j1 + 3] - a[j3 + 3];
    a[j] = x0r + x2r;
    a[j + 1] = x0i + x2i;
    a[j + 2] = y0r + y2r;
    a[j + 3] = y0i + y2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j1 + 2] = y0r - y2r;
    a[j1 + 3] = y0i - y2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wk1r * x0r - wk1i * x0i;
    a[j2 + 1] = wk1r * x0i + wk1i * x0r;
    x0r = y1r - y3i;
    x0i = y1i + y3r;
    a[j2 + 2] = wd1r * x0r - wd1i * x0i;
    a[j2 + 3] = wd1r * x0i + wd1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = wk3r * x0r + wk3i * x0i;
    a[j3 + 1] = wk3r * x0i - wk3i * x0r;
    x0r = y1r + y3i;
    x0i = y1i - y3r;
    a[j3 + 2] = wd3r * x0r + wd3i * x0i;
    a[j3 + 3] = wd3r * x0i - wd3i * x0r;
    j0 = m - j;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    y0r = a[j0 - 2] + a[j2 - 2];
    y0i = a[j0 - 1] + a[j2 - 1];
    y1r = a[j0 - 2] - a[j2 - 2];
    y1i = a[j0 - 1] - a[j2 - 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    y2r = a[j1 - 2] + a[j3 - 2];
    y2i = a[j1 - 1] + a[j3 - 1];
    y3r = a[j1 - 2] - a[j3 - 2];
    y3i = a[j1 - 1] - a[j3 - 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j0 - 2] = y0r + y2r;
    a[j0 - 1] = y0i + y2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j1 - 2] = y0r - y2r;
    a[j1 - 1] = y0i - y2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 1] = wk1i * x0i + wk1r * x0r;
    x0r = y1r - y3i;
    x0i = y1i + y3r;
    a[j2 - 2] = wd1i * x0r - wd1r * x0i;
    a[j2 - 1] = wd1i * x0i + wd1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = wk3i * x0r + wk3r * x0i;
    a[j3 + 1] = wk3i * x0i - wk3r * x0r;
    x0r = y1r + y3i;
    x0i = y1i - y3r;
    a[j3 - 2] = wd3i * x0r + wd3r * x0i;
    a[j3 - 1] = wd3i * x0i - wd3r * x0r;
  }
  wk1r = csc1 * (wd1r + wn4r);
  wk1i = csc1 * (wd1i + wn4r);
  wk3r = csc3 * (wd3r - wn4r);
  wk3i = csc3 * (wd3i - wn4r);
  j0 = mh;
  j1 = j0 + m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[j0 - 2] + a[j2 - 2];
  x0i = a[j0 - 1] + a[j2 - 1];
  x1r = a[j0 - 2] - a[j2 - 2];
  x1i = a[j0 - 1] - a[j2 - 1];
  x2r = a[j1 - 2] + a[j3 - 2];
  x2i = a[j1 - 1] + a[j3 - 1];
  x3r = a[j1 - 2] - a[j3 - 2];
  x3i = a[j1 - 1] - a[j3 - 1];
  a[j0 - 2] = x0r + x2r;
  a[j0 - 1] = x0i + x2i;
  a[j1 - 2] = x0r - x2r;
  a[j1 - 1] = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  a[j2 - 2] = wk1r * x0r - wk1i * x0i;
  a[j2 - 1] = wk1r * x0i + wk1i * x0r;
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  a[j3 - 2] = wk3r * x0r + wk3i * x0i;
  a[j3 - 1] = wk3r * x0i - wk3i * x0r;
  x0r = a[j0] + a[j2];
  x0i = a[j0 + 1] + a[j2 + 1];
  x1r = a[j0] - a[j2];
  x1i = a[j0 + 1] - a[j2 + 1];
  x2r = a[j1] + a[j3];
  x2i = a[j1 + 1] + a[j3 + 1];
  x3r = a[j1] - a[j3];
  x3i = a[j1 + 1] - a[j3 + 1];
  a[j0] = x0r + x2r;
  a[j0 + 1] = x0i + x2i;
  a[j1] = x0r - x2r;
  a[j1 + 1] = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  a[j2] = wn4r * (x0r - x0i);
  a[j2 + 1] = wn4r * (x0i + x0r);
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  a[j3] = -wn4r * (x0r + x0i);
  a[j3 + 1] = -wn4r * (x0i - x0r);
  x0r = a[j0 + 2] + a[j2 + 2];
  x0i = a[j0 + 3] + a[j2 + 3];
  x1r = a[j0 + 2] - a[j2 + 2];
  x1i = a[j0 + 3] - a[j2 + 3];
  x2r = a[j1 + 2] + a[j3 + 2];
  x2i = a[j1 + 3] + a[j3 + 3];
  x3r = a[j1 + 2] - a[j3 + 2];
  x3i = a[j1 + 3] - a[j3 + 3];
  a[j0 + 2] = x0r + x2r;
  a[j0 + 3] = x0i + x2i;
  a[j1 + 2] = x0r - x2r;
  a[j1 + 3] = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  a[j2 + 2] = wk1i * x0r - wk1r * x0i;
  a[j2 + 3] = wk1i * x0i + wk1r * x0r;
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  a[j3 + 2] = wk3i * x0r + wk3r * x0i;
  a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

static void AUP_FFTW_cftrec4(int n, float* a, int nw, float* w) {
  int isplt, j, k, m;

  m = n;
  while (m > 512) {
    m >>= 2;
    AUP_FFTW_cftmdl1(m, &a[n - m], &w[nw - (m >> 1)]);
  }
  AUP_FFTW_cftleaf(m, 1, &a[n - m], nw, w);
  k = 0;
  for (j = n - m; j > 0; j -= m) {
    k++;
    isplt = AUP_FFTW_cfttree(m, j, k, a, nw, w);
    AUP_FFTW_cftleaf(m, isplt, &a[j - m], nw, w);
  }
}

static void AUP_FFTW_cftfsub(int n, float* a, int* ip, int nw, float* w) {
  if (n > 8) {
    if (n > 32) {
      AUP_FFTW_cftf1st(n, a, &w[nw - (n >> 2)]);
      if (n > 512) {
        AUP_FFTW_cftrec4(n, a, nw, w);
      } else if (n > 128) {
        AUP_FFTW_cftleaf(n, 1, a, nw, w);
      } else {
        AUP_FFTW_cftfx41(n, a, nw, w);
      }
      AUP_FFTW_bitrv2(n, ip, a);
    } else if (n == 32) {
      AUP_FFTW_cftf161(a, &w[nw - 8]);
      AUP_FFTW_bitrv216(a);
    } else {
      AUP_FFTW_cftf081(a, w);
      AUP_FFTW_bitrv208(a);
    }
  } else if (n == 8) {
    AUP_FFTW_cftf040(a);
  } else if (n == 4) {
    AUP_FFTW_cftx020(a);
  }
}

static void AUP_FFTW_cftbsub(int n, float* a, int* ip, int nw, float* w) {
  if (n > 8) {
    if (n > 32) {
      AUP_FFTW_cftb1st(n, a, &w[nw - (n >> 2)]);
      if (n > 512) {
        AUP_FFTW_cftrec4(n, a, nw, w);
      } else if (n > 128) {
        AUP_FFTW_cftleaf(n, 1, a, nw, w);
      } else {
        AUP_FFTW_cftfx41(n, a, nw, w);
      }
      AUP_FFTW_bitrv2conj(n, ip, a);
    } else if (n == 32) {
      AUP_FFTW_cftf161(a, &w[nw - 8]);
      AUP_FFTW_bitrv216neg(a);
    } else {
      AUP_FFTW_cftf081(a, w);
      AUP_FFTW_bitrv208neg(a);
    }
  } else if (n == 8) {
    AUP_FFTW_cftb040(a);
  } else if (n == 4) {
    AUP_FFTW_cftx020(a);
  }
}

static void AUP_FFTW_bitrv2(int n, int* ip, float* a) {
  int j, j1, k, k1, l, m, nh, nm;
  float xr, xi, yr, yi;

  m = 1;
  for (l = n >> 2; l > 8; l >>= 2) {
    m <<= 1;
  }
  nh = n >> 1;
  nm = 4 * m;
  if (l == 8) {
    for (k = 0; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 4 * j + 2 * ip[m + k];
        k1 = 4 * k + 2 * ip[m + j];
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 -= nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nh;
        k1 += 2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 += nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += 2;
        k1 += nh;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 -= nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nh;
        k1 -= 2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 += nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      k1 = 4 * k + 2 * ip[m + k];
      j1 = k1 + 2;
      k1 += nh;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 += nm;
      k1 += 2 * nm;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 += nm;
      k1 -= nm;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 -= 2;
      k1 -= nh;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 += nh + 2;
      k1 += nh + 2;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 -= nh - nm;
      k1 += 2 * nm - 2;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
    }
  } else {
    for (k = 0; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 4 * j + ip[m + k];
        k1 = 4 * k + ip[m + j];
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nh;
        k1 += 2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += 2;
        k1 += nh;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nh;
        k1 -= 2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= nm;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      k1 = 4 * k + ip[m + k];
      j1 = k1 + 2;
      k1 += nh;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 += nm;
      k1 += nm;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
    }
  }
}

static void AUP_FFTW_bitrv2conj(int n, int* ip, float* a) {
  int j, j1, k, k1, l, m, nh, nm;
  float xr, xi, yr, yi;

  m = 1;
  for (l = n >> 2; l > 8; l >>= 2) {
    m <<= 1;
  }
  nh = n >> 1;
  nm = 4 * m;
  if (l == 8) {
    for (k = 0; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 4 * j + 2 * ip[m + k];
        k1 = 4 * k + 2 * ip[m + j];
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 -= nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nh;
        k1 += 2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 += nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += 2;
        k1 += nh;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 -= nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nh;
        k1 -= 2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 += nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= 2 * nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      k1 = 4 * k + 2 * ip[m + k];
      j1 = k1 + 2;
      k1 += nh;
      a[j1 - 1] = -a[j1 - 1];
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      a[k1 + 3] = -a[k1 + 3];
      j1 += nm;
      k1 += 2 * nm;
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 += nm;
      k1 -= nm;
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 -= 2;
      k1 -= nh;
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 += nh + 2;
      k1 += nh + 2;
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      j1 -= nh - nm;
      k1 += 2 * nm - 2;
      a[j1 - 1] = -a[j1 - 1];
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      a[k1 + 3] = -a[k1 + 3];
    }
  } else {
    for (k = 0; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 4 * j + ip[m + k];
        k1 = 4 * k + ip[m + j];
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nh;
        k1 += 2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += 2;
        k1 += nh;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += nm;
        k1 += nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nh;
        k1 -= 2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 -= nm;
        k1 -= nm;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      k1 = 4 * k + ip[m + k];
      j1 = k1 + 2;
      k1 += nh;
      a[j1 - 1] = -a[j1 - 1];
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      a[k1 + 3] = -a[k1 + 3];
      j1 += nm;
      k1 += nm;
      a[j1 - 1] = -a[j1 - 1];
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      a[k1 + 3] = -a[k1 + 3];
    }
  }
}

static void AUP_FFTW_bitrv216(float* a) {
  float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, x5r, x5i, x7r, x7i, x8r, x8i,
      x10r, x10i, x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;

  x1r = a[2];
  x1i = a[3];
  x2r = a[4];
  x2i = a[5];
  x3r = a[6];
  x3i = a[7];
  x4r = a[8];
  x4i = a[9];
  x5r = a[10];
  x5i = a[11];
  x7r = a[14];
  x7i = a[15];
  x8r = a[16];
  x8i = a[17];
  x10r = a[20];
  x10i = a[21];
  x11r = a[22];
  x11i = a[23];
  x12r = a[24];
  x12i = a[25];
  x13r = a[26];
  x13i = a[27];
  x14r = a[28];
  x14i = a[29];
  a[2] = x8r;
  a[3] = x8i;
  a[4] = x4r;
  a[5] = x4i;
  a[6] = x12r;
  a[7] = x12i;
  a[8] = x2r;
  a[9] = x2i;
  a[10] = x10r;
  a[11] = x10i;
  a[14] = x14r;
  a[15] = x14i;
  a[16] = x1r;
  a[17] = x1i;
  a[20] = x5r;
  a[21] = x5i;
  a[22] = x13r;
  a[23] = x13i;
  a[24] = x3r;
  a[25] = x3i;
  a[26] = x11r;
  a[27] = x11i;
  a[28] = x7r;
  a[29] = x7i;
}

static void AUP_FFTW_bitrv216neg(float* a) {
  float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, x5r, x5i, x6r, x6i, x7r, x7i,
      x8r, x8i, x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i, x13r, x13i, x14r,
      x14i, x15r, x15i;

  x1r = a[2];
  x1i = a[3];
  x2r = a[4];
  x2i = a[5];
  x3r = a[6];
  x3i = a[7];
  x4r = a[8];
  x4i = a[9];
  x5r = a[10];
  x5i = a[11];
  x6r = a[12];
  x6i = a[13];
  x7r = a[14];
  x7i = a[15];
  x8r = a[16];
  x8i = a[17];
  x9r = a[18];
  x9i = a[19];
  x10r = a[20];
  x10i = a[21];
  x11r = a[22];
  x11i = a[23];
  x12r = a[24];
  x12i = a[25];
  x13r = a[26];
  x13i = a[27];
  x14r = a[28];
  x14i = a[29];
  x15r = a[30];
  x15i = a[31];
  a[2] = x15r;
  a[3] = x15i;
  a[4] = x7r;
  a[5] = x7i;
  a[6] = x11r;
  a[7] = x11i;
  a[8] = x3r;
  a[9] = x3i;
  a[10] = x13r;
  a[11] = x13i;
  a[12] = x5r;
  a[13] = x5i;
  a[14] = x9r;
  a[15] = x9i;
  a[16] = x1r;
  a[17] = x1i;
  a[18] = x14r;
  a[19] = x14i;
  a[20] = x6r;
  a[21] = x6i;
  a[22] = x10r;
  a[23] = x10i;
  a[24] = x2r;
  a[25] = x2i;
  a[26] = x12r;
  a[27] = x12i;
  a[28] = x4r;
  a[29] = x4i;
  a[30] = x8r;
  a[31] = x8i;
}

static void AUP_FFTW_bitrv208(float* a) {
  float x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;

  x1r = a[2];
  x1i = a[3];
  x3r = a[6];
  x3i = a[7];
  x4r = a[8];
  x4i = a[9];
  x6r = a[12];
  x6i = a[13];
  a[2] = x4r;
  a[3] = x4i;
  a[6] = x6r;
  a[7] = x6i;
  a[8] = x1r;
  a[9] = x1i;
  a[12] = x3r;
  a[13] = x3i;
}

static void AUP_FFTW_bitrv208neg(float* a) {
  float x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, x5r, x5i, x6r, x6i, x7r, x7i;

  x1r = a[2];
  x1i = a[3];
  x2r = a[4];
  x2i = a[5];
  x3r = a[6];
  x3i = a[7];
  x4r = a[8];
  x4i = a[9];
  x5r = a[10];
  x5i = a[11];
  x6r = a[12];
  x6i = a[13];
  x7r = a[14];
  x7i = a[15];
  a[2] = x7r;
  a[3] = x7i;
  a[4] = x3r;
  a[5] = x3i;
  a[6] = x5r;
  a[7] = x5i;
  a[8] = x1r;
  a[9] = x1i;
  a[10] = x6r;
  a[11] = x6i;
  a[12] = x2r;
  a[13] = x2i;
  a[14] = x4r;
  a[15] = x4i;
}

static void AUP_FFTW_cftb1st(int n, float* a, float* w) {
  int j, j0, j1, j2, j3, k, m, mh;
  float wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
  float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r, y2i,
      y3r, y3i;

  mh = n >> 3;
  m = 2 * mh;
  j1 = m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[0] + a[j2];
  x0i = -a[1] - a[j2 + 1];
  x1r = a[0] - a[j2];
  x1i = -a[1] + a[j2 + 1];
  x2r = a[j1] + a[j3];
  x2i = a[j1 + 1] + a[j3 + 1];
  x3r = a[j1] - a[j3];
  x3i = a[j1 + 1] - a[j3 + 1];
  a[0] = x0r + x2r;
  a[1] = x0i - x2i;
  a[j1] = x0r - x2r;
  a[j1 + 1] = x0i + x2i;
  a[j2] = x1r + x3i;
  a[j2 + 1] = x1i + x3r;
  a[j3] = x1r - x3i;
  a[j3 + 1] = x1i - x3r;
  wn4r = w[1];
  csc1 = w[2];
  csc3 = w[3];
  wd1r = 1;
  wd1i = 0;
  wd3r = 1;
  wd3i = 0;
  k = 0;
  for (j = 2; j < mh - 2; j += 4) {
    k += 4;
    wk1r = csc1 * (wd1r + w[k]);
    wk1i = csc1 * (wd1i + w[k + 1]);
    wk3r = csc3 * (wd3r + w[k + 2]);
    wk3i = csc3 * (wd3i + w[k + 3]);
    wd1r = w[k];
    wd1i = w[k + 1];
    wd3r = w[k + 2];
    wd3i = w[k + 3];
    j1 = j + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j] + a[j2];
    x0i = -a[j + 1] - a[j2 + 1];
    x1r = a[j] - a[j2];
    x1i = -a[j + 1] + a[j2 + 1];
    y0r = a[j + 2] + a[j2 + 2];
    y0i = -a[j + 3] - a[j2 + 3];
    y1r = a[j + 2] - a[j2 + 2];
    y1i = -a[j + 3] + a[j2 + 3];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    y2r = a[j1 + 2] + a[j3 + 2];
    y2i = a[j1 + 3] + a[j3 + 3];
    y3r = a[j1 + 2] - a[j3 + 2];
    y3i = a[j1 + 3] - a[j3 + 3];
    a[j] = x0r + x2r;
    a[j + 1] = x0i - x2i;
    a[j + 2] = y0r + y2r;
    a[j + 3] = y0i - y2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    a[j1 + 2] = y0r - y2r;
    a[j1 + 3] = y0i + y2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2] = wk1r * x0r - wk1i * x0i;
    a[j2 + 1] = wk1r * x0i + wk1i * x0r;
    x0r = y1r + y3i;
    x0i = y1i + y3r;
    a[j2 + 2] = wd1r * x0r - wd1i * x0i;
    a[j2 + 3] = wd1r * x0i + wd1i * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3] = wk3r * x0r + wk3i * x0i;
    a[j3 + 1] = wk3r * x0i - wk3i * x0r;
    x0r = y1r - y3i;
    x0i = y1i - y3r;
    a[j3 + 2] = wd3r * x0r + wd3i * x0i;
    a[j3 + 3] = wd3r * x0i - wd3i * x0r;
    j0 = m - j;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] + a[j2];
    x0i = -a[j0 + 1] - a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = -a[j0 + 1] + a[j2 + 1];
    y0r = a[j0 - 2] + a[j2 - 2];
    y0i = -a[j0 - 1] - a[j2 - 1];
    y1r = a[j0 - 2] - a[j2 - 2];
    y1i = -a[j0 - 1] + a[j2 - 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    y2r = a[j1 - 2] + a[j3 - 2];
    y2i = a[j1 - 1] + a[j3 - 1];
    y3r = a[j1 - 2] - a[j3 - 2];
    y3i = a[j1 - 1] - a[j3 - 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i - x2i;
    a[j0 - 2] = y0r + y2r;
    a[j0 - 1] = y0i - y2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    a[j1 - 2] = y0r - y2r;
    a[j1 - 1] = y0i + y2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 1] = wk1i * x0i + wk1r * x0r;
    x0r = y1r + y3i;
    x0i = y1i + y3r;
    a[j2 - 2] = wd1i * x0r - wd1r * x0i;
    a[j2 - 1] = wd1i * x0i + wd1r * x0r;
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3] = wk3i * x0r + wk3r * x0i;
    a[j3 + 1] = wk3i * x0i - wk3r * x0r;
    x0r = y1r - y3i;
    x0i = y1i - y3r;
    a[j3 - 2] = wd3i * x0r + wd3r * x0i;
    a[j3 - 1] = wd3i * x0i - wd3r * x0r;
  }
  wk1r = csc1 * (wd1r + wn4r);
  wk1i = csc1 * (wd1i + wn4r);
  wk3r = csc3 * (wd3r - wn4r);
  wk3i = csc3 * (wd3i - wn4r);
  j0 = mh;
  j1 = j0 + m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[j0 - 2] + a[j2 - 2];
  x0i = -a[j0 - 1] - a[j2 - 1];
  x1r = a[j0 - 2] - a[j2 - 2];
  x1i = -a[j0 - 1] + a[j2 - 1];
  x2r = a[j1 - 2] + a[j3 - 2];
  x2i = a[j1 - 1] + a[j3 - 1];
  x3r = a[j1 - 2] - a[j3 - 2];
  x3i = a[j1 - 1] - a[j3 - 1];
  a[j0 - 2] = x0r + x2r;
  a[j0 - 1] = x0i - x2i;
  a[j1 - 2] = x0r - x2r;
  a[j1 - 1] = x0i + x2i;
  x0r = x1r + x3i;
  x0i = x1i + x3r;
  a[j2 - 2] = wk1r * x0r - wk1i * x0i;
  a[j2 - 1] = wk1r * x0i + wk1i * x0r;
  x0r = x1r - x3i;
  x0i = x1i - x3r;
  a[j3 - 2] = wk3r * x0r + wk3i * x0i;
  a[j3 - 1] = wk3r * x0i - wk3i * x0r;
  x0r = a[j0] + a[j2];
  x0i = -a[j0 + 1] - a[j2 + 1];
  x1r = a[j0] - a[j2];
  x1i = -a[j0 + 1] + a[j2 + 1];
  x2r = a[j1] + a[j3];
  x2i = a[j1 + 1] + a[j3 + 1];
  x3r = a[j1] - a[j3];
  x3i = a[j1 + 1] - a[j3 + 1];
  a[j0] = x0r + x2r;
  a[j0 + 1] = x0i - x2i;
  a[j1] = x0r - x2r;
  a[j1 + 1] = x0i + x2i;
  x0r = x1r + x3i;
  x0i = x1i + x3r;
  a[j2] = wn4r * (x0r - x0i);
  a[j2 + 1] = wn4r * (x0i + x0r);
  x0r = x1r - x3i;
  x0i = x1i - x3r;
  a[j3] = -wn4r * (x0r + x0i);
  a[j3 + 1] = -wn4r * (x0i - x0r);
  x0r = a[j0 + 2] + a[j2 + 2];
  x0i = -a[j0 + 3] - a[j2 + 3];
  x1r = a[j0 + 2] - a[j2 + 2];
  x1i = -a[j0 + 3] + a[j2 + 3];
  x2r = a[j1 + 2] + a[j3 + 2];
  x2i = a[j1 + 3] + a[j3 + 3];
  x3r = a[j1 + 2] - a[j3 + 2];
  x3i = a[j1 + 3] - a[j3 + 3];
  a[j0 + 2] = x0r + x2r;
  a[j0 + 3] = x0i - x2i;
  a[j1 + 2] = x0r - x2r;
  a[j1 + 3] = x0i + x2i;
  x0r = x1r + x3i;
  x0i = x1i + x3r;
  a[j2 + 2] = wk1i * x0r - wk1r * x0i;
  a[j2 + 3] = wk1i * x0i + wk1r * x0r;
  x0r = x1r - x3i;
  x0i = x1i - x3r;
  a[j3 + 2] = wk3i * x0r + wk3r * x0i;
  a[j3 + 3] = wk3i * x0i - wk3r * x0r;
}

static int AUP_FFTW_cfttree(int n, int j, int k, float* a, int nw, float* w) {
  int i, isplt, m;

  if ((k & 3) != 0) {
    isplt = k & 1;
    if (isplt != 0) {
      AUP_FFTW_cftmdl1(n, &a[j - n], &w[nw - (n >> 1)]);
    } else {
      AUP_FFTW_cftmdl2(n, &a[j - n], &w[nw - n]);
    }
  } else {
    m = n;
    for (i = k; (i & 3) == 0; i >>= 2) {
      m <<= 2;
    }
    isplt = i & 1;
    if (isplt != 0) {
      while (m > 128) {
        AUP_FFTW_cftmdl1(m, &a[j - m], &w[nw - (m >> 1)]);
        m >>= 2;
      }
    } else {
      while (m > 128) {
        AUP_FFTW_cftmdl2(m, &a[j - m], &w[nw - m]);
        m >>= 2;
      }
    }
  }
  return isplt;
}

static void AUP_FFTW_cftmdl1(int n, float* a, float* w) {
  int j, j0, j1, j2, j3, k, m, mh;
  float wn4r, wk1r, wk1i, wk3r, wk3i;
  float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

  mh = n >> 3;
  m = 2 * mh;
  j1 = m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[0] + a[j2];
  x0i = a[1] + a[j2 + 1];
  x1r = a[0] - a[j2];
  x1i = a[1] - a[j2 + 1];
  x2r = a[j1] + a[j3];
  x2i = a[j1 + 1] + a[j3 + 1];
  x3r = a[j1] - a[j3];
  x3i = a[j1 + 1] - a[j3 + 1];
  a[0] = x0r + x2r;
  a[1] = x0i + x2i;
  a[j1] = x0r - x2r;
  a[j1 + 1] = x0i - x2i;
  a[j2] = x1r - x3i;
  a[j2 + 1] = x1i + x3r;
  a[j3] = x1r + x3i;
  a[j3 + 1] = x1i - x3r;
  wn4r = w[1];
  k = 0;
  for (j = 2; j < mh; j += 2) {
    k += 4;
    wk1r = w[k];
    wk1i = w[k + 1];
    wk3r = w[k + 2];
    wk3i = w[k + 3];
    j1 = j + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j] + a[j2];
    x0i = a[j + 1] + a[j2 + 1];
    x1r = a[j] - a[j2];
    x1i = a[j + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j] = x0r + x2r;
    a[j + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wk1r * x0r - wk1i * x0i;
    a[j2 + 1] = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = wk3r * x0r + wk3i * x0i;
    a[j3 + 1] = wk3r * x0i - wk3i * x0r;
    j0 = m - j;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = wk1i * x0r - wk1r * x0i;
    a[j2 + 1] = wk1i * x0i + wk1r * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = wk3i * x0r + wk3r * x0i;
    a[j3 + 1] = wk3i * x0i - wk3r * x0r;
  }
  j0 = mh;
  j1 = j0 + m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[j0] + a[j2];
  x0i = a[j0 + 1] + a[j2 + 1];
  x1r = a[j0] - a[j2];
  x1i = a[j0 + 1] - a[j2 + 1];
  x2r = a[j1] + a[j3];
  x2i = a[j1 + 1] + a[j3 + 1];
  x3r = a[j1] - a[j3];
  x3i = a[j1 + 1] - a[j3 + 1];
  a[j0] = x0r + x2r;
  a[j0 + 1] = x0i + x2i;
  a[j1] = x0r - x2r;
  a[j1 + 1] = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  a[j2] = wn4r * (x0r - x0i);
  a[j2 + 1] = wn4r * (x0i + x0r);
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  a[j3] = -wn4r * (x0r + x0i);
  a[j3 + 1] = -wn4r * (x0i - x0r);
}

static void AUP_FFTW_cftmdl2(int n, float* a, float* w) {
  int j, j0, j1, j2, j3, k, kr, m, mh;
  float wn4r, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
  float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;

  mh = n >> 3;
  m = 2 * mh;
  wn4r = w[1];
  j1 = m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[0] - a[j2 + 1];
  x0i = a[1] + a[j2];
  x1r = a[0] + a[j2 + 1];
  x1i = a[1] - a[j2];
  x2r = a[j1] - a[j3 + 1];
  x2i = a[j1 + 1] + a[j3];
  x3r = a[j1] + a[j3 + 1];
  x3i = a[j1 + 1] - a[j3];
  y0r = wn4r * (x2r - x2i);
  y0i = wn4r * (x2i + x2r);
  a[0] = x0r + y0r;
  a[1] = x0i + y0i;
  a[j1] = x0r - y0r;
  a[j1 + 1] = x0i - y0i;
  y0r = wn4r * (x3r - x3i);
  y0i = wn4r * (x3i + x3r);
  a[j2] = x1r - y0i;
  a[j2 + 1] = x1i + y0r;
  a[j3] = x1r + y0i;
  a[j3 + 1] = x1i - y0r;
  k = 0;
  kr = 2 * m;
  for (j = 2; j < mh; j += 2) {
    k += 4;
    wk1r = w[k];
    wk1i = w[k + 1];
    wk3r = w[k + 2];
    wk3i = w[k + 3];
    kr -= 4;
    wd1i = w[kr];
    wd1r = w[kr + 1];
    wd3i = w[kr + 2];
    wd3r = w[kr + 3];
    j1 = j + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j] - a[j2 + 1];
    x0i = a[j + 1] + a[j2];
    x1r = a[j] + a[j2 + 1];
    x1i = a[j + 1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wk1r * x0r - wk1i * x0i;
    y0i = wk1r * x0i + wk1i * x0r;
    y2r = wd1r * x2r - wd1i * x2i;
    y2i = wd1r * x2i + wd1i * x2r;
    a[j] = y0r + y2r;
    a[j + 1] = y0i + y2i;
    a[j1] = y0r - y2r;
    a[j1 + 1] = y0i - y2i;
    y0r = wk3r * x1r + wk3i * x1i;
    y0i = wk3r * x1i - wk3i * x1r;
    y2r = wd3r * x3r + wd3i * x3i;
    y2i = wd3r * x3i - wd3i * x3r;
    a[j2] = y0r + y2r;
    a[j2 + 1] = y0i + y2i;
    a[j3] = y0r - y2r;
    a[j3 + 1] = y0i - y2i;
    j0 = m - j;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0] - a[j2 + 1];
    x0i = a[j0 + 1] + a[j2];
    x1r = a[j0] + a[j2 + 1];
    x1i = a[j0 + 1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = wd1i * x0r - wd1r * x0i;
    y0i = wd1i * x0i + wd1r * x0r;
    y2r = wk1i * x2r - wk1r * x2i;
    y2i = wk1i * x2i + wk1r * x2r;
    a[j0] = y0r + y2r;
    a[j0 + 1] = y0i + y2i;
    a[j1] = y0r - y2r;
    a[j1 + 1] = y0i - y2i;
    y0r = wd3i * x1r + wd3r * x1i;
    y0i = wd3i * x1i - wd3r * x1r;
    y2r = wk3i * x3r + wk3r * x3i;
    y2i = wk3i * x3i - wk3r * x3r;
    a[j2] = y0r + y2r;
    a[j2 + 1] = y0i + y2i;
    a[j3] = y0r - y2r;
    a[j3 + 1] = y0i - y2i;
  }
  wk1r = w[m];
  wk1i = w[m + 1];
  j0 = mh;
  j1 = j0 + m;
  j2 = j1 + m;
  j3 = j2 + m;
  x0r = a[j0] - a[j2 + 1];
  x0i = a[j0 + 1] + a[j2];
  x1r = a[j0] + a[j2 + 1];
  x1i = a[j0 + 1] - a[j2];
  x2r = a[j1] - a[j3 + 1];
  x2i = a[j1 + 1] + a[j3];
  x3r = a[j1] + a[j3 + 1];
  x3i = a[j1 + 1] - a[j3];
  y0r = wk1r * x0r - wk1i * x0i;
  y0i = wk1r * x0i + wk1i * x0r;
  y2r = wk1i * x2r - wk1r * x2i;
  y2i = wk1i * x2i + wk1r * x2r;
  a[j0] = y0r + y2r;
  a[j0 + 1] = y0i + y2i;
  a[j1] = y0r - y2r;
  a[j1 + 1] = y0i - y2i;
  y0r = wk1i * x1r - wk1r * x1i;
  y0i = wk1i * x1i + wk1r * x1r;
  y2r = wk1r * x3r - wk1i * x3i;
  y2i = wk1r * x3i + wk1i * x3r;
  a[j2] = y0r - y2r;
  a[j2 + 1] = y0i - y2i;
  a[j3] = y0r + y2r;
  a[j3 + 1] = y0i + y2i;
}

static void AUP_FFTW_cftfx41(int n, float* a, int nw, float* w) {
  if (n == 128) {
    AUP_FFTW_cftf161(a, &w[nw - 8]);
    AUP_FFTW_cftf162(&a[32], &w[nw - 32]);
    AUP_FFTW_cftf161(&a[64], &w[nw - 8]);
    AUP_FFTW_cftf161(&a[96], &w[nw - 8]);
  } else {
    AUP_FFTW_cftf081(a, &w[nw - 8]);
    AUP_FFTW_cftf082(&a[16], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[32], &w[nw - 8]);
    AUP_FFTW_cftf081(&a[48], &w[nw - 8]);
  }
}

static void AUP_FFTW_cftf161(float* a, float* w) {
  float wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r,
      y1i, y2r, y2i, y3r, y3i, y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, y8r, y8i,
      y9r, y9i, y10r, y10i, y11r, y11i, y12r, y12i, y13r, y13i, y14r, y14i,
      y15r, y15i;

  wn4r = w[1];
  wk1r = w[2];
  wk1i = w[3];
  x0r = a[0] + a[16];
  x0i = a[1] + a[17];
  x1r = a[0] - a[16];
  x1i = a[1] - a[17];
  x2r = a[8] + a[24];
  x2i = a[9] + a[25];
  x3r = a[8] - a[24];
  x3i = a[9] - a[25];
  y0r = x0r + x2r;
  y0i = x0i + x2i;
  y4r = x0r - x2r;
  y4i = x0i - x2i;
  y8r = x1r - x3i;
  y8i = x1i + x3r;
  y12r = x1r + x3i;
  y12i = x1i - x3r;
  x0r = a[2] + a[18];
  x0i = a[3] + a[19];
  x1r = a[2] - a[18];
  x1i = a[3] - a[19];
  x2r = a[10] + a[26];
  x2i = a[11] + a[27];
  x3r = a[10] - a[26];
  x3i = a[11] - a[27];
  y1r = x0r + x2r;
  y1i = x0i + x2i;
  y5r = x0r - x2r;
  y5i = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  y9r = wk1r * x0r - wk1i * x0i;
  y9i = wk1r * x0i + wk1i * x0r;
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  y13r = wk1i * x0r - wk1r * x0i;
  y13i = wk1i * x0i + wk1r * x0r;
  x0r = a[4] + a[20];
  x0i = a[5] + a[21];
  x1r = a[4] - a[20];
  x1i = a[5] - a[21];
  x2r = a[12] + a[28];
  x2i = a[13] + a[29];
  x3r = a[12] - a[28];
  x3i = a[13] - a[29];
  y2r = x0r + x2r;
  y2i = x0i + x2i;
  y6r = x0r - x2r;
  y6i = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  y10r = wn4r * (x0r - x0i);
  y10i = wn4r * (x0i + x0r);
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  y14r = wn4r * (x0r + x0i);
  y14i = wn4r * (x0i - x0r);
  x0r = a[6] + a[22];
  x0i = a[7] + a[23];
  x1r = a[6] - a[22];
  x1i = a[7] - a[23];
  x2r = a[14] + a[30];
  x2i = a[15] + a[31];
  x3r = a[14] - a[30];
  x3i = a[15] - a[31];
  y3r = x0r + x2r;
  y3i = x0i + x2i;
  y7r = x0r - x2r;
  y7i = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  y11r = wk1i * x0r - wk1r * x0i;
  y11i = wk1i * x0i + wk1r * x0r;
  x0r = x1r + x3i;
  x0i = x1i - x3r;
  y15r = wk1r * x0r - wk1i * x0i;
  y15i = wk1r * x0i + wk1i * x0r;
  x0r = y12r - y14r;
  x0i = y12i - y14i;
  x1r = y12r + y14r;
  x1i = y12i + y14i;
  x2r = y13r - y15r;
  x2i = y13i - y15i;
  x3r = y13r + y15r;
  x3i = y13i + y15i;
  a[24] = x0r + x2r;
  a[25] = x0i + x2i;
  a[26] = x0r - x2r;
  a[27] = x0i - x2i;
  a[28] = x1r - x3i;
  a[29] = x1i + x3r;
  a[30] = x1r + x3i;
  a[31] = x1i - x3r;
  x0r = y8r + y10r;
  x0i = y8i + y10i;
  x1r = y8r - y10r;
  x1i = y8i - y10i;
  x2r = y9r + y11r;
  x2i = y9i + y11i;
  x3r = y9r - y11r;
  x3i = y9i - y11i;
  a[16] = x0r + x2r;
  a[17] = x0i + x2i;
  a[18] = x0r - x2r;
  a[19] = x0i - x2i;
  a[20] = x1r - x3i;
  a[21] = x1i + x3r;
  a[22] = x1r + x3i;
  a[23] = x1i - x3r;
  x0r = y5r - y7i;
  x0i = y5i + y7r;
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  x0r = y5r + y7i;
  x0i = y5i - y7r;
  x3r = wn4r * (x0r - x0i);
  x3i = wn4r * (x0i + x0r);
  x0r = y4r - y6i;
  x0i = y4i + y6r;
  x1r = y4r + y6i;
  x1i = y4i - y6r;
  a[8] = x0r + x2r;
  a[9] = x0i + x2i;
  a[10] = x0r - x2r;
  a[11] = x0i - x2i;
  a[12] = x1r - x3i;
  a[13] = x1i + x3r;
  a[14] = x1r + x3i;
  a[15] = x1i - x3r;
  x0r = y0r + y2r;
  x0i = y0i + y2i;
  x1r = y0r - y2r;
  x1i = y0i - y2i;
  x2r = y1r + y3r;
  x2i = y1i + y3i;
  x3r = y1r - y3r;
  x3i = y1i - y3i;
  a[0] = x0r + x2r;
  a[1] = x0i + x2i;
  a[2] = x0r - x2r;
  a[3] = x0i - x2i;
  a[4] = x1r - x3i;
  a[5] = x1i + x3r;
  a[6] = x1r + x3i;
  a[7] = x1i - x3r;
}

static void AUP_FFTW_cftf162(float* a, float* w) {
  float wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, x0r, x0i, x1r, x1i, x2r, x2i,
      y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, y4r, y4i, y5r, y5i, y6r, y6i, y7r,
      y7i, y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, y12r, y12i, y13r, y13i,
      y14r, y14i, y15r, y15i;

  wn4r = w[1];
  wk1r = w[4];
  wk1i = w[5];
  wk3r = w[6];
  wk3i = -w[7];
  wk2r = w[8];
  wk2i = w[9];
  x1r = a[0] - a[17];
  x1i = a[1] + a[16];
  x0r = a[8] - a[25];
  x0i = a[9] + a[24];
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  y0r = x1r + x2r;
  y0i = x1i + x2i;
  y4r = x1r - x2r;
  y4i = x1i - x2i;
  x1r = a[0] + a[17];
  x1i = a[1] - a[16];
  x0r = a[8] + a[25];
  x0i = a[9] - a[24];
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  y8r = x1r - x2i;
  y8i = x1i + x2r;
  y12r = x1r + x2i;
  y12i = x1i - x2r;
  x0r = a[2] - a[19];
  x0i = a[3] + a[18];
  x1r = wk1r * x0r - wk1i * x0i;
  x1i = wk1r * x0i + wk1i * x0r;
  x0r = a[10] - a[27];
  x0i = a[11] + a[26];
  x2r = wk3i * x0r - wk3r * x0i;
  x2i = wk3i * x0i + wk3r * x0r;
  y1r = x1r + x2r;
  y1i = x1i + x2i;
  y5r = x1r - x2r;
  y5i = x1i - x2i;
  x0r = a[2] + a[19];
  x0i = a[3] - a[18];
  x1r = wk3r * x0r - wk3i * x0i;
  x1i = wk3r * x0i + wk3i * x0r;
  x0r = a[10] + a[27];
  x0i = a[11] - a[26];
  x2r = wk1r * x0r + wk1i * x0i;
  x2i = wk1r * x0i - wk1i * x0r;
  y9r = x1r - x2r;
  y9i = x1i - x2i;
  y13r = x1r + x2r;
  y13i = x1i + x2i;
  x0r = a[4] - a[21];
  x0i = a[5] + a[20];
  x1r = wk2r * x0r - wk2i * x0i;
  x1i = wk2r * x0i + wk2i * x0r;
  x0r = a[12] - a[29];
  x0i = a[13] + a[28];
  x2r = wk2i * x0r - wk2r * x0i;
  x2i = wk2i * x0i + wk2r * x0r;
  y2r = x1r + x2r;
  y2i = x1i + x2i;
  y6r = x1r - x2r;
  y6i = x1i - x2i;
  x0r = a[4] + a[21];
  x0i = a[5] - a[20];
  x1r = wk2i * x0r - wk2r * x0i;
  x1i = wk2i * x0i + wk2r * x0r;
  x0r = a[12] + a[29];
  x0i = a[13] - a[28];
  x2r = wk2r * x0r - wk2i * x0i;
  x2i = wk2r * x0i + wk2i * x0r;
  y10r = x1r - x2r;
  y10i = x1i - x2i;
  y14r = x1r + x2r;
  y14i = x1i + x2i;
  x0r = a[6] - a[23];
  x0i = a[7] + a[22];
  x1r = wk3r * x0r - wk3i * x0i;
  x1i = wk3r * x0i + wk3i * x0r;
  x0r = a[14] - a[31];
  x0i = a[15] + a[30];
  x2r = wk1i * x0r - wk1r * x0i;
  x2i = wk1i * x0i + wk1r * x0r;
  y3r = x1r + x2r;
  y3i = x1i + x2i;
  y7r = x1r - x2r;
  y7i = x1i - x2i;
  x0r = a[6] + a[23];
  x0i = a[7] - a[22];
  x1r = wk1i * x0r + wk1r * x0i;
  x1i = wk1i * x0i - wk1r * x0r;
  x0r = a[14] + a[31];
  x0i = a[15] - a[30];
  x2r = wk3i * x0r - wk3r * x0i;
  x2i = wk3i * x0i + wk3r * x0r;
  y11r = x1r + x2r;
  y11i = x1i + x2i;
  y15r = x1r - x2r;
  y15i = x1i - x2i;
  x1r = y0r + y2r;
  x1i = y0i + y2i;
  x2r = y1r + y3r;
  x2i = y1i + y3i;
  a[0] = x1r + x2r;
  a[1] = x1i + x2i;
  a[2] = x1r - x2r;
  a[3] = x1i - x2i;
  x1r = y0r - y2r;
  x1i = y0i - y2i;
  x2r = y1r - y3r;
  x2i = y1i - y3i;
  a[4] = x1r - x2i;
  a[5] = x1i + x2r;
  a[6] = x1r + x2i;
  a[7] = x1i - x2r;
  x1r = y4r - y6i;
  x1i = y4i + y6r;
  x0r = y5r - y7i;
  x0i = y5i + y7r;
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  a[8] = x1r + x2r;
  a[9] = x1i + x2i;
  a[10] = x1r - x2r;
  a[11] = x1i - x2i;
  x1r = y4r + y6i;
  x1i = y4i - y6r;
  x0r = y5r + y7i;
  x0i = y5i - y7r;
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  a[12] = x1r - x2i;
  a[13] = x1i + x2r;
  a[14] = x1r + x2i;
  a[15] = x1i - x2r;
  x1r = y8r + y10r;
  x1i = y8i + y10i;
  x2r = y9r - y11r;
  x2i = y9i - y11i;
  a[16] = x1r + x2r;
  a[17] = x1i + x2i;
  a[18] = x1r - x2r;
  a[19] = x1i - x2i;
  x1r = y8r - y10r;
  x1i = y8i - y10i;
  x2r = y9r + y11r;
  x2i = y9i + y11i;
  a[20] = x1r - x2i;
  a[21] = x1i + x2r;
  a[22] = x1r + x2i;
  a[23] = x1i - x2r;
  x1r = y12r - y14i;
  x1i = y12i + y14r;
  x0r = y13r + y15i;
  x0i = y13i - y15r;
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  a[24] = x1r + x2r;
  a[25] = x1i + x2i;
  a[26] = x1r - x2r;
  a[27] = x1i - x2i;
  x1r = y12r + y14i;
  x1i = y12i - y14r;
  x0r = y13r - y15i;
  x0i = y13i + y15r;
  x2r = wn4r * (x0r - x0i);
  x2i = wn4r * (x0i + x0r);
  a[28] = x1r - x2i;
  a[29] = x1i + x2r;
  a[30] = x1r + x2i;
  a[31] = x1i - x2r;
}

static void AUP_FFTW_cftf081(float* a, float* w) {
  float wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y1r, y1i, y2r,
      y2i, y3r, y3i, y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

  wn4r = w[1];
  x0r = a[0] + a[8];
  x0i = a[1] + a[9];
  x1r = a[0] - a[8];
  x1i = a[1] - a[9];
  x2r = a[4] + a[12];
  x2i = a[5] + a[13];
  x3r = a[4] - a[12];
  x3i = a[5] - a[13];
  y0r = x0r + x2r;
  y0i = x0i + x2i;
  y2r = x0r - x2r;
  y2i = x0i - x2i;
  y1r = x1r - x3i;
  y1i = x1i + x3r;
  y3r = x1r + x3i;
  y3i = x1i - x3r;
  x0r = a[2] + a[10];
  x0i = a[3] + a[11];
  x1r = a[2] - a[10];
  x1i = a[3] - a[11];
  x2r = a[6] + a[14];
  x2i = a[7] + a[15];
  x3r = a[6] - a[14];
  x3i = a[7] - a[15];
  y4r = x0r + x2r;
  y4i = x0i + x2i;
  y6r = x0r - x2r;
  y6i = x0i - x2i;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  x2r = x1r + x3i;
  x2i = x1i - x3r;
  y5r = wn4r * (x0r - x0i);
  y5i = wn4r * (x0r + x0i);
  y7r = wn4r * (x2r - x2i);
  y7i = wn4r * (x2r + x2i);
  a[8] = y1r + y5r;
  a[9] = y1i + y5i;
  a[10] = y1r - y5r;
  a[11] = y1i - y5i;
  a[12] = y3r - y7i;
  a[13] = y3i + y7r;
  a[14] = y3r + y7i;
  a[15] = y3i - y7r;
  a[0] = y0r + y4r;
  a[1] = y0i + y4i;
  a[2] = y0r - y4r;
  a[3] = y0i - y4i;
  a[4] = y2r - y6i;
  a[5] = y2i + y6r;
  a[6] = y2r + y6i;
  a[7] = y2i - y6r;
}

static void AUP_FFTW_cftf082(float* a, float* w) {
  float wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, y0r, y0i, y1r, y1i, y2r, y2i, y3r,
      y3i, y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

  wn4r = w[1];
  wk1r = w[2];
  wk1i = w[3];
  y0r = a[0] - a[9];
  y0i = a[1] + a[8];
  y1r = a[0] + a[9];
  y1i = a[1] - a[8];
  x0r = a[4] - a[13];
  x0i = a[5] + a[12];
  y2r = wn4r * (x0r - x0i);
  y2i = wn4r * (x0i + x0r);
  x0r = a[4] + a[13];
  x0i = a[5] - a[12];
  y3r = wn4r * (x0r - x0i);
  y3i = wn4r * (x0i + x0r);
  x0r = a[2] - a[11];
  x0i = a[3] + a[10];
  y4r = wk1r * x0r - wk1i * x0i;
  y4i = wk1r * x0i + wk1i * x0r;
  x0r = a[2] + a[11];
  x0i = a[3] - a[10];
  y5r = wk1i * x0r - wk1r * x0i;
  y5i = wk1i * x0i + wk1r * x0r;
  x0r = a[6] - a[15];
  x0i = a[7] + a[14];
  y6r = wk1i * x0r - wk1r * x0i;
  y6i = wk1i * x0i + wk1r * x0r;
  x0r = a[6] + a[15];
  x0i = a[7] - a[14];
  y7r = wk1r * x0r - wk1i * x0i;
  y7i = wk1r * x0i + wk1i * x0r;
  x0r = y0r + y2r;
  x0i = y0i + y2i;
  x1r = y4r + y6r;
  x1i = y4i + y6i;
  a[0] = x0r + x1r;
  a[1] = x0i + x1i;
  a[2] = x0r - x1r;
  a[3] = x0i - x1i;
  x0r = y0r - y2r;
  x0i = y0i - y2i;
  x1r = y4r - y6r;
  x1i = y4i - y6i;
  a[4] = x0r - x1i;
  a[5] = x0i + x1r;
  a[6] = x0r + x1i;
  a[7] = x0i - x1r;
  x0r = y1r - y3i;
  x0i = y1i + y3r;
  x1r = y5r - y7r;
  x1i = y5i - y7i;
  a[8] = x0r + x1r;
  a[9] = x0i + x1i;
  a[10] = x0r - x1r;
  a[11] = x0i - x1i;
  x0r = y1r + y3i;
  x0i = y1i - y3r;
  x1r = y5r + y7r;
  x1i = y5i + y7i;
  a[12] = x0r - x1i;
  a[13] = x0i + x1r;
  a[14] = x0r + x1i;
  a[15] = x0i - x1r;
}

static void AUP_FFTW_cftf040(float* a) {
  float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

  x0r = a[0] + a[4];
  x0i = a[1] + a[5];
  x1r = a[0] - a[4];
  x1i = a[1] - a[5];
  x2r = a[2] + a[6];
  x2i = a[3] + a[7];
  x3r = a[2] - a[6];
  x3i = a[3] - a[7];
  a[0] = x0r + x2r;
  a[1] = x0i + x2i;
  a[2] = x1r - x3i;
  a[3] = x1i + x3r;
  a[4] = x0r - x2r;
  a[5] = x0i - x2i;
  a[6] = x1r + x3i;
  a[7] = x1i - x3r;
}

static void AUP_FFTW_cftb040(float* a) {
  float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

  x0r = a[0] + a[4];
  x0i = a[1] + a[5];
  x1r = a[0] - a[4];
  x1i = a[1] - a[5];
  x2r = a[2] + a[6];
  x2i = a[3] + a[7];
  x3r = a[2] - a[6];
  x3i = a[3] - a[7];
  a[0] = x0r + x2r;
  a[1] = x0i + x2i;
  a[2] = x1r + x3i;
  a[3] = x1i - x3r;
  a[4] = x0r - x2r;
  a[5] = x0i - x2i;
  a[6] = x1r - x3i;
  a[7] = x1i + x3r;
}

static void AUP_FFTW_cftx020(float* a) {
  float x0r, x0i;

  x0r = a[0] - a[2];
  x0i = a[1] - a[3];
  a[0] += a[2];
  a[1] += a[3];
  a[2] = x0r;
  a[3] = x0i;
}

static void AUP_FFTW_rftfsub(int n, float* a, int nc, float* c) {
  int j, k, kk, ks, m;
  float wkr, wki, xr, xi, yr, yi;

  m = n >> 1;
  ks = 2 * nc / m;
  kk = 0;
  for (j = 2; j < m; j += 2) {
    k = n - j;
    kk += ks;
    wkr = 0.5f - c[nc - kk];
    wki = c[kk];
    xr = a[j] - a[k];
    xi = a[j + 1] + a[k + 1];
    yr = wkr * xr - wki * xi;
    yi = wkr * xi + wki * xr;
    a[j] -= yr;
    a[j + 1] -= yi;
    a[k] += yr;
    a[k + 1] -= yi;
  }
}

static void AUP_FFTW_rftbsub(int n, float* a, int nc, float* c) {
  int j, k, kk, ks, m;
  float wkr, wki, xr, xi, yr, yi;

  m = n >> 1;
  ks = 2 * nc / m;
  kk = 0;
  for (j = 2; j < m; j += 2) {
    k = n - j;
    kk += ks;
    wkr = 0.5f - c[nc - kk];
    wki = c[kk];
    xr = a[j] - a[k];
    xi = a[j + 1] + a[k + 1];
    yr = wkr * xr + wki * xi;
    yi = wkr * xi - wki * xr;
    a[j] -= yr;
    a[j + 1] -= yi;
    a[k] += yr;
    a[k + 1] -= yi;
  }
}

static void AUP_FFTW_dctsub(int n, float* a, int nc, float* c) {
  int j, k, kk, ks, m;
  float wkr, wki, xr;

  m = n >> 1;
  ks = nc / n;
  kk = 0;
  for (j = 1; j < m; j++) {
    k = n - j;
    kk += ks;
    wkr = c[kk] - c[nc - kk];
    wki = c[kk] + c[nc - kk];
    xr = wki * a[j] - wkr * a[k];
    a[j] = wkr * a[j] + wki * a[k];
    a[k] = xr;
  }
  a[m] *= c[0];
}

static void AUP_FFTW_dstsub(int n, float* a, int nc, float* c) {
  int j, k, kk, ks, m;
  float wkr, wki, xr;

  m = n >> 1;
  ks = nc / n;
  kk = 0;
  for (j = 1; j < m; j++) {
    k = n - j;
    kk += ks;
    wkr = c[kk] - c[nc - kk];
    wki = c[kk] + c[nc - kk];
    xr = wki * a[k] - wkr * a[j];
    a[k] = wkr * a[k] + wki * a[j];
    a[j] = xr;
  }
  a[m] *= c[0];
}

static void AUP_FFTW_rdft(int n, int isgn, float* a, int* ip, float* w) {
  float xi;

  int nw = ip[0];
  int nc = ip[1];

  if (isgn >= 0) {
    if (n > 4) {
      AUP_FFTW_cftfsub(n, a, ip, nw, w);
      AUP_FFTW_rftfsub(n, a, nc, w + nw);
    } else if (n == 4) {
      AUP_FFTW_cftfsub(n, a, ip, nw, w);
    }
    xi = a[0] - a[1];
    a[0] += a[1];
    a[1] = xi;
  } else {
    a[1] = 0.5f * (a[0] - a[1]);
    a[0] -= a[1];
    if (n > 4) {
      AUP_FFTW_rftbsub(n, a, nc, w + nw);
      AUP_FFTW_cftbsub(n, a, ip, nw, w);
    } else if (n == 4) {
      AUP_FFTW_cftbsub(n, a, ip, nw, w);
    }
  }
}

// ==========================================================================================
// public APIs
// ==========================================================================================

void AUP_FFTW_r2c_256(float* in, float* out) {
  int i;

  float tmp[258] = {0};

  // scale
  for (i = 0; i < 256; ++i) {
    tmp[i] = in[i] * 0.00390625f;
  }

  AUP_FFTW_rdft(256, 1, tmp, (int*)AUP_FFTW_g_ip256, (float*)AUP_FFTW_g_w256);

  out[0] = tmp[0];
  out[255] = tmp[1];
  for (i = 1; i < 255; i += 2) {
    out[i] = tmp[i + 1];
    out[i + 1] = -tmp[i + 2];
  }
}

void AUP_FFTW_c2r_256(float* in, float* out) {
  int i;

  out[0] = in[0];
  out[1] = in[255];
  for (i = 2; i < 256; i += 2) {
    out[i] = in[i - 1];
    out[i + 1] = -in[i];
  }

  AUP_FFTW_rdft(256, -1, out, (int*)AUP_FFTW_g_ip256, (float*)AUP_FFTW_g_w256);
  /* 1/N */
  for (i = 0; i < 256; i++) {
    out[i] *= 2;
  }
}

void AUP_FFTW_c2r_512(float* in, float* out) {
  int i;

  out[0] = in[0];
  out[1] = in[511];
  for (i = 2; i < 512; i += 2) {
    out[i] = in[i - 1];
    out[i + 1] = -in[i];
  }
  AUP_FFTW_rdft(512, -1, out, (int*)AUP_FFTW_g_ip512, (float*)AUP_FFTW_g_w512);
  /* 1/N */
  for (i = 0; i < 512; i++) {
    out[i] *= 2;
  }
}

void AUP_FFTW_r2c_512(float* in, float* out) {
  int i;

  float tmp[514] = {0};

  // scale
  for (i = 0; i < 512; ++i) {
    tmp[i] = in[i] * 0.001953125f;
  }

  AUP_FFTW_rdft(512, 1, tmp, (int*)AUP_FFTW_g_ip512, (float*)AUP_FFTW_g_w512);

  out[0] = tmp[0];
  out[511] = tmp[1];
  for (i = 1; i < 511; i += 2) {
    out[i] = tmp[i + 1];
    out[i + 1] = -tmp[i + 2];
  }
}

void AUP_FFTW_r2c_1024(float* in, float* out) {
  int i;

  float tmp[1026] = {0};
  // scale
  for (i = 0; i < 1024; ++i) {
    tmp[i] = in[i] * 0.0009765625f;
  }

  AUP_FFTW_rdft(1024, 1, tmp, (int*)AUP_FFTW_g_ip1024,
                (float*)AUP_FFTW_g_w1024);

  out[0] = tmp[0];
  out[1023] = tmp[1];
  for (i = 1; i < 1023; i += 2) {
    out[i] = tmp[i + 1];
    out[i + 1] = -tmp[i + 2];
  }
}

void AUP_FFTW_c2r_1024(float* in, float* out) {
  int i;
  // memcpy(out,in,sizeof(float)*1024);
  out[0] = in[0];
  out[1] = in[1023];
  for (i = 2; i < 1024; i += 2) {
    out[i] = in[i - 1];
    out[i + 1] = -in[i];
  }
  AUP_FFTW_rdft(1024, -1, out, (int*)AUP_FFTW_g_ip1024,
                (float*)AUP_FFTW_g_w1024);
  /* 1/N */
  for (i = 0; i < 1024; i++) {
    out[i] *= 2;
  }
}

void AUP_FFTW_r2c_2048(float* in, float* out) {
  int i;

  float tmp[2050] = {0};

  // scale
  for (i = 0; i < 2048; ++i) {
    tmp[i] = in[i] * 0.00048828125f;
  }

  AUP_FFTW_rdft(2048, 1, tmp, (int*)AUP_FFTW_g_ip2048,
                (float*)AUP_FFTW_g_w2048);

  out[0] = tmp[0];
  out[2047] = tmp[1];
  for (i = 1; i < 2047; i += 2) {
    out[i] = tmp[i + 1];
    out[i + 1] = -tmp[i + 2];
  }
}

void AUP_FFTW_c2r_2048(float* in, float* out) {
  int i;

  out[0] = in[0];
  out[1] = in[2047];
  for (i = 2; i < 2048; i += 2) {
    out[i] = in[i - 1];
    out[i + 1] = -in[i];
  }
  AUP_FFTW_rdft(2048, -1, out, (int*)AUP_FFTW_g_ip2048,
                (float*)AUP_FFTW_g_w2048);
  /* 1/N */
  for (i = 0; i < 2048; i++) {
    out[i] *= 2;
  }
}

void AUP_FFTW_r2c_4096(float* in, float* out) {
  int i;

  float tmp[4098] = {0};

  // scale
  for (i = 0; i < 4096; ++i) {
    tmp[i] = in[i] * 0.000244140625f;
  }

  AUP_FFTW_rdft(4096, 1, tmp, (int*)AUP_FFTW_g_ip4096,
                (float*)AUP_FFTW_g_w4096);

  out[0] = tmp[0];
  out[4095] = tmp[1];
  for (i = 1; i < 4095; i += 2) {
    out[i] = tmp[i + 1];
    out[i + 1] = -tmp[i + 2];
  }
}

void AUP_FFTW_c2r_4096(float* in, float* out) {
  int i;

  out[0] = in[0];
  out[1] = in[4095];
  for (i = 2; i < 4096; i += 2) {
    out[i] = in[i - 1];
    out[i + 1] = -in[i];
  }
  AUP_FFTW_rdft(4096, -1, out, (int*)AUP_FFTW_g_ip4096,
                (float*)AUP_FFTW_g_w4096);
  for (i = 0; i < 4096; i++) {
    out[i] *= 2;
  }
}

// if direction == 0: format1->format2
// if direction == 1: format2->format1
void AUP_FFTW_InplaceTransf(int direction, int fftSz, float* inplaceTranfBuf) {
  float nyqReal;
  int idx;

  if (direction == 0) {
    // [Real-0, Real-Nyq, Real-1, Imag-1, Real-2, Imag-2, ...] ->
    // [Real-0, Real-1, (-1)*Imag-1, Real-2, (-1)*Imag-2, ..., Real-Nyq]
    nyqReal = inplaceTranfBuf[1];
    for (idx = 1; idx < (fftSz - 1); idx += 2) {
      inplaceTranfBuf[idx] = inplaceTranfBuf[idx + 1];
      inplaceTranfBuf[idx + 1] = -(inplaceTranfBuf[idx + 2]);
    }
    inplaceTranfBuf[fftSz - 1] = nyqReal;
  } else {
    // [Real-0, Real-1, (-1)*Imag-1, Real-2, (-1)*Imag-2, ..., Real-Nyq] ->
    // [Real-0, Real-Nyq, Real-1, Imag-1, Real-2, Imag-2, ...]
    nyqReal = inplaceTranfBuf[fftSz - 1];
    for (idx = fftSz - 1; idx > 2; idx -= 2) {
      inplaceTranfBuf[idx] = -(inplaceTranfBuf[idx - 1]);
      inplaceTranfBuf[idx - 1] = inplaceTranfBuf[idx - 2];
    }
    inplaceTranfBuf[1] = nyqReal;
  }
  return;
}

void AUP_FFTW_RescaleFFTOut(int fftSz, float* inplaceBuf) {
  int idx;
  for (idx = 0; idx < fftSz; idx++) {
    inplaceBuf[idx] *= (float)fftSz;
  }
  return;
}

void AUP_FFTW_RescaleIFFTOut(int fftSz, float* inplaceBuf) {
  int idx;
  for (idx = 0; idx < fftSz; idx++) {
    inplaceBuf[idx] *= 0.5f;
  }
  return;
}

//
// Copyright © 2025 Agora
// This file is part of TEN Framework, an open source project.
// Licensed under the Apache License, Version 2.0, with certain conditions.
// Refer to the "LICENSE" file in the root directory for more information.
//
#include <cassert>
#include "ten_vad.h"
#include "aed_st.h"
#include "aed.h"

static void int16_to_float(const int16_t* inputs, int inputLen, float* output) {
  for (int i = 0; i < inputLen; ++i) {
    output[i] = float(inputs[i]);
  }
}

int ten_vad_create(ten_vad_handle_t* handle, size_t hop_size, float threshold) {
  if (AUP_Aed_create(handle) < 0) {
    return -1;
  }
  Aed_St* stHdl = nullptr;
  Aed_StaticCfg aedStCfg;
  aedStCfg.enableFlag = 1;
  aedStCfg.fftSz = 0;
  aedStCfg.hopSz = hop_size;
  aedStCfg.anaWindowSz = 0;
  aedStCfg.frqInputAvailableFlag = 0;
  stHdl = (Aed_St*)(*handle);
  stHdl->dynamCfg.extVoiceThr = threshold;

  if (AUP_Aed_memAllocate(*handle, &aedStCfg) < 0) {
    return -1;
  }
  if (AUP_Aed_init(*handle) < 0) {
    return -1;
  }
  return 0;
}

int ten_vad_process(ten_vad_handle_t handle, const int16_t* audio_data,
                    size_t audio_data_length, float* out_probability,
                    int* out_flag) {
  if (handle == nullptr || audio_data == nullptr ||
      out_probability == nullptr || out_flag == nullptr) {
    return -1;
  }
  Aed_St* ptr = (Aed_St*)handle;
  assert(audio_data_length == ptr->stCfg.hopSz);
  int16_to_float(audio_data, audio_data_length, ptr->inputFloatBuff);
  Aed_InputData aedInputData;
  Aed_OutputData aedOutputData;
  aedInputData.binPower = NULL;
  aedInputData.hopSz = ptr->stCfg.hopSz;
  aedInputData.nBins = -1;
  aedInputData.timeSignal = ptr->inputFloatBuff;
  int ret = AUP_Aed_proc(handle, &aedInputData, &aedOutputData);
  if (ret == 0) {
    *out_probability = aedOutputData.voiceProb;
    *out_flag = aedOutputData.vadRes;
  }
  return ret;
}

int ten_vad_destroy(ten_vad_handle_t* handle) {
  return AUP_Aed_destroy(handle);
}

const char* ten_vad_get_version(void) { return "1.0"; }
