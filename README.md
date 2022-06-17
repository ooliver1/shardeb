# shardeb

An nginx module to export the current discord cluster of a web request to a variable.

## Development

Getting the header files for intellisense:

```bash
wget https://nginx.org/download/nginx-1.22.0.tar.gz
tar -xzvf nginx-1.22.0.tar.gz
cd nginx-1.22.0
./configure
mv objs/*.h src/core/
```
