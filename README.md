# Algorithms Engineering Project

### Development
- Install `GoogleTest` framework
```
sudo apt-get update
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/libgtest*.a /usr/lib
sudo ldconfig
```
- Use `wsl`.
```bash
wsl
```
- To build project (including tests):
```bash
cd src
make build
```
- To delete all build files:
```bash
cd src
make clean
```
- To run project and tests:
```bash
cd src
# Run project
./bld/experiment.exe graph_file destinations_file output_file run_number
# Run tests
./bld_tst/test_experiment.exe
```