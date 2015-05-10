#!/usr/bin/env python
#encoding=utf-8

import struct

import constants
from constants import (
    CLASSEnum, DATAEnum, VersionEnum, OSABIEnum, ETEnum, EMEnum,
    SHNEnum, SHTEnum, SHFEnum,
    PTEnum, PFEnum
)
from utils import (
    ELF_TBSS_SPECIAL, ELF_SECTION_IN_SEGMENT_STRICT
)
from IPython import embed


class Reader(object):

    BIG_ENDIAN = 0
    LITTLE_ENDIAN = 1

    def __init__(self, filename, endian=BIG_ENDIAN):
        self._fp = open(filename, 'rb')
        self._endian = endian

    def setEndian(self, endian):
        self._endian = endian

    def read_uint8(self):
        return struct.unpack("B", self._fp.read(1))[0]

    def read_uint16(self):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<H", self._fp.read(2))[0]
        else:
            return struct.unpack(">H", self._fp.read(2))[0]

    def read_uint32(self):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<I", self._fp.read(4))[0]
        else:
            return struct.unpack(">I", self._fp.read(4))[0]

    def read_uint64(self):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<Q", self._fp.read(8))[0]
        else:
            return struct.unpack(">Q", self._fp.read(8))[0]

    def read_int8(self):
        return struct.unpack("b", self._fp.read(1))[0]

    def read_int16(self):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<h", self._fp.read(2))[0]
        else:
            return struct.unpack(">h", self._fp.read(2))[0]

    def read_int32(self):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<i", self._fp.read(4))[0]
        else:
            return struct.unpack(">i", self._fp.read(4))[0]

    def read_int64(self):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<q", self._fp.read(8))[0]
        else:
            return struct.unpack(">q", self._fp.read(8))[0]

    def seek(self, offset, from_what=0):
        self._fp.seek(offset, from_what)

    def read_bytes(self, nbytes):
        return self._fp.read(nbytes)

    def bytes2uint16(self, bytes):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<H", bytes)[0]
        else:
            return struct.unpack(">H", bytes)[0]

    def bytes2uint32(self, bytes):
        if self._endian == Reader.LITTLE_ENDIAN:
            return struct.unpack("<I", bytes)[0]
        else:
            return struct.unpack(">I", bytes)[0]


class NotValidElfFileException(Exception):
    pass


class ElfHdr(object):
    TEMPLATE = '''
ELF Header:
  Magic:        {magic}
  Class:        {cls}
  Data:         {data}
  Version:      {version} {str_version}
  OS/ABI:       {os_abi}
  ABI Version:  {abi_version}
  Type:         {e_type}
  Machine:      {e_machine}
  Version:      {e_version}
  Entry point address:               {e_entry}
  Start of program headers:          {e_phoff} (bytes into file)
  Start of section headers:          {e_shoff} (bytes into file)
  Flags:                             {e_flags} {str_e_flags}
  Size of this header:               {e_ehsize} (bytes)
  Size of program headers:           {e_phentsize} (bytes)
  Number of program headers:         {e_phnum}
  Size of section headers:           {e_shentsize} (bytes)
  Number of section headers:         {e_shnum}
  Section header string table index: {e_shstrndx}
'''

    def __str__(self):
        return self.TEMPLATE.format(
            magic=' '.join(['%02x' % _ for _ in self.ident]),
            cls=self._class,
            data=self.data,
            version=self.version.value, str_version=self.version,
            os_abi=self.os_abi, abi_version=self.abi_version,
            e_type=self.e_type,
            e_machine=self.e_machine,
            e_version=hex(self.e_version),
            e_entry=hex(self.e_entry),
            e_phoff=self.e_phoff,
            e_shoff=self.e_shoff,
            # FIXME: str of flags is too long...
            e_flags=hex(self.e_flags), str_e_flags='',
            e_ehsize=self.e_ehsize,
            e_phentsize=self.e_phentsize,
            e_phnum=self.e_phnum,
            # FIXME: special case: elf_header.e_phnum == PN_XNUM
            e_shentsize=self.e_shentsize,
            e_shnum=self.e_shnum,
            # FIXME: special case: elf_header.e_shnum == SHN_UNDEF
            e_shstrndx=self.e_shstrndx
            # FIXME: special case: elf_header.e_shstrndx == (SHN_XINDEX & 0xffff)
        )

    def set(self, name, value):
        self.__dict__[name] = value


