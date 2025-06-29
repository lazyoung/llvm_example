// https://gist.github.com/michaeljclark/d94b72fa3d580ea2037e0a4dc5e2fc5b

#include <cstdio>
#include <string>

#include "llvm/MC/TargetRegistry.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCDisassembler/MCRelocationInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Format.h"

using namespace llvm;

struct LLVMDisassembler
{
    static const int hexcols = 10;

    const Target *tg;
    std::string err;
    MCTargetOptions options;

    std::unique_ptr<MCRegisterInfo> ri;
    std::unique_ptr<MCAsmInfo> ai;
    std::unique_ptr<MCSubtargetInfo> si;
    std::unique_ptr<MCInstrInfo> ii;
    std::unique_ptr<MCContext> cx;
    std::unique_ptr<MCDisassembler> di;
    std::unique_ptr<MCInstPrinter> ip;

    LLVMDisassembler(std::string triple, std::string cpu, std::string features)
    {
        tg = TargetRegistry::lookupTarget(triple, err);
        ri.reset(tg->createMCRegInfo(triple));
        ai.reset(tg->createMCAsmInfo(*ri, triple, options));
        si.reset(tg->createMCSubtargetInfo(triple, cpu, features));
        ii.reset(tg->createMCInstrInfo());
        cx.reset(new MCContext(Triple(triple), ai.get(), ri.get(), si.get()));
        di.reset(tg->createMCDisassembler(*si, *cx));
        ip.reset(tg->createMCInstPrinter(Triple(triple),
            ai->getAssemblerDialect(), *ai, *ii, *ri));
    }

    void format_hex(raw_string_ostream &out, ArrayRef<uint8_t> data,
        size_t offset, size_t sz)
    {
        int nbytes = sz < hexcols ? sz : hexcols;
        out << format_hex_no_prefix(offset, 8) << ": "
            << format_bytes(data.slice(offset, nbytes), {}, hexcols, 1);
        out.indent((hexcols - nbytes) * 3 + 8 - (hexcols * 3) % 8);
    }

    int disasm(size_t offset, ArrayRef<uint8_t> data)
    {
        std::string buf;
        raw_string_ostream out(buf);
        MCInst in;
        uint64_t sz;
        while (offset < data.size() && di->getInstruction(in, sz, data.slice(offset), offset, out))
        {
            format_hex(out, data, offset, sz);
            ip->printInst(&in, offset, "", *si, out);
            if (sz == 0) break;
            printf("%s\n", buf.c_str());
            buf.clear();
            while (sz > hexcols) {
                offset += hexcols; sz -= hexcols;
                format_hex(out, data, offset, sz);
                printf("%s\n", buf.c_str());
                buf.clear();
            }
            offset += sz;
        }
        return 0;
    }
};

int main(int argc, char **argv)
{
    SmallVector<const char *> Args = { argv[0], "--x86-asm-syntax=intel" };
    llvm::cl::ParseCommandLineOptions(Args.size(), Args.data());

    InitializeAllTargetInfos();
    InitializeAllTargetMCs();
    InitializeAllDisassemblers();

    const uint8_t insn[] = {
        0x6f, 0x00, 0x00, 0x20, // j	512
        0x73, 0x11, 0x01, 0x34, // csrrw	sp, mscratch, sp
        0x63, 0x0C, 0x01, 0x1a, // beqz	sp, 440
    };

    LLVMDisassembler dis("riscv64", "", "");

    return dis.disasm(0, insn);
}