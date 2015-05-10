#!/usr/bin/env python
#encoding=utf-8
from enum import Enum, IntEnum

# Offsets within the Ehdr e_ident field.
EI_CLASS = 4
EI_DATA = 5
EI_VERSION = 6
EI_OSABI = 7
EI_ABIVERSION = 8
EI_PAD = 9
EI_NIDENT = 16

ELF_MAGIC = b'\x7fELF'

PREPROMOTE_PROGRAM_HDRS = '''
Elf file type is {:s}
Entry point 0x{:06x}
There are {:d} program headers, starting at offset {:d}'''
HEADER_PROGRAM_HDRS = '''
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align'''


class CLASSEnum(Enum):
    '''The valid values found in Ehdr e_ident[EI_CLASS].'''
    ELFCLASSNONE = 0
    ELFCLASS32 = 1
    ELFCLASS64 = 2

    def __str__(self):
        return {
            CLASSEnum.ELFCLASSNONE: 'none',
            CLASSEnum.ELFCLASS32: 'ELF32',
            CLASSEnum.ELFCLASS64: 'ELF64',
        }.get(self, '<unknown: 0x%x>' % self.value)


class DATAEnum(Enum):
    '''The valid values found in Ehdr e_ident[EI_DATA].'''
    ELFDATANONE = 0
    ELFDATA2LSB = 1
    ELFDATA2MSB = 2

    def __str__(self):
        return {
            DATAEnum.ELFDATANONE: 'none',
            DATAEnum.ELFDATA2LSB: "2's complement, little endian",
            DATAEnum.ELFDATA2MSB: "2's complement, big endian",
        }.get(self, '<unknown: 0x%x>' % self.value)


class VersionEnum(Enum):
    '''The valid values found in Ehdr e_ident[EI_VERSION] and e_version.'''
    EV_NONE = 0
    EV_CURRENT = 1

    def __str__(self):
        return {
            VersionEnum.EV_NONE: '',
            VersionEnum.EV_CURRENT: '(current)',
        }.get(self, '<unknown: %lx>' % self.value)


class OSABIEnum(Enum):
    '''The valid values found in Ehdr e_ident[EI_OSABI].'''
    ELFOSABI_NONE = 0
    ELFOSABI_HPUX = 1
    ELFOSABI_NETBSD = 2
    ELFOSABI_GNU = 3
    # ELFOSABI_LINUX is an alias for ELFOSABI_GNU.
    ELFOSABI_LINUX = 3
    ELFOSABI_SOLARIS = 6
    ELFOSABI_AIX = 7
    ELFOSABI_IRIX = 8
    ELFOSABI_FREEBSD = 9
    ELFOSABI_TRU64 = 10
    ELFOSABI_MODESTO = 11
    ELFOSABI_OPENBSD = 12
    ELFOSABI_OPENVMS = 13
    ELFOSABI_NSK = 14
    ELFOSABI_AROS = 15
    # A GNU extension for the ARM.
    ELFOSABI_ARM = 97
    # A GNU extension for the MSP.
    ELFOSABI_STANDALONE = 255

    def __str__(self):
        return {
            OSABIEnum.ELFOSABI_NONE: 'UNIX - System V',
            OSABIEnum.ELFOSABI_HPUX: 'UNIX - HP-UX',
            OSABIEnum.ELFOSABI_NETBSD: 'UNIX - NetBSD',
            OSABIEnum.ELFOSABI_GNU: 'UNIX - GNU',
            OSABIEnum.ELFOSABI_LINUX: 'UNIX - GNU',
            OSABIEnum.ELFOSABI_SOLARIS: 'UNIX - Solaris',
            OSABIEnum.ELFOSABI_AIX: 'UNIX - AIX',
            OSABIEnum.ELFOSABI_IRIX: 'UNIX - IRIX',
            OSABIEnum.ELFOSABI_FREEBSD: 'UNIX - FreeBSD',
            OSABIEnum.ELFOSABI_TRU64: 'UNIX - TRU64',
            OSABIEnum.ELFOSABI_MODESTO: 'Novell - Modesto',
            OSABIEnum.ELFOSABI_OPENBSD: 'UNIX - OpenBSD',
            OSABIEnum.ELFOSABI_OPENVMS: 'VMS - OpenVMS',
            OSABIEnum.ELFOSABI_NSK: 'HP - Non-Stop Kernel',
            OSABIEnum.ELFOSABI_AROS: 'AROS',
            OSABIEnum.ELFOSABI_ARM: 'ARM',
            OSABIEnum.ELFOSABI_STANDALONE: 'Standalone App',
        }.get(self, "<unknown: %x>" % self.value)


