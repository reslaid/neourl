#ifndef _REQUIRE_NEOURL
#define _REQUIRE_NEOURL
#endif

#pragma region includes

#include <iostream>  // Input-output stream operations
#include <regex>     // Regular expression support
#include <string>    // String class
#include <sstream>   // Input/output stream class to operate on strings
#include <iomanip>   // Parameterized stream manipulators

#pragma endregion

#pragma region specific inclusions

#ifdef _WIN32

#include <windows.h>    // Windows API main header
#include <wininet.h>    // Windows Internet API main header

#pragma comment(lib, "wininet.lib")  // Linking with WinINet library

#else

#include <sys/socket.h>  // Definitions of structures needed for sockets
#include <netdb.h>       // Definitions for network database operations
#include <unistd.h>      // Symbolic Constants and Macros for POSIX operating systems

#endif

#pragma endregion

#pragma region data types

typedef std::string responseraw;  // Typedef for raw response data

#pragma endregion

#pragma region the code itself

namespace NeoUrl
{
    // Class for parsing URL into tokens
    class URL
    {
    private:
        // Full raw URL
        std::string url;

        // Protocol (http/https)
        std::string protocol;

        // Domain
        std::string domain;

        // Path
        std::string path;

        // Query
        std::string query;

    public:
        // Class constructor (accepts the URL itself)
        URL(std::string url) : url(url)
        {
            parseURL();
        }


        // Parsing URL into tokens
        void parseURL()
        {
            // Initialize pattern
            std::regex urlRegex(R"(^(https?)://([^/]+)([^?#]*)(\?[^#]*)?(#.*)?$)");
            std::smatch matches;

            if (std::regex_match(url, matches, urlRegex)) {
                // Set values
                protocol = matches[1];
                domain = matches[2];
                path = matches[3];
                query = matches[4];
            }
            else
            {
                // Invalid URL
                std::cerr << "Invalid URL" << std::endl;
            }
        }

        // Getters for private values

        std::string getProtocol() const {
            return protocol;
        }

        std::string getDomain() const {
            return domain;
        }

        std::string getPath() const {
            return path;
        }

        std::string getQuery() const {
            return query;
        }
    };

    // Enumerator for HTTP status codes
    enum class statuscode : int {
        // Informational 1xx
        Continue = 0x64,
        SwitchingProtocols = 0x65,
        Processing = 0x66,
        EarlyHints = 0x67,

        // Success 2xx
        OK = 0xC8,
        Created = 0xC9,
        Accepted = 0xCA,
        NonAuthoritativeInformation = 0xCB,
        NoContent = 0xCC,
        ResetContent = 0xCD,
        PartialContent = 0xCE,
        MultiStatus = 0xCF,
        AlreadyReported = 0xD0,
        IMUsed = 0xE2,

        // Redirection 3xx
        MultipleChoices = 0x12C,
        MovedPermanently = 0x12D,
        Found = 0x12E,
        SeeOther = 0x12F,
        NotModified = 0x130,
        UseProxy = 0x131,
        TemporaryRedirect = 0x133,
        PermanentRedirect = 0x134,

        // Client Error 4xx
        BadRequest = 0x190,
        Unauthorized = 0x191,
        PaymentRequired = 0x192,
        Forbidden = 0x193,
        NotFound = 0x194,
        MethodNotAllowed = 0x195,
        NotAcceptable = 0x195,
        ProxyAuthenticationRequired = 0x197,
        RequestTimeout = 0x198,
        Conflict = 0x199,
        Gone = 0x19A,
        LengthRequired = 0x19B,
        PreconditionFailed = 0x19C,
        PayloadTooLarge = 0x19D,
        URITooLong = 0x19E,
        UnsupportedMediaType = 0x19F,
        RangeNotSatisfiable = 0x1A0,
        ExpectationFailed = 0x1A1,
        ImATeapot = 0x1A2,
        MisdirectedRequest = 0x1A5,
        UnprocessableEntity = 0x1A6,
        Locked = 0x1A7,
        FailedDependency = 0x1A8,
        UpgradeRequired = 0x1AA,
        PreconditionRequired = 0x1AC,
        TooManyRequests = 0x1AD,
        RequestHeaderFieldsTooLarge = 0x1AF,
        UnavailableForLegalReasons = 0x1C3,

