# -*- coding:utf-8 -*-
import binascii
import os
import sys

def crc2hex(crc):
    res=''
    for i in range(4):
        t=crc & 0xFF
        crc >>= 8
        res='%02X%s' % (t, res)
    return res
inputfile = "E:\IAP\APP\Project\OBJ\STM32F407.bin"#实际存放的bin文件路径
isfile = os.path.isfile(inputfile);
print(inputfile);

fp = open(inputfile, "r+b")  #直接打开一个文件，如果文件不存在则创建文件
filesize = os.path.getsize(inputfile)
print("ZI app firmware size:", filesize, "bytes.")

#计算bin文件的CRC，首先清空CRC32区域的4个byte
fp.seek(0x1c, 0)#从bin文件开始，偏移地址为0x1c的地方存放bin的CRC32
clear4bytes = '00000000'
c4 =binascii.unhexlify(clear4bytes)
fp.write(c4)  #将CRC32存放的区域4bytes清零
fp.seek(0, 0)#从0开始读取整个bin
file_content = fp.read()#读整个文件内容到 file_content
crc = binascii.crc32(file_content)
print('CRC32:', hex(crc))

fp.seek(0x1c, 0)#从bin文件开始，偏移地址为0x1c的地方存放bin的CRC32
#存放计算CRC32四个字节
crcstr_2 = crc2hex(crc)
r=binascii.unhexlify(crcstr_2)
fp.write(r)
fp.close()

sys.exit(0)##正常退出

