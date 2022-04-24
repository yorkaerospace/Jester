import numpy as np
import pandas as pd

# Create a dtype with the binary data format and the desired column names
# 2 U_longs, 7 floats
dt = np.dtype([
    ('count',    'u4'),
    ('time_us',  'u4'),

    ('altitude', 'f4'),
    ('gyro_x', 'f4'), ('gyro_y', 'f4'), ('gyro_z', 'f4'),
    ('acc_x',  'f4'), ('acc_y',  'f4'), ('acc_z',  'f4')
    ])
data = np.fromfile("log_files/12.log", dtype=dt)
df = pd.DataFrame(data)
df["delta_ms"] = (df["time_us"] - df['time_us'].shift(1))/1000

print(df.head(20))
print("="*40)
print(df.tail(20))