# shardeb

An nginx module to export the current discord cluster of a web request to a variable.

## Development

Getting the header files for intellisense:

```bash
wget https://nginx.org/download/nginx-{version}.tar.gz
tar -xzvf nginx-{version}.tar.gz
cd nginx-{version}
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
sudo ln -s nginx-{version}/objs/shardeb_module.so /etc/nginx/modules/shardeb_{version}.so
echo "load_module /etc/nginx/modules/shardeb_{version}.so;" | sudo tee -a /etc/nginx/nginx.conf
```
