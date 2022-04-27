import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Create a dtype with the binary data format and the desired column names
# 2 U_longs, 7 floats
dt = np.dtype([
    ('count',    'u4'),
    ('time_us',  'u4'),

    ('altitude', 'f4'),
    ('gyro_x', 'f4'), ('gyro_y', 'f4'), ('gyro_z', 'f4'),
    ('acc_x',  'f4'), ('acc_y',  'f4'), ('acc_z',  'f4')
    ])
data = np.fromfile("log_files/example.log", dtype=dt)
df = pd.DataFrame(data)
df["delta_ms"] = (df["time_us"] - df['time_us'].shift(1))/1000
df["time"] = df["time_us"] / 1e6

to_roll = ["acc_" + d for d in "xyz"]
for i in to_roll:
    df[i] = df[i].rolling(7).mean()

print(df.head(20))
print("="*40)
print(df.tail(20))

fig, axs = plt.subplots(2, 2)

# Altitude
axs[0][0].plot(df["time"], df["altitude"])

# Accelerometer
df[["acc_x", "acc_y", "acc_z", "time"]].plot(x="time", ax = axs[0][1])

# Gyro
df[["gyro_x", "gyro_y", "gyro_z", "time"]].plot(x="time", ax = axs[1][0])

# Frame times
axs[1][1].hist(df["delta_ms"], bins=100)

plt.show()