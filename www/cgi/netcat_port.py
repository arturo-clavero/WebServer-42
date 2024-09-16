#!/usr/bin/env python
import os
import sys
from PIL import Image, ImageDraw, ImageFont

# Get the port number from an environment variable set by your server
port = os.environ.get('SERVER_PORT', '8080')  # Default to 80 if not set

# Open the original image
original_image_name = "../images/netcat.jpg"
img = Image.open(original_image_name)

# Prepare to draw on the image
draw = ImageDraw.Draw(img)
font = ImageFont.truetype("../fonts/comicsans.ttf", 36)  # Adjust size as needed

# Draw the new text
draw.text((100, 10), f"Netcat listening at Port {port}", font=font, fill=(0,0,0))  # Moved text more to the right

# Generate the new image name
new_image_name = os.path.splitext(original_image_name)[0] + "_modified.jpg"

# Save the modified image
img.save(new_image_name, "JPEG")

print("Status: 200 OK")
print("Content-Type: image/jpeg")
print(f"Content-Length: {os.path.getsize(new_image_name)}")
print()  # Empty line to separate headers from body

# Output the image data
with open(new_image_name, "rb") as f:
    sys.stdout.buffer.write(f.read())

# # Clean up the temporary file
# os.remove(new_image_name)