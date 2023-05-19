from operator import length_hint
import graphviz

import os
import platform
import threading
import socket
from datetime import datetime
from time import time 
dot = graphviz.Graph('round-table', comment='The Round Table', format='png', engine='neato')
list=[]
def getMyIp():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) #Создаем сокет (UDP)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1) # Настраиваем сокет на BROADCAST вещание.
    s.connect(('<broadcast>', 0))
    return s.getsockname()[0]
    

def scan_Ip(ip):
    addr = net + str(ip)
    comm = ping_com + addr
    response = os.popen(comm)
    data = response.readlines()
    for line in data:
        if 'TTL' in line:
            print(addr, "--> Ping Ok")
            list.append(addr)
            #c =os.system('ping {}'.format(addr))# через тайм 
            
           
            break
def graf():
    l = len(list)
    print(list)
    i = 1
    for i in range(1,l):
        dot.node(list[i])
        t1 = time()
        
        os.system('ping {}'.format(list[i]))# через тайм 
        t2 = time()
        total = t2 - t1
        dot.attr('edge',len=str(total))
        dot.edge(list[0], list[i])
        
        print(total)
        



net = getMyIp()
#dot = graphviz.Digraph()
netDot=net
#dot.node(netDot, netDot, shape='Mdiamond')
print('You IP :',net)
net_split = net.split('.')
a = '.'
net = net_split[0] + a + net_split[1] + a + net_split[2] + a
start_point = 0
end_point = 255

ping_com = "ping -n 1 "


t1 = datetime.now()
print("Scanning in Progress:")

for ip in range(start_point, end_point):
    if ip == int(net_split[3]):
       continue
    #scan_Ip(ip)
    #print(ip)
    potoc = threading.Thread(target=scan_Ip, args=[ip])
    potoc.start()


    
    

potoc.join()
t2 = datetime.now()
total = t2 - t1
graf()
dot.render('test-output/test-table.gv', view=True)

print("Scanning completed in: ", total)
