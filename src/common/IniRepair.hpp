// DK2VR 0.9.47.63 - made by LoVeMaKeRz
// IniRepair: heals DK2VR.ini. Windows' GetPrivateProfile*/WritePrivateProfile* do not understand a UTF-8 byte-order mark: with one at the start
// of the file the first "[VR]" header is not recognised, every setting under it is ignored (defaults are used: Possession360 -> 0 = no 360 view)
// and the first setting the mod saves (table size, tilt...) appends a SECOND "[VR]" section at the end of the file.
// repair() removes the mark and merges the repeated sections (a later value wins: it is the one the player saved most recently).
// Everything else (comments, order, line endings of the first section) is kept. Nothing is touched when the file is already healthy.
#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iterator>
#include <filesystem>
#include <system_error>
namespace dk2vr { namespace inirepair {
struct Result {bool changed=false;bool bom=false;int mergedSections=0;int mergedKeys=0;};
inline std::string lower(std::string s) {for(char& c:s) c=char(std::tolower(static_cast<unsigned char>(c)));return s;}
inline std::string trim(const std::string& s) {size_t a=0,b=s.size();while(a<b&&(s[a]==' '||s[a]=='\t')) ++a;while(b>a&&(s[b-1]==' '||s[b-1]=='\t'||s[b-1]=='\r')) --b;return s.substr(a,b-a);}
inline std::string repair(const std::string& input,Result& r) {
    r=Result{};
    std::string text=input;
    if(text.size()>=3&&static_cast<unsigned char>(text[0])==0xEF&&static_cast<unsigned char>(text[1])==0xBB&&static_cast<unsigned char>(text[2])==0xBF) {text.erase(0,3);r.bom=true;}
    // lines (without line ending)
    std::vector<std::string> lines;
    {size_t i=0;while(i<=text.size()) {size_t j=text.find('\n',i);if(j==std::string::npos) {if(i<text.size()) lines.push_back(text.substr(i));break;}std::string l=text.substr(i,j-i);if(!l.empty()&&l.back()=='\r') l.pop_back();lines.push_back(l);i=j+1;}}
    struct Section {std::string name,key;std::vector<std::string> body;};
    std::vector<Section> sections;sections.push_back(Section{});   // [0] = lines before any header (kept as they are)
    std::vector<int> firstOf;                                          // index of the first section with this key, per section
    for(const auto& l:lines) {
        const std::string t=trim(l);
        if(t.size()>=3&&t.front()=='['&&t.back()==']') {Section s;s.name=t;s.key=lower(t);sections.push_back(s);continue;}
        sections.back().body.push_back(l);
    }
    // merge repeated sections into the first one with the same name
    std::vector<Section> out;out.push_back(sections[0]);
    for(size_t i=1;i<sections.size();++i) {
        int target=-1;for(size_t k=1;k<out.size();++k) if(out[k].key==sections[i].key) {target=int(k);break;}
        if(target<0) {out.push_back(sections[i]);continue;}
        ++r.mergedSections;
        for(const auto& l:sections[i].body) {
            const std::string t=trim(l);
            if(t.empty()||t[0]==';'||t[0]=='#') continue;
            const size_t eq=t.find('=');if(eq==std::string::npos) continue;
            const std::string key=lower(trim(t.substr(0,eq)));
            bool placed=false;
            for(auto& b:out[target].body) {
                const std::string bt=trim(b);if(bt.empty()||bt[0]==';'||bt[0]=='#') continue;
                const size_t be=bt.find('=');if(be==std::string::npos) continue;
                if(lower(trim(bt.substr(0,be)))==key) {b=t;placed=true;}   // every earlier copy of the key takes the saved value
            }
            if(!placed) out[target].body.push_back(t);
            ++r.mergedKeys;
        }
    }
    r.changed=r.bom||r.mergedSections>0;
    if(!r.changed) return input;
    std::string result;
    auto emit=[&](const std::string& l) {result+=l;result+="\r\n";};
    for(const auto& l:out[0].body) emit(l);
    for(size_t k=1;k<out.size();++k) {emit(out[k].name);for(const auto& l:out[k].body) emit(l);}
    return result;
}
// File version. A copy of the original is kept once as <file>.avant_reparation. Returns true when the file was rewritten.
inline bool repairFile(const std::filesystem::path& path,Result& r) {
    r=Result{};
    std::error_code ec;if(!std::filesystem::exists(path,ec)) return false;
    std::string data;
    {std::ifstream f(path,std::ios::binary);if(!f) return false;data.assign(std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>());}
    const std::string fixed=repair(data,r);
    if(!r.changed) return false;
    std::filesystem::path backup=path;backup+=L".avant_reparation";
    if(!std::filesystem::exists(backup,ec)) {std::ofstream b(backup,std::ios::binary);if(b) b.write(data.data(),std::streamsize(data.size()));}
    std::filesystem::path tmp=path;tmp+=L".tmp";
    {std::ofstream f(tmp,std::ios::binary|std::ios::trunc);if(!f) return false;f.write(fixed.data(),std::streamsize(fixed.size()));if(!f) return false;}
    std::filesystem::rename(tmp,path,ec);
    if(ec) {std::ofstream f(path,std::ios::binary|std::ios::trunc);if(!f) return false;f.write(fixed.data(),std::streamsize(fixed.size()));std::filesystem::remove(tmp,ec);}
    return true;
}
}}
