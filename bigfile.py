import os
import random
import string

def generate_random_string(length):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def generate_large_file(filename, size_in_gb):
    chunk_size = 1024 * 1024  # 1 MB
    total_chunks = size_in_gb * 1024  # Number of 1MB chunks in X GB

    with open(filename, 'w') as file:
        for _ in range(total_chunks):
            chunk = generate_random_string(chunk_size)
            formatted_chunk = '\n'.join(chunk[i:i+100] for i in range(0, len(chunk), 100))
            file.write(formatted_chunk + '\n')

    actual_size = os.path.getsize(filename)
    print(f"File '{filename}' created.")
    print(f"Actual size: {actual_size / (1024**3):.2f} GB")

if __name__ == "__main__":
    filename = "large_file.txt"
    size_in_gb = 5
    generate_large_file(filename, size_in_gb)