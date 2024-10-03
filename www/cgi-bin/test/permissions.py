#!/usr/bin/env python3

import cgi

print("Content-Type: text/html")    # Header for HTML
print()                              # Blank line to end header

# Intentional error: misspelled variable name
form = cgi.FieldStorage()
name = form.getvalue("name")

print("<html>")
print("<head><title>CGI Script Example</title></head>")
print("<body>")
print("<h1>Hello, " + nam + "!</h1>")  # Error here: 'nam' should be 'name'
print("</body>")
print("</html>")
