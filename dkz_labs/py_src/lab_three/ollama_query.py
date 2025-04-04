from tinydb import TinyDB, Query
import requests
import json

class bcolors:
    OKGREEN = '\033[92m'
    ENDC = '\033[0m'

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


dump = receive_dump()
dump = json.dumps(dump, separators=(',', ':'))

if dump == None:
    exit(-1)

# Define the API URL
url = "http://localhost:11434/api/generate"
postfix = " cpu usage is in percentage,memory_usage in megabytes, disk_usage is MB "
prompts = ["Is There any Signals of CPU, memory, or disk overload?, SHORT ANSWER",
               "What are the possible causes of the problems you are experiencing?, SHORT ANSWER",
               "What actions do you recommend to improve system performance?, SHORT ANSWER"]

# Define the request payload
data = {
    "model": "llama3.2",  # Change this if using a different model name
    "prompt": "",
    "stream": False  # Set to True if you want streaming responses
}

for prompt in prompts:
    data["prompt"] = prompt + postfix + dump
    # Send the request
    response = requests.post(url, json=data)

    print(bcolors.OKGREEN + prompt + "\n" + postfix + bcolors.ENDC)

    # Parse the response
    if response.status_code == 200:
        result = response.json()
        print(result["response"])
    else:
       print("Error:", response.text)
    
