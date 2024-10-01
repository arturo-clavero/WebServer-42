import sys
import os
import requests
import json
import random
from PIL import Image
import io

def generate_random_color_image():
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

try:
    UID = "u-s4t2ud-c3d6d2965823c31456699f32da9c743e9c4b987831e1c53161df5c594206c4b4"
    SECRET = "REMOVED"

    # Get access token using client credentials
    token_url = 'https://api.intra.42.fr/oauth/token'
    data = {
        'grant_type': 'client_credentials',
        'client_id': UID,
        'client_secret': SECRET
    }
    token_response = requests.post(token_url, data=data)
    token = token_response.json().get('access_token')

    # Use the token to make the API request
    url = f"https://api.intra.42.fr/v2/users/{os.environ.get('USER', '')}"
    headers = {'Authorization': f'Bearer {token}'}
    response = requests.get(url, headers=headers)

    if response.status_code == 200:
        user_data = response.json()
        medium_image_url = user_data.get('image', {}).get('versions', {}).get('medium')

        if medium_image_url:
            response = b"HTTP/1.1 302 Found\r\n"
            response += b"Location: " + medium_image_url.encode() + b"\r\n"
            response += b"\r\n"
            sys.stdout.buffer.write(response)
        else:
            generate_random_color_image()
    else:
        generate_random_color_image()
		
except Exception as e:
    generate_random_color_image()
