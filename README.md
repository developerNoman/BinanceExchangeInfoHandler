# BinanceExchangeInfoHandler
<p>This project is to develop a C++ Application that interacts with the Binance cryptocurrency exchange API, retrieves different market information including spot, usd futures and coin future data from the binance</p>

# Build And Run:

1. Clone the binance exchange handler repository: `git clone https://github.com/developerNoman/BinanceExchangeInfoHandler`
2. Create a build directory: `mkdir build`
3. Copy and paste the query.json and config.json in root of build folder and root of build/UnitTest: `cp query.json config.json build/` `cp query.json config.json build/UnitTest`
4. Go to the build directory: `cd build`
5. Run the command: `cmake ..`
6. Run the command: `make`
7. If you are using the docker, build image `sudo docker build -t myExchangeData .` and then run the container with volume mapping `sudo docker run -it -v.:/app --name mycontainer myExchangeData`
7. Run main executables: `./main`
8. Run Unit Tests: `./UnitTest/runTests`
9. 
