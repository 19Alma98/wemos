import os
from datetime import datetime

from .config import CSV_FILE


def log_reading(temp: float, hum: float) -> None:
    os.makedirs(os.path.dirname(CSV_FILE), exist_ok=True)
    write_header = not os.path.exists(CSV_FILE)
    with open(CSV_FILE, "a") as f:
        if write_header:
            f.write("time,temp,hum\n")
        f.write(f"{datetime.now()},{temp},{hum}\n")
