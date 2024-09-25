#!/usr/bin/env python3
import random
from PIL import Image
import io
import sys

# Generate a random color
color = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

# Create a 500x500 image with the random color
image = Image.new('RGB', (500, 500), color)

# Save the image to a bytes buffer
img_buffer = io.BytesIO()
image.save(img_buffer, format='PNG')
img_bytes = img_buffer.getvalue()


# Construct the full HTTP response
response = b"HTTP/1.1 200 OK\r\n"
response += b"Content-Type: image/png\r\n"
response += f"Content-Length: {len(img_bytes)}\r\n".encode()
response += b"\r\n"
response += img_bytes

# Write the full response
sys.stdout.buffer.write(response)

