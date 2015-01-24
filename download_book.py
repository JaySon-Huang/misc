#!usr/bin/env python
#encoding=utf-8

import requests
from bs4 import BeautifulSoup

# 捉鬼实习生
site_url = 'http://m.ybdu.com'
base_url = 'http://m.ybdu.com/xiaoshuo/8/8438_%d/'

def download_chapter(url, out_file):
    req = requests.get(churl)
    content = req.content.decode('gbk')
    soup = BeautifulSoup(content)
    out_file.write(soup.find(id='txt').text+'\n\n\n')

with open('捉鬼实习生.txt', 'w') as of:
    for i in range(1,8):
        url = base_url%i
        req = requests.get(url)

        content = req.content.decode('gbk')
        soup = BeautifulSoup(content)
        chapters = soup.find(class_='chapter')
        # import pdb;pdb.set_trace()
        for chapter in chapters.find_all('li'):
            chaptername = chapter.find('a')
            if not chaptername:
                of.write('\n'.join(
                    ('* * * * * *',
                     chapter.text.strip(),
                     '* * * * * *')) +'\n\n'
                )
                continue
            else:
                name = chaptername.text
                of.write(name+'\n')
                churl = site_url+chaptername.attrs['href']
                download_chapter(churl, of)
                

