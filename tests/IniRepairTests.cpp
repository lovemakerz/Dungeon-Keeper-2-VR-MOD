// DK2VR 0.9.47.63 - made by LoVeMaKeRz
#include "../src/common/IniRepair.hpp"
#include <cstdio>
#define CHECK(c, ...) do {if(!(c)) {std::printf("FAIL: " __VA_ARGS__); std::printf("\n"); return 1;}} while(0)
using namespace dk2vr::inirepair;
int main() {
    Result r;
    // 1. Healthy file: untouched, byte for byte.
    {const std::string ok="[VR]\r\n; comment\r\nPossession360=1\r\nTableSizePercent=100\r\n";
     const std::string o=repair(ok,r);CHECK(!r.changed&&o==ok,"healthy file must not change");}
    // 2. The exact real-world failure: BOM on the first line + a second [VR] appended by WritePrivateProfileString.
    {const std::string bad="\xEF\xBB\xBF[VR]\r\n; c\r\nMode=0\r\nPossession360=1\r\nTableSizePercent=100\r\nViewExpand=1.15\r\n[VR]\r\nControlsFrench=0\r\nTableTilt=0.0\r\nTableSizePercent=85\r\n";
     const std::string o=repair(bad,r);
     CHECK(r.changed&&r.bom&&r.mergedSections==1&&r.mergedKeys==3,"bom=%d sections=%d keys=%d",int(r.bom),r.mergedSections,r.mergedKeys);
     CHECK(o.compare(0,5,"[VR]\r")==0,"starts with the header, no BOM");
     CHECK(o.find("\xEF\xBB\xBF")==std::string::npos,"no BOM left");
     CHECK(o.find("[VR]")==o.rfind("[VR]"),"one section only");
     CHECK(o.find("Possession360=1")!=std::string::npos,"original setting kept");
     CHECK(o.find("TableSizePercent=85")!=std::string::npos&&o.find("TableSizePercent=100")==std::string::npos,"the saved value wins");
     CHECK(o.find("ControlsFrench=0")!=std::string::npos&&o.find("TableTilt=0.0")!=std::string::npos,"new keys appended");
     CHECK(o.find("; c")!=std::string::npos,"comments kept");
     // idempotent
     Result r2;const std::string o2=repair(o,r2);CHECK(!r2.changed&&o2==o,"second pass changes nothing");}
    // 3. BOM only.
    {const std::string bad="\xEF\xBB\xBF[VR]\r\nMode=0\r\n";const std::string o=repair(bad,r);CHECK(r.changed&&r.bom&&o=="[VR]\r\nMode=0\r\n","bom only");}
    // 4. Other sections ([User]) are preserved and not mixed with [VR].
    {const std::string s="[VR]\r\nA=1\r\n[User]\r\nB=2\r\n[VR]\r\nA=3\r\n";const std::string o=repair(s,r);
     CHECK(r.changed&&o=="[VR]\r\nA=3\r\n[User]\r\nB=2\r\n","sections kept apart: %s",o.c_str());}
    // 5. Case-insensitive section/key names (Windows API behaviour).
    {const std::string s="[vr]\r\nmode=1\r\n[VR]\r\nMODE=2\r\n";const std::string o=repair(s,r);CHECK(r.changed&&o=="[vr]\r\nMODE=2\r\n","case insensitive: %s",o.c_str());}
    // 6. File round trip + one-time backup.
    {namespace fs=std::filesystem;fs::path p=fs::temp_directory_path()/"dk2vr_inirepair_test.ini";std::error_code ec;fs::remove(p,ec);fs::path b=p;b+=L".avant_reparation";fs::remove(b,ec);
     {std::ofstream f(p,std::ios::binary);f<<"\xEF\xBB\xBF[VR]\r\nA=1\r\n[VR]\r\nB=2\r\n";}
     CHECK(repairFile(p,r),"file rewritten");CHECK(fs::exists(b),"backup kept");
     std::ifstream f(p,std::ios::binary);std::string d((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());CHECK(d=="[VR]\r\nA=1\r\nB=2\r\n","file content: %s",d.c_str());
     CHECK(!repairFile(p,r),"already healthy");fs::remove(p,ec);fs::remove(b,ec);}
    std::printf("IniRepairTests: PASS\n");return 0;
}
