import sys
import os
import requests
import json
import random
from PIL import Image, ImageDraw, ImageFont
import io
from oauthlib.oauth2 import BackendApplicationClient
from requests_oauthlib import OAuth2Session

def generate_welcome_image(welcome_message):

	background_color = (0, 255, 0)  # Bright green (00FF00)
	text_color = (255, 0, 255)  # Hot pink

	# Choose Comic Sans font and size
	font_size = 24
	font = ImageFont.truetype("/usr/share/fonts/truetype/msttcorefonts/Comic_Sans_MS.ttf", font_size)

	# Calculate text size
	dummy_draw = ImageDraw.Draw(Image.new('RGB', (1, 1)))
	text_width, text_height = dummy_draw.textsize(welcome_message, font=font)

	# Create an image just big enough to fit the text
	padding = 10
	image_width = text_width + (padding * 2)
	image_height = text_height + (padding * 2)
	image = Image.new('RGB', (image_width, image_height), background_color)

	# Create a drawing object
	draw = ImageDraw.Draw(image)

	# Draw the text on the image
	text_position = (padding, padding)
	draw.text(text_position, welcome_message, font=font, fill=text_color)

	# Add some early 2000s flair: a simple shadow effect
	shadow_color = (128, 0, 128)  # Purple shadow
	draw.text((padding+2, padding+2), welcome_message, font=font, fill=shadow_color)
	draw.text(text_position, welcome_message, font=font, fill=text_color)

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
	exit()


try:
    UID = "u-s4t2ud-c3d6d2965823c31456699f32da9c743e9c4b987831e1c53161df5c594206c4b4"
    SECRET = "REMOVED"

    client = BackendApplicationClient(client_id=UID)
    oauth = OAuth2Session(client=client)

    token = oauth.fetch_token(token_url='https://api.intra.42.fr/oauth/token', client_id=UID, client_secret=SECRET)

    url = f"https://api.intra.42.fr/v2/users/{os.environ.get('USER', '')}"

    response = oauth.get(url)

    if response.status_code == 200:
        user_data = response.json()
        first_name = user_data.get('first_name', '')
        if first_name:
            generate_welcome_image(f"Welcome, {first_name}!")
        else:
            generate_welcome_image("Welcome, stranger!")
    else:
        generate_welcome_image("Welcome, stranger!")

except Exception as e:
    generate_welcome_image("Welcome, stranger!")