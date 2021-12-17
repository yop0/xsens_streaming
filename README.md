# xsens_streaming

This package provides an API to read XSens tracking and sensor data provided by the MT Software Suite 4.6

## Instructions

- First install the "MT Software Suite for Linux" stable release (v4.6) from [Xsens download page](https://www.xsens.com/software-downloads). Only the `mtsdk` is required, and the xsens installer will by default install it to `/usr/local/xsens`
- Build and install this package:

```sh
git clone --recurse-submodules git@github.com:arntanguy/xsens_streaming
cd xsens_streaming
mkdir build && cd build
cmake ..
make -j8
sudo make install
```
## Testing

A sample program is provided to read and print the received data:

```sh
xsens_streaming_sample -h <hostname> -p <port>
```

Where:
- the hostname is the IP of the machine running the xsens_streaming client (running xsens_streaming_sample)
- Xsens MVN Analyze software has its "Network Streamer" configured to stream to `<hostname> <port>` over UDP.
