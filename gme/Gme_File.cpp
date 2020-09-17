// Game_Music_Emu 0.6.0. http://www.slack.net/~ant/

#include "Gme_File.h"

#include <cassert>
using namespace std;

#include <string.h>

/* Copyright (C) 2003-2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

const char* const gme_wrong_file_type = "Wrong file type for this emulator";

void Gme_File::clear_playlist() {
    clear_playlist_();
    track_count_ = raw_track_count_;
}

void Gme_File::unload() {
    clear_playlist(); // *before* clearing track count
    track_count_ = 0;
    raw_track_count_ = 0;
    file_data.clear();
}

Gme_File::Gme_File() {
    type_ = 0;
    user_data_ = 0;
    user_cleanup_ = 0;
    unload();                   // clears fields
}

Gme_File::~Gme_File() {
    if (user_cleanup_)
        user_cleanup_(user_data_);
}

const char* Gme_File::load_mem_(byte const* data, long size) {
    assert(data != file_data.data()); // load_mem_() or load_() must be overridden
    Mem_File_Reader in(data, size);
    return load_(in);
}

const char* Gme_File::load_(Data_Reader& in) {
    file_data.resize(in.remain());
    if (const char* res = in.read(file_data.data(), file_data.size())) return res;
    return load_mem_(file_data.data(), file_data.size());
}

// public load functions call this at beginning
void Gme_File::pre_load() {
    unload();
}

void Gme_File::post_load_() {
}

// public load functions call this at end
const char* Gme_File::post_load(const char* err) {
    if (!track_count())
        set_track_count(type()->track_count);
    if (!err)
        post_load_();
    else
        unload();

    return err;
}

// Public load functions

const char* Gme_File::load_mem(void const* in, long size) {
    pre_load();
    return post_load(load_mem_((byte const*)in, size));
}

const char* Gme_File::load(Data_Reader& in) {
    pre_load();
    return post_load(load_(in));
}

const char* Gme_File::load_file(const char* path) {
    pre_load();
    GME_FILE_READER in;
    if (const char * res = in.open(path)) return res;
    return post_load(load_(in));
}

const char* Gme_File::load_remaining_(void const* h, long s, Data_Reader& in) {
    Remaining_Reader rem(h, s, &in);
    return load(rem);
}

// Track info

void Gme_File::copy_field_(char* out, const char* in, int in_size) {
    if (!in || !*in)
        return;

    // remove spaces/junk from beginning
    while (in_size && unsigned(*in - 1) <= ' ' - 1) {
        in++;
        in_size--;
    }

    // truncate
    if (in_size > max_field_)
        in_size = max_field_;

    // find terminator
    int len = 0;
    while (len < in_size && in[len])
        len++;

    // remove spaces/junk from end
    while (len && unsigned(in[len - 1]) <= ' ')
        len--;

    // copy
    out[len] = 0;
    memcpy(out, in, len);

    // strip out stupid fields that should have been left blank
    if (!strcmp(out, "?") || !strcmp(out, "<?>") || !strcmp(out, "< ? >"))
        out[0] = 0;
}

void Gme_File::copy_field_(char* out, const char* in) {
    copy_field_(out, in, max_field_);
}

const char* Gme_File::remap_track_(int* track_io) const {
    if ((unsigned)*track_io >= (unsigned)track_count())
        return "Invalid track";
    return 0;
}

const char* Gme_File::track_info(track_info_t* out, int track) const {
    out->track_count = track_count();
    out->length = -1;
    out->loop_length = -1;
    out->intro_length = -1;
    out->song[0] = 0;

    out->game[0] = 0;
    out->author[0] = 0;
    out->copyright[0] = 0;
    out->comment[0] = 0;
    out->dumper[0] = 0;
    out->system[0] = 0;

    copy_field_(out->system, type()->system);

    int remapped = track;
    if(const char* res = remap_track_(&remapped)) return res;
    if(const char* res = track_info_(out, remapped)) return res;

    return 0;
}
