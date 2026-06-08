#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <onnxruntime_cxx_api.h>

// Available languages for multilingual TTS
extern const std::vector<std::string> AVAILABLE_LANGS;

/**
 * Configuration structure
 */
struct Config {
    struct AEConfig {
        int sample_rate;
        int base_chunk_size;
    } ae;
    
    struct TTLConfig {
        int chunk_compress_factor;
        int latent_dim;
    } ttl;
};

/**
 * Unicode text processor
 */
class UnicodeProcessor {
public:
    explicit UnicodeProcessor(const std::string& unicode_indexer_json_path);

    // Process text list to text IDs and mask
    void call(
        const std::vector<std::string>& text_list,
        const std::vector<std::string>& lang_list,
        std::vector<std::vector<int64_t>>& text_ids,
        std::vector<std::vector<std::vector<float>>>& text_mask
    );

private:
    std::vector<int64_t> indexer_;
    
    std::string preprocessText(const std::string& text, const std::string& lang);
    std::vector<uint16_t> textToUnicodeValues(const std::string& text);
    std::vector<std::vector<std::vector<float>>> getTextMask(
        const std::vector<int64_t>& text_ids_lengths
    );
};

/**
 * Style class
 */
class Style {
public:
    Style(const std::vector<float>& ttl_data, const std::vector<int64_t>& ttl_shape,
          const std::vector<float>& dp_data, const std::vector<int64_t>& dp_shape);
    
    const std::vector<float>& getTtlData() const { return ttl_data_; }
    const std::vector<float>& getDpData() const { return dp_data_; }
    const std::vector<int64_t>& getTtlShape() const { return ttl_shape_; }
    const std::vector<int64_t>& getDpShape() const { return dp_shape_; }

private:
    std::vector<float> ttl_data_;
    std::vector<float> dp_data_;
    std::vector<int64_t> ttl_shape_;
    std::vector<int64_t> dp_shape_;
};

/**
 * TextToSpeech class
 */
struct OnnxModels {
    std::unique_ptr<Ort::Session> dp;
    std::unique_ptr<Ort::Session> text_enc;
    std::unique_ptr<Ort::Session> vector_est;
    std::unique_ptr<Ort::Session> vocoder;
};

class TextToSpeech {
public:
    std::vector<std::vector<float>> g_tensor_buffers_float;
    std::vector<std::vector<int64_t>> g_tensor_buffers_int64;

    OnnxModels static_models;
    std::unique_ptr<UnicodeProcessor> static_text_processor;

    TextToSpeech(
        const Config& cfgs,
        UnicodeProcessor* text_processor,
        Ort::Session* dp_ort,
        Ort::Session* text_enc_ort,
        Ort::Session* vector_est_ort,
        Ort::Session* vocoder_ort
    );
    
    struct SynthesisResult {
        std::vector<float> wav;
        std::vector<float> duration;
    };
    
    SynthesisResult call(
        Ort::MemoryInfo& memory_info,
        const std::string& text,
        const std::string& lang,
        const Style& style,
        int total_step,
        float speed = 1.05f,
        float silence_duration = 0.3f
    );
    
    SynthesisResult batch(
        Ort::MemoryInfo& memory_info,
        const std::vector<std::string>& text_list,
        const std::vector<std::string>& lang_list,
        const Style& style,
        int total_step,
        float speed = 1.05f
    );
    
    int getSampleRate() const { return sample_rate_; }

private:
    SynthesisResult _infer(
        Ort::MemoryInfo& memory_info,
        const std::vector<std::string>& text_list,
        const std::vector<std::string>& lang_list,
        const Style& style,
        int total_step,
        float speed = 1.05f
    );
    Config cfgs_;
    UnicodeProcessor* text_processor_;
    Ort::Session* dp_ort_;
    Ort::Session* text_enc_ort_;
    Ort::Session* vector_est_ort_;
    Ort::Session* vocoder_ort_;
    int sample_rate_;
    int base_chunk_size_;
    int chunk_compress_factor_;
    int ldim_;
    
    void sampleNoisyLatent(
        const std::vector<float>& duration,
        std::vector<std::vector<std::vector<float>>>& noisy_latent,
        std::vector<std::vector<std::vector<float>>>& latent_mask
    );
};

// Utility functions
std::vector<std::vector<std::vector<float>>> lengthToMask(
    const std::vector<int64_t>& lengths, int max_len = -1
);

std::vector<std::vector<std::vector<float>>> getLatentMask(
    const std::vector<int64_t>& wav_lengths,
    int base_chunk_size,
    int chunk_compress_factor
);

// ONNX model loading
std::unique_ptr<Ort::Session> loadOnnx(
    Ort::Env& env,
    const std::string& onnx_path,
    const Ort::SessionOptions& opts
);

OnnxModels loadOnnxAll(
    Ort::Env& env,
    const std::string& onnx_dir,
    const Ort::SessionOptions& opts
);

// Configuration and processor loading
Config loadCfgs(const std::string& onnx_dir);

std::unique_ptr<UnicodeProcessor> loadTextProcessor(const std::string& onnx_dir);

// Voice style loading
Style loadVoiceStyle(const std::vector<std::string>& voice_style_paths, bool verbose = false);

// TextToSpeech loading
std::unique_ptr<TextToSpeech> loadTextToSpeech(
    Ort::Env& env,
    const std::string& onnx_dir,
    bool use_gpu = false
);

// WAV file writing
void writeWavFile(
    const std::string& filename,
    const std::vector<float>& audio_data,
    int sample_rate
);

// Tensor conversion utilities
void clearTensorBuffers(TextToSpeech *ref);

Ort::Value arrayToTensor(
    TextToSpeech *ref,
    Ort::MemoryInfo& memory_info,
    const std::vector<std::vector<std::vector<float>>>& array,
    const std::vector<int64_t>& dims
);

Ort::Value intArrayToTensor(
    TextToSpeech *ref,
    Ort::MemoryInfo& memory_info,
    const std::vector<std::vector<int64_t>>& array,
    const std::vector<int64_t>& dims
);

// JSON loading helpers
std::vector<int64_t> loadJsonInt64(const std::string& file_path);

// Timer utility
template<typename Func>
auto timer(const std::string& name, Func&& func) -> decltype(func()) {
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << name << "..." << std::endl;
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "  -> " << name << " completed in " 
              << std::fixed << std::setprecision(2) << elapsed.count() << " sec" << std::endl;
    return result;
}

// Sanitize filename
std::string sanitizeFilename(const std::string& text, int max_len);

// Chunk text into manageable segments
std::vector<std::string> chunkText(const std::string& text, int max_len = 300);
