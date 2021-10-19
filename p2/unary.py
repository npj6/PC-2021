#! /usr/bin/env python3

import threading

thread_num = 5
thread_count = 50000

var = []

def thread():
    global var

    for _ in range(thread_count):
        var.append(None)

if __name__ == "__main__":
    threads = []

    for _ in range(thread_num):
        threads.append(threading.Thread(target=thread))
        threads[-1].start()

    for t in threads:
        t.join()
    
    print("counter value "+str(len(var)))