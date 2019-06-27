#-- coding: utf-8 --
# from collections import ChainMap
# import os, argparse

# # 构造缺省参数:
# defaults = {
#     'color': 'red',
#     'user': 'guest'
# }

# # 构造命令行参数:
# parser = argparse.ArgumentParser()
# parser.add_argument('-u', '--user')
# parser.add_argument('-c', '--color')
# namespace = parser.parse_args()
# command_line_args = { k: v for k, v in vars(namespace).items() if v }

# # 组合成ChainMap:
# combined = ChainMap(command_line_args, os.environ, defaults)

# # 打印参数:
# print('color=%s' % combined['color'])
# print('user=%s' % combined['user'])

import matplotlib.pyplot as plt
import math
import numpy as np

def motorPower_Sline(lenth, start, stop, flexible, index):
    # lenth = 100     # The length of the S-curve. The number of sampling points
    # start = 1000      
    # stop = 0
    # flexible = 6    # The larger the stretch transformation of the curve, the larger the compression represents the most powerful. The ideal S-curve value is 4-6.

    if(index > lenth) :
        index = lenth

    num = lenth / 2
    melo = flexible * (index - num) / num
    deno = 1.0 / (1 + math.exp(-melo))
    current  = start - (start - stop)*deno
    print(current)
    return int(current)


# print(x)

lenth = 40
start = 255
stop = 70
flexible1 = 4

open("aaa.txt","w").write("")

with open('aaa.txt', 'a') as f:
    i = 1
    while i <= lenth:
        f.write(str(motorPower_Sline(lenth, start, stop, flexible1, i))+',')
        if i % 10 == 0:
            f.write('\n')
        if i % 100 == 0:
            f.write('\n')                
        i += 1



# flexible0 = 2
# flexible2 = 6
# flexible3 = 8
# flexible4 = 10

# y0 = []
# y1 = []
# y2 = []
# y3 = []
# y4 = []
# x = np.linspace(1, lenth)
# for i in x:
#     y0.append(9000000 / motorPower_Sline(lenth, start, stop, flexible0, i))
#     y1.append(9000000 / motorPower_Sline(lenth, start, stop, flexible1, i))
#     y2.append(9000000 / motorPower_Sline(lenth, start, stop, flexible2, i))
#     y3.append(9000000 / motorPower_Sline(lenth, start, stop, flexible3, i))
#     y4.append(9000000 / motorPower_Sline(lenth, start, stop, flexible4, i))

# plt.figure(1)
# plt.title('S-Curve line' )
# plt.plot(x, y0, label='S-flex = 2')
# plt.plot(x, y1, label='S-flex = 4')
# plt.plot(x, y2, label='S-flex = 6')
# plt.plot(x, y3, label='S-flex = 8')
# plt.plot(x, y4, label='S-flex = 10')
# plt.legend()
# plt.show()