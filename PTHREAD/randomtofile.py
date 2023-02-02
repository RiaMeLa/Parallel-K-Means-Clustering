import random
random.seed(1)
with open('nuovinumeri.txt', 'w') as f:
    for i in range(600000):
        for dimension in range(40):
            f.write(str(random.random()) + " ")
        f.write("\n")
    
