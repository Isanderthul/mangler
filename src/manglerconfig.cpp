/*
 * vim: softtabstop=4 shiftwidth=4 cindent foldmethod=marker expandtab
 *
 * $LastChangedDate: 2009-10-10 12:38:51 -0700 (Sat, 10 Oct 2009) $
 * $Revision: 63 $
 * $LastChangedBy: ekilfoil $
 * $URL: http://svn.mangler.org/mangler/trunk/src/manglersettings.h $
 *
 * Copyright 2009 Eric Kilfoil 
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
		
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mangler.h"
#include "manglerconfig.h"


ManglerConfig::ManglerConfig() {/*{{{*/
    lv3_debuglevel              = 0;
    PushToTalkKeyEnabled        = false;
    PushToTalkKeyValue          = "";
    PushToTalkMouseEnabled      = false;
    PushToTalkMouseValue        = "";
    ManglerServerConfig         qc_lastserver;
    std::vector<ManglerServerConfig> serverlist;
    load();
}/*}}}*/
bool ManglerConfig::save() {/*{{{*/
    // We're going to do this in one in C because it's easier for me...
    char buf[1024];
    bool notified;

    mutex.lock();
    Glib::ustring     cfgfilename = getenv("HOME");
    cfgfilename += "/.manglerrc";
    if (! (this->cfgstream = fopen((char *)cfgfilename.c_str(), "w"))) {
        fprintf(stderr, "could not open settings file for writing: %s\n", (char *)cfgfilename.c_str());
        mutex.unlock();
        return false;
    }
    put("PushToTalkKeyEnabled", PushToTalkKeyEnabled);
    put("PushToTalkKeyValue", PushToTalkKeyValue);
    put("PushToTalkMouseEnabled", PushToTalkMouseEnabled);
    put("PushToTalkMouseValue", PushToTalkMouseValue);
    put("qc_lastserver.name", qc_lastserver.name);
    put("qc_lastserver.port", qc_lastserver.port);
    put("qc_lastserver.username", qc_lastserver.username);
    put("qc_lastserver.password", qc_lastserver.password);
    put("qc_lastserver.phonetic", "");
    put("qc_lastserver.comment", "");
    put("lastConnectedServerId", "-1");
    // TODO: saveServerList();
    fclose(this->cfgstream);
    mutex.unlock();
}/*}}}*/
bool ManglerConfig::put(Glib::ustring name, Glib::ustring value) {/*{{{*/
    char buf[1024];
    snprintf(buf, 1023, "%s=%s\n", (char *)name.c_str(), (char *)value.c_str());
    if (!fputs(buf, this->cfgstream)) {
        return false;
    }
    return true;
}/*}}}*/
bool ManglerConfig::put(Glib::ustring name, uint32_t value) {/*{{{*/
    char buf[1024];
    snprintf(buf, 1023, "%s=%d\n", (char *)name.c_str(), value);
    if (!fputs(buf, this->cfgstream)) {
        return false;
    }
    return true;
}/*}}}*/
bool ManglerConfig::put(Glib::ustring name, bool value) {/*{{{*/
    char buf[1024];
    snprintf(buf, 1023, "%s=%d\n", (char *)name.c_str(), value ? 1 : 0);
    if (!fputs(buf, this->cfgstream)) {
        return false;
    }
    return true;
}/*}}}*/
Glib::ustring ManglerConfig::get(Glib::ustring cfgname) {/*{{{*/
    // We're going to do this in one in C too...
    struct stat cfgstat;
    static bool notified = false;
    char buf[1024];
    int ctr = 0;

    mutex.lock();
    Glib::ustring     cfgfilename = getenv("HOME");
    cfgfilename += "/.manglerrc";
    // check to see if the file exists
    if (stat(cfgfilename.c_str(), &cfgstat)) {
        // if not create it
        if (! (this->cfgstream = fopen(cfgfilename.c_str(), "w"))) {
            // if creation fails, print error
            if (!notified) {
                fprintf(stderr, "could not create settings file: %s\n", (char *)cfgfilename.c_str());
                notified = true;
            }
            mutex.unlock();
            return "";
        } else {
            fclose(this->cfgstream);
        }
    }
    if (! (this->cfgstream = fopen(cfgfilename.c_str(), "r"))) {
        if (!notified) {
            fprintf(stderr, "could not open settings file for reading: %s\n", (char *)cfgfilename.c_str());
            notified = true;
        }
        mutex.unlock();
    }

    while (! feof(this->cfgstream)) {
        char *name, *value;
        if (! fgets(buf, 1024, this->cfgstream)) {
            fclose(this->cfgstream);
            mutex.unlock();
            return "";
        }
        ctr++;
        if (buf[strlen(buf)-1] != '\n') {
            fprintf(stderr, "error in settings file: line %d is longer than 1024 characters\n", ctr);
            fclose(this->cfgstream);
            mutex.unlock();
            return "";
        }
        buf[strlen(buf)-1] = '\0';
        name = buf;
        if ((value = strchr(buf, '=')) == NULL) {
            continue;
        }
        *value = '\0';
        value++;
        if (strcmp((char *)cfgname.c_str(), name) == 0) {
            Glib::ustring uvalue = value;
            fclose(this->cfgstream);
            mutex.unlock();
            return uvalue;
        }
    }
    fclose(this->cfgstream);
    mutex.unlock();
    return "";
}/*}}}*/
void ManglerConfig::load() {/*{{{*/
    PushToTalkKeyEnabled        = get("PushToTalkKeyEnabled") == "0" ? false : true;
    PushToTalkKeyValue          = get("PushToTalkKeyValue");
    PushToTalkMouseEnabled      = get("PushToTalkMouseEnabled") == "0" ? false : true;
    PushToTalkMouseValue        = get("PushToTalkMouseValue");
    qc_lastserver.name          = get("qc_lastserver.name");
    qc_lastserver.port          = get("qc_lastserver.port");
    qc_lastserver.username      = get("qc_lastserver.username");
    qc_lastserver.password      = get("qc_lastserver.password");
    qc_lastserver.phonetic      = get("qc_lastserver.phonetic");
    qc_lastserver.comment       = get("qc_lastserver.comment");
}/*}}}*/

