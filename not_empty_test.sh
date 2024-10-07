#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'


echo "This is a test file" > test_upload.txt

echo "Testing file upload to /uploads"
response=$(curl -X POST \
  -F "file=@test_upload.txt" \
  -w "%{http_code}" \
  -o /dev/null \
  -s \
  http://127.0.0.1:8082/uploads)

if [ "$response" -eq 200 ]; then
    echo -e "${GREEN}PASS${NC}: File upload succeeded (HTTP 200)"
else
    echo -e "${RED}FAIL${NC}: File upload failed (HTTP $response)"
fi
rm test_upload.txt