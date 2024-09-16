# Webserver Request Processing Flow

## 1. Request Reception and Parsing

- The server receives the raw HTTP request in `Server::read_request` (`src/server/Server.cpp`).
- The request is parsed using `RequestParser::parse` (`src/request_parser/RequestParser.cpp`).
- This creates an `HttpRequest` object containing the method, path, headers, and body.

## 2. Server Configuration Matching

- The server finds the matching configuration for the request using `Server::find_match_config` (`src/server/Server.cpp`).
- This matches the request's host to the appropriate `ServerConfig` (`src/config/ServerConfig.hpp`).

## 3. Response Building

The `ResponseBuilder::build` method (`src/response_builder/ResponseBuilder.cpp`) orchestrates the response creation:

### a. Request Validation

- Checks if the request is valid using `ResponseUtils::isRequestValid`.
- Finds the appropriate location configuration with `ResponseUtils::findLocation`.

### b. Method-specific Handling

Depending on the HTTP method, it calls the appropriate builder method:

#### GET Requests (detailed)

Calls `buildGetResponse`:

1. Checks if GET method is allowed for the location.
2. Determines the target type (file or directory) using `ResponseUtils::getTargetType`.
3. For files (`buildGetFileResponse`):
   - Verifies file existence and readability.
   - Builds a success response with `buildSuccessResponse`:
     - Sets status code to 200.
     - Reads file content with `ResponseUtils::buildBodyFromFile`.
     - Sets content type based on file extension.
     - Sets content length and other headers.
4. For directories (`buildGetDirectoryResponse`):
   - Checks for an index file specified in the location config.
   - If index exists, serves it as a file.
   - If no index or it doesn't exist, checks for autoindex:
     - If enabled, generates directory listing with `buildAutoindexResponse`.
     - If disabled, returns 403 Forbidden error.
   - Sets appropriate content type and disposition headers.
5. Handles errors (404 Not Found, 403 Forbidden) as needed.

#### POST Requests

Calls `buildPostResponse`:

1. Verifies if POST is allowed and the target location is writable.
2. Processes POST data using `processPostData`:
   - Extracts multipart form data using `PostRequestBodySnatcher::parse`.
   - Saves uploaded files using `ResponseUtils::saveFile`.
3. Builds a success response or handles errors.

#### DELETE Requests

Calls `buildDeleteResponse`:

1. Checks if DELETE is allowed and the target file exists and is writable.
2. Attempts to delete the file.
3. Builds a success or error response accordingly.

### c. Error Handling

- Throughout the process, `buildErrorResponse` is used to generate appropriate error responses.
- It sets the status code, checks for custom error pages, and builds the response body.

## 4. Response Finalization

- The built response is encapsulated in a `RequestResponse` object (`src/response_builder/RequestResponse.hpp`).
- This object is converted to a string format ready for sending back to the client.

## 5. Response Transmission

- The server sends the response back to the client in `Server::write_response` (`src/server/Server.cpp`).