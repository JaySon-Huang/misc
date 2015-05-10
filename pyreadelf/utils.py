from constants import (
    SHFEnum, SHTEnum,
    PTEnum
)


def ELF_TBSS_SPECIAL(sec_hdr, segment):
    '''.tbss is special.  It doesn't contribute memory space to normal
   segments and it doesn't take file space in normal segments.'''
    return (
        ((sec_hdr.sh_flags & SHFEnum.SHF_TLS) != 0) and
        (sec_hdr.sh_type == SHTEnum.SHT_NOBITS) and
        (segment.p_type != PTEnum.PT_TLS)
    )


def ELF_SECTION_SIZE(sec_hdr, segment):
    return 0 if ELF_TBSS_SPECIAL(sec_hdr, segment) else (sec_hdr.sh_size)


def ELF_SECTION_IN_SEGMENT_STRICT(sec_hdr, segment, check_vma=1, strict=1):
    return (
    (# Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections.
        (
        ((sec_hdr.sh_flags & SHFEnum.SHF_TLS) != 0)
     and (segment.p_type == PTEnum.PT_TLS
     or segment.p_type == PTEnum.PT_GNU_RELRO
     or segment.p_type == PTEnum.PT_LOAD)
    )
    # PT_TLS segment contains only SHF_TLS sections, PT_PHDR no sections at all.
    or (
        (sec_hdr.sh_flags & SHFEnum.SHF_TLS) == 0
    and segment.p_type != PTEnum.PT_TLS
    and segment.p_type != PTEnum.PT_PHDR))

   # Any section besides one of type SHT_NOBITS must have file offsets within the segment.
   and (sec_hdr.sh_type == SHTEnum.SHT_NOBITS
       or (sec_hdr.sh_offset >= segment.p_offset
       and (not strict
           or (sec_hdr.sh_offset - segment.p_offset
           <= segment.p_filesz - 1))
       and ((sec_hdr.sh_offset - segment.p_offset + ELF_SECTION_SIZE(sec_hdr, segment)) <= segment.p_filesz)))
   # SHF_ALLOC sections must have VMAs within the segment.
   and (not check_vma
       or (sec_hdr.sh_flags & SHFEnum.SHF_ALLOC) == 0
       or (sec_hdr.sh_addr >= segment.p_vaddr
       and (not strict
           or (sec_hdr.sh_addr - segment.p_vaddr <= segment.p_memsz - 1))
       and ((sec_hdr.sh_addr - segment.p_vaddr + ELF_SECTION_SIZE(sec_hdr, segment)) <= segment.p_memsz)))
   # No zero size sections at start or end of PT_DYNAMIC.
   and (segment.p_type != PTEnum.PT_DYNAMIC
       or sec_hdr.sh_size != 0
       or segment.p_memsz == 0
       or ((sec_hdr.sh_type == SHTEnum.SHT_NOBITS
        or (sec_hdr.sh_offset > segment.p_offset
            and (sec_hdr.sh_offset - segment.p_offset < segment.p_filesz)))
       and ((sec_hdr.sh_flags & SHFEnum.SHF_ALLOC) == 0
           or (sec_hdr.sh_addr > segment.p_vaddr
           and (sec_hdr.sh_addr - segment.p_vaddr < segment.p_memsz)))))
   )
