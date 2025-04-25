import streamlit as st
import pandas as pd
from tinydb import TinyDB, Query
import signal
import subprocess

def receive_dump():
    db = TinyDB('system.diagnostics.json')
    stats_table = db.table('stats')
    diagnostics_table = db.table('diagnostics')
    query = Query()
    suc = stats_table.get(query.is_success == True)
    if suc == None:
        return None
    elif suc['is_success'] == False:
        return None
    elif suc['is_success'] == True:
        return diagnostics_table.all()
    return None


def run_external_script():
    command = ["python", ".\\py_src\\lab_two\\diag_storages.py", "--skipDisplay", "True", "--updateFreq", "0.1", "--updateTimes", "20"]
    subprocess.run(command, check=True)

def update_view(dump):

    df = pd.DataFrame(dump)
    # Convert timestamp
    df['timestamp'] = pd.to_datetime(df['timestamp'])

    st.write("### Raw Data")
    st.dataframe(df)

    # Line chart for CPU, Memory, Disk
    st.write("### Resource Usage Over Time")
    st.line_chart(df.set_index('timestamp')[['cpu_usage', 'memory_usage', 'disk_usage']])

    # Bytes Sent/Received
    st.write("### Network Activity (MB)")
    st.line_chart(df.set_index('timestamp')[['bytes_sent_MB', 'bytes_recv_MB']])

    # Optional: Show summary stats
    st.write("### Summary Statistics")
    st.write(df.describe())

should_run = True
def graceful_shutdown(signum, frame):
    global should_run
    should_run = False
#signal.signal(signal.SIGINT, graceful_shutdown)

# Streamlit App
st.title("📊 System Diagnostics Dashboard")

dump = receive_dump()
if dump: 
    update_view(dump)
else:
    st.info("Please wait while we collecting data :)")
run_external_script()
st.rerun()

# TO RUN: 
# $ streamlit run app.py

