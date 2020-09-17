#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <FLAC/all.h>
#include <gme.h>

using namespace std;
namespace fs = filesystem;

const int kSampleRate = 44100;
const int kTotalChannel = 2;
const int kRefreshRate = 60;
const int kSampleCount = kSampleRate / kRefreshRate;

vector<uint8_t> toFlac(const vector<int>& raw, const string& title) {
    vector<uint8_t> result;
    int sampleCount = raw.size() / kTotalChannel;

    string write = "TITLE=" + title;

    FLAC__StreamMetadata_VorbisComment_Entry key{(uint32_t)write.size(), (uint8_t*)write.data()};
    FLAC__StreamMetadata* meta = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);
    FLAC__metadata_object_vorbiscomment_append_comment(meta, key, true);

    FLAC__StreamEncoder* encoder = FLAC__stream_encoder_new();
    FLAC__stream_encoder_set_verify(encoder, true);
    FLAC__stream_encoder_set_compression_level(encoder, 5);
    FLAC__stream_encoder_set_channels(encoder, kTotalChannel);
    FLAC__stream_encoder_set_bits_per_sample(encoder, 16);
    FLAC__stream_encoder_set_sample_rate(encoder, kSampleRate);
    FLAC__stream_encoder_set_total_samples_estimate(encoder, sampleCount);
    FLAC__stream_encoder_set_metadata(encoder, &meta, 1);
    FLAC__stream_encoder_init_stream(
        encoder,
        [](const FLAC__StreamEncoder*, const FLAC__byte buffer[], size_t bytes, unsigned, unsigned, void* client_data) -> FLAC__StreamEncoderWriteStatus {
            vector<uint8_t>* result = (vector<uint8_t>*)client_data;
            copy(buffer, buffer + bytes, back_inserter(*result));
            return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
        },
        nullptr, nullptr, nullptr, &result);
    FLAC__stream_encoder_process_interleaved(encoder, raw.data(), sampleCount);
    FLAC__stream_encoder_finish(encoder);
    FLAC__stream_encoder_delete(encoder);
    FLAC__metadata_object_delete(meta);
    return result;
}

vector<int> workOnTrack(Music_Emu* emu, int track) {
    static int limit = 3 * 60 * kTotalChannel * kSampleRate;
    vector<short> buf(kSampleCount * kTotalChannel, 0);
    vector<int> res;
    gme_start_track(emu, track);
    while (res.size() < limit && !gme_track_ended(emu)) {
        gme_play(emu, buf.size(), buf.data());
        copy(buf.begin(), buf.end(), back_inserter(res));
    }
    return res;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Need nsf file" << endl;
        return 0;
    }

    fs::path nsf(argv[1]);

    Music_Emu* emu = nullptr;
    gme_open_file(argv[1], &emu, 44100);
    gme_set_stereo_depth(emu, 0.2);
    int total = gme_track_count(emu);

    filesystem::create_directory(nsf.root_path() / nsf.stem());
    for (int i = 0; i < total; ++i) {
        vector<int> raw = workOnTrack(emu, i);
        vector<uint8_t> flac = toFlac(raw, to_string(i + 1));
        ofstream out;
        out.open(nsf.root_path() / nsf.stem() / (to_string(i + 1) + ".flac"), ios::binary | ios::out);
        out.write((char*)flac.data(), flac.size());
        cout << i + 1 << "/" << total << "\r";
        cout.flush();
    }
    gme_delete(emu);

    return 0;
}
