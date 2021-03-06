/*
 * vim: softtabstop=4 shiftwidth=4 cindent foldmethod=marker expandtab
 *
 * $LastChangedDate$
 * $Revision$
 * $LastChangedBy$
 * $URL$
 *
 * Copyright 2009-2011 Eric Connell
 * Copyright 2010-2011 Roman Tetelman
 *
 * This file is part of Mangler.
 *
 * Mangler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mangler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mangler.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MANGLER_ALSA_H
#define _MANGLER_ALSA_H
#ifdef HAVE_ALSA

#include <alsa/asoundlib.h>
#define ALSA_BUF 640

class ManglerAlsa : public ManglerBackend {
    snd_pcm_t       *alsa_stream;
    snd_pcm_sframes_t alsa_frames;
    int             alsa_error;
    uint32_t        pcm_framesize;
    uint8_t         channels;
public:
    virtual bool            open(int type, Glib::ustring device, int rate, int channels);
    virtual void            close(bool drain = false);
    virtual bool            write(uint8_t *sample, uint32_t length, int channels);
    virtual bool            read(uint8_t* buf);
    virtual Glib::ustring   getAudioSubsystem(void);
    ManglerAlsa(uint32_t rate, uint8_t channels, uint32_t pcm_framesize);
    virtual ~ManglerAlsa();

    static void             getDeviceList(std::vector<ManglerAudioDevice*>& inputDevices, std::vector<ManglerAudioDevice*>& outputDevices);
};

#endif
#endif

