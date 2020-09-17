// Multi-channel effects buffer with panning, echo and reverb

// Game_Music_Emu 0.6.0
#ifndef EFFECTS_BUFFER_H
#define EFFECTS_BUFFER_H

#include <vector>
using namespace std;

#include "Multi_Buffer.h"

// Effects_Buffer uses several buffers and outputs stereo sample pairs.
class Effects_Buffer : public Multi_Buffer {
public:
    // If center_only is true, only center buffers are created and
    // less memory is used.
    Effects_Buffer(bool center_only = false);

    // Channel  Effect    Center Pan
    // ---------------------------------
    //    0,5    reverb       pan_1
    //    1,6    reverb       pan_2
    //    2,7    echo         -
    //    3      echo         -
    //    4      echo         -

    // Channel configuration
    struct config_t {
        double pan_1; // -1.0 = left, 0.0 = center, 1.0 = right
        double pan_2;
        double echo_delay;     // msec
        double echo_level;     // 0.0 to 1.0
        double reverb_delay;   // msec
        double delay_variance; // difference between left/right delays (msec)
        double reverb_level;   // 0.0 to 1.0
        bool effects_enabled;  // if false, use optimized simple mixer
        config_t();
    };

    // Set configuration of buffer
    virtual void config(const config_t&);
    void set_depth(double);

public:
    ~Effects_Buffer();
    const char* set_sample_rate(long samples_per_sec, int msec = blip_default_length);
    void clock_rate(long);
    void bass_freq(int);
    void clear();
    channel_t channel(int, int);
    void end_frame(blip_time_t);
    long read_samples(short*, long);
    long samples_avail() const;

private:
    typedef long fixed_t;

    enum { max_buf_count = 7 };
    Blip_Buffer bufs[max_buf_count];
    enum { chan_types_count = 3 };
    channel_t chan_types[3];
    config_t config_;
    long stereo_remain;
    long effect_remain;
    int buf_count;
    bool effects_enabled;

    vector<short> reverb_buf;
    vector<short> echo_buf;
    int reverb_pos;
    int echo_pos;

    struct {
        fixed_t pan_1_levels[2];
        fixed_t pan_2_levels[2];
        int echo_delay_l;
        int echo_delay_r;
        fixed_t echo_level;
        int reverb_delay_l;
        int reverb_delay_r;
        fixed_t reverb_level;
    } chans;

    void mix_mono(short*, long);
    void mix_stereo(short*, long);
    void mix_enhanced(short*, long);
    void mix_mono_enhanced(short*, long);
};

#endif
