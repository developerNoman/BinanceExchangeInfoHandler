# BinanceExchangeInfoHandler
<p>This project is to develop a C++ Application that interacts with the Binance cryptocurrency exchange API, retrieves different market information including spot, usd futures and coin future data from the binance</p>

# Build And Run:

1. Clone the binance exchange handler repository: `git clone https://github.com/developerNoman/BinanceExchangeInfoHandler`.
2. Create a build directory: `mkdir build`
3. Copy and paste the query.json and config.json in app of build folder and root of build/UnitTest: `cp query.json config.json build/app` `cp query.json config.json build/UnitTest`, same goes for benchmarks.
4. Go to the build directory: `cd build`. Note: If you don't want to make the executables for unittest, benchmarks or both. You cane use following commands in build folder `cmake -DBUILD_TEST=OFF ..` `cmake -DBUILD_BENCHMARK=OFF ..` `cmake -DBUILD_BOTH=OFF ..`
5. Run the command: `cmake ..`
6. Run the command: `make -j`
7. In docker, build image `sudo docker build -t myexchangedata .` and then run the container with volume mapping `sudo docker run -it -v.:/app --name mycontainer myexchangedata`
7. Run main executables: `./BinanceExchangeInfoHandler`
8. Run Unit Tests: `./UnitTest/unitTests`
9. Run Benchmarks: `./BenchMarks/BenchMark`

##  How to build and run the executables

```bash
mkdir build
cd build
cmake ..
make -j
./BinanceExchangeInfoHandler or cd UnitTest ./unitTests or cd BenchMarks ./BenchMark
```