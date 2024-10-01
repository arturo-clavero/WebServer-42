import sys
import os


def append_to_guestbook(message):
    guestbook_path = 'www/guestbook.html'
    message = message[8:-1]
    message = message.replace('%20', ' ')
    with open(guestbook_path, 'r+') as guestbook:
        content = guestbook.read()
        insert_position = content.find('</body>')
        if insert_position != -1:
            guestbook.seek(insert_position)
            guestbook.write(f'<div class="message"><p>{message}</p></div>\n')
            guestbook.write(content[insert_position:])
        else:
            guestbook.write(f'<div class="message"><p>{message}</p></div>\n')


if len(sys.argv) > 1:
	message = sys.argv[1]
	append_to_guestbook(message)


response = b"HTTP/1.1 200 OK\r\n"
response += b"Content-Type: text/plain\r\n"
sys.stdout.buffer.write(response)
