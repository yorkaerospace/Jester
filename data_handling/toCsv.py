import sys

import numpy as np
import pandas as pd

# Create a dtype with the binary data format and the desired column names
# 2 U_longs, 7 floats
LOG_DATA_TYPE = np.dtype([
    ('count', 'u4'),
    ('time_us', 'u4'),
    ('altitude', 'f4'),
    ('gyro_x', 'f4'),
    ('gyro_y', 'f4'),
    ('gyro_z', 'f4'),
    ('acc_x', 'f4'),
    ('acc_y', 'f4'),
    ('acc_z', 'f4'),
])

if __name__ == "__main__":
    data = np.fromfile(sys.argv[1], dtype=LOG_DATA_TYPE)
    pd.DataFrame(data).to_csv("out.csv")
