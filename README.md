# NeoUrl ![Version](https://img.shields.io/badge/Version-0.1.0-cyan.svg) ![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg) ![Standart](https://img.shields.io/badge/C++_STD-14-red.svg)

# **Dependencies**
- [**Git**](https://git-scm.com/downloads)

# Installing
- **Installation from repository**
  ```bash
  git clone https://github.com/reslaid/neourl.git
  ```

# Docs
- **Including a header file**
    ```cpp
    #include "neourl/neourl.hpp"
    ```

- **Creating Instances**
    ```cpp
    NeoUrl::Conv nconv; // Instance for quick conversions
    ```

- **Create a request**
    ```cpp
    NeoUrl::Request nreq = nconv.toReq(
        "URL-HERE"
    ); // Conversion to request by url
    ```

- **Sending a request using the get method**
    ```cpp
    // Get request
    NeoUrl::Response nGetResp = nrq.get();
    ```

- **Sending a request using the post method**
    ```cpp
    // Post request
    NeoUrl::Response nPostResp = rq.post(
        "DATA TO POST"
    );
    ```

- **Getting the result of a query**
    ```cpp
    nGetResp.raw; // Raw text on page

    // Request status (NeoUrl::statuscode)
    ngetResp.status;

    // Request status (numeric value)
    (int)nGetResp.status; // Request status
    ```
