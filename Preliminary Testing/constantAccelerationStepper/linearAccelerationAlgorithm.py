import math
delay = 0.003
acceleration = -8000

# for x in range(870):
#     dVelocity = delay * acceleration
#     delay = 1/(dVelocity+1/delay)
#     roundedDelay = round(delay*1000000)
#     if (delay<0.00025):
#         delay = 0.00025
#     print(roundedDelay)

for x in range(870):
    dVelocity = delay * acceleration
    delay = 1/(dVelocity+1/delay)
    roundedDelay = round(delay*1000000)
    if (delay>0.003):
        delay = 0.003
    print(roundedDelay)