        // Server Error 5xx
        InternalServerError = 0x1F4,
        NotImplemented = 0x1F5,
        BadGateway = 0x1F6,
        ServiceUnavailable = 0x1F7,
        GatewayTimeout = 0x1F8,
        HTTPVersionNotSupported = 0x1F9,
        VariantAlsoNegotiates = 0x1FA,
        InsufficientStorage = 0x1FB,
        LoopDetected = 0x1FC,
        NotExtended = 0x1FE,
        NetworkAuthenticationRequired = 0x1FF,

        // Error sending request
        Failed = -0x1
    };

    // Structure representing a response
    struct Response
    {
    public:
        responseraw raw;    // Raw response data
        statuscode status;  // HTTP status code
    };

    // Class responsible for handling the request
    class Request
    {
    private:
        URL url;  // URL object

        // Function to perform the request
        Response performRequest(const std::string& method, const std::string& data = "", const std::string& json = "") {
#ifndef _WIN32
            Response response;

            // Constructing request data
            std::string requestData = method + " " + url.getPath() + url.getQuery() + " HTTP/1.1\r\n";
            requestData += "Host: " + url.getDomain() + "\r\n";
            requestData += "Connection: close\r\n";
            if (!json.empty()) {
                requestData += "Content-Length: " + std::to_string(json.length()) + "\r\n";
                requestData += "Content-Type: application/json\r\n";

                requestData += "\r\n";
                requestData += json;
            }
            if (!data.empty()) {
                requestData += "Content-Length: " + std::to_string(data.length()) + "\r\n";
                requestData += "Content-Type: application/x-www-form-urlencoded\r\n";

                requestData += "\r\n";
                requestData += data;
            }

            // Socket initialization
            int sockfd;
            struct sockaddr_in serv_addr;
            struct hostent* server;

            response.raw = "";
            response.status = statuscode::Failed;  // Set default status

            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                std::cerr << "Error opening socket" << std::endl;
                return response;
            }

            server = gethostbyname(url.getDomain().c_str());
            if (server == NULL) {
                std::cerr << "Error, no such host" << std::endl;
                return response;
            }

            serv_addr.sin_family = AF_INET;
            bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
            serv_addr.sin_port = htons(80);

            // Connecting to the server
            if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                std::cerr << "Error connecting" << std::endl;
                return response;
            }

            // Sending request
            if (write(sockfd, requestData.c_str(), requestData.length()) < 0) {
                std::cerr << "Error writing to socket" << std::endl;
                return response;
            }

            // Receiving response
            responseraw responseData;
            char buffer[4096];
            int bytesRead;
            while ((bytesRead = read(sockfd, buffer, sizeof(buffer))) > 0) {
                responseData.append(buffer, bytesRead);

                // Parsing first line of response to extract status code
                size_t endOfLine = responseData.find("\r\n");
                if (endOfLine != std::string::npos) {
                    std::string firstLine = responseData.substr(0, endOfLine);

                    size_t statusCodeStart = firstLine.find(" ");
                    if (statusCodeStart != std::string::npos) {
                        size_t statusCodeEnd = firstLine.find(" ", statusCodeStart + 1);
                        if (statusCodeEnd != std::string::npos) {
                            std::string statusCodeStr = firstLine.substr(statusCodeStart + 1, statusCodeEnd - statusCodeStart - 1);
                            response.status = std::stoi(statusCodeStr);
                        }
                    }

                    break;
                }
            }

            response.raw = responseData;