class ETEnum(Enum):
    # The valid values found in the Ehdr e_type field.
    ET_NONE = 0
    ET_REL = 1
    ET_EXEC = 2
    ET_DYN = 3
    ET_CORE = 4
    ET_LOOS = 0xfe00
    ET_HIOS = 0xfeff
    ET_LOPROC = 0xff00
    ET_HIPROC = 0xffff

    def __str__(self):
        s = {
            ETEnum.ET_NONE: 'NONE (None)',
            ETEnum.ET_REL: 'REL (Relocatable file)',
            ETEnum.ET_EXEC: 'EXEC (Executable file)',
            ETEnum.ET_DYN: 'DYN (Shared object file)',
            ETEnum.ET_CORE: 'CORE (Core file)',
        }.get(self, None)
        if s is not None:
            return s

        val = self.value
        if ETEnum.ET_LOPROC.value <= val <= ETEnum.ET_HIPROC.value:
            s = 'Processor Specific: (%x)' % val
        elif ETEnum.ET_HIOS.value <= val <= ETEnum.ET_LOOS.value:
            s = 'OS Specific: (%x)' % val
        else:
            s = '<unknown>: %x' % val
        return s


class EMEnum(Enum):
    # The valid values found in the Ehdr e_machine field.
    EM_NONE = 0
    EM_M32 = 1
    EM_SPARC = 2
    EM_386 = 3
    EM_68K = 4
    EM_88K = 5
    # 6 used to be EM_486
    EM_486 = 6
    EM_860 = 7
    EM_MIPS = 8
    EM_S370 = 9
    EM_MIPS_RS3_LE = 10
    # 11 was the old Sparc V9 ABI.
    EM_OLD_SPARCV9 = 11
    # 12 through 14 are reserved.
    EM_PARISC = 15
    # 16 is reserved.
    # Some old PowerPC object files use 17.
    EM_VPP500 = 17
    EM_SPARC32PLUS = 18
    EM_960 = 19
    EM_PPC = 20
    EM_PPC64 = 21
    EM_S390 = 22
    # 23 through 35 are served.
    EM_V800 = 36
    EM_FR20 = 37
    EM_RH32 = 38
    EM_RCE = 39
    EM_ARM = 40
    EM_ALPHA = 41
    EM_SH = 42
    EM_SPARCV9 = 43
    EM_TRICORE = 44
    EM_ARC = 45
    EM_H8_300 = 46
    EM_H8_300H = 47
    EM_H8S = 48
    EM_H8_500 = 49
    EM_IA_64 = 50
    EM_MIPS_X = 51
    EM_COLDFIRE = 52
    EM_68HC12 = 53
    EM_MMA = 54
    EM_PCP = 55
    EM_NCPU = 56
    EM_NDR1 = 57
    EM_STARCORE = 58
    EM_ME16 = 59
    EM_ST100 = 60
    EM_TINYJ = 61
    EM_X86_64 = 62
    EM_PDSP = 63
    EM_PDP10 = 64
    EM_PDP11 = 65
    EM_FX66 = 66
    EM_ST9PLUS = 67
    EM_ST7 = 68
    EM_68HC16 = 69
    EM_68HC11 = 70
    EM_68HC08 = 71
    EM_68HC05 = 72
    EM_SVX = 73
    EM_ST19 = 74
    EM_VAX = 75
    EM_CRIS = 76
    EM_JAVELIN = 77
    EM_FIREPATH = 78
    EM_ZSP = 79
    EM_MMIX = 80
    EM_HUANY = 81
    EM_PRISM = 82
    EM_AVR = 83
    EM_FR30 = 84
    EM_D10V = 85
    EM_D30V = 86
    EM_V850 = 87
    EM_M32R = 88
    EM_MN10300 = 89
    EM_MN10200 = 90
    EM_PJ = 91
    EM_OPENRISC = 92
    EM_ARC_A5 = 93
    EM_XTENSA = 94
    EM_VIDEOCORE = 95
    EM_TMM_GPP = 96
    EM_NS32K = 97
    EM_TPC = 98
    # Some old picoJava object files use 99 (EM_PJ is correct).
    EM_SNP1K = 99
    EM_ST200 = 100
    EM_IP2K = 101
    EM_MAX = 102
    EM_CR = 103
    EM_F2MC16 = 104
    EM_MSP430 = 105
    EM_BLACKFIN = 106
    EM_SE_C33 = 107
    EM_SEP = 108
    EM_ARCA = 109
    EM_UNICORE = 110
    EM_ALTERA_NIOS2 = 113
    EM_CRX = 114
    EM_TILEGX = 191
    # The Morph MT.
    EM_MT = 0x2530
    # DLX.
    EM_DLX = 0x5aa5
    # FRV.
    EM_FRV = 0x5441
    # Infineon Technologies 16-bit microcontroller with C166-V2 core.
    EM_X16X = 0x4688
    # Xstorym16
    EM_XSTORMY16 = 0xad45
    # Renesas M32C
    EM_M32C = 0xfeb0
    # Vitesse IQ2000
    EM_IQ2000 = 0xfeba
    # NIOS
    EM_NIOS32 = 0xfebb
    # Old AVR objects used 0x1057 (EM_AVR is correct).
    # Old MSP430 objects used 0x1059 (EM_MSP430 is correct).
    # Old FR30 objects used 0x3330 (EM_FR30 is correct).
    # Old OpenRISC objects used 0x3426 and 0x8472 (EM_OPENRISC is correct).
    # Old D10V objects used 0x7650 (EM_D10V is correct).
    # Old D30V objects used 0x7676 (EM_D30V is correct).
    # Old IP2X objects used 0x8217 (EM_IP2K is correct).
    # Old PowerPC objects used 0x9025 (EM_PPC is correct).
    # Old Alpha objects used 0x9026 (EM_ALPHA is correct).
    # Old M32R objects used 0x9041 (EM_M32R is correct).
    # Old V850 objects used 0x9080 (EM_V850 is correct).
    # Old S/390 objects used 0xa390 (EM_S390 is correct).
    # Old Xtensa objects used 0xabc7 (EM_XTENSA is correct).
    # Old MN10300 objects used 0xbeef (EM_MN10300 is correct).
    # Old MN10200 objects used 0xdead (EM_MN10200 is correct).

    def __str__(self):
        return {
            EMEnum.EM_NONE: "None",
            EMEnum.EM_M32: "WE32100",
            EMEnum.EM_SPARC: "Sparc",
            EMEnum.EM_386: "Intel 80386",
            EMEnum.EM_68K: "MC68000",
            EMEnum.EM_88K: "MC88000",
            EMEnum.EM_486: "Intel 80486",
            EMEnum.EM_860: "Intel 80860",
            EMEnum.EM_MIPS: "MIPS R3000",
            EMEnum.EM_S370: "IBM System/370",
            EMEnum.EM_MIPS_RS3_LE: "MIPS R4000 big-endian",
            EMEnum.EM_OLD_SPARCV9: "Sparc v9 (old)",
            EMEnum.EM_PARISC: "HPPA",
            EMEnum.EM_SPARC32PLUS: "Sparc v8+",
            EMEnum.EM_960: "Intel 90860",
            EMEnum.EM_PPC: "PowerPC",
            EMEnum.EM_PPC64: "PowerPC64",
            EMEnum.EM_FR20: "Fujitsu FR20",
            EMEnum.EM_RH32: "TRW RH32",
            EMEnum.EM_ARM: "ARM",
            EMEnum.EM_SH: "Renesas / SuperH SH",
            EMEnum.EM_SPARCV9: "Sparc v9",
            EMEnum.EM_TRICORE: "Siemens Tricore",
            EMEnum.EM_ARC: "ARC",
            EMEnum.EM_H8_300: "Renesas H8/300",
            EMEnum.EM_H8_300H: "Renesas H8/300H",
            EMEnum.EM_H8S: "Renesas H8S",
            EMEnum.EM_H8_500: "Renesas H8/500",
            EMEnum.EM_IA_64: "Intel IA-64",
            EMEnum.EM_MIPS_X: "Stanford MIPS-X",
            EMEnum.EM_COLDFIRE: "Motorola Coldfire",
            EMEnum.EM_ALPHA: "Alpha",
            EMEnum.EM_D10V: "d10v",
            EMEnum.EM_D30V: "d30v",
            EMEnum.EM_M32R: "Renesas M32R (formerly Mitsubishi M32r)",
            EMEnum.EM_V800: "Renesas V850 (using RH850 ABI)",
            EMEnum.EM_V850: "Renesas V850",
            EMEnum.EM_MN10300: "mn10300",
            EMEnum.EM_MN10200: "mn10200",
            EMEnum.EM_FR30: "Fujitsu FR30",
            EMEnum.EM_PJ: "picoJava",
            EMEnum.EM_MMA: "Fujitsu Multimedia Accelerator",
            EMEnum.EM_PCP: "Siemens PCP",
            EMEnum.EM_NCPU: "Sony nCPU embedded RISC processor",
            EMEnum.EM_NDR1: "Denso NDR1 microprocesspr",
            EMEnum.EM_STARCORE: "Motorola Star*Core processor",
            EMEnum.EM_ME16: "Toyota ME16 processor",
            EMEnum.EM_ST100: "STMicroelectronics ST100 processor",
            EMEnum.EM_TINYJ: "Advanced Logic Corp. TinyJ embedded processor",
            EMEnum.EM_PDSP: "Sony DSP processor",
            EMEnum.EM_PDP10: "Digital Equipment Corp. PDP-10",
            EMEnum.EM_PDP11: "Digital Equipment Corp. PDP-11",
            EMEnum.EM_FX66: "Siemens FX66 microcontroller",
            EMEnum.EM_ST9PLUS: "STMicroelectronics ST9+ 8/16 bit microcontroller",
            EMEnum.EM_ST7: "STMicroelectronics ST7 8-bit microcontroller",
            EMEnum.EM_68HC16: "Motorola MC68HC16 Microcontroller",
            EMEnum.EM_68HC12: "Motorola MC68HC12 Microcontroller",
            EMEnum.EM_68HC11: "Motorola MC68HC11 Microcontroller",
            EMEnum.EM_68HC08: "Motorola MC68HC08 Microcontroller",
            EMEnum.EM_68HC05: "Motorola MC68HC05 Microcontroller",
            EMEnum.EM_SVX: "Silicon Graphics SVx",
            EMEnum.EM_ST19: "STMicroelectronics ST19 8-bit microcontroller",
            EMEnum.EM_VAX: "Digital VAX",
            EMEnum.EM_AVR: "Atmel AVR 8-bit microcontroller",
            EMEnum.EM_CRIS: "Axis Communications 32-bit embedded processor",
            EMEnum.EM_JAVELIN: "Infineon Technologies 32-bit embedded cpu",
            EMEnum.EM_FIREPATH: "Element 14 64-bit DSP processor",
            EMEnum.EM_ZSP: "LSI Logic's 16-bit DSP processor",
            EMEnum.EM_MMIX: "Donald Knuth's educational 64-bit processor",
            EMEnum.EM_HUANY: "Harvard Universitys's machine-independent object format",
            EMEnum.EM_PRISM: "Vitesse Prism",
            EMEnum.EM_X86_64: "Advanced Micro Devices X86-64",
            EMEnum.EM_S390: "IBM S/390",
            EMEnum.EM_XSTORMY16: "Sanyo XStormy16 CPU core",
            EMEnum.EM_OPENRISC: "OpenRISC",
            EMEnum.EM_ARC_A5: "ARC International ARCompact processor",
            EMEnum.EM_CRX: "National Semiconductor CRX microprocessor",
            EMEnum.EM_DLX: "OpenDLX",
            EMEnum.EM_IP2K: "Ubicom IP2xxx 8-bit microcontrollers",
            EMEnum.EM_IQ2000: "Vitesse IQ2000",
            EMEnum.EM_XTENSA: "Tensilica Xtensa Processor",
            EMEnum.EM_VIDEOCORE: "Alphamosaic VideoCore processor",
            EMEnum.EM_TMM_GPP: "Thompson Multimedia General Purpose Processor",
            EMEnum.EM_NS32K: "National Semiconductor 32000 series",
            EMEnum.EM_TPC: "Tenor Network TPC processor",
            EMEnum.EM_ST200: "STMicroelectronics ST200 microcontroller",
            EMEnum.EM_MAX: "MAX Processor",
            EMEnum.EM_CR: "National Semiconductor CompactRISC",
            EMEnum.EM_F2MC16: "Fujitsu F2MC16",
            EMEnum.EM_MSP430: "Texas Instruments msp430 microcontroller",
            EMEnum.EM_M32C: "Renesas M32c",
            EMEnum.EM_MT: "Morpho Techologies MT processor",
            EMEnum.EM_BLACKFIN: "Analog Devices Blackfin",
            EMEnum.EM_SE_C33: "S1C33 Family of Seiko Epson processors",
            EMEnum.EM_SEP: "Sharp embedded microprocessor",
            EMEnum.EM_ARCA: "Arca RISC microprocessor",
            EMEnum.EM_UNICORE: "Unicore",
            EMEnum.EM_NIOS32: "Altera Nios",
            EMEnum.EM_ALTERA_NIOS2: "Altera Nios II",
        }.get(self, "<unknown>: 0x%x" % self.value)

