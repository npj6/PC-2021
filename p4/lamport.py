#! /usr/bin/env python3

import threading

VALUE = 2000000
I = 0

N = 3
choosing = [False, False, False]
number = [0, 0, 0]

def pre_cs(yo):
    global choosing, number
    print("pre_cs de "+str(yo))
    
    choosing[yo] = True
    number[yo] = 1 + max(number)
    choosing[yo] = False
    
    for otro in range(0, N):
        while choosing[otro]:
            pass
        while number[otro] > 0 and (number[otro] < number[yo] or (number[otro] == number[yo] and otro < yo)):
            pass
    
def post_cs(yo):
    global choosing
    
    print("post_cs de "+str(yo))
    number[yo] = 0    

def thread(yo):
    pre_cs(yo)
    
    global I

    print("cs de "+str(yo))
    for _ in range(VALUE):
        I += 1
        
    post_cs(yo)

if __name__ == "__main__":
    threads = []

    for n in range(N):
        threads.append(threading.Thread(target=thread, args=[n]))
    
    for n in range(N):
        threads[n].start()

    for t in threads:
        t.join()
    
    print("I vale "+ str(I) +", deberia valer "+ str(VALUE*3))