            close(sockfd);  // Close socket

            return response;
#else
            Response response;

            // Default values
            response.raw = "";
            response.status = statuscode::Failed;

            // Constructing request data
            std::string requestData = method + " " + url.getPath() + url.getQuery() + " HTTP/1.1\r\n";
            requestData += "Host: " + url.getDomain() + "\r\n";
            requestData += "Connection: close\r\n";
            if (!json.empty()) {
                requestData += "Content-Length: " + std::to_string(json.length()) + "\r\n";
                requestData += "Content-Type: application/json\r\n";

                requestData += "\r\n";
                requestData += json;
            }
            if (!data.empty()) {
                requestData += "Content-Length: " + std::to_string(data.length()) + "\r\n";
                requestData += "Content-Type: application/x-www-form-urlencoded\r\n";

                requestData += "\r\n";
                requestData += data;
            }

            // WinINet API initialization
            HINTERNET hInternet = InternetOpenA("WinINet", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
            if (hInternet == NULL) {
                std::cerr << "Error opening Internet connection" << std::endl;
                return response;
            }

            // Connecting to the server
            HINTERNET hConnect = InternetConnectA(hInternet, url.getDomain().c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
            if (hConnect == NULL) {
                std::cerr << "Error connecting to server" << std::endl;
                InternetCloseHandle(hInternet);
                return response;
            }

            const char* acceptTypes[] = { "text/*", NULL };
            // Opening HTTP request
            HINTERNET hRequest = HttpOpenRequestA(hConnect, method.c_str(), (url.getPath() + url.getQuery()).c_str(), "HTTP/1.1", NULL, acceptTypes, 0, 0);
            if (hRequest == NULL) {
                std::cerr << "Error opening HTTP request" << std::endl;
                InternetCloseHandle(hConnect);
                InternetCloseHandle(hInternet);
                return response;
            }

            // Sending HTTP request
            if (!HttpSendRequestA(hRequest, NULL, 0, (LPVOID)requestData.c_str(), requestData.length())) {
                std::cerr << "Error sending HTTP request" << std::endl;
                InternetCloseHandle(hRequest);
                InternetCloseHandle(hConnect);
                InternetCloseHandle(hInternet);
                return response;
            }

            DWORD statusCode = 0;
            DWORD statusCodeSize = sizeof(statusCode);
            // Getting HTTP status code
            if (!HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL)) {
                std::cerr << "Error getting HTTP status" << std::endl;
                InternetCloseHandle(hRequest);
                InternetCloseHandle(hConnect);
                InternetCloseHandle(hInternet);
                return response;
            }

            std::string responseData;
            char buffer[4096];
            DWORD bytesRead;
            // Reading response data
            while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                responseData.append(buffer, bytesRead);
            }

            // Set response data and status
            response.raw = responseData;
            response.status = (statuscode)statusCode;

            // Close handles
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);

            return response;
#endif
        }

    public:
        // Class constructor
        Request(const URL& url) : url(url) {
            this->url = url;
        }

        // Perform GET request
        Response get(const std::string& postData = "", const std::string& json = "")
        {
            return performRequest("GET", postData, json);
        }

        // Perform POST request
        Response post(const std::string& postData = "", const std::string& json = "")
        {
            return performRequest("POST", postData, json);
        }

        // Perform PUT request
        Response put(const std::string& postData = "", const std::string& json = "")
        {
            return performRequest("PUT", postData, json);
        }

        // Perform DELETE request
        Response del(const std::string& postData = "", const std::string& json = "")
        {
            return performRequest("DELETE", postData, json);
        }
    };

    // Class responsible for conversions
    class Conv
    {
    public:
        // Convert URL string to Request object
        Request toReq(std::string urlString)
        {
            // Create a URL object from the input URL string
            URL url(urlString);

            // Create and return a Request object initialized with the URL
            return Request(url);
        }
    };
}

#pragma endregion