''' A special value found in the Ehdr e_phnum field.'''
# Number of program segments stored in sh_info field of first section headre.
PN_XNUM = 0xffff


class SHNEnum(IntEnum):
    ''' Special section indices.'''
    SHN_UNDEF = 0
    SHN_LORESERVE = 0xff00
    SHN_LOPROC = 0xff00
    SHN_HIPROC = 0xff1f
    SHN_LOOS = 0xff20
    SHN_HIOS = 0xff3f
    SHN_ABS = 0xfff1
    SHN_COMMON = 0xfff2
    SHN_XINDEX = 0xffff
    SHN_HIRESERVE = 0xffff
    # Provide for initial and final section ordering in conjunction
    # with the SHF_LINK_ORDER and SHF_ORDERED section flags.
    SHN_BEFORE = 0xff00
    SHN_AFTER = 0xff01
    # x86_64 specific large common symbol.
    SHN_X86_64_LCOMMON = 0xff02


class SHTEnum(IntEnum):
    ''' The valid values found in the Shdr sh_type field.'''
    SHT_NULL = 0
    SHT_PROGBITS = 1
    SHT_SYMTAB = 2
    SHT_STRTAB = 3
    SHT_RELA = 4
    SHT_HASH = 5
    SHT_DYNAMIC = 6
    SHT_NOTE = 7
    SHT_NOBITS = 8
    SHT_REL = 9
    SHT_SHLIB = 10
    SHT_DYNSYM = 11
    SHT_INIT_ARRAY = 14
    SHT_FINI_ARRAY = 15
    SHT_PREINIT_ARRAY = 16
    SHT_GROUP = 17
    SHT_SYMTAB_SHNDX = 18
    SHT_LOOS = 0x60000000
    SHT_HIOS = 0x6fffffff
    SHT_LOPROC = 0x70000000
    SHT_HIPROC = 0x7fffffff
    SHT_LOUSER = 0x80000000
    SHT_HIUSER = 0xffffffff
    # The remaining values are not in the standard.
    # Incremental build data.
    SHT_GNU_INCREMENTAL_INPUTS = 0x6fff4700
    SHT_GNU_INCREMENTAL_SYMTAB = 0x6fff4701
    SHT_GNU_INCREMENTAL_RELOCS = 0x6fff4702
    SHT_GNU_INCREMENTAL_GOT_PLT = 0x6fff4703
    # Object attributes.
    SHT_GNU_ATTRIBUTES = 0x6ffffff5
    # GNU style dynamic hash table.
    SHT_GNU_HASH = 0x6ffffff6
    # List of prelink dependencies.
    SHT_GNU_LIBLIST = 0x6ffffff7
    # Versions defined by file.
    SHT_SUNW_verdef = 0x6ffffffd
    SHT_GNU_verdef = 0x6ffffffd
    # Versions needed by file.
    SHT_SUNW_verneed = 0x6ffffffe
    SHT_GNU_verneed = 0x6ffffffe
    # Symbol versions,
    SHT_SUNW_versym = 0x6fffffff
    SHT_GNU_versym = 0x6fffffff

    SHT_SPARC_GOTDATA = 0x70000000

    # ARM-specific section types.
    # Exception Index table.
    SHT_ARM_EXIDX = 0x70000001
    # BPABI DLL dynamic linking pre-emption map.
    SHT_ARM_PREEMPTMAP = 0x70000002
    # Object file compatibility attributes.
    SHT_ARM_ATTRIBUTES = 0x70000003
    # Support for debugging overlaid programs.
    SHT_ARM_DEBUGOVERLAY = 0x70000004
    SHT_ARM_OVERLAYSECTION = 0x70000005

    # x86_64 unwind information.
    SHT_X86_64_UNWIND = 0x70000001

    #MIPS-specific section types.
    # Register info section
    SHT_MIPS_REGINFO = 0x70000006

    # Link editor is to sort the entries in this section based on the
    # address specified in the associated symbol table entry.
    SHT_ORDERED = 0x7fffffff

    @staticmethod
    def toString(self):
        s = {
            SHTEnum.SHT_NULL: "NULL",
            SHTEnum.SHT_PROGBITS: "PROGBITS",
            SHTEnum.SHT_SYMTAB: "SYMTAB",
            SHTEnum.SHT_STRTAB: "STRTAB",
            SHTEnum.SHT_RELA: "RELA",
            SHTEnum.SHT_HASH: "HASH",
            SHTEnum.SHT_DYNAMIC: "DYNAMIC",
            SHTEnum.SHT_NOTE: "NOTE",
            SHTEnum.SHT_NOBITS: "NOBITS",
            SHTEnum.SHT_REL: "REL",
            SHTEnum.SHT_SHLIB: "SHLIB",
            SHTEnum.SHT_DYNSYM: "DYNSYM",
            SHTEnum.SHT_INIT_ARRAY: "INIT_ARRAY",
            SHTEnum.SHT_FINI_ARRAY: "FINI_ARRAY",
            SHTEnum.SHT_PREINIT_ARRAY: "PREINIT_ARRAY",
            SHTEnum.SHT_GNU_HASH: "GNU_HASH",
            SHTEnum.SHT_GROUP: "GROUP",
            SHTEnum.SHT_SYMTAB_SHNDX: "SYMTAB SECTION INDICIES",
            SHTEnum.SHT_GNU_verdef: "VERDEF",
            SHTEnum.SHT_GNU_verneed: "VERNEED",
            SHTEnum.SHT_GNU_versym: "VERSYM",
            0x6ffffff0: "VERSYM",
            0x6ffffffc: "VERDEF",
            0x7ffffffd: "AUXILIARY",
            0x7fffffff: "FILTER",
            SHTEnum.SHT_GNU_LIBLIST: "GNU_LIBLIST",
        }.get(self, None)
        if s is not None:
            return s

        if SHTEnum.SHT_LOPROC <= self <= SHTEnum.SHT_HIPROC:
            # FIXME: according to readelf.c, here can get string section name
            return 'LOPROC+%x' % (self - SHTEnum.SHT_LOPROC)
        elif SHTEnum.SHT_LOOS <= self <= SHTEnum.SHT_HIOS:
            # FIXME: according to readelf.c, here can get string section name
            return 'LOOS+%x' % (self - SHTEnum.SHT_LOOS)
        elif SHTEnum.SHT_LOUSER <= self <= SHTEnum.SHT_HIUSER:
            return 'LOUSER+%x' % (self - SHTEnum.SHT_LOUSER)
        else:
            return '%08x: <unknown>' % self


