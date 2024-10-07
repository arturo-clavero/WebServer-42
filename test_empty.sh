#!/bin/bash
BASE_URL="http://127.0.0.1:8083"


GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'


test_endpoint() {
    local method=$1
    local endpoint=$2
    local expected_code=$3
    local description=$4

    echo "Testing: $description"
    response=$(curl -s -o /dev/null -w "%{http_code}" -X $method $BASE_URL$endpoint)
    
    if [ "$response" -eq "$expected_code" ]; then
        echo -e "${GREEN}PASS${NC}: $method $endpoint returned $response (expected $expected_code)"
    else
        echo -e "${RED}FAIL${NC}: $method $endpoint returned $response (expected $expected_code)"
    fi
    echo ""
}

test_endpoint "GET" "/" 200 "Root endpoint with GET"
test_endpoint "POST" "/" 405 "Root endpoint with POST (should fail)"

test_endpoint "GET" "/uploads" 200 "Uploads endpoint with GET"
test_endpoint "POST" "/uploads" 400 "Uploads endpoint with POST"
#test_endpoint "DELETE" "/uploads/cat.png" 200 "Uploads endpoint with DELETE"

test_endpoint "GET" "/cgi-bin/picgen.py" 302 "CGI endpoint with GET"
test_endpoint "POST" "/cgi-bin/picgen.py" 302 "CGI endpoint with POST"

test_endpoint "GET" "/nonexistent" 404 "Nonexistent page (404)"
test_endpoint "GET" "/uploads" 200 "Existing page"

echo "Testing file upload to /uploads"
curl -X POST -F "file=@test.txt" $BASE_URL/uploads -w "\nResponse code: %{http_code}\n"

test_endpoint "GET" "/uploads/" 200 "Autoindex on uploads (should show directory listing)"
test_endpoint "GET" "/images/" 200 "Autoindex on images (should show directory listing)"

echo "All tests completed!"



#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

test_endpoint() {
    local method=$1
    local endpoint=$2
    local expected_code=$3
    local description=$4
    local data=$5  # Optional data to send

    echo "Testing: $description"
    
    if [ -z "$data" ]; then
        # No data - just send the method
        response=$(curl -s -o /dev/null -w "%{http_code}" -X $method $BASE_URL$endpoint)
    else
        # Send with data
        response=$(curl -s -o /dev/null -w "%{http_code}" -X $method -F "$data" $BASE_URL$endpoint)
    fi
    
    if [ "$response" -eq "$expected_code" ]; then
        echo -e "${GREEN}PASS${NC}: $method $endpoint returned $response (expected $expected_code)"
    else
        echo -e "${RED}FAIL${NC}: $method $endpoint returned $response (expected $expected_code)"
    fi
    echo ""
}

# Test empty POST - should fail
test_endpoint "POST" "/uploads" 400 "Uploads endpoint with empty POST"

# Test actual file upload
echo "Test content" > test.txt
test_endpoint "POST" "/uploads" 200 "Uploads endpoint with file" "file=@test.txt"
rm test.txt

# Test GET method
test_endpoint "GET" "/uploads" 200 "Uploads endpoint with GET"