#!/usr/bin/env python3

import time
import cgi

# Print the required HTTP headers
print("Content-Type: text/html")
print()  # End headers

# Output the HTML response
print("<html><head><title>Infinite CGI Script</title></head><body>")
print("<h1>Running Infinite CGI Script</h1>")
print("<pre>")

try:
    while True:
        # Output the current time
        print(f"Current time: {time.ctime()}")
        time.sleep(1)  # Sleep for 1 second
except KeyboardInterrupt:
    print("\nScript interrupted.")
finally:
    print("</pre></body></html>")