class SHFEnum(IntEnum):
    ''' The valid bit flags found in the Shdr sh_flags field.'''
    SHF_WRITE = 0x1
    SHF_ALLOC = 0x2
    SHF_EXECINSTR = 0x4
    SHF_MERGE = 0x10
    SHF_STRINGS = 0x20
    SHF_INFO_LINK = 0x40
    SHF_LINK_ORDER = 0x80
    SHF_OS_NONCONFORMING = 0x100
    SHF_GROUP = 0x200
    SHF_TLS = 0x400
    SHF_MASKOS = 0x0ff00000
    SHF_MASKPROC = 0xf0000000

    # Indicates this section requires ordering in relation to
    # other sections of the same type.  Ordered sections are
    # combined within the section pointed to by the sh_link entry.
    # The sh_info values SHN_BEFORE and SHN_AFTER imply that the
    # sorted section is to precede or follow, respectively, all
    # other sections in the set being ordered.
    SHF_ORDERED = 0x40000000
    # This section is excluded from input to the link-edit of an
    # executable or shared object.  This flag is ignored if SHF_ALLOC
    # is also set, or if relocations exist against the section.
    SHF_EXCLUDE = 0x80000000

    # Section with data that is GP relative addressable.
    SHF_MIPS_GPREL = 0x10000000

    # x86_64 specific large section.
    SHF_X86_64_LARGE = 0x10000000


