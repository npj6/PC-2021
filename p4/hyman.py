#! /usr/bin/env python3

import threading

VALUE = 2000000
I = 0

states = [False, False]
turn = 0

def pre_cs(yo, otro):
    global states, turn
    print("pre_cs de "+str(yo))
    
    states[yo] = True
    while turn != yo:
        while states[otro]:
            pass
    turn = yo

def post_cs(yo, otro):
    global states, turn
    
    print("post_cs de "+str(yo))
    states[yo] = False
    

def thread(yo):
    otro = 1 - yo
    
    pre_cs(yo, otro)
    
    global I

    print("cs de "+str(yo))
    for _ in range(VALUE):
        I += 1
        
    post_cs(yo, otro)

if __name__ == "__main__":
    threads = []

    threads.append(threading.Thread(target=thread, args=[0]))
    threads.append(threading.Thread(target=thread, args=[1]))
    
    threads[0].start()
    threads[1].start()

    for t in threads:
        t.join()
    
    print("I vale "+ str(I) +", deberia valer "+ str(VALUE*2))