class ElfSectionHdr(object):

    def set(self, name, value):
        self.__dict__[name] = value


class ElfSymbol(object):

    def set(self, name, value):
        self.__dict__[name] = value


class ElfProgramHdr(object):
    TEMPLATE = '''  {:<14.14s} 0x{:016x} 0x{:016x} 0x{:016x}
                 0x{:016x} 0x{:016x}  {:s}{:s}{:s}    {:x}
'''

    def set(self, name, value):
        self.__dict__[name] = value

    def __str__(self):
        return self.TEMPLATE.format(
            PTEnum.toString(self.p_type), self.p_offset,
            self.p_vaddr, self.p_paddr,
            self.p_filesz, self.p_memsz,
            'R' if self.p_flags & PFEnum.PF_R else ' ',
            'W' if self.p_flags & PFEnum.PF_W else ' ',
            'E' if self.p_flags & PFEnum.PF_X else ' ',
            self.p_align
        )


class ElfFile(object):

    def __init__(self, filename, lazy=False):
        self._reader = Reader(filename)

        for i, magic_ch in enumerate(constants.ELF_MAGIC):
            if magic_ch != self._reader.read_uint8():
                raise NotValidElfFileException("Magic number not valid!")
        if lazy:
            return
        self.header = self.get_file_header()

    def get_file_header(self):
        header = ElfHdr()
        self.header = header

        e_ident = []
        for _ in range(len(constants.ELF_MAGIC), constants.EI_NIDENT):
            ch = self._reader.read_uint8()
            e_ident.append(ch)
        e_ident = constants.ELF_MAGIC + bytearray(e_ident)

        data = DATAEnum(e_ident[constants.EI_DATA])
        if data == DATAEnum.ELFDATA2MSB:
            self._reader.setEndian(Reader.BIG_ENDIAN)
        else:
            # DATAEnum.ELFDATA2LSB, DATAEnum.ELFDATANONE or other else
            self._reader.setEndian(Reader.LITTLE_ENDIAN)

        header.set('ident', e_ident)
        cls = CLASSEnum(e_ident[constants.EI_CLASS])
        header.set('_class', cls)
        header.set('data', data)
        header.set('version', VersionEnum(e_ident[constants.EI_VERSION]))
        header.set('os_abi', OSABIEnum(e_ident[constants.EI_OSABI]))
        header.set('abi_version', e_ident[constants.EI_OSABI])
        header.set('type', e_ident[constants.EI_ABIVERSION])

        e_type = self._reader.read_uint16()
        header.set('e_type', ETEnum(e_type))
        machine = self._reader.read_uint16()
        header.set('e_machine', EMEnum(machine))
        header.set('e_version', self._reader.read_uint32())
        if cls == CLASSEnum.ELFCLASS32:
            header.set('e_entry', self._reader.read_uint32())
            header.set('e_phoff', self._reader.read_uint32())
            header.set('e_shoff', self._reader.read_uint32())
            header.set('e_flags', self._reader.read_uint32())
            header.set('e_ehsize', self._reader.read_uint16())
            header.set('e_phentsize', self._reader.read_uint16())
            header.set('e_phnum', self._reader.read_uint16())
            header.set('e_shentsize', self._reader.read_uint16())
            header.set('e_shnum', self._reader.read_uint16())
            header.set('e_shstrndx', self._reader.read_uint16())

        elif cls == CLASSEnum.ELFCLASS64:
            header.set('e_entry', self._reader.read_uint64())
            header.set('e_phoff', self._reader.read_uint64())
            header.set('e_shoff', self._reader.read_uint64())
            header.set('e_flags', self._reader.read_uint32())
            header.set('e_ehsize', self._reader.read_uint16())
            header.set('e_phentsize', self._reader.read_uint16())
            header.set('e_phnum', self._reader.read_uint16())
            header.set('e_shentsize', self._reader.read_uint16())
            header.set('e_shnum', self._reader.read_uint16())
            header.set('e_shstrndx', self._reader.read_uint16())

        self.get_section_headers()

    def get_section_headers(self):
        section_headers = []
        self.section_headers = section_headers
        if self.header._class == CLASSEnum.ELFCLASS32:
            self._reader.seek(self.header.e_shoff)
            for _ in range(self.header.e_shnum):
                section = ElfSectionHdr()
                section.set('sh_name', self._reader.read_uint32())
                section.set('sh_type', self._reader.read_uint32())
                section.set('sh_flags', self._reader.read_uint32())
                section.set('sh_addr', self._reader.read_uint32())
                section.set('sh_offset', self._reader.read_uint32())
                section.set('sh_size', self._reader.read_uint32())
                section.set('sh_link', self._reader.read_uint32())
                section.set('sh_info', self._reader.read_uint32())
                section.set('sh_addralign', self._reader.read_uint32())
                section.set('sh_entsize', self._reader.read_uint32())

                section_headers.append(section)
        elif self.header._class == CLASSEnum.ELFCLASS64:
            self._reader.seek(self.header.e_shoff)
            for _ in range(self.header.e_shnum):
                section = ElfSectionHdr()
                section.set('sh_name', self._reader.read_uint32())
                section.set('sh_type', self._reader.read_uint32())
                section.set('sh_flags', self._reader.read_uint64())
                section.set('sh_addr', self._reader.read_uint64())
                section.set('sh_offset', self._reader.read_uint64())
                section.set('sh_size', self._reader.read_uint64())
                section.set('sh_link', self._reader.read_uint32())
                section.set('sh_info', self._reader.read_uint32())
                section.set('sh_addralign', self._reader.read_uint64())
                section.set('sh_entsize', self._reader.read_uint64())

                section_headers.append(section)

        self.get_string_table()
        self.dynamic_symbols = None
        self.dynamic_strings = None
        self.symtab_shndx_hdr = None
        for index, section in enumerate(section_headers):
            name = self.get_section_name(section)
            section.set('str_name', name)
            if section.sh_type == SHTEnum.SHT_DYNAMIC:
                # print('meet section %s @[%2d]' % (SHTEnum.toString(section.sh_type), index))
                if self.dynamic_symbols is not None:
                    print("File contains multiple dynamic symbol tables")
                    continue
                # FIXME:
                # self.dynamic_symbols = self.get_elf_symbols(section, index)
            elif ((section.sh_type == SHTEnum.SHT_STRTAB) and
                    (name == ".dynstr")):
                if self.dynamic_strings is not None:
                    print("File contains multiple dynamic string tables")
                    continue
                self._reader.seek(section.sh_offset)
                self.dynamic_strings = self._reader.read_bytes(section.sh_size)
            elif section.sh_type == SHTEnum.SHT_SYMTAB_SHNDX:
                # print('meet section %s @[%2d]' % (SHTEnum.toString(section.sh_type), index))
                if self.symtab_shndx_hdr is not None:
                    print("File contains multiple symtab shndx tables")
                    continue
                self.symtab_shndx_hdr = section
        self.print_section_headers()
        self.get_program_headers()

    def get_program_headers(self):
        program_headers = []
        self.program_headers = program_headers
        if self.header._class == CLASSEnum.ELFCLASS32:
            raise NotImplementedError
        else:
            self._reader.seek(self.header.e_phoff)
            for _ in range(self.header.e_phnum):
                program = ElfProgramHdr()
                program.set('p_type', self._reader.read_uint32())
                program.set('p_flags', self._reader.read_uint32())
                program.set('p_offset', self._reader.read_uint64())
                program.set('p_vaddr', self._reader.read_uint64())
                program.set('p_paddr', self._reader.read_uint64())
                program.set('p_filesz', self._reader.read_uint64())
                program.set('p_memsz', self._reader.read_uint64())
                program.set('p_align', self._reader.read_uint64())
                program_headers.append(program)
        self.print_program_headers()

    def get_dynamic_section(self):
        raise NotImplementedError

    def get_string_table(self):
        string_table_index = self.header.e_shstrndx
        if ((string_table_index != SHNEnum.SHN_UNDEF) and
                (string_table_index < self.header.e_shnum)):
            section = self.section_headers[string_table_index]
            if section.sh_size != 0:
                self._reader.seek(section.sh_offset)
                self.string_table = self._reader.read_bytes(section.sh_size)

    def get_section_name(self, section):
        if section is None:
            return "<none>"
        try:
            if self.string_table is None:
                return "<no-name>"
        except AttributeError:
            return "<no-name>"
        else:
            max_length = len(self.string_table)
            if section.sh_name >= max_length:
                return "<corrupt>"
            for end_index in range(section.sh_name, max_length):
                if self.string_table[end_index] == 0x00:
                    return self.string_table[section.sh_name:end_index].decode('ascii')

    def get_elf_symbols(self, section, index):
        number = section.sh_size // section.sh_entsize

        self._reader.seek(section.sh_offset)
        esyms = self._reader.read_bytes(section.sh_size)
        shndx = None
        if ((self.symtab_shndx_hdr is not None) and
                (self.symtab_shndx_hdr.sh_link == index)):
            self._reader.seek(self.symtab_shndx_hdr.sh_offset)
            shndx = self._reader.read_bytes(self.symtab_shndx_hdr.sh_size)

        syms = []
        if self.header._class == CLASSEnum.ELFCLASS32:
            offset = (4, 4, 4, 1, 1, 2)
            e_beg = 0
            for i in range(number):
                sym = ElfSymbol()
                sym.set('st_name', esyms[e_beg:e_beg+offset[0]])
                e_beg += offset[0]
                sym.set('st_value', esyms[e_beg:e_beg+offset[1]])
                e_beg += offset[1]
                sym.set('st_size', esyms[e_beg:e_beg+offset[2]])
                e_beg += offset[2]
                sym.set('st_info', esyms[e_beg:e_beg+offset[3]])
                e_beg += offset[3]
                sym.set('st_other', esyms[e_beg:e_beg+offset[4]])
                e_beg += offset[4]
                sym.set('st_shndx', esyms[e_beg+offset[4]:e_beg+offset[5]])
                e_beg += offset[5]
                sym.st_shndx = self._reader.bytes2uint16(sym.st_shndx)
                if ((sym.st_shndx == (SHNEnum.SHN_XINDEX & 0xffff)) and
                        shndx is not None):
                    sym.set('st_shndx', self._reader.bytes2uint32(shndx[i*4:i*(4+1)]))
                    embed()
                elif sym.st_shndx >= (SHNEnum.SHN_LORESERVE & 0xffff):
                    sym.st_shndx += SHNEnum.SHN_LORESERVE - (SHNEnum.SHN_LORESERVE & 0xffff)
                    embed()
            embed()
        else:
            syms = []
            offset = (4, 1, 1, 2, 8, 8)
            e_beg = 0
            for i in range(number):
                sym = ElfSymbol()
                sym.set('st_name', esyms[e_beg:e_beg+offset[0]])
                e_beg += offset[0]
                sym.set('st_info', esyms[e_beg:e_beg+offset[1]])
                e_beg += offset[1]
                sym.set('st_other', esyms[e_beg:e_beg+offset[2]])
                e_beg += offset[2]
                sym.set('st_shndx', esyms[e_beg:e_beg+offset[3]])
                e_beg += offset[3]
                print(sym.st_shndx)
                sym.st_shndx = self._reader.bytes2uint16(sym.st_shndx)
                if ((sym.st_shndx == (SHNEnum.SHN_XINDEX & 0xffff)) and
                        shndx is not None):
                    sym.set('st_shndx', self._reader.bytes2uint32(shndx[i*4:i*(4+1)]))
                    embed()
                elif sym.st_shndx >= (SHNEnum.SHN_LORESERVE & 0xffff):
                    sym.st_shndx += SHNEnum.SHN_LORESERVE - (SHNEnum.SHN_LORESERVE & 0xffff)
                    embed()
                sym.set('st_value', esyms[e_beg:e_beg+offset[4]])
                e_beg += offset[4]
                sym.set('st_size', esyms[e_beg:e_beg+offset[5]])
                e_beg += offset[5]
                embed()
        return syms

    def print_section_headers(self):
        print(
            "There are %d section headers, starting at offset 0x%lx:"
            % (self.header.e_shnum, self.header.e_shoff)
        )
        if self.header.e_shnum > 1:
            print('Section Headers:')
        else:
            print('Section Header:')
        if self.header._class == CLASSEnum.ELFCLASS32:
            print("  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al")
        else:
            print("  [Nr] Name              Type             Address           Offset")
            print("       Size              EntSize          Flags  Link  Info  Align")
        for i, section in enumerate(self.section_headers):
            if self.header._class == CLASSEnum.ELFCLASS32:
                print(
                    "  [{:2d}] {:<17.17s} {:<15s} {:08x} {:06x} {:02x}".format(
                        i, section.str_name, SHTEnum.toString(section.sh_type),
                        section.sh_addr, section.sh_offset, section.sh_size, section.sh_entsize
                    )
                )
            else:
                print(
                    ("  [{:2d}] {:<17.17s} {:<15s}  {:016x}  {:08x}\n"
                     "       {:016x} {:016x}  {:3s}      {:2d}   {:3d}     {:d}").format(
                        i, section.str_name, SHTEnum.toString(section.sh_type),
                        section.sh_addr, section.sh_offset, section.sh_size,
                        section.sh_entsize, self.get_elf_section_flags(section.sh_flags),
                        section.sh_link, section.sh_info, section.sh_addralign
                    )
                )
        if ((self.header.e_machine == EMEnum.EM_X86_64) or
                (elf_header.e_machine == EMEnum.EM_L1OM) or
                (elf_header.e_machine == EMEnum.EM_K1OM)):
            print(
                "Key to Flags:\n"
                "W (write), A (alloc), X (execute), M (merge), S (strings), l (large)\n"
                "I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)\n"
                "O (extra OS processing required) o (OS specific), p (processor specific)\n")
        else:
            print(
                "Key to Flags:\n"
                "W (write), A (alloc), X (execute), M (merge), S (strings)\n"
                "I (info), L (link order), G (group), T (TLS), E (exclude), x (unknown)\n"
                "O (extra OS processing required) o (OS specific), p (processor specific)\n")

    def print_program_headers(self):
        print(
            constants.PREPROMOTE_PROGRAM_HDRS.format(
                self.header.e_type, self.header.e_entry,
                self.header.e_phnum, self.header.e_phoff
            )
        )
        print(constants.HEADER_PROGRAM_HDRS)
        for program in self.program_headers:
            print(program, end='')
        # FIXME: handler segment.p_type == PT_DYNAMIC || PT_INTERP

        print("\n"" Section to Segment mapping:\n""  Segment Sections...")
        for i, segment in enumerate(self.program_headers):
            print("   {:2d}     ".format(i), end='')
            for j, section in enumerate(self.section_headers[1:]):
                if ((not ELF_TBSS_SPECIAL(section, segment)) and
                        (ELF_SECTION_IN_SEGMENT_STRICT(section, segment))):
                    print('{:s} '.format(section.str_name), end='')
            print()

    def get_elf_section_flags(self, flags):
        # FIXME: output the flags' string
        return ''
if __name__ == '__main__':
    ef = ElfFile('ElfCrackMe1')