class PTEnum(IntEnum):
    ''' The valid values found in the Phdr p_type field.'''
    PT_NULL = 0
    PT_LOAD = 1
    PT_DYNAMIC = 2
    PT_INTERP = 3
    PT_NOTE = 4
    PT_SHLIB = 5
    PT_PHDR = 6
    PT_TLS = 7
    PT_LOOS = 0x60000000
    PT_HIOS = 0x6fffffff
    PT_LOPROC = 0x70000000
    PT_HIPROC = 0x7fffffff
    # The remaining values are not in the standard.
    # Frame unwind information.
    PT_GNU_EH_FRAME = 0x6474e550
    PT_SUNW_EH_FRAME = 0x6474e550
    # Stack flags.
    PT_GNU_STACK = 0x6474e551
    # Read only after relocation.
    PT_GNU_RELRO = 0x6474e552
    # Platform architecture compatibility information
    PT_ARM_ARCHEXT = 0x70000000
    # Exception unwind tables
    PT_ARM_EXIDX = 0x70000001
    # Register usage information.  Identifies one .reginfo section.
    PT_MIPS_REGINFO = 0x70000000
    # Runtime procedure table.
    PT_MIPS_RTPROC = 0x70000001
    # .MIPS.options section.
    PT_MIPS_OPTIONS = 0x70000002

    def toString(self):
        s = {
            PTEnum.PT_NULL: "NULL",
            PTEnum.PT_LOAD: "LOAD",
            PTEnum.PT_DYNAMIC: "DYNAMIC",
            PTEnum.PT_INTERP: "INTERP",
            PTEnum.PT_NOTE: "NOTE",
            PTEnum.PT_SHLIB: "SHLIB",
            PTEnum.PT_PHDR: "PHDR",
            PTEnum.PT_TLS: "TLS",
            PTEnum.PT_GNU_EH_FRAME: "GNU_EH_FRAME",
            PTEnum.PT_GNU_STACK: "GNU_STACK",
            PTEnum.PT_GNU_RELRO: "GNU_RELRO",
        }.get(self, None)
        if s is not None:
            return s

        if PTEnum.PT_LOPROC <= self <= PTEnum.PT_HIPROC:
            # FIXME:
            s = "LOPROC+%lx" % self
        elif PTEnum.PT_LOOS <= self <= PTEnum.PT_HIOS:
            # FIXME:
            s = "LOOS+%lx" % self
        else:
            s = "<unknown>: %lx" % self
        return s


class PFEnum(IntEnum):
    ''' The valid bit flags found in the Phdr p_flags field.'''
    PF_X = 0x1
    PF_W = 0x2
    PF_R = 0x4
    PF_MASKOS = 0x0ff00000
    PF_MASKPROC = 0xf0000000
