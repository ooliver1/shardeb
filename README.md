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

## Building

```bash
# {version} is your nginx version from `nginx -v`
wget https://nginx.org/download/nginx-{version}.tar.gz
tar -xzvf nginx-{version}.tar.gz
cd nginx-{version}
./configure --add-dynamic-module=.. --with-compat
make modules
# the shared object is located in objs/shardeb.so
```
