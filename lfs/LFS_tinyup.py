#! usr/bin/env python
#encoding=utf-8

import requests
import bs4
from bs4 import BeautifulSoup

# Part I 下载到本地文件中
def down_html():
    url = 'http://www.linuxfromscratch.org/lfs/downloads/7.5/LFS-BOOK-7.5-NOCHUNKS.html'
    res = requests.get(url)
    print(dir(res))
    content = res.content.decode('ISO-8859-1')
    with open('lfs_html.txt', 'w') as of:
        of.write(content)

# Part II BeautifulSoup提取代码的数据
def extract_code():
    infile = open('lfs_html.txt', 'r')
    content = infile.read()
    infile.close()
    soup = BeautifulSoup(content)
    with open('lfs_command.txt', 'w') as of:
        command_blocks = soup.find_all('kbd', class_='command')
        for command_block in command_blocks:
            for command in command_block.text.split('\n'):
                of.write(command+'\n')
            of.write('\n')

# Part III 整理剪切板得到的数据
def tidy():
    import re
    regex = re.compile(r'^\d[\d\.]+\.\W.*$')

    cfile = open('lfs_command.txt', 'r')
    commands = set([_.strip('\n') for _ in cfile])
    cfile.close()

    inInstalledStuff = False
    inInstalledLib = False
    inDesc = False
    inDesc_isProgram = True
    skip_flag = False

    tidy_file = open('lfs_tidy.txt', 'w')
    with open('lfs_part3.txt', 'r') as infile:
        for lineno,line in enumerate(infile) :
            if line == '\n':
                tidy_file.write('\n')
                continue
            if skip_flag:
                skip_flag = False
                print('skip:',line)
                continue


            line = line.strip('\n')

            # 去除不必要的行
            if re.match(r'Approximate build time|Required disk space', line, re.I):
                skip_flag = True
                print('skip:',line)
                continue

            if inInstalledLib:
                inInstalledLib = False
                libs = []
                tup_libs = re.findall(r'\b(\w+\.(so|a|\{a,so\}))', line)
                for lib in tup_libs:
                    libs.append("`{0:s}`".format(lib[0]))
                tidy_file.write(','.join(libs)+'\n')
                # print(','.join(libs))
                continue
            elif inInstalledStuff:
                inInstalledStuff = False
                stuffs = [_.strip() for _ in line.split(',')]
                tidy_file.write('\t'+','.join(stuffs)+'\n')
                # print('\t'+','.join(stuffs))
                continue
            
            # 标题
            m = regex.match(line)
            if m:
                inDesc = False
                # print('{0:4d} {1:s}'.format(lineno, m.group(0)))
                num_of_sharp = len(line.split()[0].split('.')) - 1
                tidy_file.write(('#'*num_of_sharp)+' '+line+'\n')
                # print('#'*num_of_sharp+' '+line)
                continue

            # Note、Important
            elif re.match(r'^Note$', line):
                tidy_file.write(
                    '* * *\n'
                    '** 注意 **\n')
                # print('* * *')
                # print('** '+line.strip()+' **')
                continue
            elif re.match(r'^Important$', line):
                tidy_file.write(
                    '* * *\n'
                    '** 重要 **\n')
                continue

            # 安装包的内容
            elif re.match(r'^Installed Libraries', line, re.I) :
                inInstalledLib = True
                tidy_file.write('#### 安装的库文件：\n')
                # print('#### '+line)
            elif re.match(r'^Installed programs', line, re.I) :
                inInstalledStuff = True
                tidy_file.write('#### 安装的程序：\n')
                # print('#### '+line)
                continue
            elif re.match(r'^Installed directory', line, re.I):
                inInstalledStuff = True
                tidy_file.write('#### 安装的目录：\n')
                # print('#### '+line)
                continue
            elif re.match(r'^Short Descriptions', line, re.I):
                inDesc = True
                tidy_file.write('#### 简要描述\n')
                # print('#### '+line)
                continue

            if inDesc:
                line = line.strip()
                if line in (
                    "tc qdisc allows users to setup the queueing discipline",
                    "tc class allows users to setup classes based on the queuing discipline scheduling",
                    "tc estimator allows users to estimate the network flow into a network",
                    "tc filter allows users to setup the QOS/COS packet filtering",
                    "tc policy allows users to setup the QOS/COS policies"
                    ):
                    pieces = line.split()
                    tidy_file.write("`{0:s} {1:s}` {2:s}  \n".format(pieces[0],pieces[1],' '.join(pieces[2:])))
                    # print("`{0:s} {1:s}` {2:s}".format(pieces[0],pieces[1],' '.join(pieces[2:])))
                    inDesc_isProgram = True
                    continue

                if inDesc_isProgram:
                    tidy_file.write('`{0:s}`   \n'.format(line))
                    # print('`{0:s}`   \n'.format(line))
                else:
                    tidy_file.write('({0:s})\n\n'.format(line))
                    # print('({0:s})\n\n'.format(line))
                inDesc_isProgram = not inDesc_isProgram
                continue

            # 如果是代码行，进行缩进
            if line in commands:
                tidy_file.write('\t{0:s}\n'.format(line))
                # print('\t{0:s}'.format(line))
                continue

            # 普通行，输出
            tidy_file.write(line+'  \n')

    tidy_file.close()

tidy()

