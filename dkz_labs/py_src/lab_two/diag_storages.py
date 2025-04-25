from tinydb import TinyDB, Query
import psutil
import datetime
import signal
import matplotlib.pyplot as plt
import argparse
from datetime import timedelta
import os

parser = argparse.ArgumentParser(description="Command line arguments for script")
parser.add_argument('--updateFreq', type=float, default=2, help="Frequency of updates (default: 2)")
parser.add_argument('--updateTimes', type=int, default=30, help="Number of update times (default: 30)")
parser.add_argument('--skipDisplay', type=bool, default=False, help="Skip display flag (default: False)")
args = parser.parse_args()

if not os.path.exists('system.diagnostics.json'):
    open('system.diagnostics.json', 'w').close()

db = TinyDB('system.diagnostics.json')
stats_table = db.table('stats')
diagnostics_table = db.table('diagnostics')
query = Query()

should_run = True
def graceful_shutdown(signum, frame):
    global should_run
    should_run = False
signal.signal(signal.SIGINT, graceful_shutdown)

def accumulate_stats(freq, times):
    global stats_table
    global diagnostics_table
    global query
    suc = stats_table.get(query.is_success == True)
    if suc == None:
        stats_table.insert ({
            'is_success': False
        })
    elif suc['is_success'] == False:
        stats_table.update ({
            'is_success': False
        })
    elif suc['is_success'] == True:
        return diagnostics_table.all()

    x = len(diagnostics_table.all())
    # if we have one record, it can 
    # be mailformed by the potential crash
    if x > 0:
        x -= 1
        diagnostics_table.remove(doc_ids=[x])


    for x in range(x, times):
        if should_run == False:
            return
        

        timestamp = datetime.datetime.now().isoformat()
        cpu = psutil.cpu_percent(interval=freq)
        mem = psutil.virtual_memory().percent
        disk = psutil.disk_usage('/').percent
        net = psutil.net_io_counters()

        diagnostics_table.insert({
            'timestamp': timestamp,
            'cpu_usage': cpu,
            'memory_usage': mem,
            'disk_usage': disk,
            'bytes_sent_MB': net.bytes_sent / (1024 ** 2),
            'bytes_recv_MB': net.bytes_recv / (1024 ** 2)
        })
    stats_table.update ({
        'is_success': True
    })
    return diagnostics_table.all()
# Default
updateFreq = args.updateFreq
updateTimes = args.updateTimes
skipDisplay = args.skipDisplay

if skipDisplay == True:
    stats_table.truncate()  # Clears all data in stats table
    diagnostics_table.truncate()  # Clears all data in diagnostics table

data = accumulate_stats(updateFreq, updateTimes)

if skipDisplay == True:
    exit(0)

timestamps = [entry['timestamp'] for entry in data]
cpu_usage = [entry['cpu_usage'] for entry in data]
memory_usage = [entry['memory_usage'] for entry in data]
disk_usage = [entry['disk_usage'] for entry in data]
bytes_sent = [entry['bytes_sent_MB'] for entry in data]
bytes_recv = [entry['bytes_recv_MB'] for entry in data]

plt.figure(figsize=(12, 8))
plt.subplot(2, 2, 1)
plt.plot(timestamps, cpu_usage, label='CPU Usage (%)', color='tab:blue')
plt.xticks(rotation=45)
plt.xlabel('Timestamp')
plt.ylabel('CPU Usage (%)')
plt.title('CPU Usage Over Time')
plt.grid(True)

plt.subplot(2, 2, 2)
plt.plot(timestamps, memory_usage, label='Memory Usage (%)', color='tab:green')
plt.xticks(rotation=45)
plt.xlabel('Timestamp')
plt.ylabel('Memory Usage (%)')
plt.title('Memory Usage Over Time')
plt.grid(True)

plt.subplot(2, 2, 3)
plt.plot(timestamps, disk_usage, label='Disk Usage (%)', color='tab:red')
plt.xticks(rotation=45)
plt.xlabel('Timestamp')
plt.ylabel('Disk Usage (%)')
plt.title('Disk Usage Over Time')
plt.grid(True)

plt.subplot(2, 2, 4)
plt.plot(timestamps, bytes_sent, label='Bytes Sent (MB)', color='tab:orange')
plt.plot(timestamps, bytes_recv, label='Bytes Received (MB)', color='tab:purple')
plt.xticks(rotation=45)
plt.xlabel('Timestamp')
plt.ylabel('Network Traffic (MB)')
plt.title('Network Traffic Over Time')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
