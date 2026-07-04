#include "../includes/ConfigHelper.h"
#include <windows.h>
#include <stdio.h>

ConfigHelper::ConfigHelper(const std::string& iniPath) : path(iniPath) {}

bool ConfigHelper::Exists() {
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

int ConfigHelper::ReadInt(const char* sec, const char* key, int def) {
    return GetPrivateProfileIntA(sec, key, def, path.c_str());
}

float ConfigHelper::ReadFloat(const char* sec, const char* key, float def) {
    char buf[64];
    if (GetPrivateProfileStringA(sec, key, "", buf, sizeof(buf), path.c_str()) > 0) {
        return (float)atof(buf);
    }
    return def;
}

CVector2D ConfigHelper::ReadVec(const char* sec, const char* key, CVector2D def) {
    char buf[128];
    if (GetPrivateProfileStringA(sec, key, "", buf, sizeof(buf), path.c_str()) > 0) {
        float x = 0.0f, y = 0.0f;
        if (sscanf_s(buf, "%f %f", &x, &y) == 2) {
            return CVector2D(x, y);
        }
    }
    return def;
}

CRect ConfigHelper::ReadRect(const char* sec, const char* key, CRect def) {
    char buf[256];
    if (GetPrivateProfileStringA(sec, key, "", buf, sizeof(buf), path.c_str()) > 0) {
        float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
        if (sscanf_s(buf, "%f %f %f %f", &left, &top, &right, &bottom) == 4) {
            return CRect(left, top, right, bottom);
        }
    }
    return def;
}

CRGBA ConfigHelper::ReadRGBA(const char* sec, const char* key, CRGBA def) {
    char buf[128];
    if (GetPrivateProfileStringA(sec, key, "", buf, sizeof(buf), path.c_str()) > 0) {
        int r = 0, g = 0, b = 0, a = 255;
        if (sscanf_s(buf, "%d %d %d %d", &r, &g, &b, &a) >= 3) {
            return CRGBA(r, g, b, a);
        }
    }
    return def;
}

std::string ConfigHelper::ReadString(const char* sec, const char* key, const std::string& def) {
    char buf[512];
    size_t hashPos;
    std::string s;

    if (GetPrivateProfileStringA(sec, key, def.c_str(), buf, sizeof(buf), path.c_str()) > 0) {
        s = buf;
        hashPos = s.find('#');
        if (hashPos != std::string::npos) {
            s = s.substr(0, hashPos);
            while (!s.empty() && isspace((unsigned char)s.back())) {
                s.pop_back();
            }
        }
        return s;
    }
    return def;
}

void ConfigHelper::WriteInt(const char* sec, const char* key, int val, const char* comment) {
    char buf[128];
    if (comment && comment[0] != '\0') {
        sprintf_s(buf, "%d #%s", val, comment);
    } else {
        sprintf_s(buf, "%d", val);
    }
    WritePrivateProfileStringA(sec, key, buf, path.c_str());
}

void ConfigHelper::WriteFloat(const char* sec, const char* key, float val, const char* comment) {
    char buf[128];
    if (comment && comment[0] != '\0') {
        sprintf_s(buf, "%.2f #%s", val, comment);
    } else {
        sprintf_s(buf, "%.2f", val);
    }
    WritePrivateProfileStringA(sec, key, buf, path.c_str());
}

void ConfigHelper::WriteVec(const char* sec, const char* key, CVector2D val, const char* comment) {
    char buf[128];
    if (comment && comment[0] != '\0') {
        sprintf_s(buf, "%.2f %.2f #%s", val.x, val.y, comment);
    } else {
        sprintf_s(buf, "%.2f %.2f", val.x, val.y);
    }
    WritePrivateProfileStringA(sec, key, buf, path.c_str());
}

void ConfigHelper::WriteRect(const char* sec, const char* key, CRect val, const char* comment) {
    char buf[256];
    if (comment && comment[0] != '\0') {
        sprintf_s(buf, "%.2f %.2f %.2f %.2f #%s", val.left, val.top, val.right, val.bottom, comment);
    } else {
        sprintf_s(buf, "%.2f %.2f %.2f %.2f", val.left, val.top, val.right, val.bottom);
    }
    WritePrivateProfileStringA(sec, key, buf, path.c_str());
}

void ConfigHelper::WriteRGBA(const char* sec, const char* key, CRGBA val, const char* comment) {
    char buf[256];
    if (comment && comment[0] != '\0') {
        sprintf_s(buf, "%d %d %d %d #%s", val.r, val.g, val.b, val.a, comment);
    } else {
        sprintf_s(buf, "%d %d %d %d", val.r, val.g, val.b, val.a);
    }
    WritePrivateProfileStringA(sec, key, buf, path.c_str());
}

void ConfigHelper::WriteString(const char* sec, const char* key, const std::string& val, const char* comment) {
    char buf[512];
    if (comment && comment[0] != '\0') {
        sprintf_s(buf, "%s #%s", val.c_str(), comment);
    } else {
        sprintf_s(buf, "%s", val.c_str());
    }
    WritePrivateProfileStringA(sec, key, buf, path.c_str());
}

int ConfigHelper::ReadIntArray(const char* sec, const char* key, int16_t* out, int count, const int16_t* def) {
    char buf[4096];
    if (GetPrivateProfileStringA(sec, key, "", buf, sizeof(buf), path.c_str()) > 0) {
        int parsed = 0;
        char* ctx = nullptr;
        char* tok = strtok_s(buf, ", \t", &ctx);
        while (tok && parsed < count) {
            out[parsed++] = (int16_t)atoi(tok);
            tok = strtok_s(nullptr, ", \t", &ctx);
        }
        return parsed;
    }
    if (def) {
        memcpy(out, def, count * sizeof(int16_t));
    }
    return 0;
}

void ConfigHelper::WriteIntArray(const char* sec, const char* key, const int16_t* arr, int count, const char* comment) {
    int i;
    char num[16];
    std::string s;

    for (i = 0; i < count; i++) {
        if (i > 0) s += ", ";
        sprintf_s(num, "%d", arr[i]);
        s += num;
    }
    if (comment && comment[0] != '\0') {
        s += " #";
        s += comment;
    }
    WritePrivateProfileStringA(sec, key, s.c_str(), path.c_str());
}

void ConfigHelper::Format(const std::string& headerComments) {
    FILE* f = nullptr;
    fopen_s(&f, path.c_str(), "r");
    if (!f) return;
    
    std::string content;
    if (!headerComments.empty()) {
        content += headerComments;
        if (headerComments.back() != '\n') {
            content += "\n";
        }
    }
    
    char line[512];
    bool firstSection = true;
    while (fgets(line, sizeof(line), f)) {
        std::string l = line;
        if (l[0] == ';' || l[0] == '#') {
            continue;
        }
        if (l[0] == '[') {
            if (!firstSection) {
                content += "\n";
            }
            firstSection = false;
        }
        content += l;
    }
    fclose(f);
    
    fopen_s(&f, path.c_str(), "w");
    if (f) {
        fputs(content.c_str(), f);
        fclose(f);
    }
}
