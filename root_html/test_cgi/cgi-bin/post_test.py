import cgi
import json
import os
import sys

environ = os.environ
headers = {
        "Content-type": "text/html",
    }

for k, v in headers.items():
    print(f"{k}: {v}")
print()

if environ["CONTENT_TYPE"] == "application/json":
    print("Received json!<br>")
    body = json.loads(''.join(sys.stdin.readlines()))
    print(body)
elif environ["CONTENT_TYPE"] == "application/x-www-form-urlencoded":
    print("Received form!<br>")
    body = ''.join(sys.stdin.readlines()).split("&")
    for line in body:
        pair = line.split("=", 1)
        print(f"{pair[0]}: {pair[1]}", end="<br>")
else:
    for line in sys.stdin.readlines():
        if line == "":
            break
        print(line.rstrip())
