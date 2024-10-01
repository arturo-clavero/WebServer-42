#!/usr/bin/env python3
import sys
import os

def append_to_guestbook(message):
    guestbook_path = 'www/guestbook.txt'
    with open(guestbook_path, 'a') as guestbook:
        guestbook.write(message + '\n')

if len(sys.argv) > 1:
	message = sys.argv[1]
	append_to_guestbook(message)
	print("Content-Type: text/plain")
	print()
	print("Message added to guestbook successfully!")
else:
	append_to_guestbook("Guest")
	print("Content-Type: text/plain")
	print()
	print("Error: No message provided.")

