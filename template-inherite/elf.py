import struct
import sys

# ELF 魔数
ELF_MAGIC = b'\x7fELF'

# ELF 标识位定义
EI_CLASS = 4  # 文件类别（32/64位）
EI_DATA = 5   # 字节序
EI_VERSION = 6

# 文件类别
ELFCLASS32 = 1
ELFCLASS64 = 2

# 字节序
ELFDATA2LSB = 1  # 小端
ELFDATA2MSB = 2  # 大端

# ELF 文件类型
ETYPE = {
    0: "无类型",
    1: "可重定位文件",
    2: "可执行文件",
    3: "共享目标文件",
    4: "核心转储文件"
}

# 架构类型
MACHINE = {
    3: "Intel 80386",
    62: "AMD x86_64",
    40: "ARM",
    183: "AArch64"
}

def parse_elf(file_path):
    try:
        with open(file_path, 'rb') as f:
            # 读取 ELF 标识（前 16 字节）
            e_ident = f.read(16)
            if not e_ident.startswith(ELF_MAGIC):
                print("错误：不是有效的 ELF 文件！")
                return

            # 解析基础信息
            elf_class = e_ident[EI_CLASS]   # 32/64位
            elf_data = e_ident[EI_DATA]     # 大小端
            elf_version = e_ident[EI_VERSION]

            # 设置字节序格式
            endian = '<' if elf_data == ELFDATA2LSB else '>'

            print("=" * 50)
            print("          ELF 文件解析结果")
            print("=" * 50)
            print(f"魔数：7fELF (有效ELF文件)")
            print(f"文件位数：{'32位' if elf_class == ELFCLASS32 else '64位' if elf_class == ELFCLASS64 else '未知'}")
            print(f"字节序：{'小端(LSB)' if elf_data == ELFDATA2LSB else '大端(MSB)'}")
            print(f"ELF版本：{elf_version}")

            # ===================== 解析 ELF 头部 =====================
            if elf_class == ELFCLASS32:
                # 32位 ELF 头部结构体
                fmt = endian + 'HHIIIIIHHHHHH'
                header_size = struct.calcsize(fmt)
                e_data = f.read(header_size)
                e_type, e_machine, e_version, e_entry, e_phoff, e_shoff, e_flags, \
                e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx = struct.unpack(fmt, e_data)
            else:
                # 64位 ELF 头部结构体
                fmt = endian + 'HHIQQQIHHHHHH'
                header_size = struct.calcsize(fmt)
                e_data = f.read(header_size)
                e_type, e_machine, e_version, e_entry, e_phoff, e_shoff, e_flags, \
                e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx = struct.unpack(fmt, e_data)

            # 输出头部核心信息
            print(f"文件类型：{ETYPE.get(e_type, '未知')}")
            print(f"目标架构：{MACHINE.get(e_machine, f'未知({e_machine})')}")
            print(f"入口地址：0x{e_entry:08x}")
            print(f"程序头偏移：0x{e_phoff:x}")
            print(f"节头偏移：0x{e_shoff:x}")
            print(f"程序头数量：{e_phnum}")
            print(f"节头数量：{e_shnum}")

            # ===================== 解析程序头表（段） =====================
            print("\n" + "-" * 50)
            print("              程序头表（段信息）")
            print("-" * 50)
            if e_phnum > 0:
                f.seek(e_phoff)
                for i in range(e_phnum):
                    if elf_class == ELFCLASS32:
                        pfmt = endian + 'IIIIIIII'
                    else:
                        pfmt = endian + 'IIQQQQQQ'
                    phdr = f.read(struct.calcsize(pfmt))
                    p_type = struct.unpack(pfmt, phdr)[0]
                    print(f"程序头 {i}：类型=0x{p_type:x}")

            # ===================== 解析节头表（节） =====================
            print("\n" + "-" * 50)
            print("              节头表（节信息）")
            print("-" * 50)
            if e_shnum > 0:
                f.seek(e_shoff)
                for i in range(e_shnum):
                    if elf_class == ELFCLASS32:
                        sfmt = endian + 'IIIIIIIIII'
                    else:
                        sfmt = endian + 'IIQQQQIIQQ'
                    shdr = f.read(struct.calcsize(sfmt))
                    sh_type = struct.unpack(sfmt, shdr)[4]
                    print(f"节头 {i}：类型=0x{sh_type:x}")

            print("\n" + "=" * 50)
            print("解析完成！")
            print("=" * 50)

    except FileNotFoundError:
        print(f"错误：文件 {file_path} 不存在")
    except Exception as e:
        print(f"解析失败：{str(e)}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"用法：python {sys.argv[0]} <elf文件路径>")
        sys.exit(1)
    parse_elf(sys.argv[1])